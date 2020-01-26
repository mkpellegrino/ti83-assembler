CPPFLAGS=-arch x86_64 -m64

DEBUG=-g -DDEBUG
OPT=-O2
OPT_SIZE=-O3 -Os -fno-threadsafe-statics -fno-exceptions -ffunction-sections -fdata-sections -fno-rtti -flto -fvisibility-inlines-hidden

all: clean ticc debug 

ticc: ticc.cpp
	g++ ticc.cpp $(CPPFLAGS) $(OPT_SIZE) -o ticc
	strip -no_uuid -A -u -S -X -N -x ticc

debug: ticc.cpp
	g++ ticc.cpp $(DEBUG) $(CPPFLAGS) -o ticc-debug

clean:
	rm -f ./ticc
	rm -f ./ticc-debug



