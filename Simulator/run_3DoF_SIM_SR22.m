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


%% Set Initial States + Controls 
states_init = [0 -2887*0.3048 0.3057/57.3 157.50*1.852/3.6 0 0];  % initial conditions H=ft, Vx=kts
%             [x(m) y(m) theta(rad) Vx(m/s) Vy(m/s) q(rad/s)]

% CD0 = 0.00; % not necessary for SR22T (Parastic Drag included in Drag Data) 

% set to zero for trimming
deltaE = 0; % in degrees + is pitch down 
throttle = 0; % currently not implemented 

% Maneuver set to zero deflection for trimming
t_maneuver = 100;
p_maneuver = 1.5;
delta_maneuver = 0;
rate_maneuver = 7;

% import elevator movement from test flight data  - see
% extract_flight_test_elevator.m script - THis is a unitary deflection
% maneuver with the same time profile of the flight test.
load("elevator.mat");
simData = [t_seconds_zeroed+t_maneuver, ADC0_centered_normal*delta_maneuver]; % set SimData variable for FROMWORKSPACE block.

% trimming
model = 'SIM_3DoF';
load_system(model);
opspec = operspec(model);

% Set elevator deflection to be solved
opspec.Inputs(1).u = 0;         % Free (to be solved)
opspec.Inputs(1).Known = false;
% Set throttle % as fixed.
opspec.Inputs(2).u = 0.65;      % Set fixed value of % power
opspec.Inputs(2).Known = true;
% Set altitude as fixed
opspec.States.x(1) = -3000*0.3048;        % set the altitude value in meters
opspec.States.Known(1) = true;            % fix it

opspec.States(1).SteadyState = [0 1 1 1 1 1];   % allow it to grow X state(1) distance

update(opspec);
[op_point, op_report] = findop(model, opspec) % Find trim condition

% set the initial states and inputs to the trim position
states_init = [0 op_point.States.x(2) op_point.States.x(3) op_point.States.x(4) 0 0];

deltaE = op_point.Inputs(1).u;
throttle = op_point.Inputs(2).u;

% Maneuver
delta_maneuver = -5;
simData = [t_seconds_zeroed+t_maneuver, ADC0_centered_normal*delta_maneuver];

%% Open Simulink Model
SIM_3DoF

