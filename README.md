luamsgpack
==========

Another msgpack lua binding<br/>
���������洢SNS��Ϸ����ʱ����ѡJSON����MSGPACKʱ��д�ġ�ֻ֧��һ��map����������Ϊ���������

example
==========
local msgpack = require("msgpack")
local data = {
   key1="val1",key2={"a","b"},key3=false,key4=123,key5={k1="v1",k2="v2"},key6="��1��222dd#$#$#VD("
}
local tmp = msgpack.lua2msg(data) --��lua��map���͵�tableת��msgpack��ʽ
local ret = msgpack.msg2lua(tmp)  --�������map���͵Ķ���ת��lua��table

author
==========
ŷԶ�� outrace#gmail.com