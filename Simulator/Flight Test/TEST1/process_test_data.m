close all 
clear 
clc 

data = readmatrix('FlightData_1100_1200s.csv');
% t (secs) def (deg) alt (m) pitch (deg) KTAS

t_s = data(:, 1);
de_deg = data(:, 2);
h_m  = data(:, 3);
theta_deg = data(:, 4);
KTAS = data(:, 5);

t_s_corr = t_s - t_s(1);


fs = 1 / mean(diff(t_s_corr));  % sampling frequency
fc = 2;                         % cutoff frequency in Hz (adjust as needed)
[b, a] = butter(7, fc / (fs/2)); 
de_filt = filtfilt(b, a, de_deg);


% plotting elevator deflection
f = figure;
p = plot(t_s_corr, de_deg); 
p.LineWidth = 1; 
grid on 
hold on 
p = plot(t_s_corr, de_filt);
p.LineWidth = 1; 

xlabel('t (sec)')
ylabel('def (deg)')

set(f, 'Position', [100, 100, 800, 400]);

% plotting response 
f1 = figure; 
subplot(3, 1, 1)
p1 = plot(t_s_corr, KTAS); 
p1.LineWidth = 1; 
grid on 
ylabel('KATS')

subplot(3, 1, 2)
p2 = plot(t_s_corr, theta_deg); 
p2.LineWidth = 1; 
grid on 
ylabel('\theta (deg)')

subplot(3, 1, 3)
p3 = plot(t_s_corr, h_m); 
p3.LineWidth = 1; 
grid on 
ylabel('h (m)')


%% Trim Data 

% SimData_Test = [t_s_corr, de_filt]; 
SimData_Test = [t_s_corr, de_deg]; 
% simData = SimData_Test(SimData_Test(:,1) >= 6.4 & SimData_Test(:,1) <= 14, :);
simData = SimData_Test; 

figure 
plot(simData(:,1), simData(:,2))

save('ElevatorTest.mat', 'simData')
