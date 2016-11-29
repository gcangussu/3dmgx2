function [Packet,Error] = i3dmgx2_ReadEEPROMValue(SerialLink,EEPROMAddress)
%Writes word to sensor's EEPROM
%
%Arguments: SerialLink - Handle of serial link
%           EEPROMAddress - Vector with address of EEPROM word
%
%Returns:   Packet - The data packet
%           Error - Error number

Error = purgePort(SerialLink); %Clear buffer
if Error == 0
    Error = i3dmgx2_SendBuffData(SerialLink,'CMD_READ_WORD_EEPROM'); %Write command to device
    if Error == 0
        for ByteNum = 1:length(EEPROMAddress)
            fwrite(SerialLink,EEPROMAddress(ByteNum)) %Write EEPROM address to device
        end
        [Packet,Error] = i3dmgx2_ReceiveData(SerialLink,'CMD_READ_WORD_EEPROM'); %Read EEPROM word from device
        if Error ~= 0
            Packet = [];
        end
    else
        Packet = [];
    end
else
    Packet = [];
end
end