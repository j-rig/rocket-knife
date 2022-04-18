/**
 * @file rkcompress.c
 * @author Josh Righetti
 * @date 2022
 */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static const struct luaL_Reg compress_reg[] = {
	{ NULL,			 NULL		       }
};

LUAMOD_API int luaopen_rkcompress(lua_State *L)
{
	luaL_newlib(L, compress_reg);
	return 1;
}
