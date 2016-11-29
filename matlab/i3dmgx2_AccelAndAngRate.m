function [Packet,Error] = i3dmgx2_AccelAndAngRate(SerialLink)
%Reads scaled Acceleration and Angular Rate from sensor
%
%Arguments: SerialLink - Handle of serial link
%
%Returns:   Packet - The data packet
%           Error - Error number

Error = purgePort(SerialLink); %Clear buffer
if Error == 0
    Error = i3dmgx2_SendBuffData(SerialLink,'CMD_ACCELERATION_ANGU'); %Write command to device
    if Error == 0
        [Packet,Error] = i3dmgx2_ReceiveData(SerialLink,'CMD_ACCELERATION_ANGU'); %Read data from device
        if Error ~= 0
            disp(Error);
            Packet = [];
        end
    else
        Packet = [];
    end
else
    Packet = [];
end
end