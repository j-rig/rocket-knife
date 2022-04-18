#general stuff
###########################################################################
DBUILD=./build
FBUILD=$(DBUILD)/build
DAUTOGEN=$(DBUILD)/autogen
OBJ= $(DBUILD)/x86/obj
BIN= $(DBUILD)/x86/bin

$(FBUILD):
	mkdir -p $(DBUILD)
	mkdir -p $(DAUTOGEN)
	mkdir -p $(OBJ)
	mkdir -p $(BIN)
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

$(OBJ)/%.o: ./3rd/lua/src/%.c
	$(X86GCC) $(LUADEF) -I./3rd/lua/src $(X86CFLAGS) -o $@ -c $<

LUACOREO=	lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o \
	llex.o lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o \
	ltable.o ltm.o lundump.o lvm.o lzio.o

LUALIBO=	lauxlib.o lbaselib.o lcorolib.o ldblib.o liolib.o lmathlib.o \
	loadlib.o loslib.o lstrlib.o ltablib.o lutf8lib.o linit.o

$(OBJ)/lua.a: $(addprefix $(OBJ)/, $(LUACOREO)) $(addprefix $(OBJ)/, $(LUALIBO))
	ar rvs $@ $^

$(BIN)/lua: $(OBJ)/lua.o $(OBJ)/lua.a
	$(X86GCC) -static $(LUADEF) -I./3rd/lua/src $(X86CFLAGS) \
	-o $@ $^

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

#all
###########################################################################
.DEFAULT_GOAL := all
.PHONEY: clean x86 all

x86: $(FBUILD) \
	$(OBJ)/lua.a \
	$(OBJ)/blowfish.a \
	$(OBJ)/minilzo.a \
	$(BIN)/lua

all: x86
