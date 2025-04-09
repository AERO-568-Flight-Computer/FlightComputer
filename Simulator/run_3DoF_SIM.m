close all 
clear 
clc 
%% Load in Aircraft Parameters 
load("Aircraft Params\cea308.mat") % This is for the CEA 308 aircraft
% J = 300; % Pitching Moment of Inertia 
J = 30; 
gravity = 9.807; % gravitational constant

fields = fieldnames(cea308); % Get all field names
disp(fields)
for i = 1:length(fields)
    assignin('base', fields{i}, cea308.(fields{i})); % Assign each field to a variable
end

%% Set Initial States + Controls 
states_init = [0 5000 0 40 0 0];  % initial conditions 
%             [x(m) y(m) theta(rad) Vx(m/s) Vy(m/s) q(rad/s)]

CD0 = 0.00; % Parasitic Drag not included for CEA 308 

deltaE = -10; % in degrees - is pitch down 
throttle = 0.65; % currently not implemented 

%% Open Simulink Model
% This should automatically open the simulation 
SIM_3DoF
