clc; 
clear all;
close all;

%arduino set and variables
a=arduino();
ecg=0;

for k=1:500
    b=readVoltage(a,'A0');
    ecg=[ecg,b];
    plot(ecg);
    grid on;
    drawnow;
end
% ecg7500=load('BEAT_12-2-2016 17.53.13.txt');
% for k=1:500
%     ecg(k)=ecg7500(k);
%     plot(ecg);
%     drawnow;
% end

%R wave calculation
wt = modwt(ecg,5);
wtrec = zeros(size(wt));
wtrec(4:5,:) = wt(4:5,:);
y = imodwt(wtrec,'sym4');
tm = 0:1:499;
y = abs(y).^2;
[qrspeaks,locs] = findpeaks(y,tm,'MinPeakHeight',0.35,'MinPeakDistance',0.150);
figure,
plot(tm,y);
hold on
plot(locs,qrspeaks,'ro')
xlabel('Seconds')
title('R Peaks Localized by Wavelet Transform with Automatic Annotations')
grid on

distanceBetweenFirstAndLastPeaks = locs(length(locs))-locs(1);
averageDistanceBetweenPeaks = distanceBetweenFirstAndLastPeaks/length(locs);
averageHeartRate = averageDistanceBetweenPeaks*10/3;
disp('Average Heart Rate = ');
disp(averageHeartRate);

%THINGSPEAK SETUP------------------------------
channelID = ; %%add your channel ID here
writeKey = ' '; %%add your write API key here
UserApikey = ' '; %%add youe User API key here
url = sprintf('https://api.thingspeak.com/channels/%s/feeds.json?api_key=%s',num2str(channelID),UserApikey);
response = webwrite(url, weboptions('RequestMethod','delete'));

%Creating timestamp----------------------------
tStamps = [datetime('now')-seconds(499):seconds(1):datetime('now')]';
dataTable = timetable(tStamps, ecg',y');
thingSpeakWrite(channelID,dataTable,'WriteKey',writeKey);


