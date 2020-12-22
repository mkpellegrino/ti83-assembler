CPPFLAGS=-arch x86_64 -m64
#-I/usr/include
#PATH=/opt/local/bin:/usr/local/bin:./:/bin
#INCLUDE=-I/usr/include -I/usr/local/include -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include
#INCLUDE=-I/usr/local/include/c++/10.1.0/

#INCLUDE=-I/Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk/usr/include -I/Library/Developer/CommandLineTools/usr/include/c++/v1/

#CXX=/usr/bin/c++
#CC=/usr/bin/cc
#CXX=/opt/local/bin/g++-10.1.0
#CC=/opt/local/bin/gcc-10.1.0
#CC=gcc

#LD_LIBRARY_PATH=/usr/local/lib
#LIBRARY_PATH=/usr/local/lib
DEBUG=-g -DDEBUG
OPT=-O2
#OPT_SIZE=-O3 -Os -fno-threadsafe-statics -fno-exceptions -ffunction-sections -fdata-sections -fno-rtti -flto -fvisibility-inlines-hidden

all: clean tias disassemble distance

tias: tias.cpp
	$(value CXX) tias.cpp $(CPPFLAGS) $(INCLUDE) $(OPT) -o tias
	$(value CXX) tias.cpp $(DEBUG) $(CPPFLAGS) $(INCLUDE) -o tias-debug
	strip -no_uuid -A -u -S -X -N -x tias

create-raw: create-raw.cpp
	$(value CXX) create-raw.cpp $(CPPFLAGS) $(OPT) -o create-raw
	strip -no_uuid -A -u -S -X -N -x create-raw

dis2:	dis2.cpp
	$(value CXX) dis2.cpp $(CPP_FLAGS) $(OPT) -o dis2
	$(value CXX) dis2.cpp $(DEBUG) $(CPPFLAGS) $(INCLUDE) -o dis2-debug

lexer:	lexer.cpp
	$(value CXX) lexer.cpp $(CPPFLAGS) $(OPT) -o lexer
	$(value CXX) lexer.cpp $(DEBUG) $(CPPFLAGS) $(INCLUDE) -o lexer-debug
	strip -no_uuid -A -u -S -X -N -x lexer

disassemble: disassemble.cpp
	$(value CXX) disassemble.cpp $(CPPFLAGS) $(OPT) -o disassemble
	strip -no_uuid -A -u -S -X -N -x disassemble

decks: deck2.asm deck3.asm deck4.asm deck5.asm
	./tias deck2.asm deck2.8xp deck2.mem
	./tias deck4.asm deck4.8xp deck4.mem
	./tias deck5.asm deck5.8xp deck5.mem

array:	array.asm
	./tias array.asm array.8xp array.mem

twoloop: twoloop.asm
	./tias twoloop.asm twoloop.8xp twoloop.mem

formulas: formulas.asm
	./tias formulas.asm formulas.8xp formulas.mem

factors: factors.asm
	./tias factors.asm factors.8xp factors.mem

multiply: multipliers.asm multipliers.mem
	./tias multipliers.asm multiply.8xp

times: times16_to_the_x.asm times10_to_the_x.asm str2hex.asm
	./tias ./str2hex.asm ./str2hex.8xp 

hexinput: hex-input.asm
	./tias hex-input.asm hex-input.8xp hex-input.mem

ascii: ascii.asm
	./tias ascii.asm ascii.8xp

bill: bill.asm
	./tias bill.asm splitchk.8xp

qform: qform.asm
	./tias qform2.asm qform.8xp

a09: a09.c
	gcc -O4 ./a09.c -o a09

z80malloc: z80malloc.c
	sdcc -mz80 --no-std-crt0 --vc  -I /Users/mpellegrino/src/sdcc/share/sdcc/include ./z80malloc.c -o ./z80malloc

z80c: z80c.c
	sdcc -mz80 --no-std-crt0 --vc --code-loc 16384 -I /Users/mpellegrino/src/sdcc/share/sdcc/include ./z80c.c -o ./z80c
#	zcc +zx -vn ./z80c.c -o z80c -lndos

#-Cz--bb6d ?
#	sdcc -I /Users/mpellegrino/src/sdcc/share/sdcc/include ./z80c.c -o ./z80c
#	zcc +ti8x -startup=10 -Cz--altfmt z80c.c -o z80c.bin
#	bin2var z80c.bin Z80C.8xp 
#	z88dk-appmake +ti8x -b z80c -o Z80C 
#	zcc +ti8x --list -mz80 -lm -o Z80TI.8xp -startup=10 -Cz--altfmt z80c.c
#	zcc +ti8xansi -lm -o z80c z80c.c

distance: distance.asm
	./tias distance.asm distance.8xp distance.mem

clean:
	rm -f ./disassemble
	rm -f ./tias
	rm -f ./tias-debug
	rm -f ./deck*.8xp
	rm -f ./twoloop.8xp
	rm -f ./test.8xp
	rm -f ./loop-test.8xp
	rm -f ./input-test.8xp
	rm -f ./distance.8xp
	rm -f ./qform.8xp
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

