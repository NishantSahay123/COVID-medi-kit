% Enter your MATLAB code below
readChannelID = ; %%your channel ID here
readAPIKey = ' '; %%your read API key here
y1 = thingSpeakRead(readChannelID,'Field',1,'NumPoints',500,'ReadKey',readAPIKey);
fs = 250 % find the sampling rate or frequency
T = 1/fs;% sampling rate or frequency
window = 120; % 2 min 0r 120 second
N = 500;
t = (0 : N-1) / fs;% sampling period

%Compute the spectrum of the ECG and provide remarks on the spectral features of the ECG 
%DFT to describe the signal in the frequency
NFFT = 2 ^ nextpow2(N);
Y = fft(y1, NFFT) / N;
f = (fs / 2 * linspace(0, 1, NFFT / 2+1))'; % Vector containing frequencies in Hz
amp = ( 2 * abs(Y(1: NFFT / 2+1))); % Vector containing corresponding amplitudes
figure;
plot (f, amp);
title ('plot Frequency Response of ECG signal');
xlabel ('frequency (Hz)');
ylabel ('|y(f)|');
grid on;