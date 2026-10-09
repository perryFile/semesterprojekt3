clear; clc; close all;

%% Indlæs data
data = readmatrix("DSP/build/data.txt");
data = data(:);

fs = 8000;

%% Sliding window indstillinger (samme som C++)
WINDOW_SIZE = floor(fs * 40 / 1000);  % 40 ms = 320 samples
HOP_SIZE = floor(fs * 10 / 1000);     % 10 ms = 80 samples

% Antal vinduer
numBlocks = floor((length(data) - WINDOW_SIZE) / HOP_SIZE) + 1;

if numBlocks < 1
    error('Ikke nok samples til ét vindue.');
end

%% Beregn Hann-vindue (samme som C++)

n = (0:WINDOW_SIZE-1)';

hann = 0.5 * (1 - cos(2*pi*n/(WINDOW_SIZE-1)));

% Korrektionsfaktor for Hann-vinduets dæmpning
windowSum = sum(hann);
correctionFactor = WINDOW_SIZE/windowSum;

%% Frekvensakse

f = (0:floor(WINDOW_SIZE/2)) * fs / WINDOW_SIZE;

% Magnituder med og uden Hann
magnitudesNormal = zeros(numBlocks, length(f));
magnitudesHann = zeros(numBlocks, length(f));

%% Beregn FFT for alle vinduer

for i = 1:numBlocks

    % Startindeks for aktuelt vindue
    startIdx = (i-1)*HOP_SIZE + 1;

    % Indekser for vinduet
    idx = startIdx:(startIdx + WINDOW_SIZE - 1);

    % Udtræk vinduet
    block = data(idx);

    %% FFT uden Hann

    Y = fft(block);

    magnitude = abs(Y(1:length(f))) / WINDOW_SIZE;

    % Ensidet amplitudespektrum
    if mod(WINDOW_SIZE, 2) == 0
        magnitude(2:end-1) = 2*magnitude(2:end-1);
    else
        magnitude(2:end) = 2*magnitude(2:end);
    end

    magnitudesNormal(i,:) = magnitude;

    %% FFT med Hann

    % Anvend Hann-vinduet
    blockHann = block .* hann;

    YHann = fft(blockHann);

    magnitudeHann = abs(YHann(1:length(f))) / WINDOW_SIZE;

    % Ensidet amplitudespektrum
    if mod(WINDOW_SIZE, 2) == 0
        magnitudeHann(2:end-1) = 2*magnitudeHann(2:end-1);
    else
        magnitudeHann(2:end) = 2*magnitudeHann(2:end);
    end

    % Korriger for Hann-vinduets dæmpning
    magnitudeHann = magnitudeHann * correctionFactor;

    magnitudesHann(i,:) = magnitudeHann;

end

%% Opret figur

fig = figure('Position', [100 100 1100 750]);

% Første vindue
block = data(1:WINDOW_SIZE);
t = (0:WINDOW_SIZE-1) / fs;

%% Subplot 1: Tidsdomæne

ax1 = subplot(2,1,1);

hSignal = plot(ax1, t, block, 'LineWidth', 1.5);

xlabel(ax1, 'Tid [s]');
ylabel(ax1, 'Amplitude');
title(ax1, 'Signal i tidsdomænet');

xlim(ax1, [t(1) t(end)]);

maxAmplitude = max(abs(data));
ylim(ax1, [-maxAmplitude maxAmplitude]*1.1 + [-eps eps]);

grid(ax1, 'on');

%% Subplot 2: FFT

ax2 = subplot(2,1,2);

hFFT = plot(ax2, f, magnitudesNormal(1,:), 'LineWidth', 1.5);

xlabel(ax2, 'Frekvens [Hz]');
ylabel(ax2, 'Amplitude');
title(ax2, 'FFT af mikrofondata');

xlim(ax2, [0 2000]);

% Samme y-akse med og uden Hann
maxMagnitude = max([magnitudesNormal(:); magnitudesHann(:)]);

ylim(ax2, [0 maxMagnitude*1.1 + eps]);

grid(ax2, 'on');

% Plads til slider
ax1.Position = [0.1 0.59 0.85 0.32];
ax2.Position = [0.1 0.20 0.85 0.32];

%% Slider

