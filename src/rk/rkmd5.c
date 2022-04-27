/**
 * @file rkmd5.c
 * @author Josh Righetti
 * @date 2022
 */

#include <assert.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <global.h>
#include <md5.h>


int l_rkmd5_init(lua_State *L)
{
	MD5_CTX *ctx;

	ctx= (MD5_CTX *) lua_newuserdata(L, sizeof(MD5_CTX));
	assert(ctx!=NULL);
	MD5Init(ctx);
	return 1;
}

int l_rkmd5_update(lua_State *L)
{
	MD5_CTX *ctx;
	unsigned char *buff;
	size_t bufflen;

	assert(lua_isuserdata(L, 1));
	ctx = lua_touserdata(L, 1);

	buff = (unsigned char *) luaL_checklstring(L, 2, &bufflen);
	MD5Update(ctx, buff, bufflen);

	return 0;
}

int l_rkmd5_final(lua_State *L)
{
	MD5_CTX *ctx;
	unsigned char *bout;
	luaL_Buffer lb;

	assert(lua_isuserdata(L, 1));
	ctx = lua_touserdata(L, 1);

	bout = (unsigned char *) luaL_buffinitsize(L, &lb, 16);

	MD5Final(bout, ctx);

	luaL_pushresultsize(&lb,16);
	return 1;
}

static const struct luaL_Reg md5_reg[] = {
	{ "init",			 l_rkmd5_init		       },
	{ "update",			 l_rkmd5_update	       },
	{ "final",			 l_rkmd5_final	       },
	{ NULL,			 NULL		       }
};

LUAMOD_API int luaopen_rkmd5(lua_State *L)
{
	luaL_newlib(L, md5_reg);
	return 1;
}
