clear;clc
fs = 625;
t = 1/fs:1/fs:1;
sig = [chirp(t,10,1,250) zeros(1,fs)];

index = 1000+[10 30 50];
speed = 19;
for ping = 1:5
    kernel = zeros(1,2*fs);
    for i = 1:length(index)
        kernel(ping*speed+index(i)+round(1*randn(1))) = 0.2+0.8*rand();
    end
    w = conv(sig,kernel);
    w = w+3*randn(size(w));
    v = xcorr(sig,w);
    v = v./max(v);
    sigsave(:,ping) = hilbert(v);
    
end
 
sig = sigsave(900:1923,:);
for i = 1:size(sig,2)
    plot(abs(sig(:,i))+i);hold on
end
sig0 = zeros(2048,5);
sig0(1:2:end,:) = real(sig);
sig0(2:2:end,:) = imag(sig);
fid = fopen('snr10speed10.bin','wb');
fwrite(fid,sig0,"double");
fclose(fid);

%%
fid = fopen('signalMod.bin','rb');
data = fread(fid,inf,"double");
fclose(fid);
% data = data(1:end/2,:);
plot(data)
