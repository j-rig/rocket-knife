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
  count, offset = string.unpack("<H", buff)
  while i < count do
    k, v, offset= string.unpack("<ss",buff, offset)
    result[k]=v
    i=i+1
  end
  return result
end

function rkrpc.pack(enc_ctx, t)
  local raw_buff= rkrpc.pack_flat_table(t)
  local comp_buff=rkcompress.compress(raw_buff)
  if comp_buff == nil then
    comp_buff=raw_buff
  end
  local buff=string.pack("<fls", math.random(), string.len(raw_buff), comp_buff)
  local enc_buff=rkcrypt.encrypt(enc_ctx, buff)
  return string.pack("<Ls", string.len(buff), enc_buff)
end

function rkrpc.unpack(enc_ctx, buff)
  local buff_len, enc_buff= string.unpack("<Ls", buff)
  local dec_buff= rkcrypt.decrypt(enc_ctx, buff_len, enc_buff)
  local rand, buff_size, comp_buff= string.unpack("<fLs", dec_buff)
  if buff_size == string.len(comp_buff) then
    local buff=comp_buff
  else
    local buff=rkcompress.decompress(buff_size, comp_buff)
  end
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
