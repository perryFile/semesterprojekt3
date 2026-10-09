close all;
clear;
clc;

%% Parameters
fs = 8000;

WINDOW_SIZE = floor(fs * 40 / 1000);  % 40 ms = 320 samples
HOP_SIZE = floor(fs * 10 / 1000);     % 10 ms = 80 samples

yDTMF = readmatrix("DSP/build/data.txt");
yDTMF = yDTMF(:);

% DTMF frequencies
f = [697 770 852 941 1209 1336 1477 1633];

% Goertzel bins
k = round(f/fs*WINDOW_SIZE);

% Number of sliding windows
numBlocks = floor((length(yDTMF)-WINDOW_SIZE)/HOP_SIZE) + 1;

if numBlocks < 1
    error('Not enough samples for one window.');
end

%% Calculate Hann window (same as C++)

n = (0:WINDOW_SIZE-1)';

hann = 0.5 * (1 - cos(2*pi*n/(WINDOW_SIZE-1)));

% Correction factor
windowSum = sum(hann);
correctionFactor = WINDOW_SIZE/windowSum;

%% Calculate Goertzel for all windows

% Without Hann
magnitudesNormal = zeros(numBlocks,8);

% With Hann
magnitudesHann = zeros(numBlocks,8);

for blockNum = 1:numBlocks

    % Sliding window
    startIdx = (blockNum-1)*HOP_SIZE + 1;
    idx = startIdx:(startIdx + WINDOW_SIZE - 1);

    block = yDTMF(idx);

    % Apply Hann window
    blockHann = block .* hann;

    %% Goertzel

    for i = 1:8

        omega = 2*pi*k(i)/WINDOW_SIZE;
        den = [1 -2*cos(omega) 1];

        % -------------------------
        % Without Hann
        % -------------------------

        y = filter(1, den, block);

        magnitudesNormal(blockNum,i) = sqrt(max(0, ...
            y(WINDOW_SIZE)^2 + y(WINDOW_SIZE-1)^2 ...
            - 2*cos(omega)*y(WINDOW_SIZE)*y(WINDOW_SIZE-1)));

        % -------------------------
        % With Hann
        % -------------------------

        yHann = filter(1, den, blockHann);

        magnitudesHann(blockNum,i) = sqrt(max(0, ...
            yHann(WINDOW_SIZE)^2 + yHann(WINDOW_SIZE-1)^2 ...
            - 2*cos(omega)*yHann(WINDOW_SIZE)*yHann(WINDOW_SIZE-1)));

    end

    %% Normalize magnitudes

    % Without Hann
    magnitudesNormal(blockNum,:) = ...
        2*magnitudesNormal(blockNum,:)/WINDOW_SIZE;

    % With Hann + amplitude correction
    magnitudesHann(blockNum,:) = ...
        2*magnitudesHann(blockNum,:)/WINDOW_SIZE * correctionFactor;

end

%% Square magnitudes BEFORE calculating thresholds

% Without Hann
magnitudesNormal = magnitudesNormal;

% With Hann
magnitudesHann = magnitudesHann;

%% Calculate thresholds from squared magnitudes

thresholdsNormal = sum(magnitudesNormal, 2)/20;
thresholdsHann = sum(magnitudesHann, 2)/20;

%% Create figure

fig = figure('Position',[100 100 1100 750]);

t = (0:length(yDTMF)-1)/fs;

%% Time domain

ax1 = subplot(2,1,1);

plot(ax1,t,yDTMF);
hold(ax1,'on');

% Start and end of sliding window
hStart = xline(ax1,0,'r','LineWidth',2);
hEnd = xline(ax1,WINDOW_SIZE/fs,'r','LineWidth',2);

xlabel(ax1,'Time (s)');
ylabel(ax1,'Amplitude');
title(ax1,'Recorded DTMF signal');

grid(ax1,'on');

%% Goertzel

ax2 = subplot(2,1,2);

hStem = stem(ax2,f,magnitudesNormal(1,:),'filled');

hold(ax2,'on');

hThreshold = yline(ax2,thresholdsNormal(1),'r--','Threshold');

xlabel(ax2,'Frequency (Hz)');
ylabel(ax2,'Magnitude^2');

title(ax2,'Goertzel - Without Hann');

xlim(ax2,[600 1750]);

% Same y-axis for both methods
maxMagnitude = max([magnitudesNormal(:); magnitudesHann(:)]);

ylim(ax2,[0 maxMagnitude*1.2 + eps]);

grid(ax2,'on');

% Adjust plot positions
ax1.Position = [0.1 0.59 0.85 0.32];
ax2.Position = [0.1 0.20 0.85 0.32];

