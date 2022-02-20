clc; 
clear all;
close all;

%ARDUINO SET and TIME AXIS-----------------------------
a = arduino(); %'COM6', 'Nano3', 'ForceBuildOn', true
ecg=0;
f_s=250;
N=180;
t=[0:N-1]/f_s;

%loop to take data-------------------------------------
for k=1:180
    b=readVoltage(a,'A0');
    ecg=[ecg,b];
    plot(ecg);
    grid on;
    drawnow;
end

%NOTCH FILTER ECG------------------------------
w=50/(250/2);
bw=w;
[num,den]=iirnotch(w,bw); % notch filter implementation 
ecg_notch=filter(num,den,ecg);

%PLOT FILTERED ECG-----------------------------
figure, %%1
N1=length(ecg_notch);
t1=[0:N1-1]/f_s;
plot(t1,ecg_notch,'r'); title('Filtered ECG signal ')             
xlabel('time')
ylabel('amplitude')

%THINGSPEAK SETUP------------------------------
channelID = ; %%your channel ID here
writeKey = ''; %%your write API key here
UserApikey = ''; %%your User API key here
url = sprintf('https://api.thingspeak.com/channels/%s/feeds.json?api_key=%s',num2str(channelID),UserApikey);
response = webwrite(url, weboptions('RequestMethod','delete'));
%Creating timestamp----------------------------
tStamps = [datetime('now')-hours(179):hours(1):datetime('now')]';
%adding data with timestamp--------------------
for k=1:180
    ecg_180r(k) = round(ecg_notch(k));
    ecg_180(k) = ecg_notch(k);
end
dataTable = timetable(tStamps,ecg_180r');
thingSpeakWrite(channelID,dataTable,'WriteKey',writeKey);

%XLS DATA AND PLOT-----------------------------
t180 = 0:1:179;
xlsdata = [t180;ecg_180];
figure, %%2------------------------------------
plot(t180,ecg(1:180),'r'); title('Data plotting for 0 to 0.804 time frame')             
xlabel('time')
ylabel('amplitude')
hold on
plot(t180,ecg_180,'g');             
legend('ORIGINAL ECG SIGNAL',' Flitered ECG SIGNAL')
hold off
[Status, Message] =  xlswrite('ECG_FILTERED_THINGSPEAK.xlsx', xlsdata');