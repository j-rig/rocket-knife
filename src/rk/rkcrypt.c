/**
 * @file rkcrypt.c
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

int l_rkcrypt_init(lua_State *L)
{
	BLOWFISH_CTX *ctx;
	unsigned char *key;
	size_t keyLen;

	key = (unsigned char *) luaL_checklstring(L, 1, &keyLen);
	if (keyLen < 32/8 || keyLen > 448/8) goto erra;

	ctx= (BLOWFISH_CTX *) lua_newuserdata(L, sizeof(BLOWFISH_CTX));
	Blowfish_Init(ctx, key,keyLen);
	return 1;

	erra:
		fprintf(stderr, "key length not 32-448 bits.\n");
		lua_pushnil(L);
		return 1;
}

int l_rkcrypt_encrypt(lua_State *L)
{
	BLOWFISH_CTX *ctx;
	unsigned char *bin, *bout;
	size_t binLen, boutLen;
	luaL_Buffer lb;
	int i;

	assert(lua_isuserdata(L, 1));
	ctx = lua_touserdata(L, 1);

	bin = (unsigned char *) luaL_checklstring(L, 2, &binLen);

	boutLen = binLen + binLen%BFBLOCK;
	bout = (unsigned char *) luaL_buffinitsize(L, &lb, boutLen);
	memcpy(bout,bin, binLen);

	for( i=0; i< boutLen;i+=BFBLOCK)
		Blowfish_Encrypt(ctx, (unsigned long *) bout+i,
			(unsigned long *) bout+(i+BFBLOCK/2));

	lua_pushinteger(L, binLen);
	luaL_pushresultsize(&lb,boutLen);
	return 2;
}

int l_rkcrypt_decrypt(lua_State *L)
{
	BLOWFISH_CTX *ctx;
	unsigned char *bin, *bout;
	size_t binLen;
	luaL_Buffer lb;
	int i;

	assert(lua_isuserdata(L, 1));
	ctx = lua_touserdata(L, 1);

	bin = (unsigned char *) luaL_checklstring(L, 2, &binLen);
	if(binLen%BFBLOCK) goto erra;

	bout = (unsigned char *) luaL_buffinitsize(L, &lb, binLen);
	memcpy(bout, bin, binLen);

	for( i=0; i< binLen;i+=BFBLOCK)
		Blowfish_Decrypt(ctx, (unsigned long *) bout+i,
			(unsigned long *) bout+(i+BFBLOCK/2));

	luaL_pushresultsize(&lb,binLen);
	return 1;

	erra:
		fprintf(stderr, "input buffer not a multiple of 64.\n");
		lua_pushnil(L);
		return 1;
}

static const struct luaL_Reg crypt_reg[] = {
	{ "init",			 l_rkcrypt_init		       },
	{ "encrypt",			 l_rkcrypt_encrypt	       },
	{ "decrypt",			 l_rkcrypt_decrypt	       },
	{ NULL,			 NULL		       }
};

LUAMOD_API int luaopen_rkcrypt(lua_State *L)
{
	luaL_newlib(L, crypt_reg);
	return 1;
}
