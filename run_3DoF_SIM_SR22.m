close all 
clear 
clc 

%% Load in Aircraft Parameters 
load("Aircraft Params\SR22T.mat") 
% J = 5; 
gravity = 9.807; 


fields = fieldnames(SR22T); % Get all field names
disp(fields)
for i = 1:length(fields)
    assignin('base', fields{i}, SR22T.(fields{i})); % Assign each field to a variable
end


%% Set Initial States + Controls 
states_init = [0 5000 0 70 0 0]; 
deltaE = 4;  % + is pitch down 
throttle = 0.65; 




%% Open Simulink Model
SIM_3DoF

