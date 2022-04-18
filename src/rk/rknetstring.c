/**
 * @file rknetstring.c
 * @author Josh Righetti
 * @date 2022
 */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static const struct luaL_Reg netstring_reg[] = {
	{ NULL,			 NULL		       }
};

LUAMOD_API int luaopen_rknetstring(lua_State *L)
{
	luaL_newlib(L, netstring_reg);
	return 1;
}
