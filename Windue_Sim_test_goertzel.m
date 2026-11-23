close all;
clear;
clc;

%% Parameters - match the C++ receiver
fs = 8000;
WINDOW_SIZE = 220;
HOP_SIZE = 21;
f = [697 770 852 941 1209 1336 1477 1633];

PEAK_RATIO = 2.5;
MIN_AMPLITUDE = 0.005;
NOISE_MARGIN = 3.0;

% false: match C++ version with PEAK_RATIO + MIN_AMPLITUDE.
% true: match C++ version with PEAK_RATIO + NOISE_MARGIN*noiseRms.
USE_CALIBRATED_NOISE = false;

% When calibration is enabled, enter the 8 noise RMS magnitudes measured
% with the SAME Hann window, amplitude correction and eight frequencies.
noiseRms = nan(1, 8);
if USE_CALIBRATED_NOISE && ...
        (numel(noiseRms) ~= 8 || any(~isfinite(noiseRms)) || any(noiseRms <= 0))
    error('Enter eight valid calibrated noiseRms values before enabling noise calibration.');
end

yDTMF = readmatrix('DSP/build/data.txt');
yDTMF = yDTMF(:);

numBlocks = floor((numel(yDTMF) - WINDOW_SIZE) / HOP_SIZE) + 1;
if numBlocks < 1
    error('Not enough samples for one window.');
end

%% Hann window (same as C++)
n = (0:WINDOW_SIZE-1)';
hannWindow = 0.5 * (1 - cos(2*pi*n/(WINDOW_SIZE-1)));
correctionFactor = WINDOW_SIZE / sum(hannWindow);

%% Goertzel magnitudes for each sliding window
magnitudesNormal = zeros(numBlocks, 8);
magnitudesHann = zeros(numBlocks, 8);

for blockNum = 1:numBlocks
    startIdx = (blockNum-1)*HOP_SIZE + 1;
    block = yDTMF(startIdx:startIdx+WINDOW_SIZE-1);
    blockHann = block .* hannWindow;

    for i = 1:8
        % IMPORTANT: exact target frequency, like C++: omega = 2*pi*f/fs
        % Do not round to the nearest FFT bin.
        omega = 2*pi*f(i)/fs;
        den = [1, -2*cos(omega), 1];

        stateNormal = filter(1, den, block);
        stateHann = filter(1, den, blockHann);

        magnitudesNormal(blockNum,i) = sqrt(max(0, ...
            stateNormal(end)^2 + stateNormal(end-1)^2 - ...
            2*cos(omega)*stateNormal(end)*stateNormal(end-1)));

        magnitudesHann(blockNum,i) = sqrt(max(0, ...
            stateHann(end)^2 + stateHann(end-1)^2 - ...
            2*cos(omega)*stateHann(end)*stateHann(end-1)));
    end

    % C++ Goertzel normalization, with and without Hann correction.
    magnitudesNormal(blockNum,:) = ...
        2*magnitudesNormal(blockNum,:) / WINDOW_SIZE;
    magnitudesHann(blockNum,:) = ...
        2*magnitudesHann(blockNum,:) / WINDOW_SIZE * correctionFactor;
end

%% Store parameters used by the callbacks
ctx.fs = fs;
ctx.f = f;
ctx.WINDOW_SIZE = WINDOW_SIZE;
ctx.HOP_SIZE = HOP_SIZE;
ctx.numBlocks = numBlocks;
ctx.magnitudesNormal = magnitudesNormal;
ctx.magnitudesHann = magnitudesHann;
ctx.PEAK_RATIO = PEAK_RATIO;
ctx.MIN_AMPLITUDE = MIN_AMPLITUDE;
ctx.NOISE_MARGIN = NOISE_MARGIN;
ctx.USE_CALIBRATED_NOISE = USE_CALIBRATED_NOISE;
ctx.noiseRms = noiseRms;

%% Figure
fig = figure('Position', [100 100 1100 750]);
t = (0:numel(yDTMF)-1)/fs;

ax1 = subplot(2,1,1);
plot(ax1, t, yDTMF);
hold(ax1, 'on');
hStart = xline(ax1, 0, 'r', 'LineWidth', 2);
hEnd = xline(ax1, WINDOW_SIZE/fs, 'r', 'LineWidth', 2);
xlabel(ax1, 'Time (s)');
ylabel(ax1, 'Amplitude');
title(ax1, 'Recorded DTMF signal');
grid(ax1, 'on');

ax2 = subplot(2,1,2);
hStem = stem(ax2, f, magnitudesHann(1,:), 'filled', 'LineWidth', 1.2);
hold(ax2, 'on');

% The detector does NOT use a single global threshold. It has one minimum
% acceptable magnitude for each group, based on that group's runner-up.
hLowThreshold = plot(ax2, f([1 4]), [0 0], 'r--', 'LineWidth', 2);
hHighThreshold = plot(ax2, f([5 8]), [0 0], 'm--', 'LineWidth', 2);
xlabel(ax2, 'Frequency (Hz)');
ylabel(ax2, 'Goertzel magnitude (not squared)');
xlim(ax2, [600 1750]);
legend(ax2, [hStem hLowThreshold hHighThreshold], ...
    {'Magnitude','Low-group threshold','High-group threshold'}, ...
    'Location', 'northeast');
grid(ax2, 'on');

ax1.Position = [0.1 0.59 0.85 0.32];
ax2.Position = [0.1 0.20 0.85 0.32];

ctx.hStem = hStem;
ctx.hLowThreshold = hLowThreshold;
ctx.hHighThreshold = hHighThreshold;
ctx.hStart = hStart;
ctx.hEnd = hEnd;
ctx.ax2 = ax2;

