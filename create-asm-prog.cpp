#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;
#define SZ 80
int main(int argc, char *argv[])
{
  FILE *binary = fopen(argv[1], "wb");
  unsigned char buffer[SZ+1]=
    {
      '*','*','T','I','8','3','F','*', /* 0x00 - 0x07 */  
      0x1A,0x0A,0x00, /* 0x08 - 0x0A */  
      
      'A','u','t','h','o', /* 0x0B - 0x34 (42 bytes) */
      'r',' ','i','n','f','o',':',' ','h','t','t','p',':','/','/','m',
      'k','p','e','l','l','e','g','.','f','r','e','e','s','h','e','l',
      'l','.','o','r','g',
      
      0x18, 0x00, /* file length - 57d */  /*  len of prog + 19  in 2 bytes */
      0x0D, 0x00, /* unknown characters */
      0x07, 0x00, /* len of prog + 2 bytes */
      0x06, /* protection */
      'T','I','T','E','S','T','0','2',
      0x00,0x00, /* two nulls */
      (SZ-73),0x00, /* len of prog + 2 bytes */
      0x05,0x00, /* len of prog */
      0xBB,0x6D, /* program header */
      0xC9, /* code */
      0xC9, /* code */
      0xC9, /* code */
      0x4D,0x04 /* checksum */
      
  };

  int cs=0;
  // calculate the checksum
  for( int i=55; i<(SZ-2); i++ )
    {
      cs+=buffer[i];
    }


  int csL, csH=0;

  csL = cs&0xFF;

  csH = (cs & 0xFF00)/0xFF;

  buffer[79]=csL;
  buffer[80]=csH;
  
  for( int i=0; i<81; i++ )
    {
      cout << i << ": " << buffer[i] << endl;
    }
  
  fwrite(&buffer, sizeof(unsigned char), sizeof(buffer), binary);
  return 0;
}


/* 

Offset(hex)

0x00 - 0x07: "**TI83F*"
0x08 - 0x0B: 0x1A, 0x0A, 0x00
0x0B - 0x34: Comment
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
