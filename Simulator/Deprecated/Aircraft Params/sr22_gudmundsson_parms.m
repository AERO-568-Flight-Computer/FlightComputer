%% SR22T Parameters Check 

close all 
clear 
clc 

addpath(genpath('C:\Users\diego\MATLAB\Tools'))

S = 144.9; % ft^2;
MTOW = 3400; % lbf 

CLmax = 1.41; % flaps up 
CLmax_f = 1.99; % w flaps 

CL0 = 0; 
CL_minD = 0.20; 
CDmin = 0.02541; 

AR = 10.12; 


CL = linspace(0, 1.4, 100); 

k = 0.04207; 
e = 1/(k *pi*AR);
k = 1/(pi*AR*e); 



CD = CDmin + k*(CL - CL_minD).^2; 

f = figure; 
p1 = plot(CD, CL); 
p1.LineWidth = 1.1; 

grid on
xlim([0 max(CD)])
ylim([0 1.5])

xlabel('C_D')
ylabel('C_L')

set(f, 'Position', [100, 100, 1200, 600]);
ax = gca;  % Get the current axes handle
ax.FontSize = 12;  % Set font size to 14 for both x and y axis ticks



H = 0; 
[T, a, P, rho] = atmosisa_imp(H); 

WS = MTOW/S
[LD, idx] = max(CL./CD)
V = sqrt((2*WS)/rho*CL(idx)) * (1/1.688) % ft2kts 

