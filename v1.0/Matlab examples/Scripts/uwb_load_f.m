function [targets, sensorList, sceneSetup] = uwb_load_f(file_array)

    files_count = numel(file_array);
    
    % Checking if all files specified exists
    
    for i = (1:files_count)
        name = char(file_array(i));
        if (exist(name, 'file')==0)
            % If file does not exist, program will exit with failure
            error(strcat('File:', char(file_array(i)), ' was not found.'));
        end;
    end;
    
    % Check if OPERATOR.txt and setup.txt exists in working direcotry
    
    if (exist('OPERATOR.txt', 'file')==0 || exist('setup.txt', 'file')==0)
        error('OPERATOR or setup text file is missing from working directory');
    end;
    
    % Go through all files and load data into 3 dimensional targets array
    
    for i = (1:(files_count+1))
        
        if i == 1
            % Load OPERATOR.txt at index 1
            fileID = fopen('OPERATOR.txt');
            
            targets(:,:,i) = importdata('OPERATOR.txt', '#');
            
            fclose(fileID); 
        else    
            % Opening file
            fileID = fopen(char(file_array(i-1)));

            % Load all data into one universal matrix (all data in one matrix)
            targets(:,:,i) = importdata(char(file_array(i-1)), '#'); 

            % Closing file
            fclose(fileID);    
        end;    
    end;
    
    % Following variables must be read correctly, otherwise the script will be
    % killed programatically 

    sceneSetup.dimensionX = -1.0;
    sceneSetup.dimensionY = -1.0;
    sceneSetup.toaToDistanceConversion = -1;
    sceneSetup.samplingPeriod = -1.0;
    sceneSetup.targetsCount = -1.0;
    sceneSetup.targetSpeeds = -1.0;

    sceneSetup.noise = 0;
    sceneSetup.distribution = 'NORMAL';
    sceneSetup.distributionParameter = 0.0325;
    sceneSetup.refractiveIndex = 1.00029;

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
                sceneSetup.samplingPeriod = str2double(char(splittedSceneData(2)));
            case 'TOA_TO_DISTANCE_CONVERSION'
                sceneSetup.toaToDistanceConversion = str2double(char(splittedSceneData(2)));
            case 'DIMENSIONS'
                sceneSetup.dimensionX = str2double(char(splittedSceneData(2)));
                sceneSetup.dimensionY = str2double(char(splittedSceneData(3)));
            case 'NOISE'
                sceneSetup.noise = str2double(char(splittedSceneData(2)));
            case 'DISTRIBUTION'
                sceneSetup.distribution = char(splittedSceneData(2));
                sceneSetup.distributionParameter = str2double(char(splittedSceneData(3)));
            case 'REFRACTIVE_INDEX'
                sceneSetup.refractiveIndex = str2double(char(splittedSceneData(2)));
            case 'TARGETS_COUNT'
                sceneSetup.targetsCount = str2double(char(splittedSceneData(2)));
            case 'TARGET_SPEEDS'
                % targetsCount is initialized sooner then targetSpeeds
                for i = 2:(sceneSetup.targetsCount+1)
                    sceneSetup.targetSpeeds(i-1) = str2double(char(splittedSceneData(i)));
                end
            otherwise
                disp(['Could not recognize data specifier on row: ', num2str(iterator)]);
        end

        % Get next line
        sceneDataLine = fgetl(fileID);
        iterator = iterator + 1;
    end

    % Check if all important data have been correctly loaded
    if sceneSetup.dimensionX <= 0.0 || sceneSetup.dimensionY <= 0.0 || sceneSetup.samplingPeriod <= 0.0 || sceneSetup.toaToDistanceConversion < 0.0
        % Quitting script
        error('Reading from setup file failed or values are incorrect');
    end    

    % Reading sensor positions
    iterator = 1;
    sceneDataLine = fgetl(fileID);

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
        
    % Establishing new structure for setup data
    
    disp('All data have been loaded successfully');

end

