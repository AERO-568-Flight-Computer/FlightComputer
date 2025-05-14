close all 
clear 
clc 

addpath('SimData')
load("simSignal.mat")

data = readmatrix('./TEST1/FlightData_1100_1200s.csv');
% t (secs) def (deg) alt (m) pitch (deg) KTAS

timeOffset = 0; 

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
plot(t_s_corr, de_deg, LineWidth=1); 
grid on 
hold on 
plot(t_s_corr, de_filt, LineWidth=1);
xlabel('t (sec)')
ylabel('def (deg)')

set(f, 'Position', [100, 100, 800, 400]);

f1 = figure;

subplot(4, 1, 1)
h1 = plot(t_s_corr, KTAS, 'LineWidth', 1);
hold on 
grid on 
h2 = plot(simtime - timeOffset, simV_kts, 'LineWidth', 1);
ylabel('KTAS')
xlim([0 100])

subplot(4, 1, 2)
plot(t_s_corr, theta_deg, 'LineWidth', 1); 
hold on 
grid on 
plot(simtime - timeOffset, simtheta_deg, 'LineWidth', 1); 
ylabel('\theta (deg)')
xlim([0 100])

subplot(4, 1, 3)
plot(t_s_corr, h_m, 'LineWidth', 1); 
hold on 
grid on 
plot(simtime - timeOffset, simh_m, 'LineWidth', 1)
ylabel('h (m)')
xlim([0 100])

subplot(4, 1, 4)
plot(t_s_corr, de_deg, 'LineWidth', 1); 
hold on 
grid on 
plot(simtime - timeOffset, simele_def_deg, 'LineWidth', 1)
ylabel('def (deg)')
xlim([0 100])
xlabel('Time (s)')

% Legend 
% Link all axes to ensure shared x-limits
linkaxes(findall(gcf,'type','axes'), 'x');

% Create global legend with first pair of handles
lgd = legend([h1, h2], {'Flight Test Data', 'Simulator'}, 'Orientation', 'horizontal');
lgd.Units = 'normalized';
lgd.Position = [0.35, 0.01, 0.3, 0.03];  % Adjust for bottom-center placement

set(f1, 'Position', [100, 100, 1000, 600]);



% Pattern Search - MATLAB Optimization 

% Power - Speed 
% Elevator - Alt 

% Inverts for Landing 

% de -> theta -> h






