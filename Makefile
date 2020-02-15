CPPFLAGS=-arch x86_64 -m64

DEBUG=-g -DDEBUG
OPT=-O2
OPT_SIZE=-O3 -Os -fno-threadsafe-statics -fno-exceptions -ffunction-sections -fdata-sections -fno-rtti -flto -fvisibility-inlines-hidden

all: clean tias disassemble test distance

tias: tias.cpp
	g++ tias.cpp $(CPPFLAGS) $(OPT_SIZE) -o tias
	g++ tias.cpp $(DEBUG) $(CPPFLAGS) -o tias-debug
	strip -no_uuid -A -u -S -X -N -x tias

disassemble: disassemble.cpp
	g++ disassemble.cpp $(CPPFLAGS) $(OPT_SIZE) -o disassemble
	strip -no_uuid -A -u -S -X -N -x disassemble

formulas: formulas.asm
	./tias formulas.asm formulas.8xp

convop1: convop1.asm
	./tias convop1.asm convop1.8xp

testconv: testconv.asm
	./tias testconv.asm testconv.8xp

test: test.asm
	./tias test.asm test.8xp
	./tias loop-test.asm loop-test.8xp
	./tias input-test.asm input-test.8xp
	./tias repeat-test.asm repeat-test.8xp

qform: qform.asm
	./tias qform2.asm qform.8xp

z80c: z80c.c
	sdcc -mz80 -c --no-std-crt0 --code-loc 0x9D95 z80c.c
	rm -f z80c.hex
	rm -f z80c.sym
	rm -f z80c.rel

distance: distance.asm
	./tias distance.asm distance.8xp

clean:
	rm -f ./disassemble
	rm -f ./tias
	rm -f ./tias-debug
	rm -f ./test.8xp
	rm -f ./loop-test.8xp
	rm -f ./input-test.8xp
	rm -f ./distance.8xp
	rm -f *.*~
	rm -f *~
	rm -f *.ihx
	rm -f *.map
	rm -f *.mem
	rm -f *.rst
	rm -f *.hxs
	rm -f *.lk
	rm -f *.noi
	rm -f *.rei
	rm -f *.rel
	rm -f *.sym