%% Slider

slider = uicontrol( ...
    'Style','slider', ...
    'Min',1, ...
    'Max',max(2,numBlocks), ...
    'Value',1, ...
    'SliderStep',[1/max(1,numBlocks-1) ...
                  min(1,10/max(1,numBlocks-1))], ...
    'Units','normalized', ...
    'Position',[0.15 0.10 0.7 0.04]);

if numBlocks == 1
    slider.Enable = 'off';
end

%% Play/Pause

playButton = uicontrol( ...
    'Style','togglebutton', ...
    'String','Play', ...
    'Units','normalized', ...
    'Position',[0.45 0.02 0.1 0.06]);

%% Checkbox: Hann window

hannCheckbox = uicontrol( ...
    'Style','checkbox', ...
    'String','Use Hann window', ...
    'Value',0, ...
    'Units','normalized', ...
    'Position',[0.72 0.02 0.2 0.06], ...
    'FontSize',11);

%% Callbacks

slider.Callback = @(src,event) updatePlot( ...
    round(src.Value), ...
    hStem,hThreshold,hStart,hEnd, ...
    magnitudesNormal,magnitudesHann, ...
    thresholdsNormal,thresholdsHann, ...
    hannCheckbox, ...
    WINDOW_SIZE,HOP_SIZE,fs,numBlocks,ax2);

hannCheckbox.Callback = @(src,event) updatePlot( ...
    round(slider.Value), ...
    hStem,hThreshold,hStart,hEnd, ...
    magnitudesNormal,magnitudesHann, ...
    thresholdsNormal,thresholdsHann, ...
    hannCheckbox, ...
    WINDOW_SIZE,HOP_SIZE,fs,numBlocks,ax2);

playButton.Callback = @(src,event) playAnimation( ...
    src,slider, ...
    hStem,hThreshold,hStart,hEnd, ...
    magnitudesNormal,magnitudesHann, ...
    thresholdsNormal,thresholdsHann, ...
    hannCheckbox, ...
    WINDOW_SIZE,HOP_SIZE,fs,numBlocks,ax2);


%% Update function

function updatePlot(blockNum, ...
                    hStem,hThreshold,hStart,hEnd, ...
                    magnitudesNormal,magnitudesHann, ...
                    thresholdsNormal,thresholdsHann, ...
                    hannCheckbox, ...
                    WINDOW_SIZE,HOP_SIZE,fs,numBlocks,ax2)

    % Ensure valid block number
    blockNum = min(max(round(blockNum),1),numBlocks);

    %% Check if Hann is enabled

    if hannCheckbox.Value == 1

        magnitude = magnitudesHann(blockNum,:);
        threshold = thresholdsHann(blockNum);

        windowType = 'With Hann';

    else

        magnitude = magnitudesNormal(blockNum,:);
        threshold = thresholdsNormal(blockNum);

        windowType = 'Without Hann';

    end

    %% Update Goertzel magnitudes

    set(hStem,'YData',magnitude);

    %% Update threshold

    hThreshold.Value = threshold;

    %% Current window time

    currentTime = (blockNum-1)*HOP_SIZE/fs;
    endTime = currentTime + WINDOW_SIZE/fs;

    %% Update window markers

    hStart.Value = currentTime;
    hEnd.Value = endTime;

    %% Update title

    title(ax2,sprintf( ...
        'Window %d/%d | Time %.3f - %.3f s | %s', ...
        blockNum,numBlocks,currentTime,endTime,windowType));

    drawnow;

end


%% Play animation

function playAnimation(button,slider, ...
                       hStem,hThreshold,hStart,hEnd, ...
                       magnitudesNormal,magnitudesHann, ...
                       thresholdsNormal,thresholdsHann, ...
                       hannCheckbox, ...
                       WINDOW_SIZE,HOP_SIZE,fs,numBlocks,ax2)

    if button.Value == 0
        button.String = 'Play';
        return;
    end

    button.String = 'Pause';

    for i = round(slider.Value):numBlocks

        if ~isgraphics(button)
            return;
        end

        if button.Value == 0
            break;
        end

        slider.Value = i;

        updatePlot(i, ...
            hStem,hThreshold,hStart,hEnd, ...
            magnitudesNormal,magnitudesHann, ...
            thresholdsNormal,thresholdsHann, ...
            hannCheckbox, ...
            WINDOW_SIZE,HOP_SIZE,fs,numBlocks,ax2);

        % Move window every 10 ms
        pause(HOP_SIZE/fs);

    end

    if isgraphics(button)
        button.Value = 0;
        button.String = 'Play';
    end

end