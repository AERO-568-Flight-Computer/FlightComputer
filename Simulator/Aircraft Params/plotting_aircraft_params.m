close all 
clear 
clc 

%% Load Aircraft File 
load('cea308.mat')

% Drag Polar 
f1 = figure; 
p1 = plot(cea308.cd_polar, cea308.cl_polar);  
p1.LineWidth = 1.1; 
p1.Marker = 'o'; 
grid on 
xlabel('C_D')
ylabel('C_L', rotation = 0)


% CL - alpha 
f2 = figure; 
p2 = plot(cea308.alfa_polar, cea308.cl_polar);  
p2.LineWidth = 1.1; 
p2.Marker = 'o'; 
grid on 
xlabel('\alpha (deg)')
ylabel('C_L', rotation = 0)


% CD - alpha 
f3 = figure; 
p3 = plot(cea308.alfa_polar, cea308.cd_polar);  
p3.LineWidth = 1.1; 
p3.Marker = 'o'; 
grid on 
xlabel('\alpha (deg)')
ylabel('C_D', rotation = 0)

disp(cea308.pwr_kw)