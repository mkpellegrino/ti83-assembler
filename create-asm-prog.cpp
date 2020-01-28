#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>

using namespace std;

vector<unsigned char> byte_vector;

int memorylocation;
int start_counting=0;
int program_index=0;

class label;

vector<label> label_vector;
vector<label> label_cr_vector;

class label
{
public:
  label()
  {
    index=program_index;
    memory_location=memorylocation;
    label_number=-1;
#ifdef DEBUG
    cerr << "creating label: [" << hex << memory_location << "]" << endl;
#endif
  };
  label(int l)
  {
    index=program_index;
    memory_location=memorylocation;
    label_number=l;
#ifdef DEBUG
    cerr << "creating label: [prg]" << dec << index << " links to label#" << dec << label_number << endl;
#endif
  };
  ~label(){};
  int getIndex(){ return index; };
  int getMemoryLocation(){ return memory_location; };
  int gerLabelNumber(){ return label_number; };
  void add()
  {
#ifdef DEBUG
    cerr << "changing bytes: [" << index << " and " << index+1 << "] to ["
	 << hex << (label_cr_vector[label_number-1].getMemoryLocation()&0xFF) << " and "
	 << hex << ( (label_cr_vector[label_number-1].getMemoryLocation()&0xFF00 )/0xFF) << "]" <<  endl;
#endif 
    byte_vector[index] = (label_cr_vector[label_number-1].getMemoryLocation()&0xFF);// L
    byte_vector[index+1] = ((label_cr_vector[label_number-1].getMemoryLocation()&0xFF00)/0xFF );// H
  };
private:
  int index;
  int memory_location;
  int label_number;
};


void startCounting()
{
  start_counting=1;
  memorylocation=40341;
}

void addLabel()
{
  label l = label();
  label_cr_vector.push_back( l );
}

void addLabel( int i )
{
  label l = label(i);
  label_vector.push_back( l );
}

void addByte( unsigned char b )
{
  program_index++;
  if( start_counting == 1) memorylocation++;  
  byte_vector.push_back( (unsigned char) b );
}

void addWord( int w )
{
  addByte( w&0xFF );
  addByte( (w&0xFF00 )/0xFF );
}
void addWord()
{
  for( int i=0; i<2; i++ )
    {
      program_index++;
      if( start_counting == 1) memorylocation++;  
      byte_vector.push_back( (unsigned char) 0x00 );
    }
}
void addString( string s )
{
  for( int i=0; i<s.size(); i++ )
    {
      addByte( s[i] );
    }
  if( start_counting == 1 ) addByte( 0x00 );
}

void setComment( string s )
{
  s.resize(42,0x00);
  addString(s);
  
}
void setName( string s )
{
  s.resize(8,0x00);
  cerr << "NAME: " << s << endl;
  addString(s);
}

void sysCall( string s )
{
    addByte( 0xEF ); // rst $28
    if( s == "PutS")
      {
	addWord(0x450A);
      }
    else if( s == "NewLine" )
      {
	addWord(0x452E);
      }
    else if( s == "DispHL" )
      {
	addWord(0x4507);
      }
    else if( s == "CpOP1ToOP2" )
      {
	addWord(0x4111);
      }
    else if( s == "CpOP4ToOP3" )
      {
	addWord(0x4108);
      }
    else if( s == "Mov9ToOP1" )
      {
	addWord(0x417A);
      }
    else if( s == "Mov9ToOP2" )
      {
	addWord(0x4180);
      }
    else if( s == "Mov9B" )
      {
	addWord(0x415F);
      }
    else if( s == "OP1ExOP2" )
      {
	addWord(0x421F);
      }
    else if( s == "RandInit" )
      {
	addWord(0x4B7F);
      }
    else if( s == "Random" )
      {
	addWord(0x4B79);
      }
    else if( s == "RclX" )
      {
	addWord(0x4AE0);
      }
    else if( s == "StoX" )
      {
	addWord(0x4AD1);
      }

}

