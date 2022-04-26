require "rkrpc"

msg = ""
for i=1,10000 do
  msg = msg .. "test123"
end

t={}
t["msg"]=msg
t["hello"]="world"

enc_ctx=rkcrypt.init("testing")

dat=rkrpc.pack(enc_ctx, t)
t_out=rkrpc.unpack(enc_ctx, dat)
