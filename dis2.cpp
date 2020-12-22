#include <iostream>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <string>
using namespace std;


string dec2Binary( int x )
{
  string retVal="";
  double t=x;
  while( t>0 )
    {
      t/=2;
      int r = 2*(t-(int)t);
      switch(r)
	{
	case 0:
	  retVal="0"+retVal;
	  break;
	case 1:
	  retVal="1"+retVal;
	}
      t=(int)t;
      
    }
  if( retVal.length() == 1 ) retVal="0" + retVal;
  if( retVal.length() == 3 ) retVal="0" + retVal;
  if( retVal.length() == 5 ) retVal="0" + retVal;
  if( retVal.length() == 7 ) retVal="0" + retVal;

  retVal+="b";
  return retVal;
}

string dec2Hex( int x, int digits = 2)
{
  string retVal="";
  if (x==0) retVal = string("00");
  double t=x;
  while( t>0 )
    {
      t/=16;
      int r = 16*(t-(int)t);
      switch(r)
	{
	case 0:
	  retVal="0"+retVal;
	  break;
	case 1:
	  retVal="1"+retVal;
	  break;
	case 2:
	  retVal="2"+retVal;
	  break;
	case 3:
	  retVal="3"+retVal;
	  break;
	case 4:
	  retVal="4"+retVal;
	  break;
	case 5:
	  retVal="5"+retVal;
	  break;
	case 6:
	  retVal="6"+retVal;
	  break;
	case 7:
	  retVal="7"+retVal;
	  break;
	case 8:
	  retVal="8"+retVal;
	  break;
	case 9:
	  retVal="9"+retVal;
	  break;
	case 10:
	  retVal="A"+retVal;
	  break;
	case 11:
	  retVal="B"+retVal;
	  break;
	case 12:
	  retVal="C"+retVal;
	  break;
	case 13:
	  retVal="D"+retVal;
	  break;
	case 14:
	  retVal="E"+retVal;
	  break;
	case 15:
	  retVal="F"+retVal;
	}
      t=(int)t;
      
    }
  if( retVal.length() == 1 ) retVal="0" + retVal;
  if( retVal.length() == 3 ) retVal="0" + retVal;
  if( retVal.length() == 5 ) retVal="0" + retVal;
  if( retVal.length() == 7 ) retVal="0" + retVal;

  while( retVal.length() < digits ) retVal="0" + retVal;
  
  retVal="0x"+retVal;

  return retVal;
}

class label
{
public:
  label(){address=0;name=string("");};
  label(int a, string n)
  {
    address=a;
    name=n;
  }
  
private:
  int address;
  string name;
};

class mneumonic
{
public:
  ~mneumonic(){};
  mneumonic( unsigned char a, unsigned char b, unsigned char c, unsigned char d );
  mneumonic( unsigned char a, unsigned char b, unsigned char c );
  mneumonic( unsigned char a, unsigned char b );
  mneumonic( unsigned char a );
  mneumonic();
  void print();
  bool isValid(){return valid;};
  string getInstruction(){return instruction;};
  int getSize(){return size;};
  void setData( unsigned char a ){bytes[0]=a;size=1;instruction=string(".db ") + dec2Hex( a, 2 );}
  void setAddress( int a ){ address = a; };

  friend ostream &operator << (ostream &out, const mneumonic &m); 

private:
  int size;
  unsigned char bytes[4];
  string label;
  int address;
  int ticks;
  string instruction;
  bool valid;
};


ostream & operator << (ostream &out, const mneumonic &m) 
{
  out << std::hex << m.address << " ||";
  for( int i=0; i<m.size; i++)
    {
      out << " " << dec2Hex(m.bytes[i],2) << " |";
    }
  for( int i=(4-m.size); i>0; i-- )
    {
      out << " ---- |";
    }
  out << "| " << m.instruction;
  out << endl;
  return out;
}

void mneumonic::print()
{
  cout << address << " | " << instruction << endl;
}

mneumonic::mneumonic()
{
#ifdef DEBUG
  cerr << "creating an empty data byte" << endl;
#endif
}

