/**
 * @file rksqlite.c
 * @author Josh Righetti
 * @date 2022
 */

#include <assert.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <sqlite3.h>

struct sqlite_ctx {
   lua_State *L;
	 int count;
};

int l_sqlite_open(lua_State *L){
	sqlite3 *db;
	int rc;
	const char *path;

	path=luaL_checkstring (L, 1);
	rc = sqlite3_open(path, &db);
	if( rc ) goto erra;

	lua_pushlightuserdata (L, db);
	return 1;

	erra:
		fprintf(stderr, "unable to open path.\n");
		lua_pushnil(L);
		return 1;
}

int l_sqlite_close(lua_State *L){
	sqlite3 *db;
	assert(lua_islightuserdata (L, 1));
	db = lua_touserdata(L, 1);
	sqlite3_close(db);
	return 0;
}

static int qry_cb(void *c, int argc, char **argv, char **azColName){
	int i;
	struct sqlite_ctx *ctx= (struct sqlite_ctx *) c;
	lua_createtable(ctx->L, 0, argc); // new row (table)
	for(i=0; i<argc; i++){
		lua_pushstring(ctx->L, argv[i] ? argv[i] : "NULL");
		lua_setfield(ctx->L, -2, azColName[i]); // row kv
	}
	ctx->count+=1;
	return 0;
}

int l_sqlite_exec(lua_State *L){
	sqlite3 *db;
	const char *qry;
	char *zErrMsg = 0;
	int rc;
	struct sqlite_ctx ctx;

	ctx.L=L;
	ctx.count=0;

	assert(lua_islightuserdata (L, 1));
	db = lua_touserdata(L, 1);
	qry= luaL_checkstring (L, 2);

	rc = sqlite3_exec(db, qry, qry_cb, &ctx, &zErrMsg);
	if( rc!=SQLITE_OK ) goto erra;

	return ctx.count; // how many rows

	erra:
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		lua_pushnil(L);
		return 1;
}

static const struct luaL_Reg sqlite_reg[] = {
	{ "open",		l_sqlite_open },
	{ "close",	l_sqlite_close },
	{ "exec",		l_sqlite_exec },
	{ NULL,			 NULL }
};

LUAMOD_API int luaopen_rksqlite(lua_State *L)
{
	luaL_newlib(L, sqlite_reg);
	return 1;
}
