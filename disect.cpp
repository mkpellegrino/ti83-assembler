#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;
int main(int argc, char *argv[])
{
  FILE *binary = fopen(argv[1], "rb");
  unsigned char buffer;
  
  cout << hex << uppercase;
  
  while( fread(&buffer, sizeof(unsigned char), sizeof(buffer), binary) )
    {
      if( buffer < 0x10 ) cout << "0";
      cout << (int) buffer << " ";      
    }
  return 0;
}
