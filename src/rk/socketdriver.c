/**
 * @file lua_socketdriver.c
 * @author Josh Righetti
 * @date 2022
 */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "rw_it.h"
#include "netstring.h"

struct _sock_ctx_t {
	int		sock;
};
typedef struct _sock_ctx_t _sock_ctx_t;

// Helper function for reading with O_NONBLOCK set. Catches EWOULDBLOCK and
// EAGAIN so that we don't have to deal with them.
static inline ssize_t safe_read_it(int fd, void* buf, int count) {
	ssize_t i = read_it(fd, buf, count);
	if(i < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
		return 0; // This is true. There is no data.
	}
	return i; // Otherwise just give the actual value of i.
}

int l_socketdriver_tcp_open_client(lua_State *L)
{
	struct sockaddr_in sa;
	char *server_addr;
	lua_Integer server_port;
	_sock_ctx_t *r;

	server_addr = (char *)luaL_checkstring(L, 1);
	server_port = luaL_checkinteger(L, 2);

	r = lua_newuserdata(L, sizeof(_sock_ctx_t));

	r->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (r->sock <= 0) goto erra;

	sa.sin_addr.s_addr = inet_addr(server_addr);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(server_port);

	//Connect to remote server
	if (connect(r->sock, (struct sockaddr *)&(sa), sizeof(sa)) < 0) goto errb;

	/* return sock user data*/
	return 1;

erra:
	fprintf(stderr, "error opening socket\n");
	lua_pushnil(L);
	return 1;

errb:
	fprintf(stderr, "socket connect failed");
	lua_pushnil(L);
	return 1;
}

int l_socketdriver_tcp_open_server(lua_State *L)
{
	int iso;
	struct sockaddr_in sa;
	lua_Integer server_port;
	_sock_ctx_t *r;

	server_port = luaL_checkinteger(L, 1);

	r = lua_newuserdata(L, sizeof(_sock_ctx_t));

	r->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (r->sock <= 0) goto erra;

	iso = 1;
	setsockopt(r->sock, SOL_SOCKET, SO_REUSEADDR, (char *)&iso, sizeof(iso));

	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(server_port);

	if (bind(r->sock, (struct sockaddr *)&(sa), sizeof(sa)) < 0) goto errb;

	if (listen(r->sock, 10) != 0) goto errc;

	/* return sock user data*/
	return 1;

erra:
	fprintf(stderr, "error opening socket\n");
	lua_pushnil(L);
	return 1;

errb:
	fprintf(stderr, "socket bind failed");
	lua_pushnil(L);
	return 1;

errc:
	fprintf(stderr, "socket listen failed");
	lua_pushnil(L);
	return 1;
}

int l_socketdriver_tcp_accept(lua_State *L)
{
	struct sockaddr_in sa;
	int c = sizeof(struct sockaddr_in);
	char ipstr[INET_ADDRSTRLEN];
	_sock_ctx_t *rin, *rout;

	assert(lua_isuserdata(L, 1));
	rin = lua_touserdata(L, 1);
	rout = lua_newuserdata(L, sizeof(_sock_ctx_t));

	/* enable blocking */
	int flags = fcntl(rin->sock, F_GETFL, 0);
	flags &= !O_NONBLOCK;
	assert(fcntl(rin->sock, F_SETFL, flags) == 0);

	rout->sock = accept(rin->sock,
			    (struct sockaddr *)&(sa), (socklen_t *)&(c));
	if (rout->sock < 0) goto erra;

	inet_ntop(AF_INET, (struct in_addr *)&(sa.sin_addr), ipstr, INET_ADDRSTRLEN);

	/* return sock user data*/
	lua_pushstring(L, ipstr);
	return 2;

erra:
	fprintf(stderr, "error accepting socket\n");
	lua_pushnil(L);
	return 1;
}

int l_socketdriver_tcp_accept_nonblocking(lua_State *L)
{
	struct sockaddr_in sa;
	int c = sizeof(struct sockaddr_in);
	char ipstr[INET_ADDRSTRLEN];
	_sock_ctx_t *rin, *rout;

	assert(lua_isuserdata(L, 1));
	rin = lua_touserdata(L, 1);
	rout = lua_newuserdata(L, sizeof(_sock_ctx_t));

	/* disable blocking */
	int flags = fcntl(rin->sock, F_GETFL, 0);
	flags |= O_NONBLOCK;
	assert(fcntl(rin->sock, F_SETFL, flags) == 0);

	rout->sock = accept(rin->sock,
			    (struct sockaddr *)&(sa), (socklen_t *)&(c));
	//if (errno == EAGAIN || errno == EWOULDBLOCK) goto erra;
	if (rout->sock < 0) goto errb;

	inet_ntop(AF_INET, (struct in_addr *)&(sa.sin_addr), ipstr, INET_ADDRSTRLEN);

	/* return sock user data*/
	lua_pushstring(L, ipstr);
	return 2;

//erra:
//	lua_pushnil(L);
//	return 1;

errb:
	//fprintf(stderr, "error accepting socket\n");
	lua_pushnil(L);
	return 1;
}

