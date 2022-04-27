#general stuff
###########################################################################
DBUILD=./build
FBUILD=$(DBUILD)/build
DAUTOGEN=$(DBUILD)/autogen
OBJ= $(DBUILD)/x86/obj
BIN= $(DBUILD)/x86/bin
MACBIN=$(DBUILD)/x86/macbin
DTEST=$(DBUILD)/test
DLUAC=$(DBUILD)/luac

$(FBUILD):
	mkdir -p $(DBUILD)
	mkdir -p $(DAUTOGEN)
	mkdir -p $(OBJ)
	mkdir -p $(BIN)
	mkdir -p $(MACBIN)
	mkdir -p $(DTEST)
	mkdir -p $(DLUAC)
	touch $@

clean:
	rm -rf $(DBUILD)

CFLAGS= -Wall -Werror -fPIC -g -fstack-protector-all
CFLAGS+= -ffast-math -std=gnu11

#x86 compiler stuff
###########################################################################
X86GCC= gcc
X86CFLAGS= $(CFLAGS)

#lua lib
###########################################################################
./3rd/lua/src/%.c: ./3rd/lua/src/%.h

LUADEF= -D LUA_USE_POSIX
LUADEF+= -D LUA_USE_READLINE
LUADEF+= -D readline=linenoise
LUADEF+= -D add_history=linenoiseHistoryAdd

$(OBJ)/%.o: ./3rd/linenoise/%.c
	$(X86GCC) -I./3rd/linenoise $(X86CFLAGS) -o $@ -c $<

$(OBJ)/%.o: ./3rd/lua/src/%.c
	$(X86GCC) $(LUADEF) -I./3rd/lua/src $(X86CFLAGS) -o $@ -c $<

LUACOREO=	lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o \
	llex.o lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o \
	ltable.o ltm.o lundump.o lvm.o lzio.o linenoise.o

LUALIBO=	lauxlib.o lbaselib.o lcorolib.o ldblib.o liolib.o lmathlib.o \
	loadlib.o loslib.o lstrlib.o ltablib.o lutf8lib.o

$(OBJ)/lua.a: $(addprefix $(OBJ)/, $(LUACOREO)) $(addprefix $(OBJ)/, $(LUALIBO))
	ar rvs $@ $^

$(MACBIN)/lua: $(OBJ)/lua.o $(OBJ)/linit.o $(OBJ)/lua.a
	$(X86GCC) $(LUADEF) -I./3rd/lua/src $(X86CFLAGS) \
	-o $@ $^ $(addprefix $(OBJ)/, $(LUAO))

$(MACBIN)/luac: $(OBJ)/luac.o $(OBJ)/lua.a
	$(X86GCC) $(LUADEF) -I./3rd/lua/src $(X86CFLAGS) \
	-o $@ $^ $(addprefix $(OBJ)/, $(LUAO))

#misc lib
###########################################################################
./3rd/misc/%.c: ./3rd/misc/%.h

$(OBJ)/%.o: ./3rd/misc/%.c
	$(X86GCC) -I./3rd/misc $(X86CFLAGS) -o $@ -c $<

MISCLIBO= md5.o crc32.o

$(OBJ)/misc.a: $(addprefix $(OBJ)/, $(MISCLIBO))
	ar rvs $@ $^

#blowfish lib
###########################################################################
./3rd/bfsh-koc/%.c: ./3rd/bfsh-koc/%.h

$(OBJ)/%.o: ./3rd/bfsh-koc/%.c
	$(X86GCC) -I./3rd/bfsh-koc $(X86CFLAGS) -o $@ -c $<

BLOWFISHLIBO= blowfish.o

$(OBJ)/blowfish.a: $(addprefix $(OBJ)/, $(BLOWFISHLIBO))
	ar rvs $@ $^

#minilzo lib
###########################################################################
./3rd/minilzo/%.c: ./3rd/minilzo/%.h

$(OBJ)/%.o: ./3rd/minilzo/%.c
	$(X86GCC) -I./3rd/minilzo $(X86CFLAGS) -o $@ -c $<

MINLZOLIBO= minilzo.o

$(OBJ)/minilzo.a: $(addprefix $(OBJ)/, $(MINLZOLIBO))
	ar rvs $@ $^

