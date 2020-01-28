#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;
int main(int argc, char *argv[])
{
  FILE *binary = fopen(argv[1], "rb");
  unsigned char buffer;

  cout << hex << uppercase;
  int i=0;
  int cs=0;
  int lb=0;
  int hb=0;
  int e=0;

  unsigned char x1,x2;
  while( fread(&buffer, sizeof(unsigned char), sizeof(buffer), binary) )
    {
      
      if( i== 57) lb = buffer;
      if( i== 59) hb = buffer;
      if( i== 60) e=lb+hb*16*16;
      if( i>=55 ) cs+=buffer;
      cout << i++ << ":";
      if( buffer < 0x10 ) cout << "0";
      cout << (int) buffer /* << " " << (char) buffer */ << endl;
      x1=x2;
      x2=buffer;
    }
  cs-=x1;
  cs-=x2;
  cout << "Size: " << lb+16*hb << endl << "Sum: " << dec << cs << endl << "Checksum: "  << hex <<  (cs & 0xFF00)/0xFF  <<  " " <<   (cs & 0xFF)  <<  endl;
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
