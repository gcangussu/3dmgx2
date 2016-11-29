function CalcChecksum = i3dmgx2_CalcChecksum(Packet)
%Calculate bytewise checksum on a received data packet.
%
%Note:      The last two bytes, which contain the received checksum,
%           are not included in the calculation.
%
%Arguments: Packet - The received data packet
%
%Returns:   The calculated checksum

CalcChecksum = sum(Packet(2:(length(Packet)-4)));
end
    