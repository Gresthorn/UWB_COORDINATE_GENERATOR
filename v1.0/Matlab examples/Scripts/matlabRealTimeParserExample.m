% Following example will parse data from 'Radar 1.txt' and plot all routes
% in file with specified time frequency

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

% Preparing plot and preallocating arrays
preallocationBlock = 500;
preallocationIterations = 1;
x = zeros(targetsCount, preallocationBlock);
y = zeros(targetsCount, preallocationBlock);
plotHandler = zeros(targetsCount);

% Create handlers for individual targets
figure(1); clf;

% Create color map
cmap = hsv(targetsCount);
for j = 1:targetsCount
    plotHandler(j) = plot(x(j), y(j), 'o', 'MarkerSize', 5, 'Color', cmap(j,:));
    axis([-10.0, 10.0, -5.0, 25.0]);
    hold on;
end

grid on;

iterator = 1;
% History is used to draw most 'max_history' recent positions of targets
history = 0;
max_history = 10;

% In this case sampling period is 0.0769. See matlabSetupParserExample
% how to get basic setup data from file
sampling_period = 0.0769;

while(ischar(infoLine))
    
    for i = 1:targetsCount
        
        if iterator == preallocationBlock*preallocationIterations
            % Need to preallocate new block of data for faster script
            x(i) = zeros(iterator+1, preallocationBlock*preallocationIterations);
            y(i) = zeros(iterator+1, preallocationBlock*preallocationIterations);
            preallocationIterations = preallocationIterations + 1;
        end
        
        % Add new value to array
        x(i, iterator) = str2double(char(infoLineSplitted(3+i*4-3)));
        y(i, iterator) = str2double(char(infoLineSplitted(3+i*4-2)));
    end
    
    % Refreshing plot with new values
    for a = 1:targetsCount
        set(plotHandler(a), 'X', x(a, (iterator-history):iterator), 'Y', y(a, (iterator-history):iterator));     
    end
    
    drawnow;
    
    % Wait for another value
    pause(sampling_period);
    
    % Inkrementing iterator
    iterator = iterator + 1;
    
    % Incrementing history until the max value is reached
    if history < max_history
        history = history + 1;
    end
    
    % Reading new line
    infoLine = fgetl(fileID);
    if ischar(infoLine) 
        infoLineSplitted = strsplit(infoLine, '#');
    end
end

% Closing file and freeing memory
fclose(fileID);

clear;
