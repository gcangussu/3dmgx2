clear variables
close all
clc

portOpen = 0;
serialPort = '/dev/ttyUSB0';
[serialLink, err] = i3dmgx2_OpenPort(serialPort);

if err == 0
    err = setCommTimeouts(seralLink, 2.0);
    if err == 0
        error('Error opening serial port')
    end
end

i = 1;
numPacks = 1000;
packs = cell(1, numPacks);
elapsed = zeros(1, numPacks);

while i <= numPacks
    tic
    [packs{i}, err] = i3dmgx2_AccAngrMagOr(SerialLink);
    elapsed(i,:) = toc;
    if err == 0
        i = i + 1;
    end
end
