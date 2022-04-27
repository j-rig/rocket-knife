local rkuuid = {}

math.randomseed()

--[[
https://datatracker.ietf.org/doc/html/rfc4122

typedef struct {
    unsigned32  time_low;
    unsigned16  time_mid;
    unsigned16  time_hi_and_version;
    unsigned8   clock_seq_hi_and_reserved;
    unsigned8   clock_seq_low;
    byte        node[6];
} uuid_t;

The version number is in the most significant 4 bits of the time
stamp (bits 4 through 7 of the time_hi_and_version field).

0     1     0     0        4     The randomly or pseudo-
                                   randomly generated version
                                   specified in this document.

4.4.  Algorithms for Creating a UUID from Truly Random or
   Pseudo-Random Numbers

The version 4 UUID is meant for generating UUIDs from truly-random or
pseudo-random numbers.

The algorithm is as follows:

o  Set the two most significant bits (bits 6 and 7) of the
   clock_seq_hi_and_reserved to zero and one, respectively.

o  Set the four most significant bits (bits 12 through 15) of the
   time_hi_and_version field to the 4-bit version number from
   Section 4.1.3.

o  Set all the other bits to randomly (or pseudo-randomly) chosen
   values.

--]]

function rkuuid.uuid4()
  local time_low=math.random(0, 4294967295)
  local time_mid=math.random(0, 65535)
  local time_hi_and_version=math.random(0, 65535)
  local clock_seq_hi_and_reserved=math.random(0, 255)
  local clock_seq_low=math.random(0, 255)
  local byte1=math.random(0, 255)
  local byte2=math.random(0, 255)
  local byte3=math.random(0, 255)
  local byte4=math.random(0, 255)
  local byte5=math.random(0, 255)
  local byte6=math.random(0, 255)

  return string.format("%8.8x-%4.4x-%4.4x-%2.2x%2.2x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
    time_low, time_mid, time_hi_and_version, clock_seq_hi_and_reserved, clock_seq_low,
    byte1, byte2, byte3, byte4, byte5, byte6)
end

return rkuuid
