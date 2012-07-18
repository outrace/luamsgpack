local msgpack = require("msgpack")
local data = {
   key1="val1",key2={"a","b"},key3=false,key4=123,key5={k1="v1",k2="v2"},key6="中1文222dd#$#$#VD("
}
local tmp = msgpack.lua2msg(data)
local ret = msgpack.msg2lua(tmp)
print(ret['key1'])
print(ret['key2'][1])
print(ret['key3'])
print(ret['key4'])
print(ret['key5']['k2'])
print(ret['key6'])
