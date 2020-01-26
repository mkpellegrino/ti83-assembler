#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;
int main(int argc, char *argv[])
{
  FILE *binary = fopen(argv[1], "rb");
  unsigned char buffer;
  int s = 16;
  if( argc == 3 ) s = atoi(argv[2]);

  cout << hex << uppercase;
  
  while( fread(&buffer, sizeof(unsigned char), sizeof(buffer)/* /sizeof(*buffer)*/, binary) )
    {
      cout << (int) buffer << " ";      
    }
  
  return 0;
}
