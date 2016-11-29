function [Packet,Error] = i3dmgx2_ReceiveData(SerialLink,Command)
%Receives data from sensor
%
%Arguments: SerialLink - Handle of serial link
%           Command - A string from the command array in i3dmgx2_Cmd.m
%
%Returns:   Packet - Data packet, if successfully read (empty matrix if not)
%           Error - Error number
Error = 0;
response = fread(SerialLink,1);
if response == 33 %NACK
    Error = 69;
    Packet = [];
elseif response == hex2dec('AA') %ACK
    
    if strcmp(Command,'CMD_WIRELESS_PING')
        Packet = response;
    else
        
        CommandArray = i3dmgx2_Cmd; %Call command array
        CommandNum = strmatch(Command,CommandArray(:,1)); %Find command
        CommandBytes = CommandArray{CommandNum,2}; %Find command bytes
        ResponseLength = CommandArray{CommandNum,4}; %Find number of command bytes
        Packet = fread(SerialLink,ResponseLength); %Read data packet from device
        
        if isempty(Packet) == 0 %Check for response
            
            if length(Packet) == ResponseLength; %Check response length
                if Packet(8) == CommandBytes(1) %Check for proper command byte
                    if i3dmgx2_CalcChecksum(Packet) == i3dmgx2_Checksum(Packet) %Evaluate checksum
                        Packet = Packet(8:end-4); %remove wireless wrapper 
                        Error = 0; %No error
                    else
                        Error = 8; %Incorrect checksum
                    end
                else
                    Error = 5; %Output does not match command
                end
            else
                Error = 5; %Output does not match command 
                disp('Expected length is different from received one');               
            end
        else
            disp('Could not read device')
            Error = 6; %Could not read device
        end
    end
end

end

