/**
 * @file rkcrypt.c
 * @author Josh Righetti
 * @date 2022
 */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static const struct luaL_Reg crypt_reg[] = {
	{ NULL,			 NULL		       }
};

LUAMOD_API int luaopen_rkcrypt(lua_State *L)
{
	luaL_newlib(L, crypt_reg);
	return 1;
}
