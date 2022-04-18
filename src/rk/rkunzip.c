/**
 * @file rkunzip.c
 * @author Josh Righetti
 * @date 2022
 */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static const struct luaL_Reg unzip_reg[] = {
	{ NULL,			 NULL		       }
};

LUAMOD_API int luaopen_rkunzip(lua_State *L)
{
	luaL_newlib(L, unzip_reg);
	return 1;
}
