close all;
clear all;

% DTMF tone generator
N = 1024;
fs = 8000;
t = (0:1:N-1)/fs; % Sampling rate and time vector
yDTMF = readmatrix("DSP/build/data.txt");

k697 = round(697/fs*N);
k770 = round(770/fs*N);
k852 = round(852/fs*N);
k941 = round(941/fs*N);
k1209 = round(1209/fs*N);
k1336 = round(1336/fs*N);
k1477 = round(1477/fs*N);
k1633 = round(1633/fs*N);

% DTMF detector (use Goertzel algorithm)
den697  = [1 -2*cos(2*pi*k697/N) 1];
den770  = [1 -2*cos(2*pi*k770/N) 1];
den852  = [1 -2*cos(2*pi*k852/N) 1];
den941  = [1 -2*cos(2*pi*k941/N) 1];
den1209 = [1 -2*cos(2*pi*k1209/N) 1];
den1336 = [1 -2*cos(2*pi*k1336/N) 1];
den1477 = [1 -2*cos(2*pi*k1477/N) 1];
den1633 = [1 -2*cos(2*pi*k1633/N) 1];


% Filter bank bandpass filtering
y697  = filter([1], den697, yDTMF);
y770  = filter([1], den770, yDTMF);
y852  = filter([1], den852, yDTMF);
y941  = filter([1], den941, yDTMF);
y1209 = filter([1], den1209, yDTMF);
y1336 = filter([1], den1336, yDTMF);
y1477 = filter([1], den1477, yDTMF);
y1633 = filter([1], den1633, yDTMF);


% Determine the absolute magnitudes of DFT coefficients
% Husk formel bruges
m(1) = sqrt(y697(N)^2 + y697(N-1)^2 - 2*cos(2*pi*k697/N)*y697(N)*y697(N-1)); 

m(2) = sqrt(y770(N)^2 + y770(N-1)^2 - ...
    2*cos(2*pi*k770/N)*y770(N)*y770(N-1));

m(3) = sqrt(y852(N)^2 + y852(N-1)^2 - ...
    2*cos(2*pi*k852/N)*y852(N)*y852(N-1));

m(4) = sqrt(y941(N)^2 + y941(N-1)^2 - ...
    2*cos(2*pi*k941/N)*y941(N)*y941(N-1));

m(5) = sqrt(y1209(N)^2 + y1209(N-1)^2 - ...
    2*cos(2*pi*k1209/N)*y1209(N)*y1209(N-1));

m(6) = sqrt(y1336(N)^2 + y1336(N-1)^2 - ...
    2*cos(2*pi*k1336/N)*y1336(N)*y1336(N-1));

m(7) = sqrt(y1477(N)^2 + y1477(N-1)^2 - ...
    2*cos(2*pi*k1477/N)*y1477(N)*y1477(N-1));

m(8) = sqrt(y1633(N)^2 + y1633(N-1)^2 - ...
    2*cos(2*pi*k1633/N)*y1633(N)*y1633(N-1));

% Convert the magnitudes of DFT coefficients
% to the single-side spectrum
m = 2*m/N;

% Determine the threshold
th = sum(m)/4;

% Plot the DTMF spectrum with the threshold
f = [697 770 852 941 1209 1336 1477 1633];
f1 = [0 fs/2];
th = [th th];

subplot(2,1,2);
stem(f,m);
grid;
hold;
plot(f1,th);

xlabel('Frequency (Hz)');
ylabel('(b) Spectral values');
