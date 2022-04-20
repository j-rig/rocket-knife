/**
 * @file rkunzip.c
 * @author Josh Righetti
 * @date 2022
 */

#include <assert.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <junzip.h>

struct zip_ctx {
   lua_State *L;
	 int count;
	 const char *filename;
};

int l_unzip_open(lua_State *L){
	FILE *fp;
	JZFile *zip;
	const char *path;

	path=luaL_checkstring (L, 1);

	if(!(fp = fopen(path, "rb"))) goto erra;

	zip = jzfile_from_stdio_file(fp);
	lua_pushlightuserdata (L, zip);
	return 1;

	erra:
		fprintf(stderr, "unable to open path.\n");
		lua_pushnil(L);
		return 1;
}

int l_unzip_close(lua_State *L){
	JZFile *zip;
	assert(lua_islightuserdata (L, 1));
	zip = lua_touserdata(L, 1);
	zip->close(zip);
	return 0;
}

int list_cb(JZFile *zip, int idx, JZFileHeader *header, char *filename, void *user_data){
	struct zip_ctx *ctx= (struct zip_ctx *)user_data;
	lua_pushstring (ctx->L, (const char *)filename);
  lua_rawseti (ctx->L, -2, ctx->count+1); // add to list
	ctx->count+=1;
	return 1;
}

int l_unzip_list(lua_State *L){
	JZFile *zip;
	struct zip_ctx ctx;
	JZEndRecord endRecord;

	ctx.L=L;
	ctx.count=0;
	ctx.filename=NULL;

	assert(lua_islightuserdata (L, 1));
	zip = lua_touserdata(L, 1);

	if(jzReadEndRecord(zip, &endRecord)) goto erra;

  lua_createtable(L, 10, 0); // to store the list

	if(jzReadCentralDirectory(zip, &endRecord, list_cb, &ctx)) goto errb;

	return 1; // return the list

	erra:
		fprintf(stderr, "Couldn't read ZIP file end record.");
		lua_pushnil(L);
		return 1;

	errb:
		fprintf(stderr, "Couldn't read ZIP file central record.");
		lua_pushnil(L);
		return 1;
}

int extract_cb(JZFile *zip, int idx, JZFileHeader *header, char *filename, void *user_data){
	struct zip_ctx *ctx= (struct zip_ctx *)user_data;
	JZFileHeader localHeader;
	unsigned char *data;
	luaL_Buffer lb;

	if(strcmp(filename, ctx->filename)==0){
		if(zip->seek(zip, header->offset, SEEK_SET)) goto erra;
		if(jzReadLocalFileHeader(zip, &localHeader, NULL,0)) goto errb;
		data = (unsigned char *) luaL_buffinitsize(ctx->L, &lb, localHeader.uncompressedSize);
		if(jzReadData(zip, &localHeader, data) != Z_OK) goto errc;
		luaL_pushresultsize(&lb,localHeader.uncompressedSize);
		ctx->count+=1;
		return 0;
	}

	return 1;

	erra:
		fprintf(stderr, "Cannot seek in zip file!");
		lua_pushnil(ctx->L);
		ctx->count+=1;
		return 0;

	errb:
		fprintf(stderr, "Couldn't read local file header!");
		lua_pushnil(ctx->L);
		ctx->count+=1;
		return 0;

	errc:
		fprintf(stderr, "Couldn't read file data!");
		lua_pushnil(ctx->L);
		ctx->count+=1;
		return 0;
}

int l_unzip_extract(lua_State *L){
	JZFile *zip;
	struct zip_ctx ctx;
	JZEndRecord endRecord;

	assert(lua_islightuserdata (L, 1));
	zip = lua_touserdata(L, 1);

	ctx.L=L;
	ctx.count=0;
	ctx.filename=luaL_checkstring (L, 2);

	if(jzReadEndRecord(zip, &endRecord)) goto erra;

	if(jzReadCentralDirectory(zip, &endRecord, extract_cb, &ctx)) goto errb;

  if(ctx.count!=1) goto errc;

	return ctx.count;

	erra:
		fprintf(stderr, "Couldn't read ZIP file end record.");
		lua_pushnil(L);
		return 1;

	errb:
		fprintf(stderr, "Couldn't read ZIP file central record.");
		lua_pushnil(L);
		return 1;

  errc:
		fprintf(stderr, "Didn't extract any records.");
		lua_pushnil(L);
		return 1;
}

static const struct luaL_Reg unzip_reg[] = {
	{ "open",			 l_unzip_open	},
	{ "close",		l_unzip_close },
	{ "list",			 l_unzip_list	},
	{ "extract",		l_unzip_extract	},
	{ NULL,			 NULL	}
};

LUAMOD_API int luaopen_rkunzip(lua_State *L)
{
	luaL_newlib(L, unzip_reg);
	return 1;
}