void assem( string s )
{
  if( s == "ret" )
    {
      addByte( 0xC9 );
    }
  else if( s == "ld hl,**" )
    {
      addByte( 0x21 );
    }
  else if( s == "nop" )
    {
      addByte( 0x00 );
    }
  else if( s == "ld bc, **" )
    {
      addByte(0x01);
    }
  else if( s == "ld (bc), a" )
    {
      addByte(0x02);
    }
  else if( s == "inc bc" )
    {
      addByte(0x03);
    }
  else if( s == "inc b" )
    {
      addByte(0x04);
    }
  else if( s == "dec b" )
    {
      addByte(0x05);
    }
  else if( s == "ld b, *" )
    {
      addByte(0x06);
    }
  else if( s == "rlca" )
    {
      addByte(0x07);
    }
  else if( s == "ex af, af'" )
    {
      addByte(0x08);
    }
  else if( s == "add hl, bc" )
    {
      addByte(0x09);
    }
  else if( s == "ld a, (bc)" )
    {
      addByte(0x0A);
    }
  else if( s == "dec bc" )
    {
      addByte(0x0B);
    }
  else if( s == "inc c" )
    {
      addByte(0x0C);
    }
  else if( s == "dec c" )
    {
      addByte(0x0D);
    }
  else if( s == "ld c, *" )
    {
      addByte(0x0E);
    }
  else if( s == "rrca" )
    {
      addByte(0x0F);
    }
  else if( s == "djnz *" )
    {
      addByte(0x10);
    }
  else if( s == "ld de, **" )
    {
      addByte(0x11);
    }
  else if( s == "ld (de), a" )
    {
      addByte(0x12);
    }
  else if( s == "inc de" )
    {
      addByte(0x13);
    }
  else if( s == "inc d" )
    {
      addByte(0x14);
    }
  else if( s == "dec d" )
    {
      addByte(0x15);
    }
  else if( s == "ld d, *" )
    {
      addByte(0x16);
    }
  else if( s == "rla" )
    {
      addByte(0x17);
    }
  else if( s == "jr *" )
    {
      addByte(0x18);
    }
  else if( s == "add hl, de" )
    {
      addByte(0x19);
    }
  else if( s == "ld a, (de)" )
    {
      addByte(0x1A);
    }
  else if( s == "dec de" )
    {
      addByte(0x1B);
    }
  else if( s == "inc e" )
    {
      addByte(0x1C);
    }
  else if( s == "dec e" )
    {
      addByte(0x1D);
    }
  else if( s == "ld e, *" )
    {
      addByte(0x1E);
    }
  else if( s == "rra" )
    {
      addByte(0x1F);
    }
  else if( s == "jr nz, *" )
    {
      addByte(0x20);
    }
  else if( s == "ld hl, **" )
    {
      addByte(0x21);
    }
  else if( s == "ld (**), hl" )
    {
      addByte(0x22);
    }
  else if( s == "inc hl" )
    {
      addByte(0x23);
    }
  else if( s == "inc h" )
    {
      addByte(0x24);
    }
  else if( s == "dec h" )
    {
      addByte(0x25);
    }
  else if( s == "ld h, *" )
    {
      addByte(0x26);
    }
  else if( s == "daa" )
    {
      addByte(0x27);
    }
  else if( s == "jr z, *" )
    {
      addByte(0x28);
    }
  else if( s == "add hl, hl" )
    {
      addByte(0x29);
    }
  else if( s == "ld hl, (**)" )
    {
      addByte(0x2A);
    }
  else if( s == "dec hl" )
    {
      addByte(0x2B);
    }
  else if( s == "inc l" )
    {
      addByte(0x2C);
    }
  else if( s == "dec l" )
    {
      addByte(0x2D);
    }
  else if( s == "ld l, *" )
    {
      addByte(0x2E);
    }
  else if( s == "cpl" )
    {
      addByte(0x2F);
    }
  else if( s == "" )
    {
      addByte(0x00);
    }

}
int main(int argc, char *argv[])
{  
  FILE *binary = fopen(argv[1], "wb");

  addString( "**TI83F*" );
  addByte( 0x1A );
  addByte( 0x0A );
  addByte( 0x00 );
  setComment( "http://mkpelleg.freeshell.org" );
  addByte( 0xFF ); // PROGRAM LENGTH - 57 (L)  (53) (This will get overwritten)
  addByte( 0xFF ); // PROGRAM LENGTH - 57 (H)  (54) (This will get overwritten)
  addByte( 0x0D ); 
  addByte( 0x00 );

  // PROGRAM START
  addByte( 0xFF ); // len of Machine Code + 2 bytes (L) (This will get overwritten)
  addByte( 0xFF ); // len of Machine Code + 2 bytes (H) (This will get overwritten)
  addByte( 0x06 );
  setName( "TEST1" ); // MUST BE 8 BYTES!!!
  addByte( 0x00 );
  addByte( 0x00 );
  addByte( 0xFF ); // len of Machine Code + 2 bytes (L)  (This will get overwritten)
  addByte( 0xFF ); // len of Machine Code + 2 bytes (H) (This will get overwritten)
  addByte( 0xFF ); // Machine Code Size (L)  (This will get overwritten)
  addByte( 0xFF ); // Machine Code Size (H) (This will get overwritten)

  addByte( 0xBB ); // Machine Code
  addByte( 0x6D );
  startCounting();
  // ==================================================
  assem( "ld hl, **" ); addLabel(1); addWord();
  sysCall("PutS");
  
  sysCall("NewLine");

  assem( "ld hl, **"); addLabel(2); addWord();
  sysCall("PutS");

  sysCall("NewLine");


  assem( "ld hl, **"); addWord( 0x1234 );
  sysCall( "DispHL" );
  
  assem( "ret" );

  // $9D9F
  addLabel(); addString( "Written in Asm" );
  addLabel(); addString( "x:" );

  // ==================================================
  // redo labels
  for( int i=0; i<label_vector.size(); i++ )
    {
      label_vector[i].add();
    }
  
  // Set Program Size-57
  int psH,psL=0;
  psH=((byte_vector.size()-55) & 0xFF00)/0xFF; // it's -55 (instead of -57) because we haven't added in the Checksum yet
  psL=((byte_vector.size()-55) & 0xFF); // it's -55 (instead of -57) because we haven't added in the Checksum yet
  byte_vector[53]=psL;
  byte_vector[54]=psH;

  // Set Machine Code Sizes -74
  int mcsH,mcsL=0;
  mcsH=((byte_vector.size()-72) & 0xFF00)/0xFF; // it's -72 (instead of -74) because we haven't added in the Checksum yet
  mcsL=((byte_vector.size()-72) & 0xFF); // it's -72 (instead of -74) because we haven't added in the Checksum yet
  byte_vector[57]=mcsL;
  byte_vector[58]=mcsH;
  byte_vector[70]=mcsL;
  byte_vector[71]=mcsH;
  // Set Just the Code Size
  mcsH=((byte_vector.size()-74) & 0xFF00)/0xFF;
  mcsL=((byte_vector.size()-74) & 0xFF);
  byte_vector[72]=mcsL;
  byte_vector[73]=mcsH;
  
  // calculate the checksum
  int cs=0;
  for( int i=55; i<byte_vector.size(); i++ )
    {
      cs+=byte_vector[i];
    }
  addByte( cs&0xFF );
  addByte( (cs & 0xFF00)/0xFF );

  for( int i=0; i<byte_vector.size(); i++ )
    {
      unsigned char byte_to_write = byte_vector[i];
      fwrite(&byte_to_write, 1, sizeof(byte_to_write), binary);
    }
  return 0;
}
