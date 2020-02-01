CPPFLAGS=-arch x86_64 -m64

DEBUG=-g -DDEBUG
OPT=-O2
OPT_SIZE=-O3 -Os -fno-threadsafe-statics -fno-exceptions -ffunction-sections -fdata-sections -fno-rtti -flto -fvisibility-inlines-hidden

all: clean ticc create debug disect

ticc: ticc.cpp
	g++ ticc.cpp $(CPPFLAGS) $(OPT_SIZE) -o ticc
	strip -no_uuid -A -u -S -X -N -x ticc

create: create-asm-prog.cpp
	g++ create-asm-prog.cpp $(CPPFLAGS) $(OPT_SIZE) -o create-asm-prog
	g++ create-asm-prog.cpp $(DEBUG) $(CPPFLAGS) -o create-asm-prog-debug
	strip -no_uuid -A -u -S -X -N -x create-asm-prog

disassemble: disassemble.cpp
	g++ disassemble.cpp $(CPPFLAGS) $(OPT_SIZE) -o disassemble
	strip -no_uuid -A -u -S -X -N -x disassemble

debug: ticc.cpp
	g++ ticc.cpp $(DEBUG) $(CPPFLAGS) -o ticc-debug

clean:
	rm -f ./disect
	rm -f ./ticc
	rm -f ./ticc-debug
	rm -f ./create-asm-program
	rm -f ./create-asm-program-debug