slider = uicontrol( ...
    'Style', 'slider', ...
    'Min', 1, ...
    'Max', max(2,numBlocks), ...
    'Value', 1, ...
    'Units', 'normalized', ...
    'Position', [0.15 0.10 0.7 0.04], ...
    'SliderStep', [1/max(1,numBlocks-1) ...
                   min(1,10/max(1,numBlocks-1))]);

%% Play/Pause

playButton = uicontrol( ...
    'Style', 'togglebutton', ...
    'String', 'Play', ...
    'Units', 'normalized', ...
    'Position', [0.45 0.02 0.1 0.06]);

%% Checkbox: Hann-vindue

hannCheckbox = uicontrol( ...
    'Style', 'checkbox', ...
    'String', 'Brug Hann-vindue', ...
    'Value', 0, ...
    'Units', 'normalized', ...
    'Position', [0.72 0.02 0.2 0.06], ...
    'FontSize', 11);

%% Callbacks

slider.Callback = @(src,event) updatePlot( ...
    round(src.Value), ...
    hSignal, hFFT, data, ...
    magnitudesNormal, magnitudesHann, hann, hannCheckbox, ...
    ax1, ax2, WINDOW_SIZE, HOP_SIZE, fs, numBlocks);

hannCheckbox.Callback = @(src,event) updatePlot( ...
    round(slider.Value), ...
    hSignal, hFFT, data, ...
    magnitudesNormal, magnitudesHann, hann, hannCheckbox, ...
    ax1, ax2, WINDOW_SIZE, HOP_SIZE, fs, numBlocks);

playButton.Callback = @(src,event) playAnimation( ...
    src, slider, ...
    hSignal, hFFT, data, ...
    magnitudesNormal, magnitudesHann, hann, hannCheckbox, ...
    ax1, ax2, WINDOW_SIZE, HOP_SIZE, fs, numBlocks);


%% Opdater plots

function updatePlot(blockNum, hSignal, hFFT, data, ...
                    magnitudesNormal, magnitudesHann, ...
                    hann, hannCheckbox, ...
                    ax1, ax2, WINDOW_SIZE, HOP_SIZE, fs, numBlocks)

    blockNum = min(max(round(blockNum),1),numBlocks);

    % Startindeks for aktuelt vindue
    startIdx = (blockNum-1)*HOP_SIZE + 1;

    % Udtræk aktuelt vindue
    idx = startIdx:(startIdx + WINDOW_SIZE - 1);
    block = data(idx);

    % Tidsakse for aktuelt vindue
    t = (idx-1) / fs;

    %% Tjek om Hann er aktiveret

    if hannCheckbox.Value == 1

        % Anvend Hann på signalet
        block = block .* hann;

        % Vælg FFT med Hann
        magnitude = magnitudesHann(blockNum,:);

        windowType = 'Med Hann';

    else

        % Vælg FFT uden Hann
        magnitude = magnitudesNormal(blockNum,:);

        windowType = 'Uden Hann';

    end

    %% Opdater tidsdomæne

    set(hSignal, 'XData', t, 'YData', block);

    xlim(ax1, [t(1) t(end)]);

    %% Opdater FFT

    set(hFFT, 'YData', magnitude);

    %% Opdater titler

    currentTime = (blockNum-1)*HOP_SIZE/fs;

    title(ax1, sprintf( ...
        'Tidsdomæne - Vindue %d/%d (%s)', ...
        blockNum, numBlocks, windowType));

    title(ax2, sprintf( ...
        'FFT - Tid: %.3f s (%s)', ...
        currentTime, windowType));

    drawnow;

end


%% Afspil animation

function playAnimation(button, slider, hSignal, hFFT, ...
                       data, magnitudesNormal, magnitudesHann, ...
                       hann, hannCheckbox, ax1, ax2, ...
                       WINDOW_SIZE, HOP_SIZE, fs, numBlocks)

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

        updatePlot(i, hSignal, hFFT, data, ...
            magnitudesNormal, magnitudesHann, ...
            hann, hannCheckbox, ...
            ax1, ax2, WINDOW_SIZE, HOP_SIZE, fs, numBlocks);

        % Flyt animationen med 10 ms per vindue
        pause(HOP_SIZE/fs);

    end

    if isgraphics(button)
        button.Value = 0;
        button.String = 'Play';
    end

end