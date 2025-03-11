%% Cirrus SR22T 

close all 
clear 
clc 

ft2m = 1/3.281; 
ft22m2 = 1/3.281^2;
in2m = 1/39.37; 
in22m2 = 1/39.37^2; 
lbin22kgm2 = 0.0002926397; 

%% 
% Cirrus SR22T Aircraft Data
% Source: 

% Powerplants: Continental IO-550


%% ---------------------------------------------------- %%

% Indication of whether the model will be used for
% simulation or determination of the equilibrium condition.
isTrimming = 0;

% Geometric data
% Wing area - m²
Sw = 144.9 * ft22m2;
% Wing span - m
Bw = 38.3 * ft2m;
% Mean wing chord - m
Cw = 4.03 * ft2m;
% Horizontal tail area - m²
Sh = (57.13) * ft22m2;
% Horizontal tail arm
xt = 215 * in2m;
% Wing-fuselage assembly arm
xa = 0.1002;


% % Moments of inertia and product of inertia
% % kg*m²
% Jxx =  1;
% Jyy =  1.2316;
% Jzz =  1;
% Jxz =  0;
J = 377.32 * lbin22kgm2;  % lb-in^2 

% Aircraft mass - kg
M = 1633;

% Aerodynamic data
% ----------------- LIFT -----------------------%

% CL when alpha = 0.
CL_zero = 0;

% CL due to alpha.
% Input: alpha (rad).
% Stall still needs to be included.
CL_alpha = ...
   	     [
          -1.57        -0.2 
          -0.35        -0.2 
          -0.30        -0.6 
          -0.25        -0.8 
          -0.018235950 0.0703
          -0.014475358 0.0837
          -0.009557253 0.1013
          -0.002937117 0.1250
          0.006289397 0.1582
          0.019714663 0.2067
          0.040357830 0.2813
          0.074525414 0.4051
          0.137262533 0.6329
          0.190465205 0.8267
          0.272189002 1.1252
          0.344199784 1.3891
          0.407080411 1.6203
          0.43        1.60
          0.5         1.2
          0.6         0.2  
          1.57        0.2  
          ];

% CL due to elevator deflection.
% Note: Multiplicative coefficient for elevator deflection (rad).
CL_elev = 1.8863;

CLEH_alpha = 4.6086;

down_wash = 0.1268; 

% 3-2-1-1
% increase time??? (proportional) 


% figure 
% % plot(rad2deg(CL_alpha(:, 1)), CL_alpha(:, 2))
% 
