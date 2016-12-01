function [acc, angr, mag, orient, timer] = parseAccAngrMagOr(pack)
% Process the pack received with CMD_ACEL_ANG_MAG_VEC_OR command

acc = [convert2float32bin(pack(02:05))
       convert2float32bin(pack(06:09))
       convert2float32bin(pack(10:13))];

angr = [convert2float32bin(pack(14:17))
        convert2float32bin(pack(18:21))
        convert2float32bin(pack(22:25))];

mag = [convert2float32bin(pack(38:41))
       convert2float32bin(pack(42:45))
       convert2float32bin(pack(46:49))];

orient = [convert2float32bin(pack(38:41)) convert2float32bin(pack(42:45)) convert2float32bin(pack(46:49))
          convert2float32bin(pack(50:53)) convert2float32bin(pack(54:57)) convert2float32bin(pack(58:61))
          convert2float32bin(pack(62:65)) convert2float32bin(pack(66:69)) convert2float32bin(pack(70:73))];

timer = convert2ulong(pack(74:77))/19660800;

end

