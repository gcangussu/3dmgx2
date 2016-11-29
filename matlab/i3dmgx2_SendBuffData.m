function Error = i3dmgx2_SendBuffData(SerialLink,Command)
%Sends a command to the sensor
%
%Arguments: SerialLink - Handle of serial link
%           Command - A string from the command array in i3dmgx2_Cmd.m
%
%Returns:   Error number
Error = 0;
CommandArray = i3dmgx2_Cmd; %Call command array
try
    CommandNum = strmatch(Command,CommandArray(:,1)); %Find command
    CommandBytes = CommandArray{CommandNum,2}; %Find command bytes
    CommandLength = length(CommandBytes); %Find number of command bytes   
    wrapped_command_bytes=[hex2dec('0B'),0,[0,95],CommandLength+1,0,CommandBytes];
    checksum = sum(wrapped_command_bytes);
 	checksum_bin = dec2bin(checksum);
    if length(checksum_bin) >= 8
        checksum_LSB = bin2dec(checksum_bin(end-7:end));
        checksum_MSB = bin2dec(checksum_bin(1:end-8));
    else
        checksum_LSB = bin2dec(checksum_bin);
        checksum_MSB = 0;
    end
    wrapped_command_bytes = [hex2dec('AA'), wrapped_command_bytes, checksum_MSB, checksum_LSB];

    for ByteNum = 1:length(wrapped_command_bytes)        
        fwrite(SerialLink,wrapped_command_bytes(ByteNum)); %Write command to device
    end
catch %#ok<CTCH>
    Error = 4; %Could not write to device on specified port
    disp('Could not write to device on specified port');
end
end