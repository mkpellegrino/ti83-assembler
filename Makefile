CPPFLAGS=-arch x86_64 -m64

DEBUG=-g -DDEBUG
OPT=-O2
OPT_SIZE=-O3 -Os -fno-threadsafe-statics -fno-exceptions -ffunction-sections -fdata-sections -fno-rtti -flto -fvisibility-inlines-hidden

all: clean ticc debug disect create-test

ticc: ticc.cpp
	g++ ticc.cpp $(CPPFLAGS) $(OPT_SIZE) -o ticc
	strip -no_uuid -A -u -S -X -N -x ticc

create-asm-program: create-asm-program.cpp
	g++ create-asm-program.cpp $(CPPFLAGS) $(OPT_SIZE) -o create-asm-program
	strip -no_uuid -A -u -S -X -N -x create-asm-program

disect: disect.cpp
	g++ disect.cpp $(CPPFLAGS) $(OPT_SIZE) -o disect
	strip -no_uuid -A -u -S -X -N -x disect

debug: ticc.cpp
	g++ ticc.cpp $(DEBUG) $(CPPFLAGS) -o ticc-debug

clean:
	rm -f ./disect
	rm -f ./ticc
	rm -f ./ticc-debug



