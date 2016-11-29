delete(instrfind);
clear all;
close all;
clc;



SampleRate = 1;
PortOpen = 0;
warning on backtrace
warning on verbose

%Get COM port # and create serial link
ComNum = '/dev/ttyUSB0';

[SerialLink,Error] = i3dmgx2_OpenPort(ComNum);
if Error == 0
    Error = setCommTimeouts(SerialLink,1/SampleRate+.1);
   % if Error == 0
        %Determine continuous mode status and get data rate
    %    disp('Reading data')
       
            %  [SampleRate,Error] = i3dmgx2_ReadDataRate(SerialLink);
       
        if Error == 0
            PortOpen = 1;
        end
   % end
end

if PortOpen == 1
    
    
%     Error = i3dmgx2_SendBuffData(SerialLink,'CMD_WIRELESS_PING'); %Write command to device
%     if Error == 0
%         [Packet,Error] = i3dmgx2_ReceiveData(SerialLink,'CMD_WIRELESS_PING') %Read data from device
%         if Error ~= 0
%             disp(Error);%Packet = [];
%         end
%     else
%         Packet = [];
%     end
% %     
%     Error = i3dmgx2_SendBuffData(SerialLink,'CMD_WIRELESS_PING'); %Write command to device
%     if Error == 0
%         [Packet,Error] = i3dmgx2_ReceiveData(SerialLink,'CMD_WIRELESS_PING') %Read data from device
%         if Error ~= 0
%             disp(Error);%Packet = [];
%         end
%     else
%         Packet = [];
%     end
    
    %Get acceleration and angular rate, print in command window  
    
    while 1
   [Packet,Error] = i3dmgx2_AccelAndAngRate(SerialLink);
    
    if Error == 0
        i3dmgx2_PrintAccelAndAngRate(Packet);
    end    
    end
end
closePort(ComNum);

