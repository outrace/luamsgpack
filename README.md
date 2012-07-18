luamsgpack
==========

Another msgpack lua binding<br/>
当初评估存储SNS游戏数据时候是选JSON还是MSGPACK时候写的。只支持一个map类型数组作为传入参数。

install
==========
现在编译时候的lua路径是用的<br/>
#include <luajit-2.0/lua.h><br/>
#include <luajit-2.0/lauxlib.h><br/>
如果你的路径不一样，请修改一下luamsg.h的文件。<br/>
gcc luamsg.c -fPIC -shared -lmsgpack -o msgpack.so<br/>
cp msgpack.so /usr/local/lib/lua/5.1/<br/>

example
==========
local msgpack = require("msgpack")<br/>
local data = {<br/>
   key1="val1",key2={"a","b"},key3=false,key4=123,key5={k1="v1",k2="v2"},key6="中1文222dd#$#$#VD("<br/>
}<br/>
local tmp = msgpack.lua2msg(data) --将lua的map类型的table转成msgpack格式<br/>
local ret = msgpack.msg2lua(tmp)  --将上面的map类型的对象转成lua的table<br/>

author
==========
欧远宁 outrace#gmail.com