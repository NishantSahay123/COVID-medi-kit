% Enter your MATLAB code below
readChannelID = ; %%channel Id here
readAPIKey = ' '; %%read API key here
f_s=250;
ecg = thingSpeakRead(readChannelID,'Field',1,'NumPoints',500,'ReadKey',readAPIKey);

N1=length(ecg);
t1=[0:N1-1]/f_s;
plot(t1,ecg,'r'); title('Raw ECG Data')             
xlabel('time')
ylabel('amplitude')