int l_socketdriver_tcp_read(lua_State *L)
{
	char *buff;
	lua_Integer bl;
	luaL_Buffer b;
	_sock_ctx_t *r;

	assert(lua_isuserdata(L, 1));
	r = lua_touserdata(L, 1);

	bl = luaL_checkinteger(L, 2);
	buff = luaL_buffinitsize(L, &b, bl);
	luaL_pushresultsize(&b, safe_read_it(r->sock, buff, bl));
	return 1;
}

int l_socketdriver_tcp_read_nonblocking(lua_State *L)
{
	char *buff;
	lua_Integer bl;
	luaL_Buffer b;
	_sock_ctx_t *r;

	assert(lua_isuserdata(L, 1));
	r = lua_touserdata(L, 1);

	/* disable blocking */
	int flags = fcntl(r->sock, F_GETFL, 0);
	flags |= O_NONBLOCK;
	if(fcntl(r->sock, F_SETFL, flags) != 0) {
		lua_pushnil(L);
		return 1;
	}

	bl = luaL_checkinteger(L, 2);
	buff = luaL_buffinitsize(L, &b, bl);
	luaL_pushresultsize(&b, safe_read_it(r->sock, buff, bl));

	/* enable blocking */
	flags &= !O_NONBLOCK;
	assert(fcntl(r->sock, F_SETFL, flags) == 0);

	return 1;
}

int l_socketdriver_tcp_readline(lua_State *L)
{
	char *buff;
	luaL_Buffer b;
	_sock_ctx_t *r;
	const int max = 1024;
	int i;

	assert(lua_isuserdata(L, 1));
	r = lua_touserdata(L, 1);

	buff = luaL_buffinitsize(L, &b, max);
	for (i = 0;; i++) {
		if (i >= max) goto erra;
		if (safe_read_it(r->sock, buff + i, 1) != 1) goto errb;
		if (buff[i] == '\n') break;
	}
	luaL_pushresultsize(&b, i);
	return 1;
erra:
	fprintf(stderr, "line greater than %d\n", max);
	lua_pushnil(L);
	return 1;

errb:
	fprintf(stderr, "read failed");
	lua_pushnil(L);
	return 1;
}

int l_socketdriver_tcp_read_netstring(lua_State *L)
{
	char *buff;
	unsigned long bl;
	luaL_Buffer b;
	_sock_ctx_t *r;

	assert(lua_isuserdata(L, 1));
	r = lua_touserdata(L, 1);

	buff = read_netstring_fd(r->sock, &bl);
	luaL_buffinit(L, &b);
	luaL_addlstring(&b, buff, bl);
	free(buff);
	luaL_pushresult(&b);
	return 1;
}

int l_socketdriver_tcp_write(lua_State *L)
{
	size_t pl;
	char *p;
	_sock_ctx_t *r;

	assert(lua_isuserdata(L, 1));
	r = lua_touserdata(L, 1);

	p = (char *)luaL_checklstring(L, 2, &pl);
	lua_pushinteger(L, write_it(r->sock, p, pl));
	return 1;
}

int l_socketdriver_tcp_write_netstring(lua_State *L)
{
	size_t pl;
	char *p;
	_sock_ctx_t *r;

	assert(lua_isuserdata(L, 1));
	r = lua_touserdata(L, 1);

	p = (char *)luaL_checklstring(L, 2, &pl);
	lua_pushinteger(L, write_netstring_fd(r->sock, p, pl));
	return 1;
}

int l_socketdriver_tcp_close(lua_State *L)
{
	_sock_ctx_t *r;

	assert(lua_isuserdata(L, 1));
	r = lua_touserdata(L, 1);
	close(r->sock);
	return 0;
}

