% LIST OF BASIC SCENE DATA VARIABLES + DEFAULT VALUES

% Following variables must be read correctly, otherwise the script will be
% killed programatically 

dimensionX = -1.0;
dimensionY = -1.0;
toaToDistanceConversion = -1;
samplingPeriod = -1.0;
targetsCount = -1.0;
targetSpeeds = -1.0;

noise = 0;
distribution = 'NORMAL';
distributionParameter = 0.0325;
refractiveIndex = 1.00029;

% END OF BASIC SCENE DATA VARIABLES LIST


% Extraction of global information about scene saved in 'setup.txt'
fileID = fopen('setup.txt');

% Basic scene data are ended by '!' character
sceneDataLine = fgetl(fileID);
iterator = 1;

while sceneDataLine ~= '!'
    
    % Splitting string by delimiter '#' to get separate data
    splittedSceneData = strsplit(sceneDataLine, '#');
    
    % Get type of data and save in appropriate variable
    switch char(splittedSceneData(1))
        case 'SAMPLING_PERIOD'
            samplingPeriod = str2double(char(splittedSceneData(2)));
        case 'TOA_TO_DISTANCE_CONVERSION'
            toaToDistanceConversion = str2double(char(splittedSceneData(2)));
        case 'DIMENSIONS'
            dimensionX = str2double(char(splittedSceneData(2)));
            dimensionY = str2double(char(splittedSceneData(3)));
        case 'NOISE'
            noise = str2double(char(splittedSceneData(2)));
        case 'DISTRIBUTION'
            distribution = char(splittedSceneData(2));
            distributionParameter = str2double(char(splittedSceneData(3)));
        case 'REFRACTIVE_INDEX'
            refractiveIndex = str2double(char(splittedSceneData(2)));
        case 'TARGETS_COUNT'
            targetsCount = str2double(char(splittedSceneData(2)));
        case 'TARGET_SPEEDS'
            % targetsCount is initialized sooner then targetSpeeds
            for i = 2:(targetsCount+1)
            	targetSpeeds(i-1) = str2double(char(splittedSceneData(i)));
            end
        otherwise
            disp(['Could not recognize data specifier on row: ', num2str(iterator)]);
    end
    
    % Get next line
    sceneDataLine = fgetl(fileID);
    iterator = iterator + 1;
end

% Check if all important data have been correctly loaded
if dimensionX <= 0.0 || dimensionY <= 0.0 || samplingPeriod <= 0.0 || toaToDistanceConversion < 0.0
    % Quitting script
    error('Reading from file failed or values are incorrect');
end    

% Reading sensor positions
iterator = 1;
sceneDataLine = fgetl(fileID);

% Declaring empty sensor array (preallocating for 3 radars)
sensorList = zeros(3, 8);

while ischar(sceneDataLine)
    
    splittedSensorData = strsplit(sceneDataLine, '#');
    
    % In each line there should be 8 values to convert to number
    for i = 1:8
       sensorList(iterator, i) = str2double(char(splittedSensorData(i))); 
    end
    
    sceneDataLine = fgetl(fileID);
    iterator = iterator + 1;
end

% Do not forget to close file
fclose(fileID);

% Clearing all redundant variables
clearvars -except dimensionX dimensionY distribution distributionParameters noise refractiveIndex samplingPeriod toaToDistanceConversion sensorList targetsCount targetSpeeds

disp('Setup file was successfully parsed into your MATLAB script');
