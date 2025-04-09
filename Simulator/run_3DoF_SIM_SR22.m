close all 
clear 
clc 

%% Load in Aircraft Parameters 
load("Aircraft Params\SR22T.mat") 
% J = 5; 
J  = 376.7821; 
gravity = 9.807; 


fields = fieldnames(SR22T); % Get all field names
disp(fields)
for i = 1:length(fields)
    assignin('base', fields{i}, SR22T.(fields{i})); % Assign each field to a variable
end


%% Set Initial States + Controls 
states_init = [0 5000 0 90 0 0];  % initial conditions 
%             [x(m) y(m) theta(rad) Vx(m/s) Vy(m/s) q(rad/s)]

CD0 = 0.00; % not necessary for SR22T (Parastic Drag included in Drag Data) 

deltaE = 25; % in degrees + is pitch down 
throttle = 0.65; % currently not implemented 

%% Open Simulink Model
SIM_3DoF

