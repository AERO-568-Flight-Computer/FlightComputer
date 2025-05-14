close all 
clear 
clc 

% Unit Conversions
ft2m = 1/3.281; 
ft22m2 = 1/3.281^2;
in2m = 1/39.37; 
in22m2 = 1/39.37^2; 
lbin22kgm2 = 0.0002926397; 
lb2kg = 1/2.205; 

%%  Aircraft Geometric Data 
% Wing area - m²
Sw = 145.16 * ft22m2;
% Wing span - m
Bw = 38.3 * ft2m;
% Fuselage lenght - m
L = 26 * ft2m;
% Mean wing chord - m
Cw = 3.97 * ft2m;
% Horizontal tail area - m²
Sh = 28.34 * ft22m2;
% Horizontal tail arm
xt = 13.99 * ft2m;
% Wing-fuselage assembly arm
xa = 0.26 * ft2m; % 0.16 - 0.26 
% Aircraft mass - kg
M = 3600 * lb2kg;
% Airplane raddi of gyration
Ry = 0.38;
% Airplane Inertial - kg*m^2
J  = (L^2*M*Ry^2)/4; 
% Gravity acceleration - m/s^2
gravity = 9.807; 

%% Lift 
% CL when alpha = 0.
CL_zero = 0.25; % ?? unsure of this val 

% CL due to alpha.
% Input: alpha (rad).
% Stall still needs to be included.
CL_alpha = 5.16;

% CL due to elevator deflection.
% Note: Multiplicative coefficient for elevator deflection (rad).
CL_elev = 1.136;

CLEH_alpha = 4.52;

down_wash = 0.2935; 
%% Drag 
load('Cirrus Data\CD_total.mat')
load('Cirrus Data\CL_total.mat')

% extract Polar Data 
CL_Polar = real(CL_Final_integer);
CD_Polar = real(CD_total);


% figure 
% plot(CD_Polar, CL_Polar)

% applying smoothing, (polar data is not monoto
windowSize = 11; % Try 3–11 depending on how smooth you want
CL_smooth = movmean(CL_Polar, windowSize);
CD_smooth = movmean(CD_Polar, windowSize);

% figure 
% plot(CD_smooth, CL_smooth)

CL_neg = -CL_smooth; 

% Build other half of drag polar 
CL = [CL_neg, flip(CL_smooth)];
CD = [CD_smooth, flip(CD_smooth)];


% mapping CL to alpha 
alpha_CL = (CL+CL_zero)./CL_alpha;

% Store CD(alpha) as a 2-column matrix
CD_alpha = [alpha_CL', 1.2*CD']

% figure 
% plot(rad2deg(CD_alpha(:,1)), CD_alpha(:,2))
% ylabel('CD')
% xlabel('alpha (deg)')


%% Moment 
Cm_zero = -0.0524;

%% Save Vars 
% Get all variables in the workspace
vars = whos;

% Create an empty struct
SR22T = struct();

% Loop through variables and add them to the struct
for i = 1:length(vars)
    SR22T.(vars(i).name) = eval(vars(i).name);
end


save('SR22T.mat', 'SR22T');
