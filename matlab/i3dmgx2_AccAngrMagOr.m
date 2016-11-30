function [Packet,Error] = i3dmgx2_AccAngrMagOr(SerialLink)
%Reads scaled Acceleration, Angular Rate, Magnetometer and Orientation
%Matrix from sensor
%
%Arguments: SerialLink - Handle of serial link
%
%Returns:   Packet - The data packet
%           Error - Error number

Error = purgePort(SerialLink); %Clear buffer
if Error == 0
    Error = i3dmgx2_SendBuffData(SerialLink,'CMD_ACEL_ANG_MAG_VEC_OR'); %Write command to device
    if Error == 0
        [Packet,Error] = i3dmgx2_ReceiveData(SerialLink,'CMD_ACEL_ANG_MAG_VEC_OR'); %Read data from device
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