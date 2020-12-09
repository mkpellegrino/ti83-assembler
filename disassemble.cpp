#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
using namespace std;
int main(int argc, char *argv[])
{
  if (argc==1)
    {
      cout << "usage:" << endl << argv[0] << " filename.8xp a|h (optional)" << endl;
      exit(-1);
    }
  int just_asm=1;
  int raw=0;
  if( argc==3 )
    {
      if( string(argv[2]) == string("a") ) just_asm=0;
      if( string(argv[2]) == string("h") ) raw=1;
    }
  FILE *binary = fopen(argv[1], "rb");
  unsigned char buffer;
  unsigned char b1;
  unsigned char b2;
  unsigned char b3;
  string instruction;
  cout << hex << uppercase;
  int i=0;
  int cs=0;
  int lb=0;
  int hb=0;
  int e=0;

  int memory_start=40341;
  int post=0;
  
  unsigned char x1,x2;
  if( just_asm ) cout << "idx\tmem\top\tdec\tmneumonic" << endl;
  while( fread(&buffer, sizeof(unsigned char), sizeof(buffer), binary) )
    {
      // if( i<76 )
      //{
      //  cout << dec << i << " 0x" << hex << (int) buffer << endl;
      //}
      if( i== 57) lb = buffer;
      if( i== 59) hb = buffer;
      if( i== 60) e=lb+hb*16*16;
      if( i>=55 ) cs+=buffer;
      if( i>=76 || raw==1)
	{
	  switch( buffer )
	    {
	      // case 0x00:
	      // instruction="";
	      // post=0;	    
	      // break;
	    case 0x00:
	      instruction="nop";
	      post=0;
	      break;
	    case 0x01:
	      instruction="ld bc, **";
	      post=2;
	      break;
	    case 0x02:
	      instruction="ld (bc), a";
	      post=0;
	      break;
	    case 0x03:
	      instruction="inc bc";
	      post=0;
	      break;
	    case 0x04:
	      instruction="inc b";
	      post=0;
	      break;
	    case 0x05:
	      instruction="dec b";
	      post=0;
	      break;
	    case 0x06:
	      instruction="ld b, *";
	      post=1;
	      break;
	    case 0x07:
	      instruction="rlca";
	      post=0;
	      break;
	    case 0x08:
	      instruction="ex af, af'";
	      post=0;
	      break;
	    case 0x09:
	      instruction="add hl, bc";
	      post=0;
	      break;
	    case 0x0A:
	      instruction="ld a, (bc)";
	      post=0;
	      break;
	    case 0x0B:
	      instruction="dec bc";
	      post=0;
	      break;
	    case 0x0C:
	      instruction="inc c";
	      post=0;
	      break;
	    case 0x0D:
	      instruction="dec c";
	      post=0;
	      break;
	    case 0x0E:
	      instruction="ld c, *";
	      post=1;
	      break;
	    case 0x0F:
	      instruction="rrca";
	      post=0;
	      break;
	    case 0x10:
	      instruction="djnc *";
	      post=1;
	      break;
	    case 0x11:
	      instruction="ld de, **";
	      post=2;
	      break;
	    case 0x12:
	      instruction="ld (de), a";
	      post=0;
	      break;
	    case 0x13:
	      instruction="inc de";
	      post=0;
	      break;
	    case 0x14:
	      instruction="inc d";
	      post=0;
	      break;
	    case 0x15:
	      instruction="dec d";
	      post=0;
	      break;
	    case 0x16:
	      instruction="ld d, *";
	      post=1;
	      break;
	    case 0x17:
	      instruction="rla";
	      post=0;
	      break;
	    case 0x18:
	      instruction="jr *";
	      post=1;
	      break;
	    case 0x19:
	      instruction="add dl, de";
	      post=0;
	      break;
	    case 0x1A:
	      instruction="ls a, (de)";
	      post=0;
	      break;
	    case 0x1B:
	      instruction="dec de";
	      post=0;
	      break;
	    case 0x1C:
	      instruction="inc e";
	      post=0;
	      break;
	    case 0x1D:
	      instruction="dec e";
	      post=0;
	      break;
	    case 0x1E:
	      instruction="ld e, *";
	      post=1;
	      break;
	    case 0x1F:
	      instruction="rra";
	      post=0;
	      break;
	    case 0x20:
	      instruction="jr nc, *";
	      post=1;
	      break;
	    case 0x21:
	      instruction="ld hl, **";
	      post=2;
	      break;
	    case 0x22:
	      instruction="ld (**), hl";
	      post=2;
	      break;
	    case 0x23:
	      instruction="inc hl";
	      post=0;
	      break;
	    case 0x24:
	      instruction="inc h";
	      post=0;
	      break;
	    case 0x25:
	      instruction="dec h";
	      post=0;
	      break;
	    case 0x26:
	      instruction="ld h, *";
	      post=1;
	      break;
	    case 0x27:
	      instruction="daa";
	      post=0;
	      break;
	    case 0x28:
	      instruction="jr z, *";
	      post=1;
	      break;
	    case 0x29:
	      instruction="add hl, hl";
	      post=0;
	      break;
	    case 0x2A:
	      instruction="ld hl, (**)";
	      post=2;
	      break;
	    case 0x2B:
	      instruction="dec hl";
	      post=0;
	      break;
	    case 0x2C:
	      instruction="inc l";
	      post=0;
	      break;
	    case 0x2D:
	      instruction="dec l";
	      post=0;
	      break;
	    case 0x2E:
	      instruction="ld l, *";
	      post=1;
	      break;
	    case 0x2F:
	      instruction="cpl";
	      post=0;
	      break;
	    case 0x30:
	      instruction="jr nc, *";
	      post=1;
	      break;
	    case 0x31:
	      instruction="ld sp, **";
	      post=2;
	      break;
	    case 0x32:
	      instruction="ld (**), a";
	      post=2;
	      break;
	    case 0x33:
	      instruction="inc sp";
	      post=0;
	      break;
	    case 0x34:
	      instruction="inc (hl)";
	      post=0;
	      break;
	    case 0x35:
	      instruction="dec (hl)";
	      post=0;
	      break;
	    case 0x36:
	      instruction="ld (hl), *";
	      post=1;
	      break;
	    case 0x37:
	      instruction="scf";
	      post=0;
	      break;
	    case 0x38:
	      instruction="jr c, *";
	      post=1;
	      break;
	    case 0x39:
	      instruction="add hl, sp";
	      post=0;
	      break;
	    case 0x3A:
	      instruction="ld a, (**)";
	      post=2;
	      break;
	    case 0x3B:
	      instruction="dec sp";
	      post=0;
	      break;
	    case 0x3C:
	      instruction="inc a";
	      post=0;
	      break;
	    case 0x3D:
	      instruction="dec a";
	      post=0;
	      break;
	    case 0x3E:
	      instruction="ld a, *";
	      post=1;
	      break;
	    case 0x3F:
	      instruction="ccf";
	      post=0;
	      break;
	    case 0x40:
	      instruction="ld b, b";
	      post=0;
	      break;
	    case 0x41:
	      instruction="ld b, c";
	      post=0;
	      break;
	    case 0x42:
	      instruction="ld b,d";
	      post=0;
	      break;
	    case 0x43:
	      instruction="ld b,e";
	      post=0;
	      break;
	    case 0x44:
	      instruction="ld b,h";
	      post=0;
	      break;
	    case 0x45:
	      instruction="ld b,l";
	      post=0;
	      break;
	    case 0x46:
	      instruction="ld b,(hl)";
	      post=0;
	      break;
	    case 0x47:
	      instruction="ld b,a";
	      post=0;
	      break;
	    case 0x48:
	      instruction="ld c,b";
	      post=0;
	      break;
	    case 0x49:
	      instruction="ld c,c";
	      post=0;
	      break;
	    case 0x4A:
	      instruction="ld c,d";
	      post=0;
	      break;
	    case 0x4B:
	      instruction="ld c,e";
	      post=0;
	      break;
	    case 0x4C:
	      instruction="ld c,h";
	      post=0;
	      break;
	    case 0x4D:
	      instruction="ld c,l";
	      post=0;
	      break;
	    case 0x4E:
	      instruction="ld c,(hl)";
	      post=0;
	      break;
	    case 0x4F:
	      instruction="ld c,a";
	      post=0;
	      break;
	    case 0x50:
	      instruction="ld d,b";
	      post=0;
	      break;
	    case 0x51:
	      instruction="ld d,c";
	      post=0;
	      break;
	    case 0x52:
	      instruction="ld d,d";
	      post=0;
	      break;
	    case 0x53:
	      instruction="ld d,e";
	      post=0;
	      break;
	    case 0x54:
	      instruction="ld d,h";
	      post=0;
	      break;
	    case 0x55:
	      instruction="ld d,l";
	      post=0;
	      break;
	    case 0x56:
	      instruction="ld d,(hl)";
	      post=0;
	      break;
	    case 0x57:
	      instruction="ld d,a";
	      post=0;
	      break;
	    case 0x58:
	      instruction="ld e,b";
	      post=0;
	      break;
	    case 0x59:
	      instruction="ld e,c";
	      post=0;
	      break;
	    case 0x5A:
	      instruction="ld e,d";
	      post=0;
	      break;
	    case 0x5B:
	      instruction="ld e,e";
	      post=0;
	      break;
	    case 0x5C:
	      instruction="ld e,h";
	      post=0;
	      break;
	    case 0x5D:
	      instruction="ld e,l";
	      post=0;
	      break;
	    case 0x5E:
	      instruction="ld e,(hl)";
	      post=0;
	      break;
	    case 0x5F:
	      instruction="ld e,a";
	      post=0;
	      break;
	    case 0x60:
	      instruction="ld h,b";
	      post=0;
	      break;
	    case 0x61:
	      instruction="ld h,c";
	      post=0;
	      break;
	    case 0x62:
	      instruction="ld h,d";
	      post=0;
	      break;
	    case 0x63:
	      instruction="ld h,e";
	      post=0;
	      break;
	    case 0x64:
	      instruction="ld h,h";
	      post=0;
	      break;
	    case 0x65:
	      instruction="ld h,l";
	      post=0;
	      break;
	    case 0x66:
	      instruction="ld h,(hl)";
	      post=0;
	      break;
	    case 0x67:
	      instruction="ld h,a";
	      post=0;
	      break;
	    case 0x68:
	      instruction="ld l,b";
	      post=0;
	      break;
	    case 0x69:
	      instruction="ld l,c";
	      post=0;
	      break;
	    case 0x6A:
	      instruction="ld l,d";
	      post=0;
	      break;
	    case 0x6B:
	      instruction="ld l,e";
	      post=0;
	      break;
	    case 0x6C:
	      instruction="ld l,h";
	      post=0;
	      break;
	    case 0x6D:
	      instruction="ld l,l";
	      post=0;
	      break;
	    case 0x6E:
	      instruction="ld l,(hl)";
	      post=0;
	      break;
	    case 0x6F:
	      instruction="ld l,a";
	      post=0;
	      break;
	    case 0x70:
	      instruction="ld (hl),b";
	      post=0;
	      break;
	    case 0x71:
	      instruction="ld (hl),c";
	      post=0;
	      break;
	    case 0x72:
	      instruction="ld (hl),d";
	      post=0;
	      break;
	    case 0x73:
	      instruction="ld (hl),e";
	      post=0;
	      break;
	    case 0x74:
	      instruction="ld (hl),h";
	      post=0;
	      break;
	    case 0x75:
	      instruction="ld (hl),l";
	      post=0;
	      break;
	    case 0x76:
	      instruction="halt";
	      post=0;
	      break;
	    case 0x77:
	      instruction="ld (hl),a";
	      post=0;
	      break;
	    case 0x78:
	      instruction="ld a,b";
	      post=0;
	      break;
	    case 0x79:
	      instruction="ld a,c";
	      post=0;
	      break;
	    case 0x7A:
	      instruction="ld a,d";
	      post=0;
	      break;
	    case 0x7B:
	      instruction="ld a,e";
	      post=0;
	      break;
	    case 0x7C:
	      instruction="ld a,h";
	      post=0;
	      break;
	    case 0x7D:
	      instruction="ld a,l";
	      post=0;
	      break;
	    case 0x7E:
	      instruction="ld a,(hl)";
	      post=0;
	      break;
	    case 0x7F:
	      instruction="ld a,a";
	      post=0;
	      break;
	    case 0x80:
	      instruction="add a,b";
	      post=0;
	      break;
	    case 0x81:
	      instruction="add a,c";
	      post=0;
	      break;
	    case 0x82:
	      instruction="add a,d";
	      post=0;
	      break;
	    case 0x83:
	      instruction="add a,e";
	      post=0;
	      break;
	    case 0x84:
	      instruction="add a,h";
	      post=0;
	      break;
	    case 0x85:
	      instruction="add a,l";
	      post=0;
	      break;
	    case 0x86:
	      instruction="add a,(hl)";
	      post=0;
	      break;
	    case 0x87:
	      instruction="add a,a";
	      post=0;
	      break;
	    case 0x88:
	      instruction="adc a,b";
	      post=0;
	      break;
	    case 0x89:
	      instruction="adc a,c";
	      post=0;
	      break;
	    case 0x8A:
	      instruction="adc a,d";
	      post=0;
	      break;
	    case 0x8B:
	      instruction="adc a,e";
	      post=0;
	      break;
	    case 0x8C:
	      instruction="adc a,h";
	      post=0;
	      break;
	    case 0x8D:
	      instruction="adc a,l";
	      post=0;
	      break;
	    case 0x8E:
	      instruction="adc a,(hl)";
	      post=0;
	      break;
	    case 0x8F:
	      instruction="adc a,a";
	      post=0;
	      break;
	    case 0x90:
	      instruction="sub a,b";
	      post=0;
	      break;
	    case 0x91:
	      instruction="sub a,c";
	      post=0;
	      break;
	    case 0x92:
	      instruction="sub a,d";
	      post=0;
	      break;
	    case 0x93:
	      instruction="sub a,e";
	      post=0;
	      break;
	    case 0x94:
	      instruction="sub a,h";
	      post=0;
	      break;
	    case 0x95:
	      instruction="sub a,l";
	      post=0;
	      break;
	    case 0x96:
	      instruction="sub a,(hl)";
	      post=0;
	      break;
	    case 0x97:
	      instruction="sub a,a";
	      post=0;
	      break;
	    case 0x98:
	      instruction="sbc a,b";
	      post=0;
	      break;
	    case 0x99:
	      instruction="sbc a,c";
	      post=0;
	      break;
	    case 0x9A:
	      instruction="sbc a,d";
	      post=0;
	      break;
	    case 0x9B:
	      instruction="sbc a,e";
	      post=0;
	      break;
	    case 0x9C:
	      instruction="sbc a,h";
	      post=0;
	      break;
	    case 0x9D:
	      instruction="sbc a,l";
	      post=0;
	      break;
	    case 0x9E:
	      instruction="sbc a,(hl)";
	      post=0;
	      break;
	    case 0x9F:
	      instruction="sbc a,a";
	      post=0;
	      break;
	    case 0xA0:
	      instruction="and b";
	      post=0;
	      break;
	    case 0xA1:
	      instruction="and c";
	      post=0;
	      break;
	    case 0xA2:
	      instruction="and d";
	      post=0;
	      break;
	    case 0xA3:
	      instruction="and e";
	      post=0;
	      break;
	    case 0xA4:
	      instruction="and h";
	      post=0;
	      break;
	    case 0xA5:
	      instruction="and l";
	      post=0;
	      break;
	    case 0xA6:
	      instruction="and (hl)";
	      post=0;
	      break;
	    case 0xA7:
	      instruction="and a";
	      post=0;
	      break;
	    case 0xA8:
	      instruction="xor b";
	      post=0;
	      break;
	    case 0xA9:
	      instruction="xor c";
	      post=0;
	      break;
	    case 0xAA:
	      instruction="xor d";
	      post=0;
	      break;
	    case 0xAB:
	      instruction="xor e";
	      post=0;
	      break;
	    case 0xAC:
	      instruction="xor h";
	      post=0;
	      break;
	    case 0xAD:
	      instruction="xor l";
	      post=0;
	      break;
	    case 0xAE:
	      instruction="xor (hl)";
	      post=0;
	      break;
	    case 0xAF:
	      instruction="xor a";
	      post=0;
	      break;
	    case 0xB0:
	      instruction="or b";
	      post=0;
	      break;
	    case 0xB1:
	      instruction="or c";
	      post=0;
	      break;
	    case 0xB2:
	      instruction="or d";
	      post=0;
	      break;
	    case 0xB3:
	      instruction="or e";
	      post=0;
	      break;
	    case 0xB4:
	      instruction="or h";
	      post=0;
	      break;
	    case 0xB5:
	      instruction="or l";
	      post=0;
	      break;
	    case 0xB6:
	      instruction="or (hl)";
	      post=0;
	      break;
	    case 0xB7:
	      instruction="or a";
	      post=0;
	      break;
	    case 0xB8:
	      instruction="cp b";
	      post=0;
	      break;
	    case 0xB9:
	      instruction="cp c";
	      post=0;
	      break;
	    case 0xBA:
	      instruction="cp d";
	      post=0;
	      break;
	    case 0xBB:
	      instruction="cp e";
	      post=0;
	      break;
	    case 0xBC:
	      instruction="cp h";
	      post=0;
	      break;
	    case 0xBD:
	      instruction="cp l";
	      post=0;
	      break;
	    case 0xBE:
	      instruction="cp (hl)";
	      post=0;
	      break;
	    case 0xBF:
	      instruction="cp a";
	      post=0;
	      break;
	    case 0xC0:
	      instruction="ret nz";
	      post=0;
	      break;
	    case 0xC1:
	      instruction="pop bc";
	      post=0;
	      break;
	    case 0xC2:
	      instruction="jp nz, **";
	      post=2;
	      break;
	    case 0xC3:
	      instruction="jp **";
	      post=2;
	      break;
	    case 0xC4:
	      instruction="call nz, **";
	      post=2;
	      break;
	    case 0xC5:
	      instruction="push bc";
	      post=0;
	      break;
	    case 0xC6:
	      instruction="add a, *";
	      post=1;
	      break;
	    case 0xC7:
	      instruction="rst *";
	      post=1;
	      break;
	    case 0xC8:
	      instruction="ret z";
	      post=0;
	      break;
	    case 0xC9:
	      instruction="ret ";
	      post=0;
	      break;
	    case 0xCA:
	      instruction="jp z, **";
	      post=2;
	      break;
	    case 0xCB:
	      // this is an extended instruction
	      //instruction="*";
	      post=1;
	      break;
	    case 0xCC:
	      instruction="call z, **";
	      post=2;
	      break;
	    case 0xCD:
	      instruction="call **";
	      post=2;
	      break;
	    case 0xCE:
	      instruction="adc a, *";
	      post=1;
	      break;
	    case 0xCF:
	      instruction="rst 0x08";
	      post=0;
	      break;
	    case 0xD0:
	      instruction="ret nc";
	      post=0;
	      break;
	    case 0xD1:
	      instruction="pop de";
	      post=0;
	      break;
	    case 0xD2:
	      instruction="jp nc, **";
	      post=2;
	      break;
	    case 0xD3:
	      instruction="out (*), a";
	      post=1;
	      break;
	    case 0xD4:
	      instruction="call nc, **";
	      post=2;
	      break;
	    case 0xD5:
	      instruction="push de";
	      post=0;
	      break;
	    case 0xD6:
	      instruction="sub a, *";
	      post=1;
	      break;
	    case 0xD7:
	      instruction="rst 0x10";
	      post=0;
	      break;
	    case 0xD8:
	      instruction="ret c";
	      post=0;
	      break;
	    case 0xD9:
	      instruction="exx ";
	      post=0;
	      break;
	    case 0xDA:
	      instruction="jp c, **";
	      post=2;
	      break;
	    case 0xDB:
	      instruction="in a, (*)";
	      post=1;
	      break;
	    case 0xDC:
	      instruction="call c, **";
	      post=2;
	      break;
	    case 0xDD:
	      instruction="**** ";
	      post=2;
	      break;
	    case 0xDE:
	      instruction="sbc a, *";
	      post=1;
	      break;
	    case 0xDF:
	      instruction="rst 0x18";
	      post=0;
	      break;
	    case 0xE0:
	      instruction="ret po";
	      post=0;
	      break;
	    case 0xE1:
	      instruction="pop hl";
	      post=0;
	      break;
	    case 0xE2:
	      instruction="jp po, **";
	      post=2;
	      break;
	    case 0xE3:
	      instruction="ex (sp), hl";
	      post=0;
	      break;
	    case 0xE4:
	      instruction="call po, **";
	      post=2;
	      break;
	    case 0xE5:
	      instruction="push hl";
	      post=0;
	      break;
	    case 0xE6:
	      instruction="and *";
	      post=1;
	      break;
	    case 0xE7:
	      instruction="rst 0x20";
	      post=0;
	      break;
	    case 0xE8:
	      instruction="ret pe";
	      post=0;
	      break;
	    case 0xE9:
	      instruction="jp (hl)";
	      post=0;
	      break;
	    case 0xEA:
	      instruction="jp pe, **";
	      post=2;
	      break;
	    case 0xEB:
	      instruction="ex de,hl";
	      post=0;
	      break;
	    case 0xEC:
	      instruction="call pe, **";
	      post=2;
	     
	      break;
	    case 0xED:
	      {
		// These are extended op codes
		post=1;
	      }
	      break;
	    case 0xEE:
	      instruction="xor *";
	      post=1;
	      break;
	    case 0xEF:
	      instruction="rst 0x28";
	      
	      post=2;
	      break;
	    case 0xF0:
	      instruction="ret p";
	      post=0;
	      break;
	    case 0xF1:
	      instruction="pop af";
	      post=0;
	      break;
	    case 0xF2:
	      instruction="jp p, **";
	      post=2;
	      break;
	    case 0xF3:
	      instruction="di ";
	      post=0;
	      break;
	    case 0xF4:
	      instruction="call p, **";
	      post=2;
	      break;
	    case 0xF5:
	      instruction="push af";
	      post=0;
	      break;
	    case 0xF6:
	      instruction="or *";
	      post=1;
	      break;
	    case 0xF7:
	      instruction="rst 0x30";
	      post=0;
	      break;
	    case 0xF8:
	      instruction="ret m";
	      post=0;
	      break;
	    case 0xF9:
	      instruction="ld sp,hl";
	      post=0;
	      break;
	    case 0xFA:
	      instruction="jp m, **";
	      post=2;
	      break;
	    case 0xFB:
	      instruction="ei ";
	      post=0;
	      break;
	    case 0xFC:
	      instruction="call m, **";
	      post=2;
	      break;
	    case 0xFD:
	      instruction="****";
	      post=2;
	      break;
	    case 0xFE:
	      instruction="cp *";
	      post=1;
	      break;
	    case 0xFF:
	      instruction="rst 0x38";
	      post=0;
	      break;

	     
	    }
	  b1=0; b2=0; b3=0;
	  if( post==1 )
	    {
	      fread(&b1, sizeof(unsigned char), sizeof(b1), binary);
	    }
	    
	  if( post==2 )
	    {
	      fread(&b1, sizeof(unsigned char), sizeof(b1), binary);
	      fread(&b2, sizeof(unsigned char), sizeof(b2), binary);
	    }

	  if( buffer==0xCB )
	    {
	      switch(b1)
		{
		case 0x27:
		  instruction="sla a";
		  break;
		case 0x00:
		  instruction="rlc b";
		  break;
		default:
		  instruction="** unknown **";
		}

	    }
	  if( buffer==0xED )
	    {
	      switch(b1)
		{
		case 0xA0:
		  instruction="ldi";
		  //post=1;
		  break;
		case 0xB0:
		  instruction="ldir";
		  //post=1;
		  break;
		case 0x5B:
		  instruction="ld de, (**)";
		  fread(&b2, sizeof(unsigned char), sizeof(b2), binary);
		  fread(&b3, sizeof(unsigned char), sizeof(b3), binary);
		  post=3;
		  
		  //post=2;
		  break;
		case 0x6B:
		  instruction="ld hl, (**)";
		  fread(&b2, sizeof(unsigned char), sizeof(b2), binary);
		  fread(&b3, sizeof(unsigned char), sizeof(b3), binary);
		  post=3;
		  //post=2;
		  break;
		default:
		  instruction="** unknown **";
		}
	    }

	  if( just_asm ) cout << dec << i << "\t" <<  hex << memory_start << "\t";
	  if( just_asm )
	    {
	      if( buffer < 0x10 ) cout << "0";
	    }
	  if( just_asm ) cout << (int) buffer  << "\t" << dec << (int) buffer /* << "\t" << (char) buffer*/ << "\t";
	  cout << instruction << " ";
	  if( post==1 )
	    {
	      //cout << "0x";
	      memory_start++;
	    }
	  if( post>0 )
	    {
	      cout << std::uppercase << setfill('0') << setw(2) << right << std::hex << "0x";
	      if( b1< 16) cout << setw(1) << 0;
	      cout << (int)b1;
	    }
	  if( post==2 )
	    {
	      //if( b2< 16)
	      //{
		  //cout << 0;
	      //}
	      cout << std::uppercase << std::hex << (int)b2;
	      memory_start+=2;
	    }
	  if( post==3 )
	    {
	      //if( b2< 16)
	      //{
		  //cout << 0;
	      //}
	      cout << " " << std::uppercase << std::hex << (int)b2 << " " << (int)b3;
	      memory_start+=3;
	    }
	  memory_start++;
	  cout << endl;
	}
      i++;
      x1=x2;
      x2=buffer;
    }
  cs-=x1;
  cs-=x2;
  if( just_asm ) cout << "Size: " << lb+16*hb << endl << "Sum: " << dec << cs << endl << "Checksum: "  << hex <<  (cs & 0xFF00)/0xFF  <<  " " <<   (cs & 0xFF)  <<  endl;
  return 0;
}


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
