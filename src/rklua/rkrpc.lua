local rkrpc = {}

function rkrpc.pack_flat_table(t)
  local buff=""
  local count=0
  for k, v in pairs(t) do
    k= tostring(k)
    v= tostring(v)
    buff= buff..string.pack("<ss", k,v)
    count=count+1
  end
  return string.pack("<H", count)..buff
end

function rkrpc.unpack_flat_table(buff)
  local result={}
  local offset=0
  local i=0
  local count=0
  count, offset = struct.unpack("<H", buff)
  while i < count do
    k, v, offset= struct.unpack("<ss",buff, offset)
    result[k]=v
    i=i+1
  end
  return result
end

function rkrpc.pack(enc_ctx, t)
  local buff= rkrpc.pack_flat_table(t)
  local comp_buff=rkcompress.compress(buff)
  local buff=string.pack("<fls", math.random(), string.len(buff), comp_buff)
  local enc_buff=rkcrypt.encrypt(enc_ctx, buff)
  return enc_buff
end

function rkrpc.unpack(enc_ctx, enc_buff)
  local dec_buff= rkcrypt.decrypt(enc_ctx, enc_buff)
  local rand, buff_size, comp_buff= string.unpack("<fLs", dec_buff)
  local buff=rkcompress.decompress(buff_size, comp_buff)
  return rkrpc.unpack_flat_table(buff)
end

function rkrpc.send(sock, enc_ctx, t)
  local enc_buff=rkrpc.pack(enc_ctx, t)
  return rksocket.tcp_send_netstring(sock, enc_buff)
end

function rkrpc.recv(sock, enc_ctx)
  local enc_buff=rksocket.tcp_read_netstring(sock)
  return rkrpc.unpack(enc_ctx, enc_buff)
end

return rkrpc
