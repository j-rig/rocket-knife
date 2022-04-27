local rkrpc = {}

math.randomseed()

function rkrpc.pack_flat_table(t)
  local buff=""
  local count=0
  for k, v in pairs(t) do
    k= tostring(k)
    v= tostring(v)
    buff= buff..string.pack("<ss", k,v)
    count=count+1
  end
  crc=rkutil.update_crc(0, buff)
  return string.pack("<HL", count, crc)..buff
end

function rkrpc.unpack_flat_table(buff)
  local result={}
  local offset=0
  local i=0
  local count=0
  count, crc, offset = string.unpack("<HL", buff)
  crc_now=rkutil.update_crc(0,string.sub(buff,offset))
  assert(crc==crc_now)
  while i < count do
    k, v, offset= string.unpack("<ss",buff, offset)
    result[k]=v
    i=i+1
  end
  return result
end

function rkrpc.comp(buff)
  local comp_buff=rkcompress.compress(buff)
  if comp_buff == nil then
    return buff
  end
  return comp_buff
end

function rkrpc.decomp(len, buff)
  if string.len(buff)==len then
    return buff
  end
  local decomp_buff=rkcompress.decompress(len, buff)
  return decomp_buff
end

function rkrpc.pack(enc_ctx, t)
  local raw_buff= rkrpc.pack_flat_table(t)
  local comp_buff=rkrpc.comp(raw_buff)
  local buff=string.pack("<BLs", math.random(0,255), string.len(raw_buff), comp_buff)
  local enc_buff=rkcrypt.encrypt(enc_ctx, buff)
  return string.pack("<Ls", string.len(buff), enc_buff)
end

function rkrpc.unpack(enc_ctx, buff)
  local buff_len, enc_buff= string.unpack("<Ls", buff)
  local dec_buff= rkcrypt.decrypt(enc_ctx, buff_len, enc_buff)
  local rand, buff_size, comp_buff= string.unpack("<BLs", dec_buff)
  local buff=rkrpc.decomp(buff_size, comp_buff)
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
