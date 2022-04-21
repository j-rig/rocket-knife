/*
** $Id: linit.c $
** Initialization of libraries for lua.c and other clients
** See Copyright Notice in lua.h
*/


#define linit_c
#define LUA_LIB

/*
** If you embed Lua in your program and need to open the standard
** libraries, call luaL_openlibs in your program. If you need a
** different set of libraries, copy this file to your project and edit
** it to suit your needs.
**
** You can also *preload* libraries, so that a later 'require' can
** open the library, which is already linked to the application.
** For that, do the following code:
**
**  luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
**  lua_pushcfunction(L, luaopen_modname);
**  lua_setfield(L, -2, modname);
**  lua_pop(L, 1);  // remove PRELOAD table
*/

#include <assert.h>

#include "lprefix.h"


#include <stddef.h>

#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"

#include "rklua.h"


/*
** these libs are loaded by lua.c and are readily available to any Lua
** program
*/
static const luaL_Reg loadedlibs[] = {
  {LUA_GNAME, luaopen_base},
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_COLIBNAME, luaopen_coroutine},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_IOLIBNAME, luaopen_io},
  {LUA_OSLIBNAME, luaopen_os},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_MATHLIBNAME, luaopen_math},
  {LUA_UTF8LIBNAME, luaopen_utf8},
  {LUA_DBLIBNAME, luaopen_debug},

  {"rkcrypt", luaopen_rkcrypt},
  {"rkcompress", luaopen_rkcompress},
  {"rkunzip", luaopen_rkunzip},
  {"rksqlite", luaopen_rksqlite},
  {"rksocket", luaopen_socketdriver},
  {"rkutil", luaopen_rkutil},
  {"rkrpc", luaopen_rkrpc},

  {NULL, NULL}
};


LUALIB_API void luaL_openlibs (lua_State *L) {
  const luaL_Reg *lib;
  /* "require" functions from 'loadedlibs' and set results to global table */
  for (lib = loadedlibs; lib->func; lib++) {
    luaL_requiref(L, lib->name, lib->func, 1);
    lua_pop(L, 1);  /* remove lib */
  }
}

/* preload from zip appended to self */
LUALIB_API void lua_preload_zip(lua_State *L, char *path) {

  int top;
	char *cmd = "zip=rkunzip.open(_self_path) " \
    "ziplist=rkunzip.list(zip) " \
    "if ziplist ~= nil then " \
    "print(\"preloading from zip\")" \
    "for i,v in pairs(ziplist) do " \
    "data= rkunzip.extract(zip,v)" \
    "if data ~= nil then " \
    "print(v)" \
    "package.preload[v] = function() " \
    "return loadstring(data) " \
    "end " \
    "end " \
    "end " \
    "end " \
    "rkunzip.close(zip) ";

  top=lua_gettop(L);
  lua_pushstring (L, path);
  lua_setglobal(L, "_self_path");
	assert(luaL_dostring(L, cmd)==0);
  lua_settop(L, top);
}
