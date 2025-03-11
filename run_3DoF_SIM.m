close all 
clear 
clc 

%% Load in Aircraft Parameters 
load("Aircraft Params\cea308.mat") 
J = 5; 
gravity = 9.807; 


fields = fieldnames(cea308); % Get all field names
disp(fields)
for i = 1:length(fields)
    assignin('base', fields{i}, cea308.(fields{i})); % Assign each field to a variable
end


%% Set Initial States + Controls 
states_init = [0 5000 -0.02056 23.3505 1.0289 0]; 
deltaE = -10; 
throttle = 0.65; 




%% Open Simulink Model
Takeoff_Model

