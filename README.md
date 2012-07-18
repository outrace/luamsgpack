luamsgpack
==========

Another msgpack lua binding<br/>
当初评估存储SNS游戏数据时候是选JSON还是MSGPACK时候写的。只支持一个map类型数组作为传入参数。

example
==========
local msgpack = require("msgpack")
local data = {
   key1="val1",key2={"a","b"},key3=false,key4=123,key5={k1="v1",k2="v2"},key6="中1文222dd#$#$#VD("
}
local tmp = msgpack.lua2msg(data) --将lua的map类型的table转成msgpack格式
local ret = msgpack.msg2lua(tmp)  --将上面的map类型的对象转成lua的table

author
==========
欧远宁 outrace#gmail.com