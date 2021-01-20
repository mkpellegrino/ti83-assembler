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

all: clean tias disassemble 

tias: tias.cpp
	$(value CXX) tias.cpp $(CPPFLAGS) $(INCLUDE) $(OPT) -o tias
	$(value CXX) tias.cpp $(DEBUG) $(CPPFLAGS) $(INCLUDE) -o tias-debug
	strip -no_uuid -A -u -S -X -N -x tias

disassemble: disassemble.cpp
	$(value CXX) disassemble.cpp $(CPPFLAGS) $(OPT) -o disassemble
	$(value CXX) disassemble.cpp $(DEBUG) $(CPPFLAGS) $(INCLUDE) -o disassemble-debug
	strip -no_uuid -A -u -S -X -N -x disassemble

#create-raw: create-raw.cpp
#	$(value CXX) create-raw.cpp $(CPPFLAGS) $(OPT) -o create-raw
#	strip -no_uuid -A -u -S -X -N -x create-raw

#lexer:	lexer.cpp
#	$(value CXX) lexer.cpp $(CPPFLAGS) $(OPT) -o lexer
#	$(value CXX) lexer.cpp $(DEBUG) $(CPPFLAGS) $(INCLUDE) -o lexer-debug
#	strip -no_uuid -A -u -S -X -N -x lexer

#decks: deckcard.asm decktiny.asm
#	./tias deckcard.asm deckcard.8xp
#	./tias decktiny.asm decktiny.8xp

#formulas: formulas.asm
#	./tias formulas.asm formulas.8xp formulas.mem

#factors: factors.asm
#	./tias factors.asm factors.8xp factors.mem

#hexinput: hex-input.asm
#	./tias hex-input.asm hex-input.8xp hex-input.mem

#looptest: looptest.asm
#	./tias looptest.asm looptest.8xp

#bill: bill.asm
#	./tias bill.asm splitchk.8xp

#qform: qform.asm
#	./tias qform2.asm qform.8xp

#distance: distance.asm
#	./tias distance.asm distance.8xp distance.mem

clean:
	rm -f ./tias
	rm -f ./disassemble
	rm -f ./*-debug
	rm -f *.*~
	rm -f *~