#junzip
###########################################################################
./3rd/junzip/%.c: ./3rd/junzip/%.h

$(OBJ)/%.o: ./3rd/junzip/%.c
	$(X86GCC) -DHAVE_PUFF -I./3rd/junzip $(X86CFLAGS) -o $@ -c $<

MINLZOLIBO= junzip.o puff.o

$(OBJ)/junzip.a: $(addprefix $(OBJ)/, $(MINLZOLIBO))
	ar rvs $@ $^

# $(MACBIN)/junzip_dump: $(OBJ)/junzip_dump.o $(OBJ)/junzip.a
# 	$(X86GCC) -DHAVE_PUFF -I./3rd/junzip $(X86CFLAGS) \
# 	-o $@ $^

#sqlite
###########################################################################
./3rd/sqlite-amalgamation/%.c: ./3rd/sqlite-amalgamation/%.h

SQLITEDEF= -DSQLITE_ENABLE_FTS3
SQLITEDEF+= -DSQLITE_ENABLE_FTS5
SQLITEDEF+= -DSQLITE_ENABLE_RTREE
SQLITEDEF+= -DSQLITE_ENABLE_DBSTAT_VTAB
SQLITEDEF+= -DSQLITE_ENABLE_JSON1
SQLITEDEF+= -DSQLITE_ENABLE_RBU
SQLITEDEF+= -DSQLITE_ENABLE_SESSION

$(OBJ)/%.o: ./3rd/sqlite-amalgamation/%.c
	$(X86GCC) $(SQLITEDEF) -I./3rd/sqlite-amalgamation $(X86CFLAGS) -o $@ -c $<

SQLITELIBO= sqlite3.o

$(OBJ)/sqlite3.a: $(addprefix $(OBJ)/, $(SQLITELIBO))
	ar rvs $@ $^

#rocket knife lua
###########################################################################
./3rd/rk/%.c: ./3rd/rk/%.h

RKINC= -I./3rd/lua/src
RKINC+= -I./3rd/minilzo
RKINC+= -I./3rd/bfsh-koc
RKINC+= -I./3rd/junzip
RKINC+= -I./3rd/sqlite-amalgamation
RKINC+= -I./3rd/rk
RKINC+= -I./3rd/misc

$(OBJ)/%.o: ./rk/%.c
	$(X86GCC) $(LUADEF) $(RKINC) $(X86CFLAGS) -o $@ -c $<

RKLUAO= rklinit.o rkcrypt.o rkcompress.o rkunzip.o rksqlite.o \
	rw_it.o netstring.o socketdriver.o rkutil.o rkmd5.o

$(OBJ)/rklua.a: $(addprefix $(OBJ)/, $(RKLUAO))
	ar rvs $@ $^

$(MACBIN)/rk_: $(OBJ)/rklua.o $(OBJ)/lua.a $(OBJ)/rklua.a \
	$(OBJ)/blowfish.a $(OBJ)/minilzo.a $(OBJ)/sqlite3.a $(OBJ)/junzip.a $(OBJ)/misc.a
	$(X86GCC) $(LUADEF) $(RKINC) $(X86CFLAGS) -o $@ $^; \

$(MACBIN)/rk: $(MACBIN)/rk_ $(DBUILD)/lua.zip
	cat $^ > $@; chmod a+x $@

# lua zip
###########################################################################
RKLUAC= rkrpc.luac rkuuid.luac

$(DLUAC)/%.luac: ./rklua/%.lua
	$(MACBIN)/luac -o $@ $^

$(DBUILD)/lua.zip: $(MACBIN)/rk_ $(addprefix $(DLUAC)/, $(RKLUAC))
	python ./scripts/ziplua.py $@ $^

#test
###########################################################################
test: mac
	export TESTDIR=$(DTEST); \
	python -u -m unittest discover tests

#all
###########################################################################
.DEFAULT_GOAL := all
.PHONEY: clean x86 mac all test

zip: $(FBUILD) \
	$(MACBIN)/luac \
	$(MACBIN)/rk_ \
	$(DBUILD)/lua.zip

mac: $(FBUILD) \
	$(MACBIN)/lua \
	$(MACBIN)/luac \
	$(MACBIN)/rk_ \
	zip \
	$(MACBIN)/rk

all: mac
