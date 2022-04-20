---------------------------------
--! @file
--! @author Josh Righetti
--! @date 2022
---------------------------------

msg = ""
for i=1,10000 do
  msg = msg .. "test123"
end

comp_msg= rkcompress.compress(msg)
assert(comp_msg ~= nil)

decomp_msg= rkcompress.decompress(string.len(msg),comp_msg)
assert(decomp_msg ~= nil)
assert(decomp_msg == msg)

print('compressed size %:', string.len(comp_msg)/string.len(msg)*100)
print('msg size:', string.len(msg))
print('compressed msg size:', string.len(comp_msg))
