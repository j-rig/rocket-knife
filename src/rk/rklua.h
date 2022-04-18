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
LUAMOD_API int luaopen_rknetstring(lua_State *L);
LUAMOD_API int luaopen_rkunzip(lua_State *L);

#endif