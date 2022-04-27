

msg = ""
for i=1,10000 do
  msg = msg .. "test123"
end

crc=rkutil.update_crc(0,msg)
print('crc', crc)

ctx= rkmd5.init()
rkmd5.update(ctx,msg)
md5=rkmd5.final(ctx)
print('md5', md5)
