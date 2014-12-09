% Following example will parse data from 'Radar 1.txt' and plot the route

% Opening file
fileID = fopen('Radar 1.txt');

% Load all data into one universal matrix (all data in one matrix)
globalMatrix = importdata('Radar 1.txt', '#');

% Getting one line to find out radar number and number of visible targets
infoLine = fgetl(fileID);
infoLineSplitted = strsplit(infoLine, '#');

% Save radarID and targetsCount
radarID = str2double(char(infoLineSplitted(1)));
targetsCount = str2double(char(infoLineSplitted(3)));

% Choosing the target's route to plot (in this case from 1 to 4)
targetIDoption = 1;

% Checking for correctness of targetID
targetID = round(targetIDoption);
if targetID < 1 || targetID > targetsCount
    error('Illegal targetID value selected');
end

% Each target has X - column, Y - column, TOA - left, TOA - right and first
% three columns are radar number, time and number of visible targets
targetX = globalMatrix(:,3+targetID*4-3);
targetY = globalMatrix(:,3+targetID*4-2); 

% Closing file
fclose(fileID);

% Deleting all redundant variables
clearvars -except targetX targetY radarID targetsCount targetID

% Plotting results, in our example dimensions of monitored area are 15x20,
% seematlabSetupParserExample how to get these dimensions from setup.txt
delta = 5.0;
areaWidth = 15.0 + delta;
areaHeight = 25.0 + delta/2.0;
titleString = strcat('Results for Radar : ', num2str(radarID), '; Target number : ', num2str(targetID));

plot(targetX, targetY);
axis([(-1.0)*areaWidth/2.0, areaWidth/2.0, (-1.0)*delta/2.0, areaHeight]);
title(titleString);
