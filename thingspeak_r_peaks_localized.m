% Enter your MATLAB code below
readChannelID = ; %%your channel ID here
readAPIKey = ' '; %%your read API here
y1 = thingSpeakRead(readChannelID,'Field',2,'NumPoints',500,'ReadKey',readAPIKey);
N = length(y1);
ls = size(y1);
fs = 250;% find the sampling rate or frequency
t = (0 : N-1) / fs;% sampling period
T = 1/fs;% sampling rate or frequency
plot(t,y1); title('R Peaks Localized by Wavelet Transform: Automatic Annotations')