function [SampleRate,Error] = i3dmgx2_ReadDataRateCM(SerialLink,SampleRate)
%Reads the data rate from EEPROM
%
%Arguments: SerialLink - Handle of serial link
%           SampleRate - Rate at which device sends out packets
%
%Returns:   SampleRate - Rate at which device sends out packets
%           Error - Error number

EEPROMAddress = [hex2dec('FC'),hex2dec('A2')]; % Convert EEPROM address to decimal bytes
Error = setCommTimeouts(SerialLink,16); %Set timeouts to a large value to account for long gaps when data rate is low
if Error == 0
    Read = 1; %Activate Read == 1 loop
    StartTime = now; %Find starting time
    while Read == 1 && 86400*(now - StartTime) <= 16
        [Packet,Error] = i3dmgx2_ReadEEPROMValueCM(SerialLink,EEPROMAddress,SampleRate); %Read packet from device
        if Error == 0
            SampleRate = 51200/convert2ushort(Packet(2:3)); %Convert bytes to decimal sample rate
            Read = 0; %Exit Read == 1 loop
            pause(1/SampleRate) %Wait for next data packet to be sent
            Error = purgePort(SerialLink); %Clear buffer
            fread(SerialLink,1);
        else
            SampleRate = 1;
        end
    end
else
    SampleRate = 1;
end
if Error == 0
    Error = setCommTimeouts(SerialLink,1/SampleRate+.1); %Update timeouts based on found sample rate
end
end