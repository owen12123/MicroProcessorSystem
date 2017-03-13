M = csvread('0 deg calib.csv')
%% Time specifications:
   Fs = 60/size(M(:,1));                      % samples per second
   dt = 1/Fs;                     % seconds per sample
   StopTime = 60;                  % seconds
   t = (0:dt:StopTime-dt)';
   N = size(t,1);
   %% Fourier Transform:
   X = fftshift(fft(M));
   %% Frequency specifications:
   dF = Fs/N;                      % hertz
   f = -Fs/2:dF:Fs/2-dF;           % hertz
   %% Plot the spectrum:
   figure;
   plot(f,abs(X)/N);
   xlabel('Frequency (in hertz)');
   title('Magnitude Response');