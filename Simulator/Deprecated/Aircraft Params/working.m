%% 
close all 
clear 
clc

load("CD_total.mat")
load("CL_Final_integer.mat")

S = 144.9; % ft^2;
MTOW = 3600; % lbf 

addpath(genpath('C:\Users\diego\MATLAB\Tools'))

CD_real = real(CD_total);
CL_real = real(CL_Final_integer);

CL_fit = polyfit(1:length(CL_real), CL_real, 6); % 5th order polynomial
CD_fit = polyfit(1:length(CD_real), CD_real, 6);

CL_smooth = smooth(CL_real, 0.3, 'loess');  % 10% window
CD_smooth = smooth(CD_real, 0.3, 'loess');

figure 
hold on 
plot(CD_total, CL_Final_integer)
plot(CD_smooth, CL_smooth, LineWidth = 1)

[LD, idx] = max(CL_smooth./CD_smooth)

CL = CL_smooth(idx)

H = 1000; 
[T, a, P, rho] = atmosisa_imp(H);

WS = MTOW/S;
V = sqrt((2*WS)/(rho*CL)) * (1/1.688);


VTAS = V*sqrt(rho/0.00238)