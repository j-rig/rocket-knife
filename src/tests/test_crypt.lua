---------------------------------
--! @file
--! @author Josh Righetti
--! @date 2022
---------------------------------

msg = ""
for i=1,100 do
  msg = msg .. "test123"
end


ctx= rkcrypt.init("test123")
assert(ctx ~= nil)
--crypt_msg= rkcrypt.encrypt(ctx, msg)
--assert(crypt_msg ~= nil)

--decrypt_msg= rkcompress.decompress(string.len(msg),comp_msg)
--assert(decomp_msg ~= nil)
--assert(decomp_msg == msg)
