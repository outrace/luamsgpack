luamsgpack
==========

Another msgpack lua binding<br/>
���������洢SNS��Ϸ����ʱ����ѡJSON����MSGPACKʱ��д�ġ�ֻ֧��һ��map����������Ϊ���������

install
==========
���ڱ���ʱ���lua·�����õ�<br/>
#include <luajit-2.0/lua.h><br/>
#include <luajit-2.0/lauxlib.h><br/>
������·����һ�������޸�һ��luamsg.h���ļ���<br/>
gcc luamsg.c -fPIC -shared -lmsgpack -o msgpack.so<br/>
cp msgpack.so /usr/local/lib/lua/5.1/<br/>

example
==========
local msgpack = require("msgpack")<br/>
local data = {<br/>
   key1="val1",key2={"a","b"},key3=false,key4=123,key5={k1="v1",k2="v2"},key6="��1��222dd#$#$#VD("<br/>
}<br/>
local tmp = msgpack.lua2msg(data) --��lua��map���͵�tableת��msgpack��ʽ<br/>
local ret = msgpack.msg2lua(tmp)  --�������map���͵Ķ���ת��lua��table<br/>

author
==========
ŷԶ�� outrace#gmail.com