int l_socketdriver_udp_open_client(lua_State *L)
{
	_sock_ctx_t *r;

	r = lua_newuserdata(L, sizeof(_sock_ctx_t));
	if ((r->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) goto erra;
	return 1;

erra:
	fprintf(stderr, "error opening socket\n");
	lua_pushnil(L);
	return 1;
}

int l_socketdriver_udp_write(lua_State *L)
{
	_sock_ctx_t *r;
	struct sockaddr_in sa;
	int slen = sizeof(sa);
	char *host, *p;
	int port;
	size_t pl;

	assert(lua_isuserdata(L, 1));
	r = lua_touserdata(L, 1);
	host = (char *)luaL_checkstring(L, 2);
	port = luaL_checkinteger(L, 3);
	p = (char *)luaL_checklstring(L, 4, &pl);

	memset((char *)&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	if (inet_aton(host, &sa.sin_addr) == 0) goto erra;
	lua_pushinteger(L, sendto(r->sock, p, pl, 0,
				  (const struct sockaddr *)&sa, slen));
	return 1;
erra:
	fprintf(stderr, "error configuring socket\n");
	lua_pushnil(L);
	return 1;
}

int l_socketdriver_udp_write_netstring(lua_State *L)
{
	_sock_ctx_t *r;
	struct sockaddr_in sa;
	int slen = sizeof(sa);
	char *host, *p, *ns;
	int port;
	size_t pl;
	luaL_Buffer b;
	int nsl;

	assert(lua_isuserdata(L, 1));
	r = lua_touserdata(L, 1);
	host = (char *)luaL_checkstring(L, 2);
	port = luaL_checkinteger(L, 3);
	p = (char *)luaL_checklstring(L, 4, &pl);

	ns = (char *)luaL_buffinitsize(L, &b, pl + 20);
	nsl = write_netstring_buff(p, pl, ns, pl + 20);

	memset((char *)&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	if (inet_aton(host, &sa.sin_addr) == 0) goto erra;
	lua_pushinteger(L, sendto(r->sock, ns, nsl, 0,
				  (const struct sockaddr *)&sa, slen));
	return 1;
erra:
	fprintf(stderr, "error configuring socket\n");
	lua_pushnil(L);
	return 1;
}

int l_socketdriver_udp_open_server(lua_State *L)
{
	_sock_ctx_t *r;
	struct sockaddr_in sa;
	lua_Integer server_port;

	server_port = luaL_checkinteger(L, 1);

	r = lua_newuserdata(L, sizeof(_sock_ctx_t));

	r->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (r->sock == -1) goto erra;

	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(server_port);

	if (bind(r->sock, (struct sockaddr *)&sa, sizeof(sa)) < 0) goto errb;

	return 1;

erra:
	fprintf(stderr, "error opening socket\n");
	lua_pushnil(L);
	return 1;

errb:
	fprintf(stderr, "error binding socket\n");
	lua_pushnil(L);
	return 1;
}

int l_socketdriver_disable_sigpipe(lua_State *L)
{
	signal(SIGPIPE, SIG_IGN);
	return 0;
}

static const struct luaL_Reg socketdriver_reg[] = {
	{ "tcp_open_client",	    l_socketdriver_tcp_open_client },
	{ "tcp_open_server",	    l_socketdriver_tcp_open_server },
	{ "tcp_accept",		    	l_socketdriver_tcp_accept },
	{ "tcp_accept_nonblocking", l_socketdriver_tcp_accept_nonblocking },
	{ "tcp_read",		    	l_socketdriver_tcp_read },
	{ "tcp_read_nonblocking",	l_socketdriver_tcp_read_nonblocking },
	{ "tcp_readline",	    	l_socketdriver_tcp_readline },
	{ "tcp_read_netstring",	    l_socketdriver_tcp_read_netstring },
	{ "tcp_write",		    	l_socketdriver_tcp_write },
	{ "tcp_write_netstring",    l_socketdriver_tcp_write_netstring },
	{ "tcp_close",		    	l_socketdriver_tcp_close },
	{ "udp_open_client",	    l_socketdriver_udp_open_client },
	{ "udp_write",		    	l_socketdriver_udp_write },
	{ "udp_write_netstring",    l_socketdriver_udp_write_netstring },
	{ "udp_close",		    	l_socketdriver_tcp_close },
	{ "udp_open_server",	    l_socketdriver_udp_open_server },
	{ "udp_read",		    	l_socketdriver_tcp_read	},
	{ "udp_read_nonblocking", 	l_socketdriver_tcp_read_nonblocking },
	{ "disable_sigpipe",	    l_socketdriver_disable_sigpipe },
	{ NULL,			    		NULL }
};

LUAMOD_API int luaopen_socketdriver(lua_State *L)
{
	luaL_newlib(L, socketdriver_reg);
	return 1;
}
