/**
 * @file rkrpc.c
 * @author Josh Righetti
 * @date 2022
 */

#include <assert.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <blowfish.h>

#define BFBLOCK (2*sizeof(unsigned long))

struct rkrpc_ctx {
   BLOWFISH_CTX bf_ctx;
	 int sock;
};

int l_rkrpc_init(lua_State *L)
{
	struct rkrpc_ctx *ctx;
	unsigned char *key;
	size_t keyLen;

	key = (unsigned char *) luaL_checklstring(L, 1, &keyLen);
	if (keyLen < 32/8 || keyLen > 448/8) goto erra;

	ctx= (struct rkrpc_ctx *) lua_newuserdata(L, sizeof(struct rkrpc_ctx));
	//fixme get socket
	Blowfish_Init(&(ctx->bf_ctx), key,keyLen);
	return 1;

	erra:
		fprintf(stderr, "key length not 32-448 bits.\n");
		lua_pushnil(L);
		return 1;
}

int l_rkrpc_tx(lua_State *L)
{
	return 0;
}

int l_rkrpc_rx(lua_State *L)
{
	return 0;
}

static const struct luaL_Reg rkrpc_reg[] = {
	{ "init",			 l_rkrpc_init	       },
	{ "tx",			 l_rkrpc_tx       },
	{ "rx",			 l_rkrpc_rx 	       },
	{ NULL,			 NULL		       }
};

LUAMOD_API int luaopen_rkrpc(lua_State *L)
{
	luaL_newlib(L, rkrpc_reg);
	return 1;
}
