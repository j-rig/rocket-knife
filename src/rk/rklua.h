/**
 * @file rklua.h
 * @author Josh Righetti
 * @date 2022
 */

#ifndef RK_LUA_H
#define RK_LUA_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

LUAMOD_API int luaopen_rkcrypt(lua_State *L);
LUAMOD_API int luaopen_rkcompress(lua_State *L);
LUAMOD_API int luaopen_rkunzip(lua_State *L);
LUAMOD_API int luaopen_rksqlite(lua_State *L);
LUAMOD_API int luaopen_socketdriver(lua_State *L);
LUAMOD_API int luaopen_rkutil(lua_State *L);
LUAMOD_API int luaopen_rkrpc(lua_State *L);

LUALIB_API void lua_preload_zip(lua_State *L, char *path);

#endif
