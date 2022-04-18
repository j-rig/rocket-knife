/**
 * @file rksqlite.c
 * @author Josh Righetti
 * @date 2022
 */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static const struct luaL_Reg sqlite_reg[] = {
	{ NULL,			 NULL		       }
};

LUAMOD_API int luaopen_rksqlite(lua_State *L)
{
	luaL_newlib(L, sqlite_reg);
	return 1;
}
