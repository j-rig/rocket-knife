/**
 * @file rkcompress.c
 * @author Josh Righetti
 * @date 2022
 */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <minilzo.h>

#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);

int l_compress(lua_State *L)
{
	int r;
	size_t bl, bl_out;
	char *b, *b_out;
	luaL_Buffer lb;

	b = (char *) luaL_checklstring(L, 1, &bl);
	b_out = (char *) luaL_buffinitsize(L, &lb, (bl + bl / 16 + 64 + 3));
	r = lzo1x_1_compress((const unsigned char *)b,bl,
		(unsigned char *)b_out,&bl_out,wrkmem);
	if (r != LZO_E_OK) goto erra;
	if (bl_out >= bl) goto errb;

	luaL_pushresultsize(&lb,bl_out);
	return 1;

	erra:
		fprintf(stderr, "internal error - compression failed");
		lua_pushnil(L);
		return 1;
	errb:
		fprintf(stderr, "This block contains incompressible data.\n");
		lua_pushnil(L);
		return 1;
}

int l_decompress(lua_State *L)
{
	int r;
	size_t bl, bl_out;
	char *b, *b_out;
	lua_Integer l_uncompressed;
	luaL_Buffer lb;

	l_uncompressed = luaL_checkinteger(L, 1);
	b = (char *) luaL_checklstring(L, 2, &bl);

	if (l_uncompressed <= bl) goto erra;

	b_out = (char *) luaL_buffinitsize(L, &lb, l_uncompressed);

	r = lzo1x_decompress((const unsigned char *)b,bl,
		(unsigned char *)b_out,&bl_out,NULL);

	if (r != LZO_E_OK || l_uncompressed != bl_out) goto errb;

	luaL_pushresultsize(&lb,bl_out);
	return 1;

	erra:
		fprintf(stderr, "decompressed length less than or equal to input buffer length.\n");
		lua_pushnil(L);
		return 1;

	errb:
		fprintf(stderr, "internal error - decompression failed.\n");
		lua_pushnil(L);
		return 1;
}

static const struct luaL_Reg compress_reg[] = {
	{ "compress",	    l_compress },
{ "decompress",	    l_decompress },
	{ NULL,			 NULL		       }
};

LUAMOD_API int luaopen_rkcompress(lua_State *L)
{
	if (lzo_init() != LZO_E_OK)
	{
			printf("internal error - lzo_init() failed !!!\n");
			return 0;
	}
	luaL_newlib(L, compress_reg);
	return 1;
}
