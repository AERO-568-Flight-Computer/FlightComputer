close all 
clear 
clc 

addpath('SimData')
load("simSignalComp.mat")

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


%% Pitch Oscillation Analysis (Flight Test & Simulator)

pD = 20; 
pP = 1.0; 

% Flight Test 
[~, locs_max] = findpeaks(theta_deg, t_s_corr, 'MinPeakProminence', pP, 'MinPeakDistance', pD);

if numel(locs_max) >= 2
    T_test = locs_max(2) - locs_max(1);
    f_test = 1 / T_test;

    idx1 = find(t_s_corr >= locs_max(1), 1, 'first');
    idx2 = find(t_s_corr >= locs_max(2), 1, 'first');

    amp_test = (max(theta_deg(idx1:idx2)) - min(theta_deg(idx1:idx2))) / 2;
else
    f_test = NaN;
    amp_test = NaN;
end

% Simulator 
[~, sim_locs_max] = findpeaks(simtheta_deg, simtime, 'MinPeakProminence', pP, 'MinPeakDistance', pD);

if numel(sim_locs_max) >= 2
    T_sim = sim_locs_max(2) - sim_locs_max(1);
    f_sim = 1 / T_sim;

    idx1_sim = find(simtime >= sim_locs_max(1), 1, 'first');
    idx2_sim = find(simtime >= sim_locs_max(2), 1, 'first');

    amp_sim = (max(simtheta_deg(idx1_sim:idx2_sim)) - min(simtheta_deg(idx1_sim:idx2_sim))) / 2;
else
    f_sim = NaN;
    amp_sim = NaN;
end

% Print Results 
fprintf('Flight Test:   f = %.3f Hz, Amplitude = %.2f deg\n', f_test, amp_test);
fprintf('Simulator:     f = %.3f Hz, Amplitude = %.2f deg\n', f_sim, amp_sim);


% Recompute Maxima Only 
[pk_max, locs_max] = findpeaks(theta_deg, t_s_corr, 'MinPeakProminence', pP, 'MinPeakDistance', pD);
[sim_pk_max, sim_locs_max] = findpeaks(simtheta_deg, simtime, 'MinPeakProminence', pP, 'MinPeakDistance', pD);

% Plot Maxima on Pitch Subplot 
subplot(4, 1, 2)
hold on
plot(locs_max, pk_max, 'ro', 'MarkerSize', 6, 'DisplayName', 'FT Maxima');
plot(sim_locs_max - timeOffset, sim_pk_max, 'r*', 'MarkerSize', 6, 'DisplayName', 'Sim Maxima');




% Pattern Search - MATLAB Optimization 

% Power - Speed 
% Elevator - Alt 

% Inverts for Landing 

% de -> theta -> h
