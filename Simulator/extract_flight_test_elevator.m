close all
clear all
clc

%% Set up the Import Options and import the data
opts = delimitedTextImportOptions("NumVariables", 40);

% Specify range and delimiter
opts.DataLines = [2, Inf];
opts.Delimiter = ",";

% Specify column names and types
opts.VariableNames = ["YAW", "PITCH", "ROLL", "GX", "GY", "GZ", "LAT", "LONG", "ALT", "VX", "VY", "VZ", "AX", "AY", "AZ", "YEAR", "MONTH", "DAY", "HOUR", "MINUTE", "SECOND", "MS", "GNSS1SAT", "GNSS1FIX", "YAW_U", "PITCH_U", "ROLL_U", "INS_STATUS", "POS_U", "VEL_U", "GNSS2SAT", "GNSS2FIX", "ADC0", "ADC1", "ADC2", "ADC3", "ADC4", "ADC5", "ADC6", "ADC7"];
opts.VariableTypes = ["double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double"];

% Specify file level properties
opts.ExtraColumnsRule = "ignore";
opts.EmptyLineRule = "read";

% Import the data
Cirrus568 = readtable("Cirrus_568.csv", opts);

%% Clear temporary variables
clear opts

%% Convert date info
year=Cirrus568.YEAR;
month=Cirrus568.MONTH;
day=Cirrus568.DAY;
hour=Cirrus568.HOUR;
minute=Cirrus568.MINUTE;
second=Cirrus568.SECOND;
ms=Cirrus568.MS;
t = datetime(year, month, day, hour, minute, second + ms/1000);
t_seconds = seconds(t-t(1));

%% Filter ADC0 data
Fs = 100;
Fc = 5;
[b, a] = butter(4, Fc/(Fs/2));
ADC0_filt = filtfilt(b, a, Cirrus568.ADC0);

%% Find start and end index of the elevator maneuver.
i = 1333.5*100;
f = 1343*100;
% intial and final ADC0 values during maneuver.
ADC0_i=ADC0_filt(i);
ADC0_f=ADC0_filt(f);
% Center ADC_0 around zero and remove bias
ADC0_centered = (ADC0_filt(i:f)-ADC0_i)-((ADC0_f-ADC0_i)*(t_seconds(i:f)-t_seconds(i))/(t_seconds(f)-t_seconds(i)));
% normalize ADC_0 to -1 to 1
ref = max(abs(max(ADC0_centered)),abs(min(ADC0_centered)));
ADC0_centered_normal = ADC0_centered/ref;
% sero time vector
t_seconds_zeroed = t_seconds(i:f)-t_seconds(i);
plot(t_seconds_zeroed,ADC0_centered_normal)

% plot maneuver velocity and altitude profiles 
i = 1323.5*100;
f = 1443*100;
figure(2)
subplot(2,1,1)
plot(t_seconds(i:f),((Cirrus568.VX(i:f).^2+Cirrus568.VY(i:f).^2+Cirrus568.VZ(i:f).^2).^0.5)*3.6/1.852,'LineWidth',2)
grid on
ylabel("V[kts]")
subplot(2,1,2)
plot(t_seconds(i:f),Cirrus568.ALT(i:f),'LineWidth',2)
grid on
ylabel("h[m]")

V_kts = ((Cirrus568.VX(i:f).^2+Cirrus568.VY(i:f).^2+Cirrus568.VZ(i:f).^2).^0.5)*3.6/1.852; 
t_secs = t_seconds(i:f); 
h_m = Cirrus568.ALT(i:f); 

% Create folder if it doesn't exist
folder_name = 'Flight Test\Data';
if ~exist(folder_name, 'dir')
    mkdir(folder_name)
end

% Save data
file_path = fullfile(folder_name, 'maneuver_profile_data.mat');
save(file_path, 't_secs', 'V_kts', 'h_m')