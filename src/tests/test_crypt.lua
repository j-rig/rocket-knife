---------------------------------
--! @file
--! @author Josh Righetti
--! @date 2022
---------------------------------

msg = ""
for i=1,18 do
  msg = msg .. "test123"
end


ctx= rkcrypt.init("test123")
assert(ctx ~= nil)
crypt_msg= rkcrypt.encrypt(ctx, msg)
assert(crypt_msg ~= nil)

decrypt_msg= rkcrypt.decrypt(ctx,string.len(msg),crypt_msg)
assert(decrypt_msg ~= nil)
assert(decrypt_msg == msg)
