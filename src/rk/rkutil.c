/**
 * @file rkutil.c
 * @author Josh Righetti
 * @date 2022
 */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>


#include <crc32.h>

int l_sleep(lua_State *L)
{
	usleep((useconds_t)(luaL_checknumber(L, 1) * 1000000.0));
	return 0;
}

int l_subprocess_start(lua_State *L)
{
	int i, j, pid;
	char *cmd_str, *cmd_str_copy;
	char *cmd_args[20];
	size_t cmd_str_len;

	cmd_str = (char *)luaL_checklstring(L, 1, &cmd_str_len);

	//copying command string from lua
	cmd_str_copy = (char *)malloc(cmd_str_len + 1);
	assert(cmd_str_copy != NULL);
	memset(cmd_str_copy, 0, cmd_str_len + 1);
	memcpy(cmd_str_copy, cmd_str, cmd_str_len);


	//create command string vector
	cmd_args[0] = cmd_str_copy;
	for (i = 1, j = 1; i < cmd_str_len && j < 20 - 1; i++) {
		if (*(cmd_str_copy + i) == ' ')
			*(cmd_str_copy + i) = 0;
		if (*(cmd_str_copy + i - 1) == 0 && *(cmd_str_copy + i) != ' ') {
			cmd_args[j] = &cmd_str_copy[i]; j++;
		}
	}
	cmd_args[j] = NULL;

	pid = fork();
	if (pid > 0) goto parent;
	else if (pid < 0) goto err;
	execv(cmd_args[0], cmd_args); //only returns on error
err:
	lua_pushnumber(L, -1);
	return 1;
parent:
	lua_pushinteger(L, pid);
	return 1;
}

int l_subprocess_is_alive(lua_State *L)
{
	int pid, r, child_status;

	pid = luaL_checkinteger(L, 1);
	r = waitpid(pid, &child_status, WNOHANG);
	if (r == pid) goto dead;
	else if (r == 0) goto alive;
// this is an error
	lua_pushnumber(L, -1);
	return 1;
alive:
	lua_pushnumber(L, 1);
	return 1;
dead:
	lua_pushnumber(L, 0);
	return 1;
}

int l_setenv(lua_State *L)
{
	char *k, *v;

	k = (char *)luaL_checkstring(L, 1);
	v = (char *)luaL_checkstring(L, 2);
	if (setenv(k, v, 1) != 0) goto err;
	lua_pushinteger(L, 0);
	return 1;
err:
	lua_pushinteger(L, -1);
	return 1;
}

int l_fork(lua_State *L)
{
	lua_pushinteger(L, fork());
	return 1;
}

int l_update_crc(lua_State *L)
{
	unsigned long crc;
	unsigned char *buff;
	size_t bufflen;

	crc=luaL_checkinteger(L, 1);
	buff = (unsigned char *)luaL_checklstring(L, 2, &bufflen);
	lua_pushinteger(L, update_crc(crc, buff, bufflen));
	return 1;
}

static const struct luaL_Reg rkutil_reg[] = {
	{ "sleep",		 l_sleep	       },
	{ "subprocess_start",	 l_subprocess_start    },
	{ "subprocess_is_alive", l_subprocess_is_alive },
	{ "setenv",		 l_setenv	       },
	{ "fork",		 l_fork		       },
	{ "update_crc",		 l_update_crc		       },
	{ NULL,			 NULL		       }
};

LUAMOD_API int luaopen_rkutil(lua_State *L)
{
	luaL_newlib(L, rkutil_reg);
	return 1;
}
