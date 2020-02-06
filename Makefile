CPPFLAGS=-arch x86_64 -m64

DEBUG=-g -DDEBUG
OPT=-O2
OPT_SIZE=-O3 -Os -fno-threadsafe-statics -fno-exceptions -ffunction-sections -fdata-sections -fno-rtti -flto -fvisibility-inlines-hidden

all: clean tias disassemble test

tias: tias.cpp
	g++ tias.cpp $(CPPFLAGS) $(OPT_SIZE) -o tias
	g++ tias.cpp $(DEBUG) $(CPPFLAGS) -o tias-debug
	strip -no_uuid -A -u -S -X -N -x tias

disassemble: disassemble.cpp
	g++ disassemble.cpp $(CPPFLAGS) $(OPT_SIZE) -o disassemble
	strip -no_uuid -A -u -S -X -N -x disassemble

test: test.asm
	./tias test.asm test.8xp
clean:
	rm -f ./disassemble
	rm -f ./tias
	rm -f ./tias-debug
	rm -f ./test.8xp


