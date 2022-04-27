rkrpc= require "rkrpc"

print(rkrpc)
for k,v in pairs(rkrpc) do
  print(k,v)
end

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

assert(t_out["hello"]=="world")

t={}
t["hello"]="world"

dat=rkrpc.pack(enc_ctx, t)
t_out=rkrpc.unpack(enc_ctx, dat)

assert(t_out["hello"]=="world")
