close all 
clear 
clc 

%% Load in Aircraft Parameters 
run('.\Aircraft Params\Cirrus_SR22T_rev2.m');
% run('.\Aircraft Params\Cirrus_SR22T.m');
load("Aircraft Params\SR22T.mat") 


fields = fieldnames(SR22T); % Get all field names
disp(fields)
for i = 1:length(fields)
    assignin('base', fields{i}, SR22T.(fields{i})); % Assign each field to a variable
end

M = M - 200; 
CL_zero = 0.0; 
dragCorr = 1.2;
CL_elev = CL_elev * 3.0; 



%% Set Initial States + Controls 
states_init = [0 -1227 4.78/57.3 157.50*1.852/3.6 0 0];  % initial conditions H=ft, Vx=kts
%             [x(m) y(m) theta(rad) Vx(m/s) Vy(m/s) q(rad/s)]

% set to zero for trimming
deltaE = 0; % in degrees + is pitch down 
throttle = 0; % currently not implemented 

% Maneuver set to zero deflection for trimming
% t_maneuver = 100;
t_maneuver = 0; 
p_maneuver = 1.5;
delta_maneuver = 1;
rate_maneuver = 7;
simData = [0 1000; 0 0]; % time vals 

elevator_offset = -2.49;
elevator_scale = 1.0;

% trimming
model = 'SIM_3DoF';
load_system(model);
opspec = operspec(model);

% Set elevator deflection to be solved
opspec.Inputs(1).u = 0;         % Free (to be solved)
opspec.Inputs(1).Known = false;
% Set throttle % as fixed.
opspec.Inputs(2).u = 0.39;      % Set fixed value of % power
opspec.Inputs(2).Known = true;
% Set altitude as fixed
% opspec.States.x(1) = -3000*0.3048;        % set the altitude value in meters
opspec.States.x(1) = -1227; 
opspec.States.Known(1) = true;            % fix it

opspec.States(1).SteadyState = [0 1 1 1 1 1];   % allow it to grow X state(1) distance

update(opspec);
[op_point, op_report] = findop(model, opspec) % Find trim condition

% set the initial states and inputs to the trim position
states_init = [0 op_point.States.x(2) op_point.States.x(3) op_point.States.x(4) 0 0];

deltaE = op_point.Inputs(1).u;
throttle = op_point.Inputs(2).u;

% Using Test Data
load('./Flight Test/TEST1/ElevatorTest.mat')
simData = [simData(:,1) + t_maneuver, (simData(:,2)*elevator_scale) + elevator_offset];


%% Open Simulink Model
% SIM_3DoF

%% 
% Ensure output folder exists
folder = 'Flight Test/SimData';
if ~exist(folder, 'dir')
    mkdir(folder)
end

% Run simulation and store output
simOut = sim('SIM_3DoF');  % Replace with your Simulink model name (no .slx)

% Extract logged signals and time from simOut
simV_kts = simOut.V_kts;   % Match to your "To Workspace" block name
simh_m = simOut.h_m;       % Match to your "To Workspace" block name
simtime = simOut.tout;     % Simulation time vector
simtheta_deg = simOut.theta_deg; 
simele_def_deg = simOut.ele_def; 

% Save all variables into .mat file
save(fullfile(folder, 'simSignalComp.mat'), 'simV_kts', 'simh_m', 'simtime', 'simtheta_deg', 'simele_def_deg')

%% 

data = readmatrix('Flight Test/TEST1/FlightData_1100_1200s.csv');
% t (secs) def (deg) alt (m) pitch (deg) KTAS

timeOffset = 0; 

t_s = data(:, 1);
de_deg = data(:, 2);
h_m  = data(:, 3);
theta_deg = data(:, 4);
KTAS = data(:, 5);

t_s_corr = t_s - t_s(1);

f1 = figure;

t2 = 20; 

subplot(4, 1, 1)
h1 = plot(t_s_corr, KTAS, 'LineWidth', 1);
hold on 
grid on 
h2 = plot(simtime - timeOffset, simV_kts, 'LineWidth', 1);
ylabel('KTAS')
xlim([0 t2])

subplot(4, 1, 2)
plot(t_s_corr, theta_deg, 'LineWidth', 1); 
hold on 
grid on 
plot(simtime - timeOffset, simtheta_deg, 'LineWidth', 1); 
ylabel('\theta (deg)')
xlim([0 t2])

subplot(4, 1, 3)
plot(t_s_corr, h_m, 'LineWidth', 1); 
hold on 
grid on 
plot(simtime - timeOffset, simh_m, 'LineWidth', 1)
ylabel('h (m)')
xlim([0 t2])

subplot(4, 1, 4)
plot(t_s_corr, de_deg, 'LineWidth', 1); 
hold on 
grid on 
plot(simtime - timeOffset, simele_def_deg, 'LineWidth', 1)
ylabel('def (deg)')
xlim([0 t2])
xlabel('Time (s)')

% Legend 
% Link all axes to ensure shared x-limits
linkaxes(findall(gcf,'type','axes'), 'x');

% Create global legend with first pair of handles
lgd = legend([h1, h2], {'Flight Test Data', 'Simulator'}, 'Orientation', 'horizontal');
lgd.Units = 'normalized';
lgd.Position = [0.35, 0.01, 0.3, 0.03];  % Adjust for bottom-center placement

set(f1, 'Position', [100, 100, 1000, 600]);


%% Collect all data used in the plots
data_struct = struct();
data_struct.t_s_corr = t_s_corr;
data_struct.KTAS = KTAS;
data_struct.theta_deg = theta_deg;
data_struct.h_m = h_m;
data_struct.de_deg = de_deg;

data_struct.simtime = simtime;
data_struct.simV_kts = simV_kts;
data_struct.simtheta_deg = simtheta_deg;
data_struct.simh_m = simh_m;
data_struct.simele_def_deg = simele_def_deg;
data_struct.timeOffset = timeOffset;
data_struct.t2 = t2;

% Save to .mat file
save('flight_sim_comparison.mat', '-struct', 'data_struct');