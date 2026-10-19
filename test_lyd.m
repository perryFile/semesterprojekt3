clear; clc; close all;

% Indlæs data
data = readmatrix("DSP/data.txt");

Y = fft(data);
N = size(data,1);
% Samplingfrekvens
fs = 8000;

f = fs/N*(0:N-1);

% Antal samples
N = length(data);

% Tidsakse
t = (0:N-1)/fs;

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