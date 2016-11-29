function [Packet,Error] = i3dmgx2_AccelAndAngRateCM(SerialLink,SampleRate)
%Reads scaled Acceleration and Angular Rate from sensor
%
%Arguments: SerialLink - Handle of serial link
%           SampleRate - Rate at which device sends out packets
%
%Returns:   Packet - The data packet
%           Error - Error number

Error = purgePort(SerialLink); %Clear buffer
if Error == 0
    Read = 1;
    while Read == 1
        Error = i3dmgx2_SendBuffData(SerialLink,'CMD_ACCELERATION_ANGU'); %Write command to device
        if Error == 0
            [Packet,Record,Error] = i3dmgx2_ReceiveDataContinuous(SerialLink,'CMD_ACCELERATION_ANGU',SampleRate); %Read data from device
            if Error == 0
                Read = 0;
            else
                Packet = [];
            end
        else
            Packet = [];
        end
    end
else
    Packet = [];
end
end