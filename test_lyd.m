clear; clc; close all;

% Indlæs data
data = readmatrix("DSP/build/data.txt");

Y = fft(data);
N = size(data,1);
% Samplingfrekvens
fs = 8000;
blocksize = 80;
numBlocks = floor(length(data)/blockSize);
f = (0:floor(blockSize/2))*fs/blockSize;


% Plot
figure;
plot(f,abs(Y)/N,"LineWidth",3);
xlabel('Tid [s]');
ylabel('Amplitude');
title('Mikrofonsignal');
grid on;

YY = fftshift(Y);
ff = fs/N*(-N/2:N/2-1);
figure(); 
plot(ff,abs(YY)/N,"LineWidth",3) 
xlabel("f (Hz)") 
ylabel("|fft(X)|")