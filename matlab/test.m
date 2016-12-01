delete(instrfind);
clear all;
close all;
clc;

SampleRate = 1;
PortOpen = 0;
warning on backtrace
warning on verbose

ComNum = '/dev/ttyUSB0';

[SerialLink,Error] = i3dmgx2_OpenPort(ComNum);
if Error == 0
    Error = setCommTimeouts(SerialLink,1/SampleRate+.1);
    if Error == 0
        PortOpen = 1;
    end
end

if PortOpen == 1 
    i = 1;
    numPacks = 300;
    packSize = 77;
    packs = zeros(packSize, numPacks);
    elapsed = zeros(1, numPacks);
    
    while i <= numPacks
        tic
        [pack, Error] = i3dmgx2_AccAngrMagOr(SerialLink);
        elapsed(i) = toc;
        if (Error == 0 && length(pack) == packSize)
            packs(:,i) = pack;
            i = i + 1;
        end
    end
end

closePort(ComNum);
