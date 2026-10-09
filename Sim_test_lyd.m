clear; clc; close all;

%% Indlæs data
data = readmatrix("DSP/build/data.txt");
data = data(:);

fs = 8000;
blockSize = 350;

numBlocks = floor(length(data)/blockSize);
f = (0:floor(blockSize/2))*fs/blockSize;
t = (0:blockSize-1)/fs;

%% Beregn FFT for alle blokke
magnitudes = zeros(numBlocks, length(f));

for i = 1:numBlocks

    idx = (i-1)*blockSize + (1:blockSize);
    block = data(idx);

    Y = fft(block);

    magnitude = abs(Y(1:length(f)))/blockSize;
    magnitude(2:end-1) = 2*magnitude(2:end-1);

    magnitudes(i,:) = magnitude;

end

%% Opret figur
fig = figure('Position', [100 100 1100 750]);

% Subplot 1: Tidsdomæne
ax1 = subplot(2,1,1);

hSignal = plot(ax1, t, data(1:blockSize), 'LineWidth', 1.5);

xlabel(ax1, 'Tid [s]');
ylabel(ax1, 'Amplitude');
title(ax1, 'Signal i tidsdomænet');
xlim(ax1, [0 blockSize/fs]);
ylim(ax1, [-max(abs(data)) max(abs(data))] * 1.1 + [-eps eps]);
grid(ax1, 'on');

% Subplot 2: FFT
ax2 = subplot(2,1,2);

hFFT = plot(ax2, f, magnitudes(1,:), 'LineWidth', 1.5);

xlabel(ax2, 'Frekvens [Hz]');
ylabel(ax2, 'Amplitude');
title(ax2, 'FFT af mikrofondata');

xlim(ax2, [0 2000]);
ylim(ax2, [0 max(magnitudes(:))*1.1 + eps]);
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

%% Callbacks
slider.Callback = @(src,event) updatePlot( ...
    round(src.Value), hSignal, hFFT, data, magnitudes, ...
    ax1, ax2, blockSize, fs, numBlocks);

playButton.Callback = @(src,event) playAnimation( ...
    src, slider, hSignal, hFFT, data, magnitudes, ...
    ax1, ax2, blockSize, fs, numBlocks);

%% Opdater plots
function updatePlot(blockNum, hSignal, hFFT, data, magnitudes, ...
                    ax1, ax2, blockSize, fs, numBlocks)

    blockNum = min(max(round(blockNum),1),numBlocks);

    % Udtræk aktuel blok
    idx = (blockNum-1)*blockSize + (1:blockSize);
    block = data(idx);

    % Opdater tidsdomæne
    set(hSignal, 'YData', block);

    % Opdater FFT
    set(hFFT, 'YData', magnitudes(blockNum,:));

    % Tidspunkt
    currentTime = (blockNum-1)*blockSize/fs;

    title(ax1, sprintf('Tidsdomæne - Blok %d/%d', ...
        blockNum, numBlocks));

    title(ax2, sprintf('FFT - Tid: %.3f s', currentTime));

    drawnow;
end

%% Afspil animation
function playAnimation(button, slider, hSignal, hFFT, ...
                       data, magnitudes, ax1, ax2, blockSize, fs, numBlocks)

    button.String = 'Pause';

    for i = round(slider.Value):numBlocks

        if ~isgraphics(button)
            return;
        end

        if button.Value == 0
            break;
        end

        slider.Value = i;

        updatePlot(i, hSignal, hFFT, data, magnitudes, ...
            ax1, ax2, blockSize, fs, numBlocks);

        pause(blockSize/fs);
    end

    if isgraphics(button)
        button.Value = 0;
        button.String = 'Play';
    end
end