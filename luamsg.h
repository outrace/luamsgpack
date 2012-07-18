#include <msgpack.h>
#include <luajit-2.0/lua.h>
#include <luajit-2.0/lauxlib.h>
static int msg2lua(lua_State *L);
static int lua2msg(lua_State *L);

static int packlua(lua_State *L,msgpack_sbuffer* sbuf,int index);
static int unpacklua(lua_State *L,const char *str,size_t tsize);

static int packTable(lua_State *L, msgpack_packer* pk, int index);
static int packArray(lua_State *L, msgpack_packer* pk, int index);
static int pack(lua_State *L,msgpack_packer *pk, int index);

static int unpackArray(lua_State *L, msgpack_object_array arr);
static int unpackTable(lua_State *L, msgpack_object_map map);
static int unpack(lua_State *L, msgpack_object obj);
