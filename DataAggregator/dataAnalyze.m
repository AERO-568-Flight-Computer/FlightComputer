clear; close all; clc;


% Load data
internalData = readmatrix("internalData_name3.csv");
externalData = readmatrix("externalData_name3.csv");

externalData(:,1) = externalData(:,1) - internalData(1,1);
internalData(:,1) = internalData(:,1) - internalData(1,1);

% Plot 100 points of data, increasing the starting index by 100 each time

startIndex = 101;
endIndex = 300;

while endIndex <= length(internalData)
    % Plot internal data
    figure(1)
    internal2Plot1 = internalData(startIndex:endIndex, 1) - internalData(startIndex,1);
    internal2Plot2 = internalData(startIndex:endIndex, 2);
    plot(internal2Plot1, internal2Plot2, 'r', "Marker",".");
    hold on
    externalData2Plot = externalData(externalData(:, 1) >= internalData(startIndex, 1), :);
    joe1 = externalData2Plot(1:abs(startIndex-endIndex), 1) - internalData(startIndex,1);
    joe2 = externalData2Plot(1:abs(startIndex-endIndex), 2);
    plot(joe1, joe2, 'b', "Marker",".");
    title("Internal and External Data");
    xlabel("Time (s)");
    ylabel("Data");
    legend("Internal Data", "External Data");
    ax = gca;
    ax.XAxis.Exponent = 0;
%     ylim([-1,1])
    xlim([0,0.3])

    hold off;
    
    startIndex = startIndex + 20;
    endIndex = endIndex + 20;
end
