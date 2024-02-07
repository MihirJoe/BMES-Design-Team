function [f, P1] = frequencyDomain(struct)
%INPUTS -
%struct - the data file recorded from arduino. It must be in the form of
%struct.t = time, struct.v = voltage, struct.fs = sampling frequency.

%OUTPUTS -
%t - time vector (x of time domain)
%f - frequency vector (x of frequency domain)
%P1 - power in the frequency domain (y of frequency domain)

S = struct.v;
fs = struct.fs;

%If the signal is odd, it turns it even.
if mod(length(S),2) >0 S = S(1:end-1); end

Fs = fs;              % Sampling frequency
T = 1/Fs;             % Sampling period
L = length(S);        % Length of signal
t = (0:L-1)*T;        % Time vector

Y = fft(S);

P2 = abs(Y/L);
P1 = P2(1:(L/2)+1);
P1(2:end-1) = 2*P1(2:end-1);

f = Fs*(0:(L/2))/L;

subplot(2,1,1)
plot(t,S)
title('Signal - Time Domain')
xlabel('t (s)')
ylabel('X(t)')
axis('tight')

subplot(2,1,2)
plot(f,P1)
title('Signal - Frequency Domain')
xlabel('f (Hz)')
ylabel('|P1(f)|')
axis('tight')


end
