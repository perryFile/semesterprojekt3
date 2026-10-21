close all;
clear;
clc;

%% Parameters
N = 80;
fs = 8000;

yDTMF = readmatrix("DSP/build/data.txt");
yDTMF = yDTMF(:);

f = [697 770 852 941 1209 1336 1477 1633];
k = round(f/fs*N);

numBlocks = floor(length(yDTMF)/N);

%% Calculate Goertzel for all blocks
magnitudes = zeros(numBlocks,8);
thresholds = zeros(numBlocks,1);

for blockNum = 1:numBlocks

    idx = (blockNum-1)*N + (1:N);
    block = yDTMF(idx);

    for i = 1:8

        omega = 2*pi*k(i)/N;
        den = [1 -2*cos(omega) 1];

        y = filter(1, den, block);

        magnitudes(blockNum,i) = sqrt( ...
            y(N)^2 + y(N-1)^2 ...
            - 2*cos(omega)*y(N)*y(N-1));

    end

    magnitudes(blockNum,:) = 2*magnitudes(blockNum,:)/N;
    thresholds(blockNum) = sum(magnitudes(blockNum,:))/4;

end

%% Create figure
fig = figure('Position',[100 100 1000 650]);

t = (0:length(yDTMF)-1)/fs;

% Time domain
ax1 = subplot(2,1,1);

plot(t,yDTMF);
hold on;

hTime = xline(0,'r','LineWidth',2);

xlabel('Time (s)');
ylabel('Amplitude');
title('Recorded DTMF signal');
grid on;

% Goertzel
ax2 = subplot(2,1,2);

hStem = stem(f,magnitudes(1,:),'filled');
hold on;

hThreshold = yline(thresholds(1),'r--','Threshold');

xlabel('Frequency (Hz)');
ylabel('Magnitude');
xlim([600 1750]);
ylim([0 max(magnitudes(:))*1.2 + eps]);
grid on;

%% Slider
slider = uicontrol( ...
    'Style','slider', ...
    'Min',1, ...
    'Max',max(2,numBlocks), ...
    'Value',1, ...
    'SliderStep',[1/max(1,numBlocks-1) 1/max(1,numBlocks-1)], ...
    'Units','normalized', ...
    'Position',[0.15 0.01 0.7 0.04]);

% Callback
slider.Callback = @(src,event) updatePlot( ...
    round(src.Value), hStem, hThreshold, hTime, ...
    magnitudes, thresholds, N, fs, numBlocks, ax2);

%% Update function
function updatePlot(blockNum, hStem, hThreshold, hTime, ...
                    magnitudes, thresholds, N, fs, numBlocks, ax2)

    % Ensure valid block number
    blockNum = min(max(blockNum,1),numBlocks);

    % Update Goertzel magnitudes
    set(hStem, 'YData', magnitudes(blockNum,:));

    % Update threshold
    hThreshold.Value = thresholds(blockNum);

    % Update time marker
    currentTime = (blockNum-1)*N/fs;
    hTime.Value = currentTime;

    % Update title
    title(ax2, sprintf( ...
        'Block %d/%d | Time %.3f s', ...
        blockNum, numBlocks, currentTime));

    drawnow;

end