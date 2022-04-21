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
	assert(ctx!=NULL);
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
	unsigned long *p;
	luaL_Buffer lb;
	int i, n_blocks;

	assert(lua_isuserdata(L, 1));
	ctx = lua_touserdata(L, 1);

	bin = (unsigned char *) luaL_checklstring(L, 2, &binLen);
	boutLen = binLen;
	if(binLen%BFBLOCK) boutLen+= BFBLOCK-binLen%BFBLOCK;
	n_blocks=boutLen/BFBLOCK;

	bout = (unsigned char *) luaL_buffinitsize(L, &lb, boutLen);
	memcpy(bout,bin, binLen);
	p= (unsigned long*) bout;

	for( i=0; i< n_blocks;i+=2)
		Blowfish_Encrypt(ctx, p+i,p+(i+1));

	luaL_pushresultsize(&lb,boutLen);
	return 1;
}

int l_rkcrypt_decrypt(lua_State *L)
{
	BLOWFISH_CTX *ctx;
	unsigned char *bin, *bout;
	size_t binLen;
	unsigned long *p;
	lua_Integer l_decrypt;
	luaL_Buffer lb;
	int i, n_blocks;

	assert(lua_isuserdata(L, 1));
	ctx = lua_touserdata(L, 1);

	l_decrypt = luaL_checkinteger(L, 2);

	bin = (unsigned char *) luaL_checklstring(L, 3, &binLen);
	if(binLen%BFBLOCK) goto erra;
	if(binLen < l_decrypt) goto errb;
	n_blocks= binLen/BFBLOCK;

	bout = (unsigned char *) luaL_buffinitsize(L, &lb, binLen);
	assert(bout!=NULL);
	memcpy(bout, bin, binLen);
	p= (unsigned long*) bout;

	for( i=0; i< n_blocks;i+=2)
		Blowfish_Decrypt(ctx, p+i,p+(i+1));

	luaL_pushresultsize(&lb,l_decrypt);
	return 1;

	erra:
		fprintf(stderr, "input buffer not a multiple of 64 bits.\n");
		lua_pushnil(L);
		return 1;

	errb:
		fprintf(stderr, "input buffer size smaller than decypted buffer size.\n");
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
