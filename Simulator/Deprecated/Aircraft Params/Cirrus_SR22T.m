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
xa = 0.5;


% % Moments of inertia and product of inertia
% % kg*m²
% Jxx =  1;
% Jyy =  1.2316;
% Jzz =  1;
% Jxz =  0;
% J = 377.32 * lbin22kgm2;  % lb-in^2  
Ry = 0.38; % Raymer 
L = 25.98 * ft2m; 
J = (L^2 * (1633/9.81) * Ry^2)/(4);% Raymer 

% Aircraft mass - kg
M = 1633;

% Aerodynamic data
% ----------------- LIFT -----------------------%

% CL when alpha = 0.
CL_zero = 0.27; % ?? unsure of this val 

% CL due to alpha.
% Input: alpha (rad).
% Stall still needs to be included.

load('Cirrus Data\CLWBTable.mat')
CL_alpha = [CLWBTable(:,2), CLWBTable(:,1)]

figure 
plot(rad2deg(CLWBTable(:,2)), CLWBTable(:,1)) % this works 
ylabel('CL')
xlabel('alpha (deg)')


% CL due to elevator deflection.
% Note: Multiplicative coefficient for elevator deflection (rad).
CL_elev = 1.8863;

CLEH_alpha = 4.6086;

down_wash = 0.1268; 

% CD due to alpha.
% Input: alpha (rad).
% Values do not include parasitic drag.

load('Cirrus Data\CD_total.mat')
load('Cirrus Data\CL_total.mat')

CL_Polar = real(CL_Final_integer);
CD_Polar = real(CD_total);

% Create mirrored values
CL_Mirror = -flip(CL_Polar);
CD_Mirror = flip(CD_Polar);

% Insert NaN row to break the connection
CL_Polar = [CL_Mirror, CL_Polar]; 
CD_Polar = [CD_Mirror, CD_Polar]; 

% figure 
% plot(CD_Polar, CL_Polar)
% xlabel('CD')
% ylabel('CL')

% Extracting CL as a function of alpha
alpha_CL = CLWBTable(:,2); % alpha in radians
CL_alpha2 = CLWBTable(:,1); % Corresponding CL values

% Interpolating CD using available CL data
CL_query = interp1(alpha_CL, CL_alpha2, alpha_CL, 'linear', 'extrap'); % CL at given alpha
CD_query = interp1(CL_Polar, CD_Polar, CL_query, 'linear', 'extrap'); % Interpolating CD for those CL values

% Store CD(alpha) as a 2-column matrix
CD_alpha = [alpha_CL, CD_query]

figure 
plot(rad2deg(CD_alpha(:,1)), CD_alpha(:,2))
ylabel('CD')
xlabel('alpha (deg)')


%% TODO 
% ----------------- Pitch -----------------------%

% Cm for alpha = 0.
Cm_zero = -0.0623;

% Cm due to elevator deflection.
% Input: alpha (rad).
% Note: Coefficient multiplied by elevator deflection.
Cm_elev = ...
	[-0.35   -0.5
     -0.27   -0.8
     0.0000  -1.1903
     0.25    -1.1902];
     % 0.30    20];

de = deg2rad(linspace(-20, 20));
Cmde = 0.00585; % 0.00585
Cme = - de .* Cmde; 

figure 
plot(rad2deg(de), Cme)


hold on
plot(rad2deg(Cm_elev(:,1)), Cm_elev(:,2))

Cm_elev = [de(:), Cme(:)]; % Ensures column format


% Get all variables in the workspace
vars = whos;

% Create an empty struct
SR22T = struct();

% Loop through variables and add them to the struct
for i = 1:length(vars)
    SR22T.(vars(i).name) = eval(vars(i).name);
end


save('SR22T.mat', 'SR22T');