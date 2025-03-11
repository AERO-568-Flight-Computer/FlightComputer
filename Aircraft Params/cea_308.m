% % CEA-308 Aircraft Data
% Source: Calculations and measurements carried out by the Center for Aeronautical Studies at UFMG 

% Powerplants:
% Jabiru 2200 - fixed-pitch propeller

%% ---------------------------------------------------- %%

% Indication of whether the model will be used for
% simulation or determination of the equilibrium condition.
isTrimming = 0;

% Geometric data
% Wing area - m²
Sw = 4.74;
% Wing span - m
Bw = 5.76;
% Mean wing chord - m
Cw = 0.84;
% Horizontal tail area - m²
Sh = 0.75;
% Horizontal tail arm
xt = 2.4697;
% Wing-fuselage assembly arm
xa = 0.1002;


% Moments of inertia and product of inertia
% kg*m²
Jxx =  1;
Jyy =  1.2316;
Jzz =  1;
Jxz =  0;

% Aircraft mass - kg
M = 300;

% Nominal and current CG position in the structural reference frame - m
Rcg_nom = [0.2964  0.0 0];
Rcg = [0.2964  0.0 0];

% Structural axis defined as:
%    x -> backward;
%    y -> to the right;
%    z -> upward.
Rcg_nom = Rcg_nom*[-1 0 0;0 1 0;0 0 -1];
Rcg = Rcg*[-1 0 0;0 1 0;0 0 -1];

% Engine position
% in (inches) -> m
Rmotor = [0 0.0 0];
Rmotor = Rmotor*[-1 0 0;0 1 0;0 0 -1];

% Maximum thrust estimate = Weight * Factor (g = 9.81m/s²)
Tmax = M*9.81*0.7;

% Aircraft control limits:
aileron_limits = 20*pi/180;
rudder_limits = 30*pi/180;
elevator_limits = 30*pi/180;
thrust_limits = 1;

% Initial condition for the aircraft at the airport. ;-)
x0 = zeros(12,1);
x0(4) = 70;
x0(3) = 1000;
y0 = zeros(12,1);
% Aerodynamic data

% ----------------- LIFT -----------------------%

% CL when alpha = 0.
CL_zero = 0.1356;

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
CL_elev = 2.6841;

CLEH_alpha = 3.7805;

down_wash = 0.4280;

% ----------------- DRAG -----------------------%

% CD due to alpha.
% Input: alpha (rad).
% Values do not include parasitic drag.
CD_alpha = ...
	     [-0.018235950 0.01387
          -0.014475358 0.01407
          -0.009557253 0.01430
          -0.002937117 0.01461
          0.006289397 0.01526
          0.019714663 0.01645
          0.040357830 0.01882
          0.074525414 0.02392
          0.137262533 0.04051
          0.190465205 0.06192
          0.272189002 0.10641
          0.344199784 0.18303
          0.407080411 0.28762
          ];
      

% ----------------- Pitch -----------------------%

% Cm for alpha = 0.
Cm_zero = -0.0594;

% Cm due to elevator deflection.
% Input: alpha (rad).
% Note: Coefficient multiplied by elevator deflection.
Cm_elev = ...
	[-0.35   -0.5
     -0.27   -0.8
     0.0000  -1.1903
     0.25    -1.1902
     0.30    20];

% Aircraft
alfa_polar   = [23.3240	19.7212	15.5953	10.9129	7.8646	4.2700	2.3123	1.1296	0.3604	-0.1683	-0.5476	-0.8294	-1.0448];
cl_polar     = [1.6203	1.3891	1.1252	0.8267	0.6329	0.4051	0.2813	0.2067	0.1582	0.1250	0.1013	0.0837	0.0703];
cd_polar     = [0.28762	0.18303	0.10641	0.06192	0.04051	0.02392	0.01882	0.01645	0.01526	0.01461	0.01430	0.01407	0.01387];
% Engine
rpm      = [1700 1900 2100 2300 2500 2700 2900 3100 3300];
pwr_hp   = [40   47.5 57   66   73   76   80   83   85  ]; 
pwr_kw    = 745.69987 * pwr_hp / 1000;
% Propeller
% v/nD	Cp	Ct	?	?*	Loss	v	n	Power	Thrust
% [-]	[-]	[-]	[%]	[%]	[%]	[m/s]	[1/min]	[kW]	[N]
D = 1.397;
helice = [... 
0.000	0.070465	0.058040	0.00	0.00	100.00 	0.00	1700	10.413	216.7
0.100	0.082704	0.062406	7.55	38.82	100.00 	3.96	1700	12.221	233.0
0.200	0.080052	0.063204	15.79	61.20	100.00	7.92	1700	11.829	236.0
0.300	0.077883	0.062664	24.14	74.43	100.00	11.87	1700	11.509	233.9
0.400	0.075010	0.061426	32.76	82.46	100.00	15.83	1700	11.084	229.3
0.500	0.072015	0.059754	41.49	87.49	100.00	19.79	1700	10.642	223.1
0.600	0.069693	0.057855	49.81	90.76	100.00	23.75	1700	10.299	216.0
0.700	0.067804	0.056011	57.83	92.95	86.00	27.71	1700	10.020	209.1
0.800	0.066748	0.055097	66.04	94.37	60.00	31.67	1700	9.863	205.7
0.900	0.067073	0.055134	73.98	95.32	0.00	35.62	1700	9.912	205.8
1.000	0.067762	0.054186	79.97	96.09	0.00	39.58	1700	10.013	202.3
1.100	0.067389	0.051923	84.76	96.76	0.00	43.54	1700	9.958	193.8
1.200	0.065259	0.047791	87.88	97.37	0.00	47.50	1700	9.643	178.4
1.300	0.055879	0.038316	89.14	98.11	0.00	51.46	1700	8.257	143.0
1.400	0.044850	0.028703	89.60	98.72	0.00	55.41	1700	6.628	107.2
1.500	0.032127	0.018947	88.46	99.23	0.00	59.37	1700	4.747	70.7
1.520	0.029377	0.016979	87.85	99.32	0.00	60.16	1700	4.341	63.4
1.540	0.026552	0.015003	87.02	99.41	0.00	60.96	1700	3.924	56.0
1.560	0.023660	0.013023	85.87	99.49	0.00	61.75	1700	3.496	48.6
1.580	0.020697	0.011037	84.26	99.58	0.00	62.54	1700	3.058	41.2
1.600	0.017663	0.009045	81.93	99.66	0.00	63.33	1700	2.610	33.8
1.620	0.014558	0.007047	78.42	99.74	0.00	64.12	1700	2.151	26.3
1.640	0.011382	0.005045	72.69	99.82	0.00	64.91	1700	1.682	18.8
1.660	0.008133	0.003035	61.95	99.89	0.00	65.71	1700	1.202	11.3
1.680	0.004748	0.000983	34.78	99.97	0.00	66.50	1700	0.702	3.7
1.700	0.001300	-0.001067	-139.54	99.99	0.00	67.29	1700	0.192	-4.0
];



% Get all variables in the workspace
vars = whos;

% Create an empty struct
cea308 = struct();

% Loop through variables and add them to the struct
for i = 1:length(vars)
    cea308.(vars(i).name) = eval(vars(i).name);
end


save('cea308.mat', 'cea308');




