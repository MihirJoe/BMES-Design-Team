function Arduino_Read(cP,cn)
%Input Parameters
% cP is the COM address number (ex COM5 cp = 5) or MAC address (starts with /dev/...)
% cn is the number of channels (default 1).
%% Arduino_Read_U1 
%Version 2.0 of Arduino_Read
%This script creates a serial port connection with arduino, then collects
%and graphs the analog voltage information using updated serialport
%commands
%
%Authors: Dr. Ortiz-Rosario & Matthew Lerner
%Last modified 8/6/2021
%


format shortG %Format command window outputs to be short engineering.

switch nargin
    case 2
        if cn < 1 || cn >2
            error('Only 1 or 2 channels possible.');
        end
    case 1
        cn = 1;
    otherwise
        cn = 0;
        error('Parameters do not match requried.');
        return
end

%Reseting and clearing serial connections, setting baud 
instrreset;
clear s;
baud = 115200;

%Properly formatting COM address and opening serial connection
try
    if ischar(cP)
        s = serialport(cP,baud);
    else
        com = strcat('COM',num2str(cP));
        s = serialport(com,baud);
    end
    
catch
    print ('invalid port')
    return
end

%This dialogue box will prompt the user to select a time window.
%This time window will ensure recording is adequate.
tSelect = [5,15,30,60,120];
[index,tf] = listdlg('PromptString','Recording Time:',...
    'SelectionMode','single',...
    'ListString',{'5','15', '30', '60', '120'},...
    'ListSize',[100,75],...
    'OKString','Record');

if ~tf
    error('A time window selection must be made!')
    instrreset; % old version - delete(instrfind);
    clear s; % close and delete open port
end

%Figure Creation
fig = figure('Name','Arduino Output');
set(fig,'Resize','off'); %Blocks the window from being resized.

%Variables
offM = [2, 3, 4, 5, 1; 0, 4, 3, 2, 1]';
if cn == 1
    data1 = [];
else
    data1 = [];
    data2 = [];
end

%Axis Definitions
axlbl = @(h) [xlabel(h, 'Time (s)'); ylabel(h, 'Voltage (V)')];
Axes = [0, tSelect(index), 0, 5];

% flush first packets, also speeds up sampling rate during loop
flush(s)

tic 
cl = 500;
raw = read(s,cl,'char');
start = (raw(1:5) == ' ') * offM(:,1);
e = (raw(1:5) == ' ') * offM(:,2);
tmp = str2num(raw(start:end-e));
if cn == 1
    data1 = [data1,tmp];
else
    LM = rem(length(tmp),2); %Length Modifier. If it is odd, it will take one from v1.
    data1 = [data1,tmp(1:2:end-LM)];
    data2 = [data2,tmp(2:2:end)];
end

tStart = toc;
startLength = length(data1);

t = linspace(0,tStart,startLength);

%Plotting the data
if cn == 1
    plotGraph = plot(t,data1);
    title('Channel 1'); axlbl(gca); axis(Axes);

else
    ax1 = subplot(2,1,1)
    plotGraph1 = plot(t,data1);
    title('Channel 1'); axlbl(gca); axis(Axes);
    
    ax2 = subplot(2,1,2)
    plotGraph2 = plot(t,data2);
    title('Channel 2'); axlbl(gca); axis(Axes);
    linkaxes([ax1,ax2],'x')
end

while toc <= tSelect(index) %ishandle(plotGraph)
    raw = read(s,cl,'char'); %reading arduino buffer and stream data, 500 seems to work the best
    start = (raw(1:5) == ' ') * offM(:,1);
    e = (raw(1:5) == ' ') * offM(:,2);
    tmp = str2num(raw(start:end-e));
        
    if cn == 1
        data1 = [data1,tmp];
        t = linspace(0,toc,length(data1));
        set(plotGraph,'XData',t, 'YData', data1);
    else
        LM = rem(length(tmp),2); %Length Modifier. If it is odd, it will take one from v1.
        data1 = [data1,tmp(1:2:end-LM)];
        data2 = [data2,tmp(2:2:end)];
        t = linspace(0,toc,length(data2));        
        set(plotGraph1,'XData',t, 'YData', data1);
        set(plotGraph2,'XData',t, 'YData', data2);
    end
   % pause(0.002);
end

%close and reset serial connections
clear s;
instrreset;


%Calculates the sampling frequency achieved by the acquisition.
tDelta = t(end) - t(1);
endLength = length(t);
fs = (endLength/tDelta); %Sampling frequency achieved.
signal.t = t';
signal.fs = fs;

%It removes the mean voltage offset implemented in the arduino, and saves
%the signal.
if cn == 1
    data1 = data1 - mean(data1);
    signal.v = data1';
else
    %It removes the mean voltage offset implemented in the arduino.
    data1 = data1 - mean(data1);
    data2 = data2 - mean(data2);
    signal.v1 = data1';
    signal.v2 = data2';
end

answer = questdlg('Save Acquisition', ...
    'Do you wish to save?', ...
    'Yes','No','Yes');

% Handle response and either saves file or not.
switch answer
    case 'Yes'
        %Asks user for a name.
        str = inputdlg('File name (NO SPACES or SPECIAL characters):',...
            'File Save', [1 50]);
        assignin('base',str{1},signal);
        eval([str{1},' = signal;'])
        save(str{1},str{1});
        
    case 'No'
        %Dummy, nothing happens.
end

end