// 4 BYTE OPCODES
mneumonic::mneumonic( unsigned char a, unsigned char b, unsigned char c, unsigned char d )
{
#ifdef DEBUG
  cerr << "created mneumonic( " << (int)a << " , " << (int)b << " , " << (int)c << " , " << (int)d << " );" << endl;
#endif
  size=4;
  valid=true;
  bytes[0]=a; bytes[1]=b; bytes[2]=c; bytes[3]=d;
  switch(a)
    {
    case 0xDD:
      switch(b)
	{
	case 0x21:
	  instruction=string("ld ix, ") + dec2Hex(d*256+c,4);
	  ticks=14;
	  break;
	case 0x22:
	  instruction=string("ld (")+ dec2Hex(d*256+c,4) + "), ix";
	  ticks=20;
	  break;
	case 0x2A:
	  instruction=string("ld ix, (") + dec2Hex(d*256+c,4) + ")";
	  ticks=20;
	  break;
	case 0x36:
	  instruction=string("ld (ix+") + dec2Hex(c,2) + "), " + dec2Hex(d,2);
	  break;
	case 0xCB:
	  switch(d)
	    {
	    case 0x00:
	      instruction=string("rlc (ix+") + dec2Hex(c,2) + "), b";
	      ticks=23;
	      break;
	    case 0x01:
	      instruction=string("rlc (ix+") + dec2Hex(c,2) + "), c";
	      ticks=23;
	      break;
	    case 0x02:
	      instruction=string("rlc (ix+") + dec2Hex(c,2) + "), d";
	      ticks=23;
	      break;
	    case 0x03:
	      instruction=string("rlc (ix+") + dec2Hex(c,2) + "), e";
	      ticks=23;
	      break;
	    case 0x04:
	      instruction=string("rlc (ix+") + dec2Hex(c,2) + "), h";
	      ticks=23;
	      break;
	    case 0x05:
	      instruction=string("rlc (ix+") + dec2Hex(c,2) + "), l";
	      ticks=23;
	      break;
	    case 0x06:
	      instruction=string("rlc (ix+") +  dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0x07:
	      instruction=string("rlc (ix+") + dec2Hex(c,2) + "), a";
	      ticks=23;
	      break;
	    case 0x08:
	      instruction=string("rrc (ix+") + dec2Hex(c,2) + "), b";
	      ticks=23;
	      break;
	    case 0x09:
	      instruction=string("rrc (ix+") + dec2Hex(c,2) + "), c";
	      ticks=23;
	      break;
	    case 0x0A:
	      instruction=string("rrc (ix+") + dec2Hex(c,2) + "), d";
	      ticks=23;
	      break;
	    case 0x0B:
	      instruction=string("rrc (ix+") + dec2Hex(c,2) + "), e";
	      ticks=23;
	      break;
	    case 0x0C:
	      instruction=string("rrc (ix+") + dec2Hex(c,2) + "), h";
	      ticks=23;
	      break;
	    case 0x0D:
	      instruction=string("rrc (ix+") + dec2Hex(c,2) + "), l";
	      ticks=23;
	      break;
	    case 0x0E:
	      instruction=string("rrc (ix+") +  dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0x0F:
	      instruction=string("rrc (ix+") + dec2Hex(c,2) + "), a";
	      ticks=23;
	      break;
	      // ================

	    case 0x10:
	      instruction=string("rl (ix+") + dec2Hex(c,2) + "), b";
	      ticks=23;
	      break;
	    case 0x11:
	      instruction=string("rl (ix+") + dec2Hex(c,2) + "), c";
	      ticks=23;
	      break;
	    case 0x12:
	      instruction=string("rl (ix+") + dec2Hex(c,2) + "), d";
	      ticks=23;
	      break;
	    case 0x13:
	      instruction=string("rl (ix+") + dec2Hex(c,2) + "), e";
	      ticks=23;
	      break;
	    case 0x14:
	      instruction=string("rl (ix+") + dec2Hex(c,2) + "), h";
	      ticks=23;
	      break;
	    case 0x15:
	      instruction=string("rl (ix+") + dec2Hex(c,2) + "), l";
	      ticks=23;
	      break;
	    case 0x16:
	      instruction=string("rl (ix+") +  dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0x17:
	      instruction=string("rl (ix+") + dec2Hex(c,2) + "), a";
	      ticks=23;
	      break;
	    case 0x18:
	      instruction=string("rr (ix+") + dec2Hex(c,2) + "), b";
	      ticks=23;
	      break;
	    case 0x19:
	      instruction=string("rr (ix+") + dec2Hex(c,2) + "), c";
	      ticks=23;
	      break;
	    case 0x1A:
	      instruction=string("rr (ix+") + dec2Hex(c,2) + "), d";
	      ticks=23;
	      break;
	    case 0x1B:
	      instruction=string("rr (ix+") + dec2Hex(c,2) + "), e";
	      ticks=23;
	      break;
	    case 0x1C:
	      instruction=string("rr (ix+") + dec2Hex(c,2) + "), h";
	      ticks=23;
	      break;
	    case 0x1D:
	      instruction=string("rr (ix+") + dec2Hex(c,2) + "), l";
	      ticks=23;
	      break;
	    case 0x1E:
	      instruction=string("rr (ix+") +  dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0x1F:
	      instruction=string("rr (ix+") + dec2Hex(c,2) + "), a";
	      ticks=23;
	      break;

	      // ================
	    case 0x20:
	      instruction=string("sla (ix+") + dec2Hex(c,2) + "), b";
	      ticks=23;
	      break;
	    case 0x21:
	      instruction=string("sla (ix+") + dec2Hex(c,2) + "), c";
	      ticks=23;
	      break;
	    case 0x22:
	      instruction=string("sla (ix+") + dec2Hex(c,2) + "), d";
	      ticks=23;
	      break;
	    case 0x23:
	      instruction=string("sla (ix+") + dec2Hex(c,2) + "), e";
	      ticks=23;
	      break;
	    case 0x24:
	      instruction=string("sla (ix+") + dec2Hex(c,2) + "), h";
	      ticks=23;
	      break;
	    case 0x25:
	      instruction=string("sla (ix+") + dec2Hex(c,2) + "), l";
	      ticks=23;
	      break;
	    case 0x26:
	      instruction=string("sla (ix+") +  dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0x27:
	      instruction=string("sla (ix+") + dec2Hex(c,2) + "), a";
	      ticks=23;
	      break;
	    case 0x28:
	      instruction=string("sra (ix+") + dec2Hex(c,2) + "), b";
	      ticks=23;
	      break;
	    case 0x29:
	      instruction=string("sra (ix+") + dec2Hex(c,2) + "), c";
	      ticks=23;
	      break;
	    case 0x2A:
	      instruction=string("sra (ix+") + dec2Hex(c,2) + "), d";
	      ticks=23;
	      break;
	    case 0x2B:
	      instruction=string("sra (ix+") + dec2Hex(c,2) + "), e";
	      ticks=23;
	      break;
	    case 0x2C:
	      instruction=string("sra (ix+") + dec2Hex(c,2) + "), h";
	      ticks=23;
	      break;
	    case 0x2D:
	      instruction=string("sra (ix+") + dec2Hex(c,2) + "), l";
	      ticks=23;
	      break;
	    case 0x2E:
	      instruction=string("sra (ix+") +  dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0x2F:
	      instruction=string("sra (ix+") + dec2Hex(c,2) + "), a";
	      ticks=23;
	      break;

	      // ================
	    case 0x30:
	      instruction=string("sll (ix+") + dec2Hex(c,2) + "), b";
	      ticks=23;
	      break;
	    case 0x31:
	      instruction=string("sll (ix+") + dec2Hex(c,2) + "), c";
	      ticks=23;
	      break;
	    case 0x32:
	      instruction=string("sll (ix+") + dec2Hex(c,2) + "), d";
	      ticks=23;
	      break;
	    case 0x33:
	      instruction=string("sll (ix+") + dec2Hex(c,2) + "), e";
	      ticks=23;
	      break;
	    case 0x34:
	      instruction=string("sll (ix+") + dec2Hex(c,2) + "), h";
	      ticks=23;
	      break;
	    case 0x35:
	      instruction=string("sll (ix+") + dec2Hex(c,2) + "), l";
	      ticks=23;
	      break;
	    case 0x36:
	      instruction=string("sll (ix+") +  dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0x37:
	      instruction=string("sll (ix+") + dec2Hex(c,2) + "), a";
	      ticks=23;
	      break;
	    case 0x38:
	      instruction=string("srl (ix+") + dec2Hex(c,2) + "), b";
	      ticks=23;
	      break;
	    case 0x39:
	      instruction=string("srl (ix+") + dec2Hex(c,2) + "), c";
	      ticks=23;
	      break;
	    case 0x3A:
	      instruction=string("srl (ix+") + dec2Hex(c,2) + "), d";
	      ticks=23;
	      break;
	    case 0x3B:
	      instruction=string("srl (ix+") + dec2Hex(c,2) + "), e";
	      ticks=23;
	      break;
	    case 0x3C:
	      instruction=string("srl (ix+") + dec2Hex(c,2) + "), h";
	      ticks=23;
	      break;
	    case 0x3D:
	      instruction=string("srl (ix+") + dec2Hex(c,2) + "), l";
	      ticks=23;
	      break;
	    case 0x3E:
	      instruction=string("srl (ix+") +  dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0x3F:
	      instruction=string("srl (ix+") + dec2Hex(c,2) + "), a";
	      ticks=23;
	      break;
	      // ================

	    case 0x40:
	    case 0x41:
	    case 0x42:
	    case 0x43:
	    case 0x44:
	    case 0x45:
	    case 0x46:
	    case 0x47:
	      instruction=string("bit 0, (ix+") + dec2Hex(c,2) + ")";
	      ticks=20;
	      break;
	    case 0x48:
	    case 0x49:
	    case 0x4A:
	    case 0x4B:
	    case 0x4C:
	    case 0x4D:
	    case 0x4E:
	    case 0x4F:
	      instruction=string("bit 1, (ix+") + dec2Hex(c,2) + ")";
	      ticks=20;
	      break;
	      // =======================
	    case 0x50:
	    case 0x51:
	    case 0x52:
	    case 0x53:
	    case 0x54:
	    case 0x55:
	    case 0x56:
	    case 0x57:
	      instruction=string("bit 2, (ix+") + dec2Hex(c,2) + ")";
	      ticks=20;
	      break;
	    case 0x58:
	    case 0x59:
	    case 0x5A:
	    case 0x5B:
	    case 0x5C:
	    case 0x5D:
	    case 0x5E:
	    case 0x5F:
	      instruction=string("bit 3, (ix+") + dec2Hex(c,2) + ")";
	      ticks=20;
	      break;
	      // =======================
	    case 0x60:
	    case 0x61:
	    case 0x62:
	    case 0x63:
	    case 0x64:
	    case 0x65:
	    case 0x66:
	    case 0x67:
	      instruction=string("bit 4, (ix+") + dec2Hex(c,2) + ")";
	      ticks=20;
	      break;
	    case 0x68:
	    case 0x69:
	    case 0x6A:
	    case 0x6B:
	    case 0x6C:
	    case 0x6D:
	    case 0x6E:
	    case 0x6F:
	      instruction=string("bit 5, (ix+") + dec2Hex(c,2) + ")";
	      ticks=20;
	      break;
	      // =======================
	    case 0x70:
	    case 0x71:
	    case 0x72:
	    case 0x73:
	    case 0x74:
	    case 0x75:
	    case 0x76:
	    case 0x77:
	      instruction=string("bit 6, (ix+") + dec2Hex(c,2) + ")";
	      ticks=20;
	      break;
	    case 0x78:
	    case 0x79:
	    case 0x7A:
	    case 0x7B:
	    case 0x7C:
	    case 0x7D:
	    case 0x7E:
	    case 0x7F:
	      instruction=string("bit 7, (ix+") + dec2Hex(c,2) + ")";
	      ticks=20;
	      break;
	      // =======================
	      
	    case 0x80:
	    case 0x81:
	    case 0x82:
	    case 0x83:
	    case 0x84:
	    case 0x85:
	    case 0x86:
	    case 0x87:
	      instruction=string("res 0, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0x88:
	    case 0x89:
	    case 0x8A:
	    case 0x8B:
	    case 0x8C:
	    case 0x8D:
	    case 0x8E:
	    case 0x8F:
	      instruction=string("res 1, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	      // =======================
	    case 0x90:
	    case 0x91:
	    case 0x92:
	    case 0x93:
	    case 0x94:
	    case 0x95:
	    case 0x96:
	    case 0x97:
	      instruction=string("res 2, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0x98:
	    case 0x99:
	    case 0x9A:
	    case 0x9B:
	    case 0x9C:
	    case 0x9D:
	    case 0x9E:
	    case 0x9F:
	      instruction=string("res 3, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	      // =======================
	    case 0xA0:
	    case 0xA1:
	    case 0xA2:
	    case 0xA3:
	    case 0xA4:
	    case 0xA5:
	    case 0xA6:
	    case 0xA7:
	      instruction=string("res 4, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0xA8:
	    case 0xA9:
	    case 0xAA:
	    case 0xAB:
	    case 0xAC:
	    case 0xAD:
	    case 0xAE:
	    case 0xAF:
	      instruction=string("res 5, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	      // =======================
     	    case 0xB0:
	    case 0xB1:
	    case 0xB2:
	    case 0xB3:
	    case 0xB4:
	    case 0xB5:
	    case 0xB6:
	    case 0xB7:
	      instruction=string("res 6, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0xB8:
	    case 0xB9:
	    case 0xBA:
	    case 0xBB:
	    case 0xBC:
	    case 0xBD:
	    case 0xBE:
	    case 0xBF:
	      instruction=string("res 7, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	      // =======================

	    case 0xC0:
	    case 0xC1:
	    case 0xC2:
	    case 0xC3:
	    case 0xC4:
	    case 0xC5:
	    case 0xC6:
	    case 0xC7:
	      instruction=string("set 0, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0xC8:
	    case 0xC9:
	    case 0xCA:
	    case 0xCB:
	    case 0xCC:
	    case 0xCD:
	    case 0xCE:
	    case 0xCF:
	      instruction=string("set 1, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	      // =======================

	    case 0xD0:
	    case 0xD1:
	    case 0xD2:
	    case 0xD3:
	    case 0xD4:
	    case 0xD5:
	    case 0xD6:
	    case 0xD7:
	      instruction=string("set 2, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0xD8:
	    case 0xD9:
	    case 0xDA:
	    case 0xDB:
	    case 0xDC:
	    case 0xDD:
	    case 0xDE:
	    case 0xDF:
	      instruction=string("set 3, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	      // =======================

	    case 0xE0:
	    case 0xE1:
	    case 0xE2:
	    case 0xE3:
	    case 0xE4:
	    case 0xE5:
	    case 0xE6:
	    case 0xE7:
	      instruction=string("set 4, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0xE8:
	    case 0xE9:
	    case 0xEA:
	    case 0xEB:
	    case 0xEC:
	    case 0xED:
	    case 0xEE:
	    case 0xEF:
	      instruction=string("set 5, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	      // =======================

	    case 0xF0:
	    case 0xF1:
	    case 0xF2:
	    case 0xF3:
	    case 0xF4:
	    case 0xF5:
	    case 0xF6:
	    case 0xF7:
	      instruction=string("set 6, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	    case 0xF8:
	    case 0xF9:
	    case 0xFA:
	    case 0xFB:
	    case 0xFC:
	    case 0xFD:
	    case 0xFE:
	    case 0xFF:
	      instruction=string("set 7, (ix+") + dec2Hex(c,2) + ")";
	      ticks=23;
	      break;
	      // =======================
	    default:
	      instruction="invalid";
	      valid=false;
	      size=0;
	    }

	  
	default:
	  instruction="invalid";
	  valid=false;
	  size=0;
	}
      break;

    case 0xED:


      
      break;

    case 0xFD:
      break;

    default:
      instruction="invalid";
      valid=false;
      size=0;
    }

}

// 3 BYTE OPCODES
mneumonic::mneumonic( unsigned char a, unsigned char b, unsigned char c )
{
#ifdef DEBUG
  cerr << "created mneumonic( " << (int)a << " , " << (int)b << " , " << (int)c << " );" << endl;
#endif
  size=3;
  valid=true;
  bytes[0]=a; bytes[1]=b; bytes[2]=c;
  switch(a)
    {

      // SYSTEM CALLS
    case 0xEF:
      instruction=string("bCall");
      switch(c*256+b)
	{
	case 0x450A:
	  instruction += string("(PutS)");
	  break;
	case 0x410E:
	  instruction += string("(AbsO1O2Cp)");
	  break;
	case 0x405A:
	  instruction += string("(AbsO1PAbsO2)");
	  break;
	case 0x40DE:
	  instruction += string("(ACos)");
	  break;
	case 0x40F0:
	  instruction += string("(ACosH)");
	  break;
	case 0x40D2:
	  instruction += string("(ACosRad)");
	  break;
	case 0x462D:
	  instruction += string("(AdrLEle)");
	  break;
	case 0x4609:
	  instruction += string("(AdrMEle)");
	  break;
	case 0x4606:
	  instruction += string("(AdrMRow)");
	  break;
	case 0x4876:
	  instruction += string("(AllEq)");
	  break;
	case 0x43A5:
	  instruction += string("(AllocFPS)");
	  break;
	case 0x43A8:
	  instruction += string("(AllocFPS1)");
	  break;
	case 0x4102:
	  instruction += string("(Angle)");
	  break;
	case 0x4B52:
	  instruction += string("(AnsName)");
	  break;
	case 0x4C93:
	  instruction += string("(ApdSetup)");
	  break;
	case 0x4C78:
	  instruction += string("(AppGetCalc)");
	  break;
	case 0x4C75:
	  instruction += string("(AppGetCbl)");
	  break;
	case 0x404B:
	  instruction += string("(AppInit)");
	  break;
	case 0x4FD8:
	  instruction += string("(Arc_Unarc)");
	  break;
	case 0x5014:
	  instruction += string("(ArcChk)");
	  break;
	case 0x4FDB:
	  instruction += string("(ArchiveVar)");
	  break;
	case 0x40E4:
	  instruction += string("(ASin)");
	  break;
	case 0x40ED:
	  instruction += string("(ASinH)");
	  break;
	case 0x40DB:
	  instruction += string("(ASinRad)");
	  break;
	case 0x40E1:
	  instruction += string("(ATan)");
	  break;
	case 0x40E7:
	  instruction += string("(ATan2)");
	  break;
	case 0x40D8:
	  instruction += string("(ATan2Rad)");
	  break;
	case 0x40EA:
	  instruction += string("(ATanH)");
	  break;
	case 0x40D5:
	  instruction += string("(ATanRad)");
	  break;
	case 0x4663:
	  instruction += string("(BinOPExec)");
	  break;
	case 0x4FA8:
	  instruction += string("(Bit_VertSplit)");
	  break;
	case 0x4936:
	  instruction += string("(BufClear)");
	  break;
	case 0x5074:
	  instruction += string("(BufClr)");
	  break;
	case 0x5071:
	  instruction += string("(BufCpy)");
	  break;
	case 0x4912:
	  instruction += string("(BufDelete)");
	  break;
	case 0x4909:
	  instruction += string("(BufInsert)");
	  break;
	case 0x4903:
	  instruction += string("(BufLeft)");
	  break;
	case 0x490F:
	  instruction += string("(BufReplace)");
	  break;
	case 0x4906:
	  instruction += string("(BufRight)");
	  break;
	case 0x4E97:
	  instruction += string("(CAbs)");
	  break;
	case 0x4E88:
	  instruction += string("(CAdd)");
	  break;
	case 0x4C69:
	  instruction += string("(CanAlphIns)");
	  break;
	case 0x4ECA:
	  instruction += string("(CancelTransmission)");
	  break;
	case 0x4E94:
	  instruction += string("(CDiv)");
	  break;
	case 0x4EBB:
	  instruction += string("(CDivByReal)");
	  break;
	case 0x4EA9:
	  instruction += string("(CEtoX)");
	  break;
	case 0x4EC1:
	  instruction += string("(CFrac)");
	  break;
	case 0x809C:
	  instruction += string("(CheckOSValid)");
	  break;
	case 0x49F0:
	  instruction += string("(CheckSplitFlag)");
	  break;
	case 0x5221:
	  instruction += string("(Chk_Batt_Level)");
	  break;
	case 0x50B3:
	  instruction += string("(Chk_Batt_Low)");
	  break;
	case 0x42F1:
	  instruction += string("(ChkFindSym)");
	  break;
	case 0x5176:
	  instruction += string("(ChkTimer0)");
	  break;
	case 0x4EC4:
	  instruction += string("(CIntgr)");
	  break;
	case 0x47D4:
	  instruction += string("(CircCmd)");
	  break;
	case 0x4234:
	  instruction += string("(CkInt)");
	  break;
	case 0x4237:
	  instruction += string("(CkOdd)");
	  break;
	case 0x4225:
	  instruction += string("(CkOP1C0)");
	  break;
	case 0x40FC:
	  instruction += string("(CkOP1Cplx)");
	  break;
	case 0x4228:
	  instruction += string("(CkOP1FP0)");
	  break;
	case 0x4258:
	  instruction += string("(CkOP1Pos)");
	  break;
	case 0x40FF:
	  instruction += string("(CkOP1Real)");
	  break;
	case 0x422B:
	  instruction += string("(CkOP2FP0)");
	  break;
	case 0x4255:
	  instruction += string("(CkOP2Pos)");
	  break;
	case 0x42DF:
	  instruction += string("(CkOP2Real)");
	  break;
	case 0x4231:
	  instruction += string("(CkPosInt)");
	  break;
	case 0x4270:
	  instruction += string("(CkValidNum)");
	  break;
	case 0x4A50:
	  instruction += string("(CleanAll)");
	  break;
	case 0x4D5C:
	  instruction += string("(ClearRect)");
	  break;
	case 0x4CED:
	  instruction += string("(ClearRow)");
	  break;
	case 0x4798:
	  instruction += string("(CLine)");
	  break;
	case 0x479B:
	  instruction += string("(CLineS)");
	  break;
	case 0x4EA0:
	  instruction += string("(CLN)");
	  break;
	case 0x4EA3:
	  instruction += string("(CLog)");
	  break;
	case 0x48D3:
	  instruction += string("(CloseEditBuf)");
	  break;
	case 0x476E:
	  instruction += string("(CloseEditBufNoR)");
	  break;
	case 0x496C:
	  instruction += string("(CloseEditEqu)");
	  break;
	case 0x4A35:
	  instruction += string("(CloseProg)");
	  break;
	case 0x4A38:
	  instruction += string("(ClrGraphRef)");
	  break;
	case 0x4543:
	  instruction += string("(ClrLCD)");
	  break;
	case 0x4540:
	  instruction += string("(ClrLCDFull)");
	  break;
	case 0x41D1:
	  instruction += string("(ClrLp)");
	  break;
	case 0x425E:
	  instruction += string("(ClrOP1S)");
	  break;
	case 0x425B:
	  instruction += string("(ClrOP2S)");
	  break;
	case 0x4549:
	  instruction += string("(ClrScrn)");
	  break;
	case 0x4546:
	  instruction += string("(ClrScrnFull)");
	  break;
	case 0x454C:
	  instruction += string("(ClrTxtShd)");
	  break;
	case 0x454F:
	  instruction += string("(ClrWindow)");
	  break;
	case 0x4EB8:
	  instruction += string("(CMltByReal)");
	  break;
	case 0x4A4A:
	  instruction += string("(CmpSyms)");
	  break;
	case 0x4E8E:
	  instruction += string("(CMult)");
	  break;
	case 0x4EB5:
	  instruction += string("(Conj)");
	  break;
	case 0x4F00:
	  instruction += string("(ContinueGetByte)");
	  break;
	case 0x4B43:
	  instruction += string("(ConvDim)");
	  break;
	case 0x4B46:
	  instruction += string("(ConvDim00)");
	  break;
	case 0x4A23:
	  instruction += string("(ConvLcToLr)");
	  break;
	case 0x4A56:
	  instruction += string("(ConvLrToLc)");
	  break;
	case 0x4AEF:
	  instruction += string("(ConvOP1)");
	  break;
	case 0x4105:
	  instruction += string("(COP1Set0)");
	  break;
	case 0x5098:
	  instruction += string("(CopyFlashPage)");
	  break;
	case 0x40C0:
	  instruction += string("(Cos)");
	  break;
	case 0x40CC:
	  instruction += string("(CosH)");
	  break;
	case 0x400C:
	  instruction += string("(CpHLDE)");
	  break;
	case 0x4DC8:
	  instruction += string("(CPoint)");
	  break;
	case 0x47F5:
	  instruction += string("(CPointS)");
	  break;
	case 0x4111:
	  instruction += string("(CpOP1OP2)");
	  break;
	case 0x4108:
	  instruction += string("(CpOP4OP3)");
	  break;
	case 0x445C:
	  instruction += string("(CpyO1ToFPS1)");
	  break;
	case 0x446B:
	  instruction += string("(CpyO1ToFPS2)");
	  break;
	case 0x4477:
	  instruction += string("(CpyO1ToFPS3)");
	  break;
	case 0x4489:
	  instruction += string("(CpyO1ToFPS4)");
	  break;
	case 0x4483:
	  instruction += string("(CpyO1ToFPS5)");
	  break;
	case 0x447D:
	  instruction += string("(CpyO1ToFPS6)");
	  break;
	case 0x4480:
	  instruction += string("(CpyO1ToFPS7)");
	  break;
	case 0x444A:
	  instruction += string("(CpyO1ToFPST)");
	  break;
	case 0x4459:
	  instruction += string("(CpyO2ToFPS1)");
	  break;
	case 0x4462:
	  instruction += string("(CpyO2ToFPS2)");
	  break;
	case 0x4474:
	  instruction += string("(CpyO2ToFPS3)");
	  break;
	case 0x4486:
	  instruction += string("(CpyO2ToFPS4)");
	  break;
	case 0x4444:
	  instruction += string("(CpyO2ToFPST)");
	  break;
	case 0x4453:
	  instruction += string("(CpyO3ToFPS1)");
	  break;
	case 0x4465:
	  instruction += string("(CpyO3ToFPS2)");
	  break;
	case 0x4441:
	  instruction += string("(CpyO3ToFPST)");
	  break;
	case 0x4456:
	  instruction += string("(CpyO5ToFPS1)");
	  break;
	case 0x4471:
	  instruction += string("(CpyO5ToFPS3)");
	  break;
	case 0x4468:
	  instruction += string("(CpyO6ToFPS2)");
	  break;
	case 0x4447:
	  instruction += string("(CpyO6ToFPST)");
	  break;
	case 0x4429:
	  instruction += string("(CpyStack)");
	  break;
	case 0x4432:
	  instruction += string("(CpyTo1FPS1)");
	  break;
	case 0x43F3:
	  instruction += string("(CpyTo1FPS10)");
	  break;
	case 0x43D8:
	  instruction += string("(CpyTo1FPS11)");
	  break;
	case 0x443B:
	  instruction += string("(CpyTo1FPS2)");
	  break;
	case 0x4408:
	  instruction += string("(CpyTo1FPS3)");
	  break;
	case 0x440E:
	  instruction += string("(CpyTo1FPS4)");
	  break;
	case 0x43DE:
	  instruction += string("(CpyTo1FPS5)");
	  break;
	case 0x43E4:
	  instruction += string("(CpyTo1FPS6)");
	  break;
	case 0x43EA:
	  instruction += string("(CpyTo1FPS7)");
	  break;
	case 0x43ED:
	  instruction += string("(CpyTo1FPS8)");
	  break;
	case 0x43F6:
	  instruction += string("(CpyTo1FPS9)");
	  break;
	case 0x4423:
	  instruction += string("(CpyTo1FPST)");
	  break;
	case 0x442F:
	  instruction += string("(CpyTo2FPS1)");
	  break;
	case 0x4438:
	  instruction += string("(CpyTo2FPS2)");
	  break;
	case 0x4402:
	  instruction += string("(CpyTo2FPS3)");
	  break;
	case 0x43F9:
	  instruction += string("(CpyTo2FPS4)");
	  break;
	case 0x43DB:
	  instruction += string("(CpyTo2FPS5)");
	  break;
	case 0x43E1:
	  instruction += string("(CpyTo2FPS6)");
	  break;
	case 0x43E7:
	  instruction += string("(CpyTo2FPS7)");
	  break;
	case 0x43F0:
	  instruction += string("(CpyTo2FPS8)");
	  break;
	case 0x4420:
	  instruction += string("(CpyTo2FPST)");
	  break;
	case 0x442C:
	  instruction += string("(CpyTo3FPS1)");
	  break;
	case 0x4411:
	  instruction += string("(CpyTo3FPS2)");
	  break;
	case 0x441D:
	  instruction += string("(CpyTo3FPST)");
	  break;
	case 0x441A:
	  instruction += string("(CpyTo4FPST)");
	  break;
	case 0x4414:
	  instruction += string("(CpyTo5FPST)");
	  break;
	case 0x43FF:
	  instruction += string("(CpyTo6FPS2)");
	  break;
	case 0x43FC:
	  instruction += string("(CpyTo6FPS3)");
	  break;
	case 0x4417:
	  instruction += string("(CpyTo6FPST)");
	  break;
	case 0x445F:
	  instruction += string("(CpyToFPS1)");
	  break;
	case 0x446E:
	  instruction += string("(CpyToFPS2)");
	  break;
	case 0x447A:
	  instruction += string("(CpyToFPS3)");
	  break;
	case 0x444D:
	  instruction += string("(CpyToFPST)");
	  break;
	case 0x4450:
	  instruction += string("(CpyToStack)");
	  break;
	case 0x432A:
	  instruction += string("(Create0Equ)");
	  break;
	case 0x4E6A:
	  instruction += string("(CreateAppVar)");
	  break;
	case 0x431B:
	  instruction += string("(CreateCList)");
	  break;
	case 0x430C:
	  instruction += string("(CreateCplx)");
	  break;
	case 0x4330:
	  instruction += string("(CreateEqu)");
	  break;
	case 0x4B0D:
	  instruction += string("(CreatePair)");
	  break;
	case 0x4333:
	  instruction += string("(CreatePict)");
	  break;
	case 0x4339:
	  instruction += string("(CreateProg)");
	  break;
	case 0x4E6D:
	  instruction += string("(CreateProtProg)");
	  break;
	case 0x430F:
	  instruction += string("(CreateReal)");
	  break;
	case 0x4315:
	  instruction += string("(CreateRList)");
	  break;
	case 0x4321:
	  instruction += string("(CreateRMat)");
	  break;
	case 0x4327:
	  instruction += string("(CreateStrng)");
	  break;
	case 0x432D:
	  instruction += string("(CreateTempEqu)");
	  break;
	case 0x4E70:
	  instruction += string("(CreateVar)");
	  break;
	case 0x4E91:
	  instruction += string("(CRecip)");
	  break;
	case 0x4E9D:
	  instruction += string("(CSqRoot)");
	  break;
	case 0x4E8B:
	  instruction += string("(CSquare)");
	  break;
	case 0x4E85:
	  instruction += string("(CSub)");
	  break;
	case 0x4EA6:
	  instruction += string("(CTenX)");
	  break;
	case 0x4EBE:
	  instruction += string("(CTrunc)");
	  break;
	case 0x407B:
	  instruction += string("(Cube)");
	  break;
	case 0x4948:
	  instruction += string("(CursorDown)");
	  break;
	case 0x493F:
	  instruction += string("(CursorLeft)");
	  break;
	case 0x45BE:
	  instruction += string("(CursorOff)");
	  break;
	case 0x45C4:
	  instruction += string("(CursorOn)");
	  break;
	case 0x4EAC:
	  instruction += string("(CXrootY)");
	  break;
	case 0x4EB2:
	  instruction += string("(CYtoX)");
	  break;
	case 0x47DD:
	  instruction += string("(DarkLine)");
	  break;
	case 0x47F2:
	  instruction += string("(DarkPnt)");
	  break;
	case 0x436C:
	  instruction += string("(DataSize)");
	  break;
	case 0x4369:
	  instruction += string("(DataSizeA)");
	  break;
	case 0x439F:
	  instruction += string("(DeallocFPS)");
	  break;
	case 0x43A2:
	  instruction += string("(DeallocFPS1)");
	  break;
	case 0x4267:
	  instruction += string("(DecO1Exp)");
	  break;
	case 0x4DCB:
	  instruction += string("(DeleteApp)");
	  break;
	case 0x4A2F:
	  instruction += string("(DelListEl)");
	  break;
	case 0x4357:
	  instruction += string("(DelMem)");
	  break;
	case 0x4A20:
	  instruction += string("(DelRes)");
	  break;
	case 0x4351:
	  instruction += string("(DelVar)");
	  break;
	case 0x4FC6:
	  instruction += string("(DelVarArc)");
	  break;
	case 0x4FC9:
	  instruction += string("(DelVarNoArc)");
	  break;
	case 0x4A1D:
	  instruction += string("(DeselectAllVars)");
	  break;
	case 0x4C84:
	  instruction += string("(DisableApd)");
	  break;
	case 0x502F:
	  instruction += string("(DisableAppChangeHook)");
	  break;
	case 0x4F69:
	  instruction += string("(DisableCursorHook)");
	  break;
	case 0x506E:
	  instruction += string("(DisablecxRedispHook)");
	  break;
	case 0x4FE7:
	  instruction += string("(DisableFontHook)");
	  break;
	case 0x4F7E:
	  instruction += string("(DisableGetCSCHook)");
	  break;
	case 0x504D:
	  instruction += string("(DisableHelpHook)");
	  break;
	case 0x4FAE:
	  instruction += string("(DisableHomescreenHook)");
	  break;
	case 0x5086:
	  instruction += string("(DisableMenuHook)");
	  break;
	case 0x5029:
	  instruction += string("(DisableParserHook)");
	  break;
	case 0x4F6F:
	  instruction += string("(DisableRawKeyHook)");
	  break;
	case 0x50D1:
	  instruction += string("(DisableSilentLinkHook)");
	  break;
	case 0x4F9C:
	  instruction += string("(DisableTokenHook)");
	  break;
	case 0x4F45:
	  instruction += string("(Disp)");
	  break;
	case 0x51CD:
	  instruction += string("(Disp32)");
	  break;
	case 0x51C7:
	  instruction += string("(DispAboutScreen)");
	  break;
	case 0x52FF:
	  instruction += string("(DispAppRestrictions)");
	  break;
	case 0x45B5:
	  instruction += string("(DispDone)");
	  break;
	case 0x45A6:
	  instruction += string("(DispEOL)");
	  break;
	case 0x4957:
	  instruction += string("(DispEOW)");
	  break;
	case 0x49D5:
	  instruction += string("(DispForward)");
	  break;
	case 0x495A:
	  instruction += string("(DispHead)");
	  break;
	case 0x4507:
	  instruction += string("(DispHL)");
	  break;
	case 0x4D9B:
	  instruction += string("(DisplayImage)");
	  break;
	case 0x80F9:
	  instruction += string("(DisplayOSProgress)");
	  break;
	case 0x5065:
	  instruction += string("(DispMenuTitle)");
	  break;
	case 0x4BF7:
	  instruction += string("(DispOP1A)");
	  break;
	case 0x495D:
	  instruction += string("(DispTail)");
	  break;
	case 0x80B1:
	  instruction += string("(Div32By16)");
	  break;
	case 0x400F:
	  instruction += string("(DivHLBy10)");
	  break;
	case 0x4012:
	  instruction += string("(DivHLByA)");
	  break;
	case 0x8048:
	  instruction += string("(DivHLbyE)");
	  break;
	case 0x804B:
	  instruction += string("(DivHLbyDE)");
	  break;
	case 0x4C66:
	  instruction += string("(DrawCirc2)");
	  break;
	case 0x48C1:
	  instruction += string("(DrawCmd)");
	  break;
	case 0x4D7D:
	  instruction += string("(DrawRectBorder)");
	  break;
	case 0x4D8C:
	  instruction += string("(DrawRectBorderClear)");
	  break;
	case 0x5320:
	  instruction += string("(DispResetComplete)");
	  break;
	case 0x4075:
	  instruction += string("(DToR)");
	  break;
	case 0x4CBD:
	  instruction += string("(DoNothing)");
	  break;
	case 0x4A32:
	  instruction += string("(EditProg)");
	  break;
	case 0x4C87:
	  instruction += string("(EnableApd)");
	  break;
	case 0x502C:
	  instruction += string("(EnableAppChangeHook)");
	  break;
	case 0x4F60:
	  instruction += string("(EnableCursorHook)");
	  break;
	case 0x506B:
	  instruction += string("(EnablecxRedispHook)");
	  break;
	case 0x4FE4:
	  instruction += string("(EnableFontHook)");
	  break;
	case 0x4F7B:
	  instruction += string("(EnableGetCSCHook)");
	  break;
	case 0x504A:
	  instruction += string("(EnableHelpHook)");
	  break;
	case 0x4FAB:
	  instruction += string("(EnableHomescreenHook)");
	  break;
	case 0x5083:
	  instruction += string("(EnableMenuHook)");
	  break;
	case 0x5026:
	  instruction += string("(EnableParserHook)");
	  break;
	case 0x4F66:
	  instruction += string("(EnableRawKeyHook)");
	  break;
	case 0x50CE:
	  instruction += string("(EnableSilentLinkHook)");
	  break;
	case 0x4F99:
	  instruction += string("(EnableTokenHook)");
	  break;
	case 0x42FD:
	  instruction += string("(EnoughMem)");
	  break;
	case 0x4279:
	  instruction += string("(EOP1NotReal)");
	  break;
	case 0x42C4:
	  instruction += string("(Equ_or_NewEqu)");
	  break;
	case 0x4552:
	  instruction += string("(EraseEOL)");
	  break;
	case 0x4555:
	  instruction += string("(EraseEOW)");
	  break;
	case 0x8024:
	  instruction += string("(EraseFlash)");
	  break;
	case 0x8084:
	  instruction += string("(EraseFlashPage)");
	  break;
	case 0x4D86:
	  instruction += string("(EraseRectBorder)");
	  break;
	case 0x44AD:
	  instruction += string("(ErrArgument)");
	  break;
	case 0x44CB:
	  instruction += string("(ErrBadGuess)");
	  break;
	case 0x44BF:
	  instruction += string("(ErrBreak)");
	  break;
	case 0x4D41:
	  instruction += string("(ErrCustom1)");
	  break;
	case 0x4D44:
	  instruction += string("(ErrCustom2)");
	  break;
	case 0x42D3:
	  instruction += string("(ErrD_OP1_0)");
	  break;
	case 0x42D0:
	  instruction += string("(ErrD_OP1_LE_0)");
	  break;
	case 0x42CA:
	  instruction += string("(ErrD_OP1Not_R)");
	  break;
	case 0x42C7:
	  instruction += string("(ErrD_OP1NotPos)");
	  break;
	case 0x42CD:
	  instruction += string("(ErrD_OP1NotPosInt)");
	  break;
	case 0x44AA:
	  instruction += string("(ErrDataType)");
	  break;
	case 0x44B3:
	  instruction += string("(ErrDimension)");
	  break;
	case 0x44B0:
	  instruction += string("(ErrDimMismatch)");
	  break;
	case 0x4498:
	  instruction += string("(ErrDivBy0)");
	  break;
	case 0x449E:
	  instruction += string("(ErrDomain)");
	  break;
	case 0x44A1:
	  instruction += string("(ErrIncrement)");
	  break;
	case 0x44BC:
	  instruction += string("(ErrInvalid)");
	  break;
	case 0x44C8:
	  instruction += string("(ErrIterations)");
	  break;
	case 0x44D4:
	  instruction += string("(ErrLinkXmit)");
	  break;
	case 0x44B9:
	  instruction += string("(ErrMemory)");
	  break;
	case 0x44A4:
	  instruction += string("(ErrNon_Real)");
	  break;
	case 0x4A8C:
	  instruction += string("(ErrNonReal)");
	  break;
	case 0x448C:
	  instruction += string("(ErrNotEnoughMem)");
	  break;
	case 0x4495:
	  instruction += string("(ErrOverflow)");
	  break;
	case 0x44C5:
	  instruction += string("(ErrSignChange)");
	  break;
	case 0x449B:
	  instruction += string("(ErrSingularMat)");
	  break;
	case 0x44C2:
	  instruction += string("(ErrStat)");
	  break;
	case 0x44D1:
	  instruction += string("(ErrStatPlot)");
	  break;
	case 0x44A7:
	  instruction += string("(ErrSyntax)");
	  break;
	case 0x44CE:
	  instruction += string("(ErrTolTooSmall)");
	  break;
	case 0x44B6:
	  instruction += string("(ErrUndefined)");
	  break;
	case 0x40B4:
	  instruction += string("(EToX)");
	  break;
	case 0x43D5:
	  instruction += string("(Exch9)");
	  break;
	case 0x4C51:
	  instruction += string("(ExecuteApp)");
	  break;
	case 0x4C3C:
	  instruction += string("(ExecuteNewPrgm)");
	  break;
	case 0x4E7C:
	  instruction += string("(ExecutePrgm)");
	  break;
	case 0x4222:
	  instruction += string("(ExLp)");
	  break;
	case 0x424F:
	  instruction += string("(ExpToHex)");
	  break;
	case 0x4B85:
	  instruction += string("(Factorial)");
	  break;
	case 0x5011:
	  instruction += string("(FillBasePageTable)");
	  break;
	case 0x4D62:
	  instruction += string("(FillRect)");
	  break;
	case 0x4D89:
	  instruction += string("(FillRectPattern)");
	  break;
	case 0x4AF2:
	  instruction += string("(Find_Parse_Formula)");
	  break;
	case 0x4A47:
	  instruction += string("(FindAlphaDn)");
	  break;
	case 0x4A44:
	  instruction += string("(FindAlphaUp)");
	  break;
	case 0x4C4E:
	  instruction += string("(FindApp)");
	  break;
	case 0x4C4B:
	  instruction += string("(FindAppDn)");
	  break;
	case 0x509B:
	  instruction += string("(FindAppNumPages)");
	  break;
	case 0x4C48:
	  instruction += string("(FindAppUp)");
	  break;
	case 0x8075:
	  instruction += string("(FindOSHeaderSubField)");
	  break;
	case 0x805D:
	  instruction += string("(FindSubField)");
	  break;
	case 0x5095:
	  instruction += string("(FindSwapSector EQU 5095h)");
	  break;
	case 0x42F4:
	  instruction += string("(FindSym)");
	  break;
	case 0x467E:
	  instruction += string("(FiveExec)");
	  break;
	case 0x4A3B:
	  instruction += string("(FixTempCnt)");
	  break;
	case 0x5017:
	  instruction += string("(FlashToRam)");
	  break;
	case 0x8054:
	  instruction += string("(FlashToRam2)");
	  break;
	case 0x4F3C:
	  instruction += string("(FlashWriteDisable)");
	  break;
	case 0x4C90:
	  instruction += string("(ForceCmd)");
	  break;
	case 0x508F:
	  instruction += string("(ForceFullScreen)");
	  break;
	case 0x50AA:
	  instruction += string("(FormBase)");
	  break;
	case 0x4996:
	  instruction += string("(FormDCplx)");
	  break;
	case 0x4990:
	  instruction += string("(FormEReal)");
	  break;
	case 0x4999:
	  instruction += string("(FormReal)");
	  break;
	case 0x467B:
	  instruction += string("(FourExec)");
	  break;
	case 0x4072:
	  instruction += string("(FPAdd)");
	  break;
	case 0x4099:
	  instruction += string("(FPDiv)");
	  break;
	case 0x4084:
	  instruction += string("(FPMult)");
	  break;
	case 0x4096:
	  instruction += string("(FPRecip)");
	  break;
	case 0x4081:
	  instruction += string("(FPSquare)");
	  break;
	case 0x406F:
	  instruction += string("(FPSub)");
	  break;
	case 0x4093:
	  instruction += string("(Frac)");
	  break;
	case 0x4594:
	  instruction += string("(Get_Tok_Strng)");
	  break;
	case 0x4EF7:
	  instruction += string("(Get3Bytes)");
	  break;
	case 0x4EF4:
	  instruction += string("(Get4Bytes)");
	  break;
	case 0x4F18:
	  instruction += string("(Get4BytesCursor)");
	  break;
	case 0x4F1B:
	  instruction += string("(Get4BytesNC)");
	  break;
	case 0x4C6F:
	  instruction += string("(GetBaseVer)");
	  break;
	case 0x80B7:
	  instruction += string("(GetBootVer)");
	  break;
	case 0x4F54:
	  instruction += string("(GetBytePaged)");
	  break;
	case 0x807E:
	  instruction += string("(GetCalcSerial)");
	  break;
	case 0x8057:
	  instruction += string("(GetCertificateStart)");
	  break;
	case 0x4018:
	  instruction += string("(GetCSC)");
	  break;
	case 0x4EEE:
	  instruction += string("(GetDataPacket)");
	  break;
	case 0x805A:
	  instruction += string("(GetFieldSize)");
	  break;
	case 0x4972:
	  instruction += string("(GetKey)");
	  break;
	case 0x500B:
	  instruction += string("(GetKeyRetOff)");
	  break;
	case 0x4636:
	  instruction += string("(GetLToOP1)");
	  break;
	case 0x4615:
	  instruction += string("(GetMToOP1)");
	  break;
	case 0x496F:
	  instruction += string("(GetPrevTok)");
	  break;
	case 0x531D:
	  instruction += string("(GetRestrictionOptions)");
	  break;
	case 0x4EEB:
	  instruction += string("(GetSmallPacket)");
	  break;
	case 0x50DD:
	  instruction += string("(GetSysInfo )");
	  break;
	case 0x515B:
	  instruction += string("(GetTime)");
	  break;
	case 0x4591:
	  instruction += string("(GetTokLen)");
	  break;
	case 0x4CD8:
	  instruction += string("(GoToErr)");
	  break;
	case 0x5233:
	  instruction += string("(GoToLastRow)");
	  break;
	case 0x4BD0:
	  instruction += string("(GrBufClr)");
	  break;
	case 0x486A:
	  instruction += string("(GrBufCpy)");
	  break;
	case 0x52C9:
	  instruction += string("(GrBufCpyCustom)");
	  break;
	case 0x47D7:
	  instruction += string("(GrphCirc)");
	  break;
	case 0x509E:
	  instruction += string("(HLMinus5)");
	  break;
	case 0x40F9:
	  instruction += string("(HLTimes9)");
	  break;
	case 0x4558:
	  instruction += string("(HomeUp)");
	  break;
	case 0x48A6:
	  instruction += string("(HorizCmd)");
	  break;
	case 0x4E67:
	  instruction += string("(HorizontalLine)");
	  break;
	case 0x4276:
	  instruction += string("(HTimesL)");
	  break;
	case 0x42A3:
	  instruction += string("(IsA2ByteTok)");
	  break;
	case 0x4C60:
	  instruction += string("(IBounds)");
	  break;
	case 0x4D98:
	  instruction += string("(IBoundsFull)");
	  break;
	case 0x47E0:
	  instruction += string("(ILine)");
	  break;
	case 0x4B73:
	  instruction += string("(IncFetch)");
	  break;
	case 0x4A29:
	  instruction += string("(IncLstSize)");
	  break;
	case 0x494E:
	  instruction += string("(InsDisp)");
	  break;
	case 0x4A2C:
	  instruction += string("(InsertList)");
	  break;
	case 0x42F7:
	  instruction += string("(InsertMem)");
	  break;
	case 0x40A5:
	  instruction += string("(Int)");
	  break;
	case 0x405D:
	  instruction += string("(Intgr)");
	  break;
	case 0x48C7:
	  instruction += string("(InvCmd)");
	  break;
	case 0x4D5F:
	  instruction += string("(InvertRect)");
	  break;
	case 0x408D:
	  instruction += string("(InvOP1S)");
	  break;
	case 0x408A:
	  instruction += string("(InvOP1SC)");
	  break;
	case 0x4090:
	  instruction += string("(InvOP2S)");
	  break;
	case 0x4063:
	  instruction += string("(InvSub)");
	  break;
	case 0x4C63:
	  instruction += string("(IOffset)");
	  break;
	case 0x47E3:
	  instruction += string("(IPoint)");
	  break;
	case 0x4933:
	  instruction += string("(IsAtBtm)");
	  break;
	case 0x4930:
	  instruction += string("(IsAtTop)");
	  break;
	case 0x492D:
	  instruction += string("(IsEditEmpty)");
	  break;
	case 0x44D7:
	  instruction += string("(JError)");
	  break;
	case 0x4000:
	  instruction += string("(JErrorNo)");
	  break;
	case 0x402A:
	  instruction += string("(JForceCmd)");
	  break;
	case 0x4027:
	  instruction += string("(JForceCmdNoChar)");
	  break;
	case 0x5005:
	  instruction += string("(JForceGraphKey)");
	  break;
	case 0x5002:
	  instruction += string("(JForceGraphNoKey)");
	  break;
	case 0x5131:
	  instruction += string("(JForceGroup)");
	  break;
	case 0x4015:
	  instruction += string("(kdbScan)");
	  break;
	case 0x45CA:
	  instruction += string("(KeyToString)");
	  break;
	case 0x810E:
	  instruction += string("(KillUSB)");
	  break;
	case 0x4051:
	  instruction += string("(LCD_BUSY)");
	  break;
	case 0x4978:
	  instruction += string("(LCD_DRIVERON)");
	  break;
	case 0x4009:
	  instruction += string("(LdHLind)");
	  break;
	case 0x48AC:
	  instruction += string("(LineCmd)");
	  break;
	case 0x40AB:
	  instruction += string("(LnX)");
	  break;
	case 0x4783:
	  instruction += string("(Load_SFont)");
	  break;
	case 0x5242:
	  instruction += string("(LoadA5)");
	  break;
	case 0x8051:
	  instruction += string("(LoadAIndPaged)");
	  break;
	case 0x501D:
	  instruction += string("(LoadCIndPaged)");
	  break;
	case 0x501A:
	  instruction += string("(LoadDEIndPaged)");
	  break;
	case 0x4CB1:
	  instruction += string("(LoadPattern)");
	  break;
	case 0x40AE:
	  instruction += string("(LogX)");
	  break;
	case 0x80D2:
	  instruction += string("(LowBatteryBoot)");
	  break;
	case 0x8099:
	  instruction += string("(MarkOSValid)");
	  break;
	case 0x4057:
	  instruction += string("(Max)");
	  break;
	case 0x42E5:
	  instruction += string("(MemChk)");
	  break;
	case 0x4C30:
	  instruction += string("(MemClear)");
	  break;
	case 0x4C33:
	  instruction += string("(MemSet)");
	  break;
	case 0x4054:
	  instruction += string("(Min)");
	  break;
	case 0x406C:
	  instruction += string("(Minus1)");
	  break;
	case 0x401E:
	  instruction += string("(Mon)");
	  break;
	case 0x4021:
	  instruction += string("(MonForceKey)");
	  break;
	case 0x4C54:
	  instruction += string("(MonReset)");
	  break;
	case 0x415C:
	  instruction += string("(Mov10B)");
	  break;
	case 0x47DA:
	  instruction += string("(Mov18B)");
	  break;
	case 0x4168:
	  instruction += string("(Mov7B)");
	  break;
	case 0x4165:
	  instruction += string("(Mov8B)");
	  break;
	case 0x415F:
	  instruction += string("(Mov9B)");
	  break;
	case 0x417D:
	  instruction += string("(Mov9OP1OP2)");
	  break;
	case 0x410B:
	  instruction += string("(Mov9OP2Cp)");
	  break;
	  //case 0x20h:
	  //instruction += string("(Mov9ToOP1)");
	  //break;
	case 0x4180:
	  instruction += string("(Mov9ToOP2)");
	  break;
	case 0x4183:
	  instruction += string("(MovFrOP1)");
	  break;
	case 0x8045:
	  instruction += string("(MultAbyDE)");
	  break;
	case 0x8042:
	  instruction += string("(MultAbyE)");
	  break;
	case 0x4030:
	  instruction += string("(NewContext)");
	  break;
	case 0x4033:
	  instruction += string("(NewContext0)");
	  break;
	case 0x452E:
	  instruction += string("(NewLine)");
	  break;
	case 0x50E0:
	  instruction += string("(NZIf83Plus)");
	  break;
	case 0x44F2:
	  instruction += string("(OffPageJump)");
	  break;
	case 0x4BA3:
	  instruction += string("(OneVar)");
	  break;
	case 0x421F:
	  instruction += string("(OP1ExOP2)");
	  break;
	case 0x4219:
	  instruction += string("(OP1ExOP3)");
	  break;
	case 0x421C:
	  instruction += string("(OP1ExOP4)");
	  break;
	case 0x420D:
	  instruction += string("(OP1ExOP5)");
	  break;
	case 0x4210:
	  instruction += string("(OP1ExOP6)");
	  break;
	case 0x4252:
	  instruction += string("(OP1ExpToDec)");
	  break;
	case 0x41BF:
	  instruction += string("(OP1Set0)");
	  break;
	case 0x419B:
	  instruction += string("(OP1Set1)");
	  break;
	case 0x41A7:
	  instruction += string("(OP1Set2)");
	  break;
	case 0x41A1:
	  instruction += string("(OP1Set3)");
	  break;
	case 0x419E:
	  instruction += string("(OP1Set4)");
	  break;
	case 0x412F:
	  instruction += string("(OP1ToOP2)");
	  break;
	case 0x4123:
	  instruction += string("(OP1ToOP3)");
	  break;
	case 0x4117:
	  instruction += string("(OP1ToOP4)");
	  break;
	case 0x4153:
	  instruction += string("(OP1ToOP5)");
	  break;
	case 0x4150:
	  instruction += string("(OP1ToOP6)");
	  break;
	case 0x4213:
	  instruction += string("(OP2ExOP4)");
	  break;
	case 0x4216:
	  instruction += string("(OP2ExOP5)");
	  break;
	case 0x4207:
	  instruction += string("(OP2ExOP6)");
	  break;
	case 0x41BC:
	  instruction += string("(OP2Set0)");
	  break;
	case 0x41AD:
	  instruction += string("(OP2Set1)");
	  break;
	case 0x41AA:
	  instruction += string("(OP2Set2)");
	  break;
	case 0x4198:
	  instruction += string("(OP2Set3)");
	  break;
	case 0x4195:
	  instruction += string("(OP2Set4)");
	  break;
	case 0x418F:
	  instruction += string("(OP2Set5)");
	  break;
	case 0x4AB0:
	  instruction += string("(OP2Set60)");
	  break;
	case 0x418C:
	  instruction += string("(OP2Set8)");
	  break;
	case 0x4192:
	  instruction += string("(OP2SetA)");
	  break;
	case 0x4156:
	  instruction += string("(OP2ToOP1)");
	  break;
	case 0x416E:
	  instruction += string("(OP2ToOP3)");
	  break;
	case 0x411A:
	  instruction += string("(OP2ToOP4)");
	  break;
	case 0x414A:
	  instruction += string("(OP2ToOP5)");
	  break;
	case 0x414D:
	  instruction += string("(OP2ToOP6)");
	  break;
	case 0x41B9:
	  instruction += string("(OP3Set0)");
	  break;
	case 0x4189:
	  instruction += string("(OP3Set1)");
	  break;
	case 0x41A4:
	  instruction += string("(OP3Set2)");
	  break;
	case 0x413E:
	  instruction += string("(OP3ToOP1)");
	  break;
	case 0x4120:
	  instruction += string("(OP3ToOP2)");
	  break;
	case 0x4114:
	  instruction += string("(OP3ToOP4)");
	  break;
	case 0x4147:
	  instruction += string("(OP3ToOP5)");
	  break;
	case 0x41B6:
	  instruction += string("(OP4Set0)");
	  break;
	case 0x4186:
	  instruction += string("(OP4Set1)");
	  break;
	case 0x4138:
	  instruction += string("(OP4ToOP1)");
	  break;
	case 0x411D:
	  instruction += string("(OP4ToOP2)");
	  break;
	case 0x4171:
	  instruction += string("(OP4ToOP3)");
	  break;
	case 0x4144:
	  instruction += string("(OP4ToOP5)");
	  break;
	case 0x4177:
	  instruction += string("(OP4ToOP6)");
	  break;
	case 0x420A:
	  instruction += string("(OP5ExOP6)");
	  break;
	case 0x41B3:
	  instruction += string("(OP5Set0)");
	  break;
	case 0x413B:
	  instruction += string("(OP5ToOP1)");
	  break;
	case 0x4126:
	  instruction += string("(OP5ToOP2)");
	  break;
	case 0x4174:
	  instruction += string("(OP5ToOP3)");
	  break;
	case 0x412C:
	  instruction += string("(OP5ToOP4)");
	  break;
	case 0x4129:
	  instruction += string("(OP5ToOP6)");
	  break;
	case 0x4135:
	  instruction += string("(OP6ToOP1)");
	  break;
	case 0x4132:
	  instruction += string("(OP6ToOP2)");
	  break;
	case 0x4141:
	  instruction += string("(OP6ToOP5)");
	  break;
	case 0x4BB2:
	  instruction += string("(OutputExpr)");
	  break;
	case 0x5023:
	  instruction += string("(PagedGet)");
	  break;
	case 0x4A9B:
	  instruction += string("(ParseInp)");
	  break;
	case 0x48A3:
	  instruction += string("(PDspGrph)");
	  break;
	case 0x48B5:
	  instruction += string("(PixelTest)");
	  break;
	case 0x4069:
	  instruction += string("(Plus1)");
	  break;
	case 0x48B2:
	  instruction += string("(PointCmd)");
	  break;
	case 0x4C39:
	  instruction += string("(PointOn)");
	  break;
	case 0x436F:
	  instruction += string("(PopMCplxO1)");
	  break;
	case 0x437E:
	  instruction += string("(PopOP1)");
	  break;
	case 0x437B:
	  instruction += string("(PopOP3)");
	  break;
	case 0x4378:
	  instruction += string("(PopOP5)");
	  break;
	case 0x4393:
	  instruction += string("(PopReal)");
	  break;
	case 0x4390:
	  instruction += string("(PopRealO1)");
	  break;
	case 0x438D:
	  instruction += string("(PopRealO2)");
	  break;
	case 0x438A:
	  instruction += string("(PopRealO3)");
	  break;
	case 0x4387:
	  instruction += string("(PopRealO4)");
	  break;
	case 0x4384:
	  instruction += string("(PopRealO5)");
	  break;
	case 0x4381:
	  instruction += string("(PopRealO6)");
	  break;
	case 0x422E:
	  instruction += string("(PosNo0Int)");
	  break;
	case 0x5008:
	  instruction += string("(PowerOff)");
	  break;
	case 0x4756:
	  instruction += string("(ProcessBufKeys)");
	  break;
	case 0x40F3:
	  instruction += string("(PToR)");
	  break;
	case 0x45CD:
	  instruction += string("(PullDownChk)");
	  break;
	case 0x43CF:
	  instruction += string("(PushMCplxO1)");
	  break;
	case 0x43C6:
	  instruction += string("(PushMCplxO3)");
	  break;
	case 0x43C9:
	  instruction += string("(PushOP1)");
	  break;
	case 0x43C3:
	  instruction += string("(PushOP3)");
	  break;
	case 0x43C0:
	  instruction += string("(PushOP5)");
	  break;
	case 0x43BD:
	  instruction += string("(PushReal)");
	  break;
	case 0x43BA:
	  instruction += string("(PushRealO1)");
	  break;
	case 0x43B7:
	  instruction += string("(PushRealO2)");
	  break;
	case 0x43B4:
	  instruction += string("(PushRealO3)");
	  break;
	case 0x43B1:
	  instruction += string("(PushRealO4)");
	  break;
	case 0x43AE:
	  instruction += string("(PushRealO5)");
	  break;
	case 0x43AB:
	  instruction += string("(PushRealO6)");
	  break;
	case 0x403c:
	  instruction += string("(PutAway)");
	  break;
	case 0x4516:
	  instruction += string("(PutBuf)");
	  break;
	case 0x4504:
	  instruction += string("(PutC)");
	  break;
	case 0x4501:
	  instruction += string("(PutMap)");
	  break;
	case 0x4510:
	  instruction += string("(PutPS)");
	  break;
	case 0x450d:
	  instruction += string("(PutPSB)");
	  break;
	case 0x4960:
	  instruction += string("(PutTokString)");
	  break;
	case 0x4645:
	  instruction += string("(PutToL)");
	  break;
	case 0x461E:
	  instruction += string("(PutToMat)");
	  break;
	case 0x4B7F:
	  instruction += string("(RandInit)");
	  break;
	case 0x4B79:
	  instruction += string("(Random)");
	  break;
	case 0x42DC:
	  instruction += string("(Rcl_StatVar)");
	  break;
	case 0x4AD7:
	  instruction += string("(RclAns)");
	  break;
	case 0x47D1:
	  instruction += string("(RclGDB2)");
	  break;
	case 0x4ADD:
	  instruction += string("(RclN)");
	  break;
	case 0x4AE6:
	  instruction += string("(RclSysTok)");
	  break;
	case 0x4AE3:
	  instruction += string("(RclVarSym)");
	  break;
	case 0x4AE0:
	  instruction += string("(RclX)");
	  break;
	case 0x4ADA:
	  instruction += string("(RclY)");
	  break;
	case 0x4EFA:
	  instruction += string("(Rec1stByte)");
	  break;
	case 0x4EFD:
	  instruction += string("(Rec1stByteNC)");
	  break;
	case 0x4F03:
	  instruction += string("(RecAByteIO)");
	  break;
	case 0x4ED9:
	  instruction += string("(ReceiveAck)");
	  break;
	case 0x8072:
	  instruction += string("(ReceiveOS)");
	  break;
	case 0x4F06:
	  instruction += string("(ReceiveVar)");
	  break;
	case 0x4F0C:
	  instruction += string("(ReceiveVarData)");
	  break;
	case 0x4F09:
	  instruction += string("(ReceiveVarData2)");
	  break;
	case 0x4A26:
	  instruction += string("(RedimMat)");
	  break;
	case 0x488E:
	  instruction += string("(Regraph)");
	  break;
	case 0x4F4B:
	  instruction += string("(RejectCommand)");
	  break;
	case 0x4771:
	  instruction += string("(ReleaseBuffer)");
	  break;
	case 0x4C36:
	  instruction += string("(ReloadAppEntryVecs)");
	  break;
	case 0x52F9:
	  instruction += string("(RemoveAppRestrictions)");
	  break;
	case 0x80FC:
	  instruction += string("(ResetCalc)");
	  break;
	case 0x4128:
	  instruction += string("(ResetDefaults)");
	  break;
	case 0x404E:
	  instruction += string("(ResetRam)");
	  break;
	case 0x4870:
	  instruction += string("(RestoreDisp)");
	  break;
	case 0x427F:
	  instruction += string("(RName)");
	  break;
	case 0x409F:
	  instruction += string("(RndGuard)");
	  break;
	case 0x40A2:
	  instruction += string("(RnFx)");
	  break;
	case 0x40A8:
	  instruction += string("(Round)");
	  break;
	case 0x4078:
	  instruction += string("(RToD)");
	  break;
	case 0x40F6:
	  instruction += string("(RToP)");
	  break;
	case 0x4570:
	  instruction += string("(RunIndicOff)");
	  break;
	case 0x456D:
	  instruction += string("(RunIndicOn)");
	  break;
	case 0x4C7B:
	  instruction += string("(SaveDisp)");
	  break;
	case 0x4EE2:
	  instruction += string("(Send4Bytes)");
	  break;
	case 0x4EDC:
	  instruction += string("(Send4BytePacket)");
	  break;
	case 0x4EE5:
	  instruction += string("(SendAByte)");
	  break;
	case 0x4EE8:
	  instruction += string("(SendCByte)");
	  break;
	case 0x4EF1:
	  instruction += string("(SendAck)");
	  break;
	case 0x4F30:
	  instruction += string("(SendApplication)");
	  break;
	case 0x4F3F:
	  instruction += string("(SendCmd)");
	  break;
	case 0x4EDF:
	  instruction += string("(SendDataByte)");
	  break;
	case 0x4F21:
	  instruction += string("(SendDirectoryContents)");
	  break;
	case 0x4969:
	  instruction += string("(SetEmptyEditPtr)");
	  break;
	case 0x4F48:
	  instruction += string("(SendGetKeyPress)");
	  break;
	case 0x4EC7:
	  instruction += string("(SendHeaderPacket)");
	  break;
	case 0x4F39:
	  instruction += string("(SendOs)");
	  break;
	case 0x4F33:
	  instruction += string("(SendOsHeader)");
	  break;
	case 0x4F36:
	  instruction += string("(SendOsPage)");
	  break;
	case 0x4ED6:
	  instruction += string("(SendPacket)");
	  break;
	case 0x4ED3:
	  instruction += string("(SendRAMCmd)");
	  break;
	case 0x4ED0:
	  instruction += string("(SendRAMVarData)");
	  break;
	case 0x4F24:
	  instruction += string("(SendReadyPacket)");
	  break;
	case 0x4ECD:
	  instruction += string("(SendScreenContents)");
	  break;
	case 0x4A14:
	  instruction += string("(SendVarCmd)");
	  break;
	case 0x4F15:
	  instruction += string("(SendVariable)");
	  break;
	case 0x4FCC:
	  instruction += string("(SetAllPlots)");
	  break;
	case 0x52F6:
	  instruction += string("(SetAppRestrictions)");
	  break;
	case 0x50BF:
	  instruction += string("(SetExSpeed )");
	  break;
	case 0x4840:
	  instruction += string("(SetFuncM)");
	  break;
	case 0x49FC:
	  instruction += string("(SetNorm_Vals)");
	  break;
	case 0x452B:
	  instruction += string("(SetNumWindow)");
	  break;
	case 0x4849:
	  instruction += string("(SetParM)");
	  break;
	case 0x4846:
	  instruction += string("(SetPolM)");
	  break;
	case 0x4843:
	  instruction += string("(SetSeqM)");
	  break;
	case 0x4C00:
	  instruction += string("(SetTblGraphDraw)");
	  break;
	case 0x809F:
	  instruction += string("(SetupAppPubKey)");
	  break;
	case 0x4963:
	  instruction += string("(SetupEditCmd)");
	  break;
	case 0x5302:
	  instruction += string("(SetupHome)");
	  break;
	case 0x80FF:
	  instruction += string("(SetupOSPubKey)");
	  break;
	case 0x5020:
	  instruction += string("(SetupPagedPtr)");
	  break;
	case 0x478C:
	  instruction += string("(SetXXOP1)");
	  break;
	case 0x478F:
	  instruction += string("(SetXXOP2)");
	  break;
	case 0x4792:
	  instruction += string("(SetXXXXOP2)");
	  break;
	case 0x4786:
	  instruction += string("(SFont_Len)");
	  break;
	case 0x40BD:
	  instruction += string("(Sin)");
	  break;
	case 0x40BA:
	  instruction += string("(SinCosRad)");
	  break;
	case 0x40CF:
	  instruction += string("(SinH)");
	  break;
	case 0x40C6:
	  instruction += string("(SinHCosH)");
	  break;
	case 0x409C:
	  instruction += string("(SqRoot)");
	  break;
	case 0x4E79:
	  instruction += string("(SquishPrgm)");
	  break;
	case 0x4F12:
	  instruction += string("(SrchVLstDn)");
	  break;
	case 0x4F0F:
	  instruction += string("(SrchVLstUp)");
	  break;
	case 0x4CB4:
	  instruction += string("(SStringLength)");
	  break;
	case 0x4AE9:
	  instruction += string("(StMatEl)");
	  break;
	case 0x4ABF:
	  instruction += string("(StoAns)");
	  break;
	case 0x47CE:
	  instruction += string("(StoGDB2)");
	  break;
	case 0x4ACB:
	  instruction += string("(StoN)");
	  break;
	case 0x4AD4:
	  instruction += string("(StoOther)");
	  break;
	case 0x4AC5:
	  instruction += string("(StoR)");
	  break;
	case 0x4B7C:
	  instruction += string("(StoRand)");
	  break;
	case 0x4ABC:
	  instruction += string("(StoSysTok)");
	  break;
	case 0x4ACE:
	  instruction += string("(StoT)");
	  break;
	case 0x4AC2:
	  instruction += string("(StoTheta)");
	  break;
	case 0x4AD1:
	  instruction += string("(StoX)");
	  break;
	case 0x4AC8:
	  instruction += string("(StoY)");
	  break;
	case 0x44E3:
	  instruction += string("(StrCopy)");
	  break;
	case 0x4C3F:
	  instruction += string("(StrLength)");
	  break;
	case 0x40C3:
	  instruction += string("(Tan)");
	  break;
	case 0x40C9:
	  instruction += string("(TanH)");
	  break;
	case 0x48BB:
	  instruction += string("(TanLnF)");
	  break;
	case 0x40B7:
	  instruction += string("(TenX)");
	  break;
	case 0x427C:
	  instruction += string("(ThetaName)");
	  break;
	case 0x4675:
	  instruction += string("(ThreeExec)");
	  break;
	case 0x4066:
	  instruction += string("(Times2)");
	  break;
	case 0x407E:
	  instruction += string("(TimesPt5)");
	  break;
	case 0x428E:
	  instruction += string("(TName)");
	  break;
	case 0x4657:
	  instruction += string("(ToFrac)");
	  break;
	case 0x4A0B:
	  instruction += string("(TokToKey)");
	  break;
	case 0x4060:
	  instruction += string("(Trunc)");
	  break;
	case 0x50D4:
	  instruction += string("(TwoVarSet)");
	  break;
	case 0x4795:
	  instruction += string("(UCLineS)");
	  break;
	case 0x4FDE:
	  instruction += string("(UnarchiveVar)");
	  break;
	case 0x48AF:
	  instruction += string("(UnLineCmd)");
	  break;
	case 0x4672:
	  instruction += string("(UnOPExec)");
	  break;
	case 0x48A9:
	  instruction += string("(VertCmd)");
	  break;
	case 0x4D95:
	  instruction += string("(VerticalLine)");
	  break;
	case 0x455E:
	  instruction += string("(VPutMap)");
	  break;
	case 0x4561:
	  instruction += string("(VPutS)");
	  break;
	case 0x4564:
	  instruction += string("(VPutSN)");
	  break;
	case 0x47FB:
	  instruction += string("(VtoWHLDE)");
	  break;
	case 0x51A0:
	  instruction += string("(WaitEnterKey)");
	  break;
	case 0x4525:
	  instruction += string("(wDispEOL)");
	  break;
	case 0x451F:
	  instruction += string("(wPutS)");
	  break;
	case 0x4522:
	  instruction += string("(wPutSEOL)");
	  break;
	case 0x8021:
	  instruction += string("(WriteAByte)");
	  break;
	case 0x80C6:
	  instruction += string("(WriteAByteSafe)");
	  break;
	case 0x80C9:
	  instruction += string("(WriteFlash)");
	  break;
	case 0x8087:
	  instruction += string("(WriteFlashUnsafe)");
	  break;
	case 0x50CB:
	  instruction += string("(WriteToFlash)");
	  break;
	case 0x4804:
	  instruction += string("(XftoI)");
	  break;
	case 0x47FE:
	  instruction += string("(Xitof)");
	  break;
	case 0x4288:
	  instruction += string("(XName)");
	  break;
	case 0x808D:
	  instruction += string("(XorA)");
	  break;
	case 0x479E:
	  instruction += string("(XRootY)");
	  break;
	case 0x4801:
	  instruction += string("(YftoI)");
	  break;
	case 0x428B:
	  instruction += string("(YName)");
	  break;
	case 0x47A1:
	  instruction += string("(YToX)");
	  break;
	case 0x41B0:
	  instruction += string("(Zero16D)");
	  break;
	case 0x41CE:
	  instruction += string("(ZeroOP)");
	  break;
	case 0x41C5:
	  instruction += string("(ZeroOP1)");
	  break;
	case 0x41C8:
	  instruction += string("(ZeroOP2)");
	  break;
	case 0x41CB:
	  instruction += string("(ZeroOP3)");
	  break;
	case 0x484F:
	  instruction += string("(ZmDecml)");
	  break;
	case 0x485B:
	  instruction += string("(ZmFit)");
	  break;
	case 0x484C:
	  instruction += string("(ZmInt)");
	  break;
	case 0x4852:
	  instruction += string("(ZmPrev)");
	  break;
	case 0x485E:
	  instruction += string("(ZmSquare)");
	  break;
	case 0x47A4:
	  instruction += string("(ZmStats)");
	  break;
	case 0x4861:
	  instruction += string("(ZmTrig)");
	  break;
	case 0x4855:
	  instruction += string("(ZmUsr)");
	  break;
	case 0x4867:
	  instruction += string("(ZooDefault)");
	  break;



	}
      break;
      // NON SYSTEM CALLS
    case 0x01:
      instruction=string("ld bc, ") + dec2Hex(c*256+b,4); 
	      
      break;
    case 0x11:
      instruction=string("ld de, ")  + dec2Hex(c*256+b,4);
	      
      break;
    case 0x21:
      instruction=string("ld hl, ")  + dec2Hex(c*256+b,4);
	      
      break;
    case 0x22:
      instruction=string("ld (")+ dec2Hex(c*256+b,4) + "), hl";
      
    case 0x2A:
      instruction=string("ld hl, (")  + dec2Hex(c*256+b,4) + ")";
      
      break;
    case 0x31:
      instruction=string("ld sp, ") + dec2Hex(c*256+b,4) + ")";
      
      break;
    case 0x32:
      instruction=string("ld (") + dec2Hex(c*256+b,4) + "), a";
			 
      break;
    case 0x3A:
      instruction=string("ld a, (") + dec2Hex(c*256+b,4) + ")";
	      
      break;
    case 0xC2:
      instruction=string("jp nz, ")  + dec2Hex(c*256+b,4);
	      
      break;
    case 0xC3:
      instruction=string("jp ")  + dec2Hex(c*256+b,4);
	      
      break;
    case 0xC4:
      instruction=string("call nz, ")  + dec2Hex(c*256+b,4);
	      
      break;
    case 0xCA:
      instruction=string("jp z, ")  + dec2Hex(c*256+b,4);
	      
      break;
    case 0xCB:
      // this is an extended instruction
      //instruction=string(*"  + dec2Hex(b) + " " + dec2Hex(c);
	      
      break;
    case 0xCC:
      instruction=string("call z, ")  + dec2Hex(c*256+b,4);
	      
      break;
    case 0xCD:
      instruction=string("call ")  + dec2Hex(c*256+b,4);
	      
      break;
    case 0xD2:
      instruction=string("jp nc, ")  + dec2Hex(c*256+b);
	      
      break;
    case 0xD4:
      instruction=string("call nc, ")  + dec2Hex(c*256+b);
	      
      break;
    case 0xDA:
      instruction=string("jp c, ")  + dec2Hex(c*256+b);
	      
      break;
    case 0xDC:
      instruction=string("call c, ") + dec2Hex(c*256+b);
      break;
    case 0xDD:
      switch(b)
	{
	case 0x46:
	  instruction=string("ld b, (ix+") + dec2Hex(c) + ")";
	  ticks=19;
	  break;
	case 0x56:
	  instruction=string("ld d, (ix+") + dec2Hex(c) + ")";
	  ticks=19;
	  break;
	case 0x66:
	  instruction=string("ld h, (ix+") + dec2Hex(c) + ")";
	  ticks=19;
	  break;
	case 0x86:
	  instruction=string("add a, (ix+") + dec2Hex(c) + ")";
	  ticks=19;
	  break;
	case 0x96:
	  instruction=string("sub (ix+") + dec2Hex(c) + ")";
	  ticks=19;
	  break;
	case 0xA6:
	  instruction=string("and (ix+") + dec2Hex(c) + ")";
	  ticks=19;
	  break;
	case 0xB6:
	  instruction=string("or (ix+") + dec2Hex(c) + ")";
	  ticks=19;
	  break;

	default:
	  instruction=string("invalid");
	  valid=false;
	  size=0;
	}
      

      
    case 0xE2:
      instruction=string("jp po, ") + dec2Hex(c*256+b);
	      
      break;
    case 0xE4:
      instruction=string("call po, ") + dec2Hex(c*256+b);
	      
      break;
    case 0xEA:
      instruction=string("jp pe, ") + dec2Hex(c*256+b);
	      
      break;
    case 0xEC:
      instruction=string("call pe, ") + dec2Hex(c*256+b);
      break;
    case 0xF2:
      instruction=string("jp p, ") + dec2Hex(c*256+b);
      break;
    case 0xF4:
      instruction=string("call p, ") + dec2Hex(c*256+b);
      break;
      break;
    case 0xFA:
      instruction=string("jp m, ") + dec2Hex(c*256+b);
	      
      break;
    case 0xFC:
      instruction=string("call m, ") + dec2Hex(c*256+b);
	      
      break;
    default:
      instruction = string("invalid");
			      
      valid=false;
      size=0;
     
    }
}

// 2 BYTE OPCODES
mneumonic::mneumonic( unsigned char a, unsigned char b )
{
#ifdef DEBUG
  cerr << "created mneumonic( " << (int)a << " , " << (int)b << " );" << endl;
#endif
  size=2;
  valid=true;
  bytes[0]=a; bytes[1]=b;
  switch(a)
    {
    case 0x06:
      instruction=string("ld b, ") + dec2Hex(b);
      ticks=0;
      break;
    case 0x0E:
      instruction=string("ld c, ") + dec2Hex(b);
      break;
    case 0x10:
      instruction=string("djnz ") + dec2Hex(b) + "; (" + std::to_string(-(256-b)) + ")";
      break;
    case 0x16:
      instruction=string("ld d, ") + dec2Hex(b);
      break;
    case 0x18:
      instruction=string("jr ") + dec2Hex(b) + "; (" + std::to_string(-(256-b)) + ")";
      break;
    case 0x1E:
      instruction=string("ld e, ") + dec2Hex(b);
      break;
    case 0x20:
      instruction=string("jr nc, ") + dec2Hex(b) + "; (" + std::to_string(-(256-b)) + ")";
      break;
    case 0x26:
      instruction=string("ld h, ") + dec2Hex(b);
      break;
    case 0x28:
      instruction=string("jr z, ") + dec2Hex(b) + "; (" + std::to_string(-(256-b)) + ")";
      break;
    case 0x2E:
      instruction=string("ld l, ") + dec2Hex(b);
      break;
    case 0x30:
      instruction=string("jr nc, ") + dec2Hex(b) + "; (" + std::to_string(-(256-b)) + ")";
      break;
    case 0x36:
      instruction=string("ld (hl), ") + dec2Hex(b);
      break;
    case 0x38:
      instruction=string("jr c, ") + dec2Hex(b) + "; (" + std::to_string(-(256-b)) + ")";
      break;
    case 0x3E:
      instruction=string("ld a, ") + dec2Hex(b);
      break;
    case 0xC6:
      instruction=string("add a, ") + dec2Hex(b);
      break;
    case 0xC7:
      instruction=string("rst ") + dec2Hex(b);
      break;
    case 0xCB:
      switch(b)
	{
	case 0x27:
	  instruction=string("sla a"); ticks=8;
	  break;
	default:
	  valid=false;
	  size=0;
	  instruction=string("invalid");
	}
      break;
    case 0xCE:
      instruction=string("adc a, ") + dec2Hex(b);
      break;
    case 0xD3:
      instruction=string("out (") + dec2Hex(b) + "), a";
      break;
    case 0xD6:
      instruction=string("sub a, ") + dec2Hex(b);
      break;
    case 0xDB:
      instruction=string("in a, (") + dec2Hex(b) + ")";
      break;
    case 0xDD:
      switch(b)
	{
	case 0x09:
	  instruction=string("add ix, bc"); ticks=15;
	  break;
	case 0x19:
	  instruction=string("add ix, de"); ticks=15;
	  break;

	case 0x23:
	  instruction=string("inc ix"); ticks=10;
	  break;
	case 0x24:
	  instruction=string("inc ixh"); ticks=8;
	  break;
	case 0x25:
	  instruction=string("dec ixh"); ticks=8;
	  break;
	case 0x29:
	  instruction=string("add ix, ix"); ticks=15;
	  break;
	case 0x2B:
	  instruction=string("dec ix"); ticks=10;
	  break;
	case 0x2C:
	  instruction=string("inc ixl"); ticks=8;
	  break;
	case 0x2D:
	  instruction=string("dec ixl"); ticks=8;
	  break;
	case 0x39:
	  instruction=string("add ix, sp"); ticks=15;
	  break;

	default:
	  valid=false;
	  instruction=string("invalid");
	  size=0;
	}
      
      break;
    case 0xDE:
      instruction=string("sbc a, ") + dec2Hex(b);
      break;
    case 0xE6:
      instruction=string("and ") + dec2Hex(b);
      break;
    case 0xEE:
      instruction=string("xor ") + dec2Hex(b);
      break;
    case 0xF6:
      instruction=string("or ") + dec2Hex(b);
      break;
    case 0xFE:
      instruction=string("cp ") + dec2Hex(b);
      break;
    default:
      instruction = string("invalid");
			      
      valid=false;
      size=0;
      
    }
}


mneumonic::mneumonic( unsigned char a )
{
#ifdef DEBUG
  cerr << "created mneumonic( " << std::hex << (int)a << " );" << endl;
#endif
  size=1;
  valid=true;
  bytes[0]=a;
  switch(a)
    {
    case 0x00:
      instruction = string("nop");
      ticks=4;
      break;
    case 0x02:
      instruction = string("ld (bc), a");
      ticks=7;
      break;
    case 0x03:
      instruction = string("inc bc");
      ticks=6;
      break;
    case 0x04:
      instruction = string("inc b");
      ticks=4;
      break;
    case 0x05:
      instruction = string("dec b");
      ticks=4;
      break;
    case 0x07:
      instruction = string("rlca");
      ticks=4;
      break;
    case 0x08:
      instruction = string("ex af, af'");
      ticks=4;
      break;
    case 0x09:
      instruction = string("add hl, bc");
      ticks=11;
      break;
    case 0x0A:
      instruction = string("ld a, (bc)");
      ticks=7;
      break;
    case 0x0B:
      instruction = string("dec bc");
      ticks=6;
      break;
    case 0x0C:
      instruction = string("inc c");
      ticks=4;
      break;
    case 0x0D:
      instruction = string("dec c");
      ticks=4;
      break;
    case 0x0F:
      instruction = string("rrca");
      ticks=4;
      break;
    case 0x12:
      instruction = string("ld (de), a");
      ticks=7;
      break;
    case 0x13:
      instruction = string("inc de");
      ticks=6;
      break;
    case 0x14:
      instruction = string("inc d");
      ticks=4;
      break;
    case 0x15:
      instruction = string("dec d");
      ticks=4;
      break;
    case 0x17:
      instruction = string("rla");
      ticks=4;
      break;
    case 0x19:
      instruction = string("add hl, de");
      ticks=11;
      break;
    case 0x1A:
      instruction = string("ld a, (de)");
      ticks=7;
      break;
    case 0x1B:
      instruction = string("dec de");
      ticks=6;
      break;
    case 0x1C:
      instruction = string("inc e");
      ticks=4;
      break;
    case 0x1D:
      instruction = string("dec e");
      ticks=4;
      break;
    case 0x1F:
      instruction = string("rra");
      ticks=4;
      break;
    case 0x23:
      instruction = string("inc hl");
      ticks=6;
      break;
    case 0x24:
      instruction = string("inc h");
      ticks=4;
      break;
    case 0x25:
      instruction = string("dec h");
      ticks=4;
      break;
    case 0x27:
      instruction = string("daa");
      ticks=4;
      break;
    case 0x29:
      instruction = string("add hl, hl");
      ticks=11;
      break;
    case 0x2B:
      instruction = string("dec hl");
      ticks=6;
      break;
    case 0x2C:
      instruction = string("inc l");
      
      break;
    case 0x2D:
      instruction = string("dec l");
      
      break;
    case 0x2F:
      instruction = string("cpl");
      
      break;
    case 0x33:
      instruction = string("inc sp");
      
      break;
    case 0x34:
      instruction = string("inc (hl)");
      
      break;
    case 0x35:
      instruction = string("dec (hl)");
      
      break;
    case 0x37:
      instruction = string("scf");
      
      break;
    case 0x39:
      instruction = string("add hl, sp");
      
      break;
    case 0x3B:
      instruction = string("dec sp");
      
      break;
    case 0x3C:
      instruction = string("inc a");
      
      break;
    case 0x3D:
      instruction = string("dec a");
      
      break;
    case 0x3F:
      instruction = string("ccf");
      
      break;
    case 0x40:
      instruction = string("ld b, b");
      
      break;
    case 0x41:
      instruction = string("ld b, c");
      
      break;
    case 0x42:
      instruction = string("ld b, d");
      
      break;
    case 0x43:
      instruction = string("ld b, e");
      
      break;
    case 0x44:
      instruction = string("ld b, h");
      
      break;
    case 0x45:
      instruction = string("ld b, l");
      
      break;
    case 0x46:
      instruction = string("ld b, (hl)");
      
      break;
    case 0x47:
      instruction = string("ld b, a");
      
      break;
    case 0x48:
      instruction = string("ld c, b");
      
      break;
    case 0x49:
      instruction = string("ld c, c");
      
      break;
    case 0x4A:
      instruction = string("ld c, d");
      
      break;
    case 0x4B:
      instruction = string("ld c, e");
      
      break;
    case 0x4C:
      instruction = string("ld c, h");
      
      break;
    case 0x4D:
      instruction = string("ld c, l");
      
      break;
    case 0x4E:
      instruction = string("ld c, (hl)");
      
      break;
    case 0x4F:
      instruction = string("ld c, a");
      
      break;
    case 0x50:
      instruction = string("ld d, b");
      
      break;
    case 0x51:
      instruction = string("ld d, c");
      
      break;
    case 0x52:
      instruction = string("ld d, d");
      
      break;
    case 0x53:
      instruction = string("ld d, e");
      
      break;
    case 0x54:
      instruction = string("ld d, h");
      
      break;
    case 0x55:
      instruction = string("ld d, l");
      
      break;
    case 0x56:
      instruction = string("ld d, (hl)");
      
      break;
    case 0x57:
      instruction = string("ld d, a");
      
      break;
    case 0x58:
      instruction = string("ld e, b");
      
      break;
    case 0x59:
      instruction = string("ld e, c");
      
      break;
    case 0x5A:
      instruction = string("ld e, d");
      
      break;
    case 0x5B:
      instruction = string("ld e, e");
      
      break;
    case 0x5C:
      instruction = string("ld e, h");
      
      break;
    case 0x5D:
      instruction = string("ld e, l");
      
      break;
    case 0x5E:
      instruction = string("ld e, (hl)");
      
      break;
    case 0x5F:
      instruction = string("ld e, a");
      
      break;
    case 0x60:
      instruction = string("ld h, b");
      
      break;
    case 0x61:
      instruction = string("ld h, c");
      
      break;
    case 0x62:
      instruction = string("ld h, d");
      
      break;
    case 0x63:
      instruction = string("ld h, e");
      
      break;
    case 0x64:
      instruction = string("ld h, h");
      
      break;
    case 0x65:
      instruction = string("ld h, l");
      
      break;
    case 0x66:
      instruction = string("ld h, (hl)");
      
      break;
    case 0x67:
      instruction = string("ld h, a");
      
      break;
    case 0x68:
      instruction = string("ld l, b");
      
      break;
    case 0x69:
      instruction = string("ld l, c");
      
      break;
    case 0x6A:
      instruction = string("ld l, d");
      
      break;
    case 0x6B:
      instruction = string("ld l, e");
      
      break;
    case 0x6C:
      instruction = string("ld l, h");
      
      break;
    case 0x6D:
      instruction = string("ld l, l");
      
      break;
    case 0x6E:
      instruction = string("ld l, (hl)");
      
      break;
    case 0x6F:
      instruction = string("ld l, a");
      
      break;
    case 0x70:
      instruction = string("ld (hl), b");
      
      break;
    case 0x71:
      instruction = string("ld (hl), c");
      
      break;
    case 0x72:
      instruction = string("ld (hl), d");
      
      break;
    case 0x73:
      instruction = string("ld (hl), e");
      
      break;
    case 0x74:
      instruction = string("ld (hl), h");
      
      break;
    case 0x75:
      instruction = string("ld (hl), l");
      
      break;
    case 0x76:
      instruction = string("halt");
      
      break;
    case 0x77:
      instruction = string("ld (hl), a");
      
      break;
    case 0x78:
      instruction = string("ld a, b");
      
      break;
    case 0x79:
      instruction = string("ld a, c");
      
      break;
    case 0x7A:
      instruction = string("ld a, d");
      
      break;
    case 0x7B:
      instruction = string("ld a, e");
      
      break;
    case 0x7C:
      instruction = string("ld a, h");
      
      break;
    case 0x7D:
      instruction = string("ld a, l");
      
      break;
    case 0x7E:
      instruction = string("ld a, (hl)");
      
      break;
    case 0x7F:
      instruction = string("ld a, a");
      
      break;
    case 0x80:
      instruction = string("add a, b");
      
      break;
    case 0x81:
      instruction = string("add a, c");
      
      break;
    case 0x82:
      instruction = string("add a, d");
      
      break;
    case 0x83:
      instruction = string("add a, e");
      
      break;
    case 0x84:
      instruction = string("add a, h");
      
      break;
    case 0x85:
      instruction = string("add a, l");
      
      break;
    case 0x86:
      instruction = string("add a, (hl)");
      
      break;
    case 0x87:
      instruction = string("add a, a");
      
      break;
    case 0x88:
      instruction = string("adc a, b");
      
      break;
    case 0x89:
      instruction = string("adc a, c");
      
      break;
    case 0x8A:
      instruction = string("adc a, d");
      
      break;
    case 0x8B:
      instruction = string("adc a, e");
      
      break;
    case 0x8C:
      instruction = string("adc a, h");
      
      break;
    case 0x8D:
      instruction = string("adc a, l");
      
      break;
    case 0x8E:
      instruction = string("adc a, (hl)");
      
      break;
    case 0x8F:
      instruction = string("adc a, a");
      
      break;
    case 0x90:
      instruction = string("sub a, b");
      
      break;
    case 0x91:
      instruction = string("sub a, c");
      
      break;
    case 0x92:
      instruction = string("sub a, d");
      
      break;
    case 0x93:
      instruction = string("sub a, e");
      
      break;
    case 0x94:
      instruction = string("sub a, h");
      
      break;
    case 0x95:
      instruction = string("sub a, l");
      
      break;
    case 0x96:
      instruction = string("sub a, (hl)");
      
      break;
    case 0x97:
      instruction = string("sub a, a");
      
      break;
    case 0x98:
      instruction = string("sbc a, b");
      
      break;
    case 0x99:
      instruction = string("sbc a, c");
      
      break;
    case 0x9A:
      instruction = string("sbc a, d");
      
      break;
    case 0x9B:
      instruction = string("sbc a, e");
      
      break;
    case 0x9C:
      instruction = string("sbc a, h");
      
      break;
    case 0x9D:
      instruction = string("sbc a, l");
      
      break;
    case 0x9E:
      instruction = string("sbc a, (hl)");
      
      break;
    case 0x9F:
      instruction = string("sbc a, a");
      
      break;
    case 0xA0:
      instruction = string("and b");
      
      break;
    case 0xA1:
      instruction = string("and c");
      
      break;
    case 0xA2:
      instruction = string("and d");
      
      break;
    case 0xA3:
      instruction = string("and e");
      
      break;
    case 0xA4:
      instruction = string("and h");
      
      break;
    case 0xA5:
      instruction = string("and l");
      
      break;
    case 0xA6:
      instruction = string("and (hl)");
      
      break;
    case 0xA7:
      instruction = string("and a");
      
      break;
    case 0xA8:
      instruction = string("xor b");
      
      break;
    case 0xA9:
      instruction = string("xor c");
      
      break;
    case 0xAA:
      instruction = string("xor d");
      
      break;
    case 0xAB:
      instruction = string("xor e");
      
      break;
    case 0xAC:
      instruction = string("xor h");
      
      break;
    case 0xAD:
      instruction = string("xor l");
      
      break;
    case 0xAE:
      instruction = string("xor (hl)");
      
      break;
    case 0xAF:
      instruction = string("xor a");
      
      break;
    case 0xB0:
      instruction = string("or b");
      
      break;
    case 0xB1:
      instruction = string("or c");
      
      break;
    case 0xB2:
      instruction = string("or d");
      
      break;
    case 0xB3:
      instruction = string("or e");
      
      break;
    case 0xB4:
      instruction = string("or h");
      
      break;
    case 0xB5:
      instruction = string("or l");
      
      break;
    case 0xB6:
      instruction = string("or (hl)");
      
      break;
    case 0xB7:
      instruction = string("or a");
      
      break;
    case 0xB8:
      instruction = string("cp b");
      
      break;
    case 0xB9:
      instruction = string("cp c");
      
      break;
    case 0xBA:
      instruction = string("cp d");
      
      break;
    case 0xBB:
      instruction = string("cp e");
      
      break;
    case 0xBC:
      instruction = string("cp h");
      
      break;
    case 0xBD:
      instruction = string("cp l");
      
      break;
    case 0xBE:
      instruction = string("cp (hl)");
      
      break;
    case 0xBF:
      instruction = string("cp a");
      
      break;
    case 0xC0:
      instruction = string("ret nz");
      
      break;
    case 0xC1:
      instruction = string("pop bc");
      
      break;
    case 0xC5:
      instruction = string("push bc");
      
      break;
    case 0xC8:
      instruction = string("ret z");
      
      break;
    case 0xC9:
      instruction = string("ret");
      
      break;
    case 0xCF:
      instruction = string("rst 0x08");
      
      break;
    case 0xD0:
      instruction = string("ret nc");
      
      break;
    case 0xD1:
      instruction = string("pop de");
      
      break;
    case 0xD5:
      instruction = string("push de");
      
      break;
    case 0xD7:
      instruction = string("rst 0x10");
      
      break;
    case 0xD8:
      instruction = string("ret c");
      
      break;
    case 0xD9:
      instruction = string("exx");
      
      break;
    case 0xDF:
      instruction = string("rst 0x18");
      
      break;
    case 0xE0:
      instruction = string("ret po");
      
      break;
    case 0xE1:
      instruction = string("pop hl");
      
      break;
    case 0xE3:
      instruction = string("ex (sp), hl");
      
      break;
    case 0xE5:
      instruction = string("push hl");
      
      break;
    case 0xE7:
      instruction = string("rst 0x20");
      
      break;
    case 0xE8:
      instruction = string("ret pe");
      
      break;
    case 0xE9:
      instruction = string("jp (hl)");
      
      break;
    case 0xEB:
      instruction = string("ex de,hl");
      
      break;
    case 0xF0:
      instruction = string("ret p");
      
      break;
    case 0xF1:
      instruction = string("pop af");
      
      break;
    case 0xF3:
      instruction = string("di");
      
      break;
    case 0xF5:
      instruction = string("push af");
      
      break;
    case 0xF7:
      instruction = string("rst 0x30");
      
      break;
    case 0xF8:
      instruction = string("ret m");
      
      break;
    case 0xF9:
      instruction = string("ld sp,hl");
      
      break;
    case 0xFB:
      instruction = string("ei");
      
      break;
    case 0xFF:
      instruction = string("rst 0x38");
      break;
      //case 0xEF:
      //instruction=string("rst 0x28");
      //break;

    default:
      instruction = string("invalid");

      valid=false;
      size=0;
      break;
    }
#ifdef DEBUG
  cerr << instruction << " valid: " << valid << endl;
#endif
}

  
int main(int argc, char *argv[])
{

  vector <mneumonic*> mneumonics;
  vector <unsigned char> raw_code;

  if (argc==1)
    {
      cout << "usage:" << endl << argv[0] << " filename.8xp" << endl;
      exit(-1);
    }
  
  FILE *binary = fopen(argv[1], "rb");
  unsigned char buffer;

  cout << hex << uppercase;
  
  int number_of_bytes=0;
  int memory_start=40341;
  
  //if( just_asm ) cout << "idx\tmem\top\tdec\tmneumonic" << endl;

  // read them all in
  while( fread(&buffer, sizeof(unsigned char), sizeof(buffer), binary) )
    {
      //cerr << buffer << " ";
      raw_code.push_back(buffer);
      number_of_bytes++;
    }

  // parse them and create a vector of mneumonics
  // printout entire program as values
  cerr << "# of bytes read: " << std::dec << number_of_bytes << endl;

  int memory_location = memory_start;
  for( int i=76; i<(number_of_bytes-2); i++ )
    {
      //cout << std::hex << (int) raw_code[i] << " ";;
      mneumonic * m = new mneumonic(raw_code[i]);
      // if the one-byte opcode doesn't create a full
      // instruction, then try a two-byte opcode
      if (!m->isValid())
	{
	  delete m;
#ifdef DEBUG
	  cerr << "[1 Byte Mneumonic is not valid - trying 2]" << endl;
#endif
	  m = new mneumonic(raw_code[i], raw_code[i+1]);
	}
      // or maybe it's a three
      if (!m->isValid())
	{
	  delete m;
#ifdef DEBUG
	  cerr << "[2 Byte Mneumonic is not valid - trying 3]" << endl;
#endif
	  m = new mneumonic(raw_code[i], raw_code[i+1], raw_code[i+2]);
	}

      // maybe a 4?  If not - it's got to be data
      if (!m->isValid())
	{
	  delete m;
#ifdef DEBUG
	  cerr << "[3 Byte Mneumonic is not valid - trying 4]" << endl;
#endif
	  m = new mneumonic(raw_code[i], raw_code[i+1], raw_code[i+2], raw_code[i+3]);
	}
      
      // if it's STILL NOT VALID then just do one byte.
      if( !m->isValid() )
	{
	  delete m;
#ifdef DEBUG
	  cerr << "[4 Byte Mneumonic is not valid - must be data]" << endl;
#endif
	  m = new mneumonic();
	  m->setData( raw_code[i] );
	}
      
      m->setAddress( memory_location );
      memory_location += m->getSize();
      
      mneumonics.push_back(m);
      i+=(m->getSize()-1);

#ifdef DEBUG
      cerr << "size of mneumonic is: " << m->getSize() << endl;
#endif


    }
  cout << endl;

  cout << "SIZE: " << mneumonics.size() << endl;
  
  for( int j = 0; j<mneumonics.size(); j++ )
    {
      //cout << "#" << j << ": ";
      cout << *mneumonics[j];
      // mneumonics[j]->print();
    }
  
  return 0;
}
// if( just_asm ) cout << "Size: " << lb+16*hb << endl << "Sum: " << dec << cs << endl << "Checksum: "  << hex <<  (cs & 0xFF00)/0xFF  <<  " " <<   (cs & 0xFF)  <<  endl;


/* 

   Offset(hex)

   0x00 - 0x07: "**TI83F*"
   0x08 - 0x0B: 0x1A, 0x0A, 0x00
   0x0C - 0x34: Comment
   0x35 - 0x36: File length - $39 = Size of all data in the .8xx file from byte $37 to last byte before checksum
   Variable Header

   0x37 - 0x38: Variable header length = 0B 00
   0x39 - 0x41: Variable Header

   0x39 - 0x3A: Length of data (word)
   0x3B : Program Type - 5=Unprotected, 6= Protected
   0x3C - 0x43: Program Name (0-filled)

   Data

   0x44 - 0x45: Length of data
   0x46 - 0x47: Length of program
   0x50 - 0x??: Program Data
   0x?? - EOF: Checksum (word)

*/

// Byte 0x39 and 0x3A are Low Order then High Order
// of End of file minus 0x49 (73 decimal)
// 0x3C - 0x43 Program Name Zero Filled)

// The last two bytes are always a checksum of the data section, which always confused me, but is apparently just lowest 16 bits of the sum of all the data.
//http://www.ticalc.org/archives/files/fileinfo/247/24750.html does a pretty good job detail this all. 



// 2A 2A 54 49 38 33 46 2A 1A A 0 28 43 29 32 30 32 30 20 6D 20 70 65 6C 6C 65 67 72 69 6E 6F 20 6D 6B 70 65 6C 6C 65 67 2E 66 72 65 73 68 65 6C 6C 2E 6F 72 67 7E 2 D 0 6D 2 6 44 49 53 54 41 4E 43 45 0 0 2 FF 6B 2 BB 6D 21 29 9E 22 3 9E 21 5 9E 6 4 E5 2A 3 9E E5 EF A 45 E1 23 23 23 23 22 3 9E E1 CD 39 9E 54 5D E5 21 69 9F EF 83 41 E1 11 9 0 19 10 DC 21 20 9E E7 21 E 9E EF 80 41 EF 6F 40 EF 81 40 11 20 9E EF 83 41 21 17 9E E7 21 5 9E EF 80 41 EF 6F 40 EF 81 40 21 20 9E EF 80 41 F7 EF 9C 40 EF BF 4A 3E A EF 99 49 21 8E 84 EF A 45 C9 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 58 31 3E 0 59 31 3E 0 58 32 3E 0 59 32 3E 0 F5 C5 D5 E5 EF C4 45 21 F2 9F 22 F0 9F 1 0 A 36 0 23 10 FB 21 EF 9F 36 0 21 F2 9F 22 F0 9F 3A EF 9F FE A D2 E 9F EF 18 40 FE 0 28 F9 32 FD 9F FE 9 CA EC 9F FE 38 CA A4 9F FE 2 CA A4 9F FE 11 CA D2 9F FE B CA D2 9F FE 21 CA E6 9F FE 22 CA E6 9F FE 1A CA E6 9F FE 12 CA E6 9F FE 23 CA E6 9F FE 1B CA E6 9F FE 13 CA E6 9F FE 24 CA E6 9F FE 1C CA E6 9F FE 14 CA E6 9F FE 19 CA E6 9F 18 A4 F5 E5 3E 19 21 B8 9E 2B 77 E1 F1 C9 F5 E5 AF 0 21 B8 9E 2B 77 E1 F1 C9 F5 E5 3E B 21 81 9E 2B 77 3E 11 21 7C 9E 2B 77 E1 F1 C9 F5 E5 3E 0 21 81 9E 2B 77 3E 0 21 7C 9E 2B 77 E1 F1 C9 33 36 39 B0 FF FF FF 3A 32 35 38 FF FF FF FF 30 31 34 37 3A EF 9F FE 0 28 43 21 60 9F E7 D7 38 3 EF 51 43 3A EF 9F 26 0 6F EF 30 43 13 13 21 F2 9F 3A EF 9F 6 0 4F ED B0 21 60 9F 11 78 84 1 4 0 ED B0 EF 9B 4A 11 69 9F EF 83 41 21 60 9F E7 D7 EF 51 43 21 69 9F E7 EF 2E 45 EF BE 45 E1 D1 C1 F1 C9 3 5E 5 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 F5 C5 E5 3A FD 9F E 12 91 4F 6 0 21 FB 9E 9 7E 2A F0 9F 77 FE B0 38 4 3E 2D 18 6 FE 3A 38 2 3E 2E EF 4 45 21 F0 9F 34 21 EF 9F 34 E1 C1 F1 C9 3A EF 9F FE 0 CA 59 9E 2A F0 9F 2B 36 0 22 F0 9F 3A EF 9F 3D 32 EF 9F 2A 4C 84 2B 22 4C 84 3E 20 EF 4 45 2A 4C 84 2B 22 4C 84 C3 59 9E AF 21 EF 9F 46 B8 DA 59 9E 3E 15 32 FD 9F CD 72 9F C3 59 9E CD 72 9F C3 59 9E C3 E 9F 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 2D 13 
