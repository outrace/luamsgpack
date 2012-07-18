#include "luamsg.h"
//gcc luamsg.c -fPIC -shared -lmsgpack -o msgpack.so
//


/*将lua的table转成msgpack格式*/
static int lua2msg(lua_State *L){
	int n = lua_gettop(L);
	if (n != 1){
	  return luaL_error(L, "need one para,but %d now",n);
	} else if ( lua_type(L,1) != LUA_TTABLE){
	  return luaL_error(L, "only support table");
	}

	msgpack_sbuffer* sbuf = msgpack_sbuffer_new();
	packlua(L,sbuf,1);
	lua_pushlstring(L,sbuf->data,sbuf->size);
    msgpack_sbuffer_free(sbuf);
	return 1;
}

static int packlua(lua_State *L,msgpack_sbuffer* sbuf, int index){
	msgpack_packer* pk = msgpack_packer_new(sbuf, msgpack_sbuffer_write); ;
	packTable(L,pk,index);
    msgpack_packer_free(pk);
    return 1;
}

static int packTable(lua_State *L,msgpack_packer* pk, int index){
	int len = 0;
	lua_pushnil(L);
	while (lua_next(L, index) != 0) {
	    len++;
	    lua_pop(L, 1);
	}
	msgpack_pack_map(pk, len);
	lua_pushnil(L);
	while (lua_next(L, index) != 0) {
	    pack(L, pk,index + 1); // -2:key
	    pack(L, pk,index + 2); // -1:value
	    lua_pop(L, 1); // removes value, keeps key for next iteration
	}
	return 1;
}

static int packArray(lua_State *L, msgpack_packer *pk, int index){
	size_t len = lua_objlen(L, index);
    size_t i = 1;
	msgpack_pack_array(pk, len);
	for (i = 1; i <= len; i++) {
	  lua_rawgeti(L, index, i);
	  pack(L, pk, index + 1);
	  lua_pop(L, 1);
	}
	return 1;
}

static int pack(lua_State *L,msgpack_packer *pk, int index){
	  int t = lua_type(L, index);
	  double n;
	  int b;
	  bool isArray;
	  int64_t i;
	  size_t len;
	  const char* str;

	  switch (t) {
		  case LUA_TNUMBER:
			  n = lua_tonumber(L, index);
			  i = (int64_t) n;
			  if (i == n) {
				  msgpack_pack_int64(pk, i);
			  } else {
				  msgpack_pack_double(pk, n);
			  }
			  break;
		  case LUA_TBOOLEAN:
			  b = lua_toboolean(L, index);
			  if (b == 0) {
				  msgpack_pack_false(pk);
			  } else {
				  msgpack_pack_true(pk);
			  }
			  break;
		  case LUA_TSTRING:
			  str = lua_tolstring(L, index, &len);
			  if (str == NULL) {
			    int t = lua_type(L, index);
			    return luaL_error(L, "lua_tolstring failed for index %d: type = ",
			               index, lua_typename(L, t));
			  }

			  msgpack_pack_raw(pk, len);
			  msgpack_pack_raw_body(pk, str, len);
			  break;
		  case LUA_TTABLE:
			  len = lua_objlen(L, index);
			  if (len > 0) {
			      lua_pushnumber(L, len);
			      if (lua_next(L, index) == 0) {
			    	  isArray = true;
			      } else {
			    	  lua_pop(L, 2);
			      }
			  }
			  if (isArray) {
				  packArray(L, pk, index);
			  } else {
				  packTable(L, pk, index);
			  }
			  break;
		  case LUA_TUSERDATA:
		  case LUA_TNIL:
		  case LUA_TTHREAD:
		  case LUA_TLIGHTUSERDATA:
		  default:
			return luaL_error(L, "invalid type for pack: %s",
					   lua_typename(L, t));
			break;
	  }
	  return 1;
}

/*将msgpack转成lua的table*/
static int msg2lua(lua_State *L){
	int n = lua_gettop(L);
	if (n != 1){
	  return luaL_error(L, "must one para");
	}
	size_t total_size;
	const char *v;
	v = luaL_checklstring(L, 1, &total_size);
	return unpacklua(L,v,total_size);
}

static int unpacklua(lua_State *L,const char *str,size_t tsize){
	msgpack_sbuffer* buffer = msgpack_sbuffer_new();
	msgpack_sbuffer_write(buffer,str,tsize);

	msgpack_unpacked msg;
	msgpack_unpacked_init(&msg);
	msgpack_unpack_next(&msg, buffer->data, buffer->size, NULL);

	if (msg.data.type != MSGPACK_OBJECT_MAP){
		return luaL_error(L, "only map support");
	}

	unpackTable(L, msg.data.via.map);

	msgpack_sbuffer_free(buffer);
	msgpack_unpacked_destroy(&msg);
	return 1;
}

static int unpack(lua_State *L, msgpack_object obj){
    switch(obj.type){
		case MSGPACK_OBJECT_BOOLEAN:
			lua_pushboolean(L,obj.via.boolean);
			return 1;
		case MSGPACK_OBJECT_NEGATIVE_INTEGER:
			lua_pushnumber(L,obj.via.i64);
			return 1;
		case MSGPACK_OBJECT_POSITIVE_INTEGER:
			lua_pushnumber(L,obj.via.u64);
			return 1;
		case MSGPACK_OBJECT_DOUBLE:
			lua_pushnumber(L,obj.via.dec);
			return 1;
		case MSGPACK_OBJECT_RAW:
			lua_pushlstring(L, obj.via.raw.ptr, obj.via.raw.size);
			return 1;
		case MSGPACK_OBJECT_MAP:
			return unpackTable(L, obj.via.map);
		case MSGPACK_OBJECT_ARRAY:
			return unpackArray(L, obj.via.array);
	    case MSGPACK_OBJECT_NIL:
		default:
		    return luaL_error(L, "invalid type for unpack: %d", obj.type);
    }
    return 0;
}

static int unpackArray(lua_State *L, msgpack_object_array arr){
	  lua_newtable(L);
	  uint max = arr.size;
	  uint i = 0;
	  for (i = 0; i < max; i++) {
	    if (unpack(L, arr.ptr[i]) <= 0) {
	      lua_pop(L, 1);
	      return luaL_error(L, "failed to unpack %d-th element of the array: ", i);
	    }

	    lua_rawseti(L, -2, i + 1);
	  }
	  return 1;
}

static int unpackTable(lua_State *L, msgpack_object_map map){
	lua_newtable(L);
	uint max = map.size;
	uint i = 0;

    for (i = 0; i < max; i++) {
	    if (unpack(L, map.ptr[i].key) <= 0) {
	        lua_pop(L, 1);
	        return luaL_error(L, "failed to unpack %d-th key of the table: ", i);
	    }

	    if (unpack(L, map.ptr[i].val) <= 0) {
	    	lua_pop(L, 2);
	    	return luaL_error(L, "failed to unpack %d-th value of the table: ", i);
	    }
	    lua_rawset(L, -3);
    }
    return 1;
}

static const struct luaL_reg msgpack_f [] = {
	{"msg2lua" , msg2lua}, 
	{"lua2msg" , lua2msg}, 
	{NULL, NULL} 
}; 

int luaopen_msgpack(lua_State *L) { 
	luaL_newmetatable(L, "now.msgpack" ); 

	lua_pushstring(L, "__index"); 
	lua_pushvalue(L, -2); /* pushes the metatable */ 
	lua_settable(L, -3); /* metatable.__index = metatable */    
	luaL_openlib(L, "msgpack", msgpack_f, 0); 
	return 1; 
};