%% Controls
slider = uicontrol(fig, ...
    'Style', 'slider', ...
    'Min', 1, 'Max', max(2, numBlocks), 'Value', 1, ...
    'SliderStep', [1/max(1,numBlocks-1), min(1,10/max(1,numBlocks-1))], ...
    'Units', 'normalized', 'Position', [0.15 0.10 0.7 0.04]);
if numBlocks == 1
    slider.Enable = 'off';
end

playButton = uicontrol(fig, ...
    'Style', 'togglebutton', 'String', 'Play', ...
    'Units', 'normalized', 'Position', [0.45 0.02 0.1 0.06]);

hannCheckbox = uicontrol(fig, ...
    'Style', 'checkbox', 'String', 'Use Hann window (C++)', ...
    'Value', 1, ...
    'Units', 'normalized', 'Position', [0.72 0.02 0.25 0.06], ...
    'FontSize', 11);

% Calibrated noiseRms values are for the Hann-corrected C++ detector.
if USE_CALIBRATED_NOISE
    hannCheckbox.Enable = 'off';
end
ctx.hannCheckbox = hannCheckbox;

slider.Callback = @(src,~) updatePlot(round(src.Value), ctx);
hannCheckbox.Callback = @(~,~) updatePlot(round(slider.Value), ctx);
playButton.Callback = @(src,~) playAnimation(src, slider, ctx);

updatePlot(1, ctx);

%% Local functions
function result = detectTone(magnitude, ctx)
    % First four magnitudes = low group; last four = high group.
    [lowPeak, lowIndex] = max(magnitude(1:4));
    [highPeak, localHighIndex] = max(magnitude(5:8));
    highIndex = localHighIndex + 4;

    lowOthers = magnitude(1:4);
    lowOthers(lowIndex) = [];
    highOthers = magnitude(5:8);
    highOthers(localHighIndex) = [];

    secondLargestLow = max(lowOthers);
    secondLargestHigh = max(highOthers);

    % C++ threshold = PEAK_RATIO * second-largest peak in each group.
    peakThresholdLow = ctx.PEAK_RATIO * secondLargestLow;
    peakThresholdHigh = ctx.PEAK_RATIO * secondLargestHigh;

    if ctx.USE_CALIBRATED_NOISE
        floorLow = ctx.NOISE_MARGIN * ctx.noiseRms(lowIndex);
        floorHigh = ctx.NOISE_MARGIN * ctx.noiseRms(highIndex);
        floorOK = lowPeak > floorLow && highPeak > floorHigh;
    else
        floorLow = ctx.MIN_AMPLITUDE;
        floorHigh = ctx.MIN_AMPLITUDE;
        floorOK = lowPeak >= floorLow && highPeak >= floorHigh;
    end

    % Visualize the stronger of the two required magnitude checks.
    result.thresholdLow = max(peakThresholdLow, floorLow);
    result.thresholdHigh = max(peakThresholdHigh, floorHigh);
    result.lowPeak = lowPeak;
    result.highPeak = highPeak;
    result.lowIndex = lowIndex;
    result.highIndex = highIndex;
    result.accepted = lowPeak > peakThresholdLow && ...
        highPeak > peakThresholdHigh && floorOK;
end

function updatePlot(blockNum, ctx)
    blockNum = min(max(round(blockNum), 1), ctx.numBlocks);

    if ctx.hannCheckbox.Value == 1
        magnitude = ctx.magnitudesHann(blockNum,:);
        windowType = 'With Hann (C++)';
    else
        magnitude = ctx.magnitudesNormal(blockNum,:);
        windowType = 'Without Hann (diagnostic only)';
    end

    result = detectTone(magnitude, ctx);
    set(ctx.hStem, 'YData', magnitude);
    set(ctx.hLowThreshold, 'YData', repmat(result.thresholdLow, 1, 2));
    set(ctx.hHighThreshold, 'YData', repmat(result.thresholdHigh, 1, 2));

    timeStart = (blockNum-1) * ctx.HOP_SIZE / ctx.fs;
    timeEnd = timeStart + ctx.WINDOW_SIZE / ctx.fs;
    ctx.hStart.Value = timeStart;
    ctx.hEnd.Value = timeEnd;

    if result.accepted
        verdict = 'ACCEPTED';
    else
        verdict = 'REJECTED';
    end

    title(ctx.ax2, {sprintf('Window %d/%d | %.3f-%.3f s | %s | %s', ...
        blockNum, ctx.numBlocks, timeStart, timeEnd, windowType, verdict), ...
        sprintf('Low: %.4g vs %.4g (%.0f Hz)   |   High: %.4g vs %.4g (%.0f Hz)', ...
        result.lowPeak, result.thresholdLow, ctx.f(result.lowIndex), ...
        result.highPeak, result.thresholdHigh, ctx.f(result.highIndex))});

    ylim(ctx.ax2, [0 max([magnitude(:); result.thresholdLow; ...
        result.thresholdHigh; eps]) * 1.2]);
    drawnow;
end

function playAnimation(button, slider, ctx)
    if button.Value == 0
        button.String = 'Play';
        return;
    end
    button.String = 'Pause';

    for i = round(slider.Value):ctx.numBlocks
        if ~isgraphics(button)
            return;
        end
        if button.Value == 0
            break;
        end
        slider.Value = i;
        updatePlot(i, ctx);
        pause(ctx.HOP_SIZE/ctx.fs);
    end

    if isgraphics(button)
        button.Value = 0;
        button.String = 'Play';
    end
end