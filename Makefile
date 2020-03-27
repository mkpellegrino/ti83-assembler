CPPFLAGS=-arch x86_64 -m64

DEBUG=-g -DDEBUG
OPT=-O2
OPT_SIZE=-O3 -Os -fno-threadsafe-statics -fno-exceptions -ffunction-sections -fdata-sections -fno-rtti -flto -fvisibility-inlines-hidden

all: clean tias disassemble distance

tias: tias.cpp
	g++ tias.cpp $(CPPFLAGS) $(OPT_SIZE) -o tias
	g++ tias.cpp $(DEBUG) $(CPPFLAGS) -o tias-debug
	strip -no_uuid -A -u -S -X -x tias

disassemble: disassemble.cpp
	g++ disassemble.cpp $(CPPFLAGS) $(OPT_SIZE) -o disassemble
	strip -no_uuid -A -u -S -X -x disassemble

formulas: formulas.asm
	./tias formulas.asm formulas.8xp

factors: factors.asm
	./tias factors.asm factors.8xp

times: times16_to_the_x.asm times10_to_the_x.asm str2hex.asm
	./tias ./str2hex.asm ./str2hex.8xp

hexinput: hex-input.asm
	./tias hex-input.asm hex-input.8xp

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

