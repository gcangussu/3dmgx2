function [SampleRate,Error] = i3dmgx2_ReadDataRate(SerialLink)
%Reads the data rate from EEPROM
%
%Arguments: SerialLink - Handle of serial link
%
%Returns:   SampleRate - Rate at which device sends out packets
%           Error - Error number

EEPROMAddress = [hex2dec('FC'),hex2dec('A2')]; %Convert EEPROM address to decimal bytes
Error = setCommTimeouts(SerialLink,16); %Set timeout to a large value to account for long gaps when the sample rate is low
if Error == 0
    [Packet,Error] = i3dmgx2_ReadEEPROMValue(SerialLink,EEPROMAddress); %Read sample rate from device
    if Error == 0
        SampleRate = 51200/convert2ushort(Packet(2:3)); %Convert EEPROM word to decimal
    else
        SampleRate = 1;
    end
else
    SampleRate = 1;
end
Error = setCommTimeouts(SerialLink,1/SampleRate+.1); %Update timeouts based on found sample rate
end
