// a simple TI8x Assembler with some built-in functions.
//
// By: Michael K. Pellegrino - January and February 2020
//
// Built-in functions: user_input, disp_op1, and store_op1
// the built-in functions are not compiled in unless needed.
//
// This is a work in progress and a few important notes follow.
// (more like todo's rather than notes actually)
//
// *) Not all of the z80 instruction set is in here yet for the
// disassembler
//
// *) the instruction: ld a, @P may not work as intended
//
// *) I would like to add a while-loop or a for-loop syntax that
// sets up some more robust looping stuff - but then we're
// turning it into a language other than assembler.
//
// *) The ability to have a string of bytes would be nice.  Like
// .db 0x00, 0x45, 0xFE, 0x23 instead of having them all on
// different lines.
//
// *) the DISassembler that I made is not great, but works well enough
//
// *) I think there's a problem with the ldir instruction (probably because
// it's one of the extended opcodes)
//
// *) like this program, this to-do/notes list is a work-in-progress
// and may grow or shrink from time-to-time
//
// =======================================================
//
// To build: make tias
//           make disassemble
//
// To compile: tias input.asm output.8xp
//
// To disassemble: disassemble input.8xp
//             or  disassemble input.8xp a <--- this will give you just the mneumonic listing.

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <cctype>
#include <math.h>
#include <cstdlib>

// included for the parser
#include <sstream>
#include <string>


#define RealObj 0x00
#define _OP1 0x8478
#define _OP2 0x8483
#define _OP3 0x848E
#define _OP4 0x8499
#define _OP5 0x84A4
#define _OP6 0x84AF

#define _tA 0x41
#define _tB 0x42
#define _tC 0x43
#define _tD 0x44
#define _tE 0x45
#define _tF 0x46
#define _tG 0x47
#define _tH 0x48
#define _tI 0x49
#define _tJ 0x4A
#define _tK 0x4B
#define _tL 0x4C
#define _tM 0x4D
#define _tN 0x4E
#define _tO 0x4F
#define _tP 0x50
#define _tQ 0x51
#define _tR 0x52
#define _tS 0x53
#define _tT 0x54
#define _tU 0x55
#define _tV 0x56
#define _tW 0x57
#define _tX 0x58
#define _tY 0x59
#define _tZ 0x5A
#define _tTheta 0x5B
#define _CurCol 0x844C
#define _CurRow 0x844B
using namespace std;

vector<unsigned char> byte_vector;

int error_count=0;
int compilation_failed=0;
int memorylocation;
int start_counting=0;
int program_index=0;
int function_ui=0;
int store_op1=0;
int disp_op1=0;
string name;
int line_number=0;

int add_input=0;
int add_store_op1=0;
int add_disp_op1=0;

class offset;
class label;
void a(string s);
void sysCall(string s);
void addByte(unsigned char);

vector<label> label_vector;
vector<label> label_cr_vector;
vector<offset> offset_vector;

class label
{
public:
  label( string s, int a )
  {
    index=program_index;
    memory_location=a;
    label_number=-1;
    name=s;
    line_num=line_number;
    
  }
  label( string s )
  {
    index=program_index;
    memory_location=memorylocation;
    label_number=-1;
    name=s;
    line_num=line_number;
  }
  label()
  {
    index=program_index;
    memory_location=memorylocation;
    label_number=-1;
    line_num=line_number;
    name="";
  };
  label(int l)
  {
    index=program_index;
    memory_location=memorylocation;
    label_number=l;
    line_num=line_number;
    name="";
  };
  ~label(){};
  int getIndex(){ return index; };
  int getMemoryLocation(){ return memory_location; };
  int getLabelNumber(){ return label_number; };
  string getName(){ return name; };
  void process()
  {
    if( label_number==-1 )
      {
	// search by name instead
	int ml=-1;
	for( int i=0; i<label_cr_vector.size(); i++ )
	  {
	    
	    if( label_cr_vector[i].getName() == name )
	      {
		ml=label_cr_vector[i].getMemoryLocation();
	      }	    
	  }
	if( ml == -1 )
	  {
	    // Label wasn't found
	    cerr << endl << "*** label wasn't found [" << name << "] - compilation failed ***" << endl;
	    cerr << "    line number: " << dec << line_number << endl;

	    cerr << "    program index: 0x" << hex << uppercase << program_index << " (" << dec << program_index << " decimal)" << endl << endl;
	    compilation_failed=1;
	    error_count++;
	  }

	// OPTIMIZE HERE
	
	{
	  unsigned char c = byte_vector[index-1];
	  if(  ((c==0xC3)||(c==0xC2)||(c==0xD2)||(c==0xE2)||(c==0xF2)||(c==0xCA)||(c==0xDA)||(c==0xEA)||(c==0xFA)||(c==0xE9)) && (abs(ml-memory_location)<126) )
	    {
	      cerr << "*** jump [" << index-1 << "] could be made relative...  distance is: " << dec << abs( ml-memory_location )<< " [" << name << "]\nLine Number: " << dec << line_num << endl;
	    }

	}
	byte_vector[index]=ml&0xFF;
	byte_vector[index+1]=(ml&0xFF00)/0xFF;
      }
    else
      {
#ifdef DEBUG
	cerr << "changing bytes: [" << index << " and " << index+1 << "] to ["
	     << hex << (label_cr_vector[label_number-1].getMemoryLocation()&0xFF) << " and "
	     << hex << ( (label_cr_vector[label_number-1].getMemoryLocation()&0xFF00 )/0xFF) << "]" <<  endl;
#endif 
	byte_vector[index] = (label_cr_vector[label_number-1].getMemoryLocation()&0xFF);// L
	byte_vector[index+1] = ((label_cr_vector[label_number-1].getMemoryLocation()&0xFF00)/0xFF );// H
      }
  };
private:
  int index;
  int memory_location;
  int label_number;
  int line_num;
  string name;
};



class offset
{
public:
  offset(){};
  offset(string  s)
  {
    from=program_index-1;
    offset_name=s;
    label_number=-1;
    line_num=line_number;;
#ifdef DEBUG
    cerr << "creating an offset to label: " << s << " from prog index " << program_index << " (Line Number: " << dec << line_num << ")" << endl;
#endif
  };
  
  offset(int i)
  {
    from=program_index-1;
    label_number=i;
    line_num=line_number;
#ifdef DEBUG
    cerr << "creating an offset to label: " << label_number << " from prog index " << program_index << " (Line Number: " << dec << line_num << ")" << endl;
#endif
  };
  ~offset(){};

  void process()
  {
    if( label_number == -1 )
      {
	
	// search by name
	for( int i=0; i<label_cr_vector.size(); i++ )
	  {
	    if(label_cr_vector[i].getName() == offset_name)
	      {
		label_number=i+1;
	      }
	  }

      }
    to=label_cr_vector[label_number-1].getIndex();
    diff=to-from-2;
#ifdef DEBUG
    cerr << endl << "offset -> label [" << dec << from << " -> " << to << "]\tdiff: " << diff << endl << endl;
#endif
    
    if( (diff>126) || (diff<-126) )
      {
	cerr << endl << "*** relative jump is too far - compilation failed ***" << endl;
	cerr << "    line number: " << dec << line_number << endl;
	cerr << "    program index: 0x" << hex << uppercase << from << " (" << dec << from << " decimal)" << " to [" << offset_name << "]" << endl << endl;
	compilation_failed=1;
	error_count++;
      }
    byte_vector[from+1]=(diff&0xFF);// Actually change the code
#ifdef DEBUG
    cerr << "processing offset [diff=" << hex << (unsigned int) (diff&0xFF) << " hex][" << dec << (diff&0xFF) << " dec](" << offset_name << ")" << endl;
#endif
    
  };
private:
  //int index;// Where this is in the code.
  string offset_name;
  int label_number;// Which label number i am pointing to
  int from;// Where this is in the code.
  int to;//Where are we going?
  int line_num;
  signed int diff;//How far is it to there?
};

void startCounting()
{
  start_counting=1;
  memorylocation=40341;
}

void addLabel( string s, int addr )
{
#ifdef DEBUG
  cerr <<  "[" << hex << program_index << "] creating label at [" << hex << memorylocation << "] called [" << s << "]" << endl;
#endif
  label l=label(s,addr);
  // *******

  label_cr_vector.push_back( l );
  
}
void addLabel( string s )
{
#ifdef DEBUG
  cerr <<  "[" << hex << program_index << "] creating label at [" << hex << memorylocation << "] called [" << s << "]" << endl;
#endif
  
  
  label l=label(s);
  label_cr_vector.push_back( l );
}

void addLabel()
{
#ifdef DEBUG
  cerr << "[" << hex << program_index << " creating label at [" << hex << memorylocation << "] with no name" << endl;
#endif

  label l = label();
  label_cr_vector.push_back( l );
}

void subtractByte()
{
#ifdef DEBUG
  
#endif
  program_index--;
  if( start_counting == 1) memorylocation--;
  byte_vector.erase(byte_vector.begin()+byte_vector.size()-1);
}

void addByte( unsigned char b )
{
  program_index++;
  if( start_counting == 1) memorylocation++;  
  byte_vector.push_back( (unsigned char) b );
  //#ifdef DEBUG
  //cerr << "Program Index [" << hex << program_index << "]" << endl;
  //#endif
}

void addOffset(string s)
{
  offset o = offset(s);
  offset_vector.push_back(o);
  addByte(0x00);  
}
void addOffset(int i)
{
  offset o = offset(i);
  offset_vector.push_back(o);
  addByte(0x00);
}

void addWord( int w=0x0000 )
{
  addByte( w&0xFF );
  addByte( (w&0xFF00 )/0xFF );
}

void addAddress( string s )
{
#ifdef DEBUG
  cerr << "creating placeholder at [" << hex << memorylocation << "][" << program_index << "] for [" << s << "]" << endl;
#endif

  label l = label(s);
  label_vector.push_back( l );
  addWord(0x0000);
}

void addAddress( int i )
{
#ifdef DEBUG
  cerr << "creating placeholder at [" << hex << memorylocation << "][" << program_index << "] for label #[" << i << "]" << endl;
#endif
  label l = label(i);
  label_vector.push_back( l );
  addWord(0x0000);
}

void addString( string s )
{
  for( int i=0; i<s.size(); i++ ) addByte( s[i] );
  if( start_counting == 1 ) addByte( 0x00 );
}

void setComment( string s )
{
  s.resize(42,0x00);
  addString(s);
  
}
void setName( string s )
{
  if( name == "" )
    {
      s.resize(8,0x00);
      name = s;
      addString(s);
    }
  else
    {
      name.resize(8,0x00);
      for( int i=0; i<8; i++ ) name[i]=toupper(name[i]);
      addString(name);
    }
}

void sysCall( string s )
{
  addByte( 0xEF ); // rst $28
  if( s == "PutS"){addWord(0x450A);}
  else if( s == "AbsO1O2Cp" ){addWord(0x410E);}
  else if( s == "AbsO1PAbsO2" ){addWord(0x405A);}
  else if( s == "ACos" ){addWord(0x40DE);}
  else if( s == "ACosH" ){addWord(0x40F0);}
  else if( s == "ACosRad" ){addWord(0x40D2);}
  else if( s == "AdrLEle" ){addWord(0x462D);}
  else if( s == "AdrMEle" ){addWord(0x4609);}
  else if( s == "AdrMRow" ){addWord(0x4606);}
  else if( s == "AllEq" ){addWord(0x4876);}
  else if( s == "AllocFPS" ){addWord(0x43A5);}
  else if( s == "AllocFPS1" ){addWord(0x43A8);}
  else if( s == "Angle" ){addWord(0x4102);}
  else if( s == "AnsName" ){addWord(0x4B52);}
  else if( s == "ApdSetup" ){addWord(0x4C93);}
  else if( s == "AppGetCalc" ){addWord(0x4C78);}
  else if( s == "AppGetCbl" ){addWord(0x4C75);}
  else if( s == "AppInit" ){addWord(0x404B);}
  else if( s == "Arc_Unarc" ){addWord(0x4FD8);}
  else if( s == "ArcChk" ){addWord(0x5014);}
  else if( s == "ArchiveVar" ){addWord(0x4FDB);}
  else if( s == "ASin" ){addWord(0x40E4);}
  else if( s == "ASinH" ){addWord(0x40ED);}
  else if( s == "ASinRad" ){addWord(0x40DB);}
  else if( s == "ATan" ){addWord(0x40E1);}
  else if( s == "ATan2" ){addWord(0x40E7);}
  else if( s == "ATan2Rad" ){addWord(0x40D8);}
  else if( s == "ATanH" ){addWord(0x40EA);}
  else if( s == "ATanRad" ){addWord(0x40D5);}
  else if( s == "BinOPExec" ){addWord(0x4663);}
  else if( s == "Bit_VertSplit" ){addWord(0x4FA8);}
  else if( s == "BufClear" ){addWord(0x4936);}
  else if( s == "BufClr" ){addWord(0x5074);}
  else if( s == "BufCpy" ){addWord(0x5071);}
  else if( s == "BufDelete" ){addWord(0x4912);}
  else if( s == "BufInsert" ){addWord(0x4909);}
  else if( s == "BufLeft" ){addWord(0x4903);}
  else if( s == "BufReplace" ){addWord(0x490F);}
  else if( s == "BufRight" ){addWord(0x4906);}
  else if( s == "CAbs" ){addWord(0x4E97);}
  else if( s == "CAdd" ){addWord(0x4E88);}
  else if( s == "CanAlphIns" ){addWord(0x4C69);}
  else if( s == "CancelTransmission" ){addWord(0x4ECA);}
  else if( s == "CDiv" ){addWord(0x4E94);}
  else if( s == "CDivByReal" ){addWord(0x4EBB);}
  else if( s == "CEtoX" ){addWord(0x4EA9);}
  else if( s == "CFrac" ){addWord(0x4EC1);}
  else if( s == "CheckOSValid" ){addWord(0x809C);}
  else if( s == "CheckSplitFlag" ){addWord(0x49F0);}
  else if( s == "Chk_Batt_Level" ){addWord(0x5221);}
  else if( s == "Chk_Batt_Low" ){addWord(0x50B3);}
  else if( s == "ChkFindSym" ){addWord(0x42F1);}
  else if( s == "ChkTimer0" ){addWord(0x5176);}
  else if( s == "CIntgr" ){addWord(0x4EC4);}
  else if( s == "CircCmd" ){addWord(0x47D4);}
  else if( s == "CkInt" ){addWord(0x4234);}
  else if( s == "CkOdd" ){addWord(0x4237);}
  else if( s == "CkOP1C0" ){addWord(0x4225);}
  else if( s == "CkOP1Cplx" ){addWord(0x40FC);}
  else if( s == "CkOP1FP0" ){addWord(0x4228);}
  else if( s == "CkOP1Pos" ){addWord(0x4258);}
  else if( s == "CkOP1Real" ){addWord(0x40FF);}
  else if( s == "CkOP2FP0" ){addWord(0x422B);}
  else if( s == "CkOP2Pos" ){addWord(0x4255);}
  else if( s == "CkOP2Real" ){addWord(0x42DF);}
  else if( s == "CkPosInt" ){addWord(0x4231);}
  else if( s == "CkValidNum" ){addWord(0x4270);}
  else if( s == "CleanAll" ){addWord(0x4A50);}
  else if( s == "ClearRect" ){addWord(0x4D5C);}
  else if( s == "ClearRow" ){addWord(0x4CED);}
  else if( s == "CLine" ){addWord(0x4798);}
  else if( s == "CLineS" ){addWord(0x479B);}
  else if( s == "CLN" ){addWord(0x4EA0);}
  else if( s == "CLog" ){addWord(0x4EA3);}
  else if( s == "CloseEditBuf" ){addWord(0x48D3);}
  else if( s == "CloseEditBufNoR" ){addWord(0x476E);}
  else if( s == "CloseEditEqu" ){addWord(0x496C);}
  else if( s == "CloseProg" ){addWord(0x4A35);}
  else if( s == "ClrGraphRef" ){addWord(0x4A38);}
  else if( s == "ClrLCD" ){addWord(0x4543);}
  else if( s == "ClrLCDFull" ){addWord(0x4540);}
  else if( s == "ClrLp" ){addWord(0x41D1);}
  else if( s == "ClrOP1S" ){addWord(0x425E);}
  else if( s == "ClrOP2S" ){addWord(0x425B);}
  else if( s == "ClrScrn" ){addWord(0x4549);}
  else if( s == "ClrScrnFull" ){addWord(0x4546);}
  else if( s == "ClrTxtShd" ){addWord(0x454C);}
  else if( s == "ClrWindow" ){addWord(0x454F);}
  else if( s == "CMltByReal" ){addWord(0x4EB8);}
  else if( s == "CmpSyms" ){addWord(0x4A4A);}
  else if( s == "CMult" ){addWord(0x4E8E);}
  else if( s == "Conj" ){addWord(0x4EB5);}
  else if( s == "ContinueGetByte" ){addWord(0x4F00);}
  else if( s == "ConvDim" ){addWord(0x4B43);}
  else if( s == "ConvDim00" ){addWord(0x4B46);}
  else if( s == "ConvLcToLr" ){addWord(0x4A23);}
  else if( s == "ConvLrToLc" ){addWord(0x4A56);}
  else if( s == "ConvOP1" ){addWord(0x4AEF);}
  else if( s == "COP1Set0" ){addWord(0x4105);}
  else if( s == "CopyFlashPage" ){addWord(0x5098);}
  else if( s == "Cos" ){addWord(0x40C0);}
  else if( s == "CosH" ){addWord(0x40CC);}
  else if( s == "CpHLDE" ){addWord(0x400C);}
  else if( s == "CPoint" ){addWord(0x4DC8);}
  else if( s == "CPointS" ){addWord(0x47F5);}
  else if( s == "CpOP1OP2" ){addWord(0x4111);}
  else if( s == "CpOP4OP3" ){addWord(0x4108);}
  else if( s == "CpyO1ToFPS1" ){addWord(0x445C);}
  else if( s == "CpyO1ToFPS2" ){addWord(0x446B);}
  else if( s == "CpyO1ToFPS3" ){addWord(0x4477);}
  else if( s == "CpyO1ToFPS4" ){addWord(0x4489);}
  else if( s == "CpyO1ToFPS5" ){addWord(0x4483);}
  else if( s == "CpyO1ToFPS6" ){addWord(0x447D);}
  else if( s == "CpyO1ToFPS7" ){addWord(0x4480);}
  else if( s == "CpyO1ToFPST" ){addWord(0x444A);}
  else if( s == "CpyO2ToFPS1" ){addWord(0x4459);}
  else if( s == "CpyO2ToFPS2" ){addWord(0x4462);}
  else if( s == "CpyO2ToFPS3" ){addWord(0x4474);}
  else if( s == "CpyO2ToFPS4" ){addWord(0x4486);}
  else if( s == "CpyO2ToFPST" ){addWord(0x4444);}
  else if( s == "CpyO3ToFPS1" ){addWord(0x4453);}
  else if( s == "CpyO3ToFPS2" ){addWord(0x4465);}
  else if( s == "CpyO3ToFPST" ){addWord(0x4441);}
  else if( s == "CpyO5ToFPS1" ){addWord(0x4456);}
  else if( s == "CpyO5ToFPS3" ){addWord(0x4471);}
  else if( s == "CpyO6ToFPS2" ){addWord(0x4468);}
  else if( s == "CpyO6ToFPST" ){addWord(0x4447);}
  else if( s == "CpyStack" ){addWord(0x4429);}
  else if( s == "CpyTo1FPS1" ){addWord(0x4432);}
  else if( s == "CpyTo1FPS10" ){addWord(0x43F3);}
  else if( s == "CpyTo1FPS11" ){addWord(0x43D8);}
  else if( s == "CpyTo1FPS2" ){addWord(0x443B);}
  else if( s == "CpyTo1FPS3" ){addWord(0x4408);}
  else if( s == "CpyTo1FPS4" ){addWord(0x440E);}
  else if( s == "CpyTo1FPS5" ){addWord(0x43DE);}
  else if( s == "CpyTo1FPS6" ){addWord(0x43E4);}
  else if( s == "CpyTo1FPS7" ){addWord(0x43EA);}
  else if( s == "CpyTo1FPS8" ){addWord(0x43ED);}
  else if( s == "CpyTo1FPS9" ){addWord(0x43F6);}
  else if( s == "CpyTo1FPST" ){addWord(0x4423);}
  else if( s == "CpyTo2FPS1" ){addWord(0x442F);}
  else if( s == "CpyTo2FPS2" ){addWord(0x4438);}
  else if( s == "CpyTo2FPS3" ){addWord(0x4402);}
  else if( s == "CpyTo2FPS4" ){addWord(0x43F9);}
  else if( s == "CpyTo2FPS5" ){addWord(0x43DB);}
  else if( s == "CpyTo2FPS6" ){addWord(0x43E1);}
  else if( s == "CpyTo2FPS7" ){addWord(0x43E7);}
  else if( s == "CpyTo2FPS8" ){addWord(0x43F0);}
  else if( s == "CpyTo2FPST" ){addWord(0x4420);}
  else if( s == "CpyTo3FPS1" ){addWord(0x442C);}
  else if( s == "CpyTo3FPS2" ){addWord(0x4411);}
  else if( s == "CpyTo3FPST" ){addWord(0x441D);}
  else if( s == "CpyTo4FPST" ){addWord(0x441A);}
  else if( s == "CpyTo5FPST" ){addWord(0x4414);}
  else if( s == "CpyTo6FPS2" ){addWord(0x43FF);}
  else if( s == "CpyTo6FPS3" ){addWord(0x43FC);}
  else if( s == "CpyTo6FPST" ){addWord(0x4417);}
  else if( s == "CpyToFPS1" ){addWord(0x445F);}
  else if( s == "CpyToFPS2" ){addWord(0x446E);}
  else if( s == "CpyToFPS3" ){addWord(0x447A);}
  else if( s == "CpyToFPST" ){addWord(0x444D);}
  else if( s == "CpyToStack" ){addWord(0x4450);}
  else if( s == "Create0Equ" ){addWord(0x432A);}
  else if( s == "CreateAppVar" ){addWord(0x4E6A);}
  else if( s == "CreateCList" ){addWord(0x431B);}
  else if( s == "CreateCplx" ){addWord(0x430C);}
  else if( s == "CreateEqu" ){addWord(0x4330);}
  else if( s == "CreatePair" ){addWord(0x4B0D);}
  else if( s == "CreatePict" ){addWord(0x4333);}
  else if( s == "CreateProg" ){addWord(0x4339);}
  else if( s == "CreateProtProg" ){addWord(0x4E6D);}
  else if( s == "CreateReal" ){addWord(0x430F);}
  else if( s == "CreateRList" ){addWord(0x4315);}
  else if( s == "CreateRMat" ){addWord(0x4321);}
  else if( s == "CreateStrng" ){addWord(0x4327);}
  else if( s == "CreateTempEqu" ){addWord(0x432D);}
  else if( s == "CreateVar" ){addWord(0x4E70);}
  else if( s == "CRecip" ){addWord(0x4E91);}
  else if( s == "CSqRoot" ){addWord(0x4E9D);}
  else if( s == "CSquare" ){addWord(0x4E8B);}
  else if( s == "CSub" ){addWord(0x4E85);}
  else if( s == "CTenX" ){addWord(0x4EA6);}
  else if( s == "CTrunc" ){addWord(0x4EBE);}
  else if( s == "Cube" ){addWord(0x407B);}
  else if( s == "CursorDown" ){addWord(0x4948);}
  else if( s == "CursorLeft" ){addWord(0x493F);}
  else if( s == "CursorOff" ){addWord(0x45BE);}
  else if( s == "CursorOn" ){addWord(0x45C4);}
  else if( s == "CXrootY" ){addWord(0x4EAC);}
  else if( s == "CYtoX" ){addWord(0x4EB2);}
  else if( s == "DarkLine" ){addWord(0x47DD);}
  else if( s == "DarkPnt" ){addWord(0x47F2);}
  else if( s == "DataSize" ){addWord(0x436C);}
  else if( s == "DataSizeA" ){addWord(0x4369);}
  else if( s == "DeallocFPS" ){addWord(0x439F);}
  else if( s == "DeallocFPS1" ){addWord(0x43A2);}
  else if( s == "DecO1Exp" ){addWord(0x4267);}
  else if( s == "DeleteApp" ){addWord(0x4DCB);}
  else if( s == "DelListEl" ){addWord(0x4A2F);}
  else if( s == "DelMem" ){addWord(0x4357);}
  else if( s == "DelRes" ){addWord(0x4A20);}
  else if( s == "DelVar" ){addWord(0x4351);}
  else if( s == "DelVarArc" ){addWord(0x4FC6);}
  else if( s == "DelVarNoArc" ){addWord(0x4FC9);}
  else if( s == "DeselectAllVars" ){addWord(0x4A1D);}
  else if( s == "DisableApd" ){addWord(0x4C84);}
  else if( s == "DisableAppChangeHook" ){addWord(0x502F);}
  else if( s == "DisableCursorHook" ){addWord(0x4F69);}
  else if( s == "DisablecxRedispHook" ){addWord(0x506E);}
  else if( s == "DisableFontHook" ){addWord(0x4FE7);}
  else if( s == "DisableGetCSCHook" ){addWord(0x4F7E);}
  else if( s == "DisableHelpHook" ){addWord(0x504D);}
  else if( s == "DisableHomescreenHook" ){addWord(0x4FAE);}
  else if( s == "DisableMenuHook" ){addWord(0x5086);}
  else if( s == "DisableParserHook" ){addWord(0x5029);}
  else if( s == "DisableRawKeyHook" ){addWord(0x4F6F);}
  else if( s == "DisableSilentLinkHook" ){addWord(0x50D1);}
  else if( s == "DisableTokenHook" ){addWord(0x4F9C);}
  else if( s == "Disp" ){addWord(0x4F45);}
  else if( s == "Disp32" ){addWord(0x51CD);}
  else if( s == "DispAboutScreen" ){addWord(0x51C7);}
  else if( s == "DispAppRestrictions" ){addWord(0x52FF);}
  else if( s == "DispDone" ){addWord(0x45B5);}
  else if( s == "DispEOL" ){addWord(0x45A6);}
  else if( s == "DispEOW" ){addWord(0x4957);}
  else if( s == "DispForward" ){addWord(0x49D5);}
  else if( s == "DispHead" ){addWord(0x495A);}
  else if( s == "DispHL" ){addWord(0x4507);}
  else if( s == "DisplayImage" ){addWord(0x4D9B);}
  else if( s == "DisplayOSProgress" ){addWord(0x80F9);}
  else if( s == "DispMenuTitle" ){addWord(0x5065);}
  else if( s == "DispOP1A" ){addWord(0x4BF7);}
  else if( s == "DispTail" ){addWord(0x495D);}
  else if( s == "Div32By16" ){addWord(0x80B1);}
  else if( s == "DivHLBy10" ){addWord(0x400F);}
  else if( s == "DivHLByA" ){addWord(0x4012);}
  else if( s == "DivHLbyE" ){addWord(0x8048);}
  else if( s == "DivHLbyDE" ){addWord(0x804B);}
  else if( s == "DrawCirc2" ){addWord(0x4C66);}
  else if( s == "DrawCmd" ){addWord(0x48C1);}
  else if( s == "DrawRectBorder" ){addWord(0x4D7D);}
  else if( s == "DrawRectBorderClear" ){addWord(0x4D8C);}
  else if( s == "DispResetComplete" ){addWord(0x5320);}
  else if( s == "DToR" ){addWord(0x4075);}
  else if( s == "DoNothing" ){addWord(0x4CBD);}
  else if( s == "EditProg" ){addWord(0x4A32);}
  else if( s == "EnableApd" ){addWord(0x4C87);}
  else if( s == "EnableAppChangeHook" ){addWord(0x502C);}
  else if( s == "EnableCursorHook" ){addWord(0x4F60);}
  else if( s == "EnablecxRedispHook" ){addWord(0x506B);}
  else if( s == "EnableFontHook" ){addWord(0x4FE4);}
  else if( s == "EnableGetCSCHook" ){addWord(0x4F7B);}
  else if( s == "EnableHelpHook" ){addWord(0x504A);}
  else if( s == "EnableHomescreenHook" ){addWord(0x4FAB);}
  else if( s == "EnableMenuHook" ){addWord(0x5083);}
  else if( s == "EnableParserHook" ){addWord(0x5026);}
  else if( s == "EnableRawKeyHook" ){addWord(0x4F66);}
  else if( s == "EnableSilentLinkHook" ){addWord(0x50CE);}
  else if( s == "EnableTokenHook" ){addWord(0x4F99);}
  else if( s == "EnoughMem" ){addWord(0x42FD);}
  else if( s == "EOP1NotReal" ){addWord(0x4279);}
  else if( s == "Equ_or_NewEqu" ){addWord(0x42C4);}
  else if( s == "EraseEOL" ){addWord(0x4552);}
  else if( s == "EraseEOW" ){addWord(0x4555);}
  else if( s == "EraseFlash" ){addWord(0x8024);}
  else if( s == "EraseFlashPage" ){addWord(0x8084);}
  else if( s == "EraseRectBorder" ){addWord(0x4D86);}
  else if( s == "ErrArgument" ){addWord(0x44AD);}
  else if( s == "ErrBadGuess" ){addWord(0x44CB);}
  else if( s == "ErrBreak" ){addWord(0x44BF);}
  else if( s == "ErrCustom1" ){addWord(0x4D41);}
  else if( s == "ErrCustom2" ){addWord(0x4D44);}
  else if( s == "ErrD_OP1_0" ){addWord(0x42D3);}
  else if( s == "ErrD_OP1_LE_0" ){addWord(0x42D0);}
  else if( s == "ErrD_OP1Not_R" ){addWord(0x42CA);}
  else if( s == "ErrD_OP1NotPos" ){addWord(0x42C7);}
  else if( s == "ErrD_OP1NotPosInt" ){addWord(0x42CD);}
  else if( s == "ErrDataType" ){addWord(0x44AA);}
  else if( s == "ErrDimension" ){addWord(0x44B3);}
  else if( s == "ErrDimMismatch" ){addWord(0x44B0);}
  else if( s == "ErrDivBy0" ){addWord(0x4498);}
  else if( s == "ErrDomain" ){addWord(0x449E);}
  else if( s == "ErrIncrement" ){addWord(0x44A1);}
  else if( s == "ErrInvalid" ){addWord(0x44BC);}
  else if( s == "ErrIterations" ){addWord(0x44C8);}
  else if( s == "ErrLinkXmit" ){addWord(0x44D4);}
  else if( s == "ErrMemory" ){addWord(0x44B9);}
  else if( s == "ErrNon_Real" ){addWord(0x44A4);}
  else if( s == "ErrNonReal" ){addWord(0x4A8C);}
  else if( s == "ErrNotEnoughMem" ){addWord(0x448C);}
  else if( s == "ErrOverflow" ){addWord(0x4495);}
  else if( s == "ErrSignChange" ){addWord(0x44C5);}
  else if( s == "ErrSingularMat" ){addWord(0x449B);}
  else if( s == "ErrStat" ){addWord(0x44C2);}
  else if( s == "ErrStatPlot" ){addWord(0x44D1);}
  else if( s == "ErrSyntax" ){addWord(0x44A7);}
  else if( s == "ErrTolTooSmall" ){addWord(0x44CE);}
  else if( s == "ErrUndefined" ){addWord(0x44B6);}
  else if( s == "EToX" ){addWord(0x40B4);}
  else if( s == "Exch9" ){addWord(0x43D5);}
  else if( s == "ExecuteApp" ){addWord(0x4C51);}
  else if( s == "ExecuteNewPrgm" ){addWord(0x4C3C);}
  else if( s == "ExecutePrgm" ){addWord(0x4E7C);}
  else if( s == "ExLp" ){addWord(0x4222);}
  else if( s == "ExpToHex" ){addWord(0x424F);}
  else if( s == "Factorial" ){addWord(0x4B85);}
  else if( s == "FillBasePageTable" ){addWord(0x5011);}
  else if( s == "FillBasePageTab" ){addWord(0x5011);}
  else if( s == "FillRect" ){addWord(0x4D62);}
  else if( s == "FillRectPattern" ){addWord(0x4D89);}
  else if( s == "Find_Parse_Formula" ){addWord(0x4AF2);}
  else if( s == "FindAlphaDn" ){addWord(0x4A47);}
  else if( s == "FindAlphaUp" ){addWord(0x4A44);}
  else if( s == "FindApp" ){addWord(0x4C4E);}
  else if( s == "FindAppDn" ){addWord(0x4C4B);}
  else if( s == "FindAppNumPages" ){addWord(0x509B);}
  else if( s == "FindAppUp" ){addWord(0x4C48);}
  else if( s == "FindOSHeaderSubField" ){addWord(0x8075);}
  else if( s == "FindSubField" ){addWord(0x805D);}
  else if( s == "FindSwapSector EQU 5095h" ){addWord(0x5095);}
  else if( s == "FindSym" ){addWord(0x42F4);}
  else if( s == "FiveExec" ){addWord(0x467E);}
  else if( s == "FixTempCnt" ){addWord(0x4A3B);}
  else if( s == "FlashToRam" ){addWord(0x5017);}
  else if( s == "FlashToRam2" ){addWord(0x8054);}
  else if( s == "FlashWriteDisable" ){addWord(0x4F3C);}
  else if( s == "ForceCmd" ){addWord(0x4C90);}
  else if( s == "ForceFullScreen" ){addWord(0x508F);}
  else if( s == "FormBase" ){addWord(0x50AA);}
  else if( s == "FormDCplx" ){addWord(0x4996);}
  else if( s == "FormEReal" ){addWord(0x4990);}
  else if( s == "FormReal" ){addWord(0x4999);}
  else if( s == "FourExec" ){addWord(0x467B);}
  else if( s == "FPAdd" ){addWord(0x4072);}
  else if( s == "FPDiv" ){addWord(0x4099);}
  else if( s == "FPMult" ){addWord(0x4084);}
  else if( s == "FPRecip" ){addWord(0x4096);}
  else if( s == "FPSquare" ){addWord(0x4081);}
  else if( s == "FPSub" ){addWord(0x406F);}
  else if( s == "Frac" ){addWord(0x4093);}
  else if( s == "Get_Tok_Strng" ){addWord(0x4594);}
  else if( s == "Get3Bytes" ){addWord(0x4EF7);}
  else if( s == "Get4Bytes" ){addWord(0x4EF4);}
  else if( s == "Get4BytesCursor" ){addWord(0x4F18);}
  else if( s == "Get4BytesNC" ){addWord(0x4F1B);}
  else if( s == "GetBaseVer" ){addWord(0x4C6F);}
  else if( s == "GetBootVer" ){addWord(0x80B7);}
  else if( s == "GetBytePaged" ){addWord(0x4F54);}
  else if( s == "GetCalcSerial" ){addWord(0x807E);}
  else if( s == "GetCertificateStart" ){addWord(0x8057);}
  else if( s == "GetCSC" ){addWord(0x4018);}
  else if( s == "GetDataPacket" ){addWord(0x4EEE);}
  else if( s == "GetFieldSize" ){addWord(0x805A);}
  else if( s == "GetKey" ){addWord(0x4972);}
  else if( s == "GetKeyRetOff" ){addWord(0x500B);}
  else if( s == "GetLToOP1" ){addWord(0x4636);}
  else if( s == "GetMToOP1" ){addWord(0x4615);}
  else if( s == "GetPrevTok" ){addWord(0x496F);}
  else if( s == "GetRestrictionOptions" ){addWord(0x531D);}
  else if( s == "GetSmallPacket" ){addWord(0x4EEB);}
  else if( s == "GetSysInfo " ){addWord(0x50DD);}
  else if( s == "GetTime" ){addWord(0x515B);}
  else if( s == "GetTokLen" ){addWord(0x4591);}
  else if( s == "GoToErr" ){addWord(0x4CD8);}
  else if( s == "GoToLastRow" ){addWord(0x5233);}
  else if( s == "GrBufClr" ){addWord(0x4BD0);}
  else if( s == "GrBufCpy" ){addWord(0x486A);}
  else if( s == "GrBufCpyCustom" ){addWord(0x52C9);}
  else if( s == "GrphCirc" ){addWord(0x47D7);}
  else if( s == "HLMinus5" ){addWord(0x509E);}
  else if( s == "HLTimes9" ){addWord(0x40F9);}
  else if( s == "HomeUp" ){addWord(0x4558);}
  else if( s == "HorizCmd" ){addWord(0x48A6);}
  else if( s == "HorizontalLine" ){addWord(0x4E67);}
  else if( s == "HTimesL" ){addWord(0x4276);}
  else if( s == "IsA2ByteTok" ){addWord(0x42A3);}
  else if( s == "IBounds" ){addWord(0x4C60);}
  else if( s == "IBoundsFull" ){addWord(0x4D98);}
  else if( s == "ILine" ){addWord(0x47E0);}
  else if( s == "IncFetch" ){addWord(0x4B73);}
  else if( s == "IncLstSize" ){addWord(0x4A29);}
  else if( s == "InsDisp" ){addWord(0x494E);}
  else if( s == "InsertList" ){addWord(0x4A2C);}
  else if( s == "InsertMem" ){addWord(0x42F7);}
  else if( s == "Int" ){addWord(0x40A5);}
  else if( s == "Intgr" ){addWord(0x405D);}
  else if( s == "InvCmd" ){addWord(0x48C7);}
  else if( s == "InvertRect" ){addWord(0x4D5F);}
  else if( s == "InvOP1S" ){addWord(0x408D);}
  else if( s == "InvOP1SC" ){addWord(0x408A);}
  else if( s == "InvOP2S" ){addWord(0x4090);}
  else if( s == "InvSub" ){addWord(0x4063);}
  else if( s == "IOffset" ){addWord(0x4C63);}
  else if( s == "IPoint" ){addWord(0x47E3);}
  else if( s == "IsAtBtm" ){addWord(0x4933);}
  else if( s == "IsAtTop" ){addWord(0x4930);}
  else if( s == "IsEditEmpty" ){addWord(0x492D);}
  else if( s == "JError" ){addWord(0x44D7);}
  else if( s == "JErrorNo" ){addWord(0x4000);}
  else if( s == "JForceCmd" ){addWord(0x402A);}
  else if( s == "JForceCmdNoChar" ){addWord(0x4027);}
  else if( s == "JForceGraphKey" ){addWord(0x5005);}
  else if( s == "JForceGraphNoKey" ){addWord(0x5002);}
  else if( s == "JForceGroup" ){addWord(0x5131);}
  else if( s == "kdbScan" ){addWord(0x4015);}
  else if( s == "KeyToString" ){addWord(0x45CA);}
  else if( s == "KillUSB" ){addWord(0x810E);}
  else if( s == "LCD_BUSY" ){addWord(0x4051);}
  else if( s == "LCD_DRIVERON" ){addWord(0x4978);}
  else if( s == "LdHLind" ){addWord(0x4009);}
  else if( s == "LineCmd" ){addWord(0x48AC);}
  else if( s == "LnX" ){addWord(0x40AB);}
  else if( s == "Load_SFont" ){addWord(0x4783);}
  else if( s == "LoadA5" ){addWord(0x5242);}
  else if( s == "LoadAIndPaged" ){addWord(0x8051);}
  else if( s == "LoadCIndPaged" ){addWord(0x501D);}
  else if( s == "LoadDEIndPaged" ){addWord(0x501A);}
  else if( s == "LoadPattern" ){addWord(0x4CB1);}
  else if( s == "LogX" ){addWord(0x40AE);}
  else if( s == "LowBatteryBoot" ){addWord(0x80D2);}
  else if( s == "MarkOSValid" ){addWord(0x8099);}
  else if( s == "Max" ){addWord(0x4057);}
  else if( s == "MemChk" ){addWord(0x42E5);}
  else if( s == "MemClear" ){addWord(0x4C30);}
  else if( s == "MemSet" ){addWord(0x4C33);}
  else if( s == "Min" ){addWord(0x4054);}
  else if( s == "Minus1" ){addWord(0x406C);}
  else if( s == "Mon" ){addWord(0x401E);}
  else if( s == "MonForceKey" ){addWord(0x4021);}
  else if( s == "MonReset" ){addWord(0x4C54);}
  else if( s == "Mov10B" ){addWord(0x415C);}
  else if( s == "Mov18B" ){addWord(0x47DA);}
  else if( s == "Mov7B" ){addWord(0x4168);}
  else if( s == "Mov8B" ){addWord(0x4165);}
  else if( s == "Mov9B" ){addWord(0x415F);}
  else if( s == "Mov9OP1OP2" ){addWord(0x417D);}
  else if( s == "Mov9OP2Cp" ){addWord(0x410B);}
  else if( s == "Mov9ToOP1" )
    {
      subtractByte();
      addByte( 0xE7 );
      //addWord(0x417A);
    }
  else if( s == "Mov9ToOP2" ){addWord(0x4180);}
  else if( s == "MovFrOP1" ){addWord(0x4183);}
  else if( s == "MultAbyDE" ){addWord(0x8045);}
  else if( s == "MultAbyE" ){addWord(0x8042);}
  else if( s == "NewContext" ){addWord(0x4030);}
  else if( s == "NewContext0" ){addWord(0x4033);}
  else if( s == "NewLine" ){addWord(0x452E);}
  else if( s == "NZIf83Plus" ){addWord(0x50E0);}
  else if( s == "OffPageJump" ){addWord(0x44F2);}
  else if( s == "OneVar" ){addWord(0x4BA3);}
  else if( s == "OP1ExOP2" ){addWord(0x421F);}
  else if( s == "OP1ExOP3" ){addWord(0x4219);}
  else if( s == "OP1ExOP4" ){addWord(0x421C);}
  else if( s == "OP1ExOP5" ){addWord(0x420D);}
  else if( s == "OP1ExOP6" ){addWord(0x4210);}
  else if( s == "OP1ExpToDec" ){addWord(0x4252);}
  else if( s == "OP1Set0" ){addWord(0x41BF);}
  else if( s == "OP1Set1" ){addWord(0x419B);}
  else if( s == "OP1Set2" ){addWord(0x41A7);}
  else if( s == "OP1Set3" ){addWord(0x41A1);}
  else if( s == "OP1Set4" ){addWord(0x419E);}
  else if( s == "OP1ToOP2" ){addWord(0x412F);}
  else if( s == "OP1ToOP3" ){addWord(0x4123);}
  else if( s == "OP1ToOP4" ){addWord(0x4117);}
  else if( s == "OP1ToOP5" ){addWord(0x4153);}
  else if( s == "OP1ToOP6" ){addWord(0x4150);}
  else if( s == "OP2ExOP4" ){addWord(0x4213);}
  else if( s == "OP2ExOP5" ){addWord(0x4216);}
  else if( s == "OP2ExOP6" ){addWord(0x4207);}
  else if( s == "OP2Set0" ){addWord(0x41BC);}
  else if( s == "OP2Set1" ){addWord(0x41AD);}
  else if( s == "OP2Set2" ){addWord(0x41AA);}
  else if( s == "OP2Set3" ){addWord(0x4198);}
  else if( s == "OP2Set4" ){addWord(0x4195);}
  else if( s == "OP2Set5" ){addWord(0x418F);}
  else if( s == "OP2Set60" ){addWord(0x4AB0);}
  else if( s == "OP2Set8" ){addWord(0x418C);}
  else if( s == "OP2SetA" ){addWord(0x4192);}
  else if( s == "OP2ToOP1" ){addWord(0x4156);}
  else if( s == "OP2ToOP3" ){addWord(0x416E);}
  else if( s == "OP2ToOP4" ){addWord(0x411A);}
  else if( s == "OP2ToOP5" ){addWord(0x414A);}
  else if( s == "OP2ToOP6" ){addWord(0x414D);}
  else if( s == "OP3Set0" ){addWord(0x41B9);}
  else if( s == "OP3Set1" ){addWord(0x4189);}
  else if( s == "OP3Set2" ){addWord(0x41A4);}
  else if( s == "OP3ToOP1" ){addWord(0x413E);}
  else if( s == "OP3ToOP2" ){addWord(0x4120);}
  else if( s == "OP3ToOP4" ){addWord(0x4114);}
  else if( s == "OP3ToOP5" ){addWord(0x4147);}
  else if( s == "OP4Set0" ){addWord(0x41B6);}
  else if( s == "OP4Set1" ){addWord(0x4186);}
  else if( s == "OP4ToOP1" ){addWord(0x4138);}
  else if( s == "OP4ToOP2" ){addWord(0x411D);}
  else if( s == "OP4ToOP3" ){addWord(0x4171);}
  else if( s == "OP4ToOP5" ){addWord(0x4144);}
  else if( s == "OP4ToOP6" ){addWord(0x4177);}
  else if( s == "OP5ExOP6" ){addWord(0x420A);}
  else if( s == "OP5Set0" ){addWord(0x41B3);}
  else if( s == "OP5ToOP1" ){addWord(0x413B);}
  else if( s == "OP5ToOP2" ){addWord(0x4126);}
  else if( s == "OP5ToOP3" ){addWord(0x4174);}
  else if( s == "OP5ToOP4" ){addWord(0x412C);}
  else if( s == "OP5ToOP6" ){addWord(0x4129);}
  else if( s == "OP6ToOP1" ){addWord(0x4135);}
  else if( s == "OP6ToOP2" ){addWord(0x4132);}
  else if( s == "OP6ToOP5" ){addWord(0x4141);}
  else if( s == "OutputExpr" ){addWord(0x4BB2);}
  else if( s == "PagedGet" ){addWord(0x5023);}
  else if( s == "ParseInp" ){addWord(0x4A9B);}
  else if( s == "PDspGrph" ){addWord(0x48A3);}
  else if( s == "PixelTest" ){addWord(0x48B5);}
  else if( s == "Plus1" ){addWord(0x4069);}
  else if( s == "PointCmd" ){addWord(0x48B2);}
  else if( s == "PointOn" ){addWord(0x4C39);}
  else if( s == "PopMCplxO1" ){addWord(0x436F);}
  else if( s == "PopOP1" ){addWord(0x437E);}
  else if( s == "PopOP3" ){addWord(0x437B);}
  else if( s == "PopOP5" ){addWord(0x4378);}
  else if( s == "PopReal" ){addWord(0x4393);}
  else if( s == "PopRealO1" ){addWord(0x4390);}
  else if( s == "PopRealO2" ){addWord(0x438D);}
  else if( s == "PopRealO3" ){addWord(0x438A);}
  else if( s == "PopRealO4" ){addWord(0x4387);}
  else if( s == "PopRealO5" ){addWord(0x4384);}
  else if( s == "PopRealO6" ){addWord(0x4381);}
  else if( s == "PosNo0Int" ){addWord(0x422E);}
  else if( s == "PowerOff" ){addWord(0x5008);}
  else if( s == "ProcessBufKeys" ){addWord(0x4756);}
  else if( s == "PToR" ){addWord(0x40F3);}
  else if( s == "PullDownChk" ){addWord(0x45CD);}
  else if( s == "PushMCplxO1" ){addWord(0x43CF);}
  else if( s == "PushMCplxO3" ){addWord(0x43C6);}
  else if( s == "PushOP1" ){addWord(0x43C9);}
  else if( s == "PushOP3" ){addWord(0x43C3);}
  else if( s == "PushOP5" ){addWord(0x43C0);}
  else if( s == "PushReal" ){addWord(0x43BD);}
  else if( s == "PushRealO1" ){addWord(0x43BA);}
  else if( s == "PushRealO2" ){addWord(0x43B7);}
  else if( s == "PushRealO3" ){addWord(0x43B4);}
  else if( s == "PushRealO4" ){addWord(0x43B1);}
  else if( s == "PushRealO5" ){addWord(0x43AE);}
  else if( s == "PushRealO6" ){addWord(0x43AB);}
  else if( s == "PutAway" ){addWord(0x403c);}
  else if( s == "PutBuf" ){addWord(0x4516);}
  else if( s == "PutC" ){addWord(0x4504);}
  else if( s == "PutMap" ){addWord(0x4501);}
  else if( s == "PutPS" ){addWord(0x4510);}
  else if( s == "PutPSB" ){addWord(0x450d);}
  else if( s == "PutTokString" ){addWord(0x4960);}
  else if( s == "PutToL" ){addWord(0x4645);}
  else if( s == "PutToMat" ){addWord(0x461E);}
  else if( s == "RandInit" ){addWord(0x4B7F);}
  else if( s == "Random" ){addWord(0x4B79);}
  else if( s == "Rcl_StatVar" ){addWord(0x42DC);}
  else if( s == "RclAns" ){addWord(0x4AD7);}
  else if( s == "RclGDB2" ){addWord(0x47D1);}
  else if( s == "RclN" ){addWord(0x4ADD);}
  else if( s == "RclSysTok" ){addWord(0x4AE6);}
  else if( s == "RclVarSym" ){addWord(0x4AE3);}
  else if( s == "RclX" ){addWord(0x4AE0);}
  else if( s == "RclY" ){addWord(0x4ADA);}
  else if( s == "Rec1stByte" ){addWord(0x4EFA);}
  else if( s == "Rec1stByteNC" ){addWord(0x4EFD);}
  else if( s == "RecAByteIO" ){addWord(0x4F03);}
  else if( s == "ReceiveAck" ){addWord(0x4ED9);}
  else if( s == "ReceiveOS" ){addWord(0x8072);}
  else if( s == "ReceiveVar" ){addWord(0x4F06);}
  else if( s == "ReceiveVarData" ){addWord(0x4F0C);}
  else if( s == "ReceiveVarData2" ){addWord(0x4F09);}
  else if( s == "RedimMat" ){addWord(0x4A26);}
  else if( s == "Regraph" ){addWord(0x488E);}
  else if( s == "RejectCommand" ){addWord(0x4F4B);}
  else if( s == "ReleaseBuffer" ){addWord(0x4771);}
  else if( s == "ReloadAppEntryVecs" ){addWord(0x4C36);}
  else if( s == "RemoveAppRestrictions" ){addWord(0x52F9);}
  else if( s == "ResetCalc" ){addWord(0x80FC);}
  else if( s == "ResetDefaults" ){addWord(0x4128);}
  else if( s == "ResetRam" ){addWord(0x404E);}
  else if( s == "RestoreDisp" ){addWord(0x4870);}
  else if( s == "RName" ){addWord(0x427F);}
  else if( s == "RndGuard" ){addWord(0x409F);}
  else if( s == "RnFx" ){addWord(0x40A2);}
  else if( s == "Round" ){addWord(0x40A8);}
  else if( s == "RToD" ){addWord(0x4078);}
  else if( s == "RToP" ){addWord(0x40F6);}
  else if( s == "RunIndicOff" ){addWord(0x4570);}
  else if( s == "RunIndicOn" ){addWord(0x456D);}
  else if( s == "SaveDisp" ){addWord(0x4C7B);}
  else if( s == "Send4Bytes" ){addWord(0x4EE2);}
  else if( s == "Send4BytePacket" ){addWord(0x4EDC);}
  else if( s == "SendAByte" ){addWord(0x4EE5);}
  else if( s == "SendCByte" ){addWord(0x4EE8);}
  else if( s == "SendAck" ){addWord(0x4EF1);}
  else if( s == "SendApplication" ){addWord(0x4F30);}
  else if( s == "SendCmd" ){addWord(0x4F3F);}
  else if( s == "SendDataByte" ){addWord(0x4EDF);}
  else if( s == "SendDirectoryContents" ){addWord(0x4F21);}
  else if( s == "SetEmptyEditPtr" ){addWord(0x4969);}
  else if( s == "SendGetKeyPress" ){addWord(0x4F48);}
  else if( s == "SendHeaderPacket" ){addWord(0x4EC7);}
  else if( s == "SendOs" ){addWord(0x4F39);}
  else if( s == "SendOsHeader" ){addWord(0x4F33);}
  else if( s == "SendOsPage" ){addWord(0x4F36);}
  else if( s == "SendPacket" ){addWord(0x4ED6);}
  else if( s == "SendRAMCmd" ){addWord(0x4ED3);}
  else if( s == "SendRAMVarData" ){addWord(0x4ED0);}
  else if( s == "SendReadyPacket" ){addWord(0x4F24);}
  else if( s == "SendScreenContents" ){addWord(0x4ECD);}
  else if( s == "SendVarCmd" ){addWord(0x4A14);}
  else if( s == "SendVariable" ){addWord(0x4F15);}
  else if( s == "SetAllPlots" ){addWord(0x4FCC);}
  else if( s == "SetAppRestrictions" ){addWord(0x52F6);}
  else if( s == "SetExSpeed " ){addWord(0x50BF);}
  else if( s == "SetFuncM" ){addWord(0x4840);}
  else if( s == "SetNorm_Vals" ){addWord(0x49FC);}
  else if( s == "SetNumWindow" ){addWord(0x452B);}
  else if( s == "SetParM" ){addWord(0x4849);}
  else if( s == "SetPolM" ){addWord(0x4846);}
  else if( s == "SetSeqM" ){addWord(0x4843);}
  else if( s == "SetTblGraphDraw" ){addWord(0x4C00);}
  else if( s == "SetupAppPubKey" ){addWord(0x809F);}
  else if( s == "SetupEditCmd" ){addWord(0x4963);}
  else if( s == "SetupHome" ){addWord(0x5302);}
  else if( s == "SetupOSPubKey" ){addWord(0x80FF);}
  else if( s == "SetupPagedPtr" ){addWord(0x5020);}
  else if( s == "SetXXOP1" ){addWord(0x478C);}
  else if( s == "SetXXOP2" ){addWord(0x478F);}
  else if( s == "SetXXXXOP2" ){addWord(0x4792);}
  else if( s == "SFont_Len" ){addWord(0x4786);}
  else if( s == "Sin" ){addWord(0x40BD);}
  else if( s == "SinCosRad" ){addWord(0x40BA);}
  else if( s == "SinH" ){addWord(0x40CF);}
  else if( s == "SinHCosH" ){addWord(0x40C6);}
  else if( s == "SqRoot" ){addWord(0x409C);}
  else if( s == "SquishPrgm" ){addWord(0x4E79);}
  else if( s == "SrchVLstDn" ){addWord(0x4F12);}
  else if( s == "SrchVLstUp" ){addWord(0x4F0F);}
  else if( s == "SStringLength" ){addWord(0x4CB4);}
  else if( s == "StMatEl" ){addWord(0x4AE9);}
  else if( s == "StoAns" ){addWord(0x4ABF);}
  else if( s == "StoGDB2" ){addWord(0x47CE);}
  else if( s == "StoN" ){addWord(0x4ACB);}
  else if( s == "StoOther" ){addWord(0x4AD4);}
  else if( s == "StoR" ){addWord(0x4AC5);}
  else if( s == "StoRand" ){addWord(0x4B7C);}
  else if( s == "StoSysTok" ){addWord(0x4ABC);}
  else if( s == "StoT" ){addWord(0x4ACE);}
  else if( s == "StoTheta" ){addWord(0x4AC2);}
  else if( s == "StoX" ){addWord(0x4AD1);}
  else if( s == "StoY" ){addWord(0x4AC8);}
  else if( s == "StrCopy" ){addWord(0x44E3);}
  else if( s == "StrLength" ){addWord(0x4C3F);}
  else if( s == "Tan" ){addWord(0x40C3);}
  else if( s == "TanH" ){addWord(0x40C9);}
  else if( s == "TanLnF" ){addWord(0x48BB);}
  else if( s == "TenX" ){addWord(0x40B7);}
  else if( s == "ThetaName" ){addWord(0x427C);}
  else if( s == "ThreeExec" ){addWord(0x4675);}
  else if( s == "Times2" ){addWord(0x4066);}
  else if( s == "TimesPt5" ){addWord(0x407E);}
  else if( s == "TName" ){addWord(0x428E);}
  else if( s == "ToFrac" ){addWord(0x4657);}
  else if( s == "TokToKey" ){addWord(0x4A0B);}
  else if( s == "Trunc" ){addWord(0x4060);}
  else if( s == "TwoVarSet" ){addWord(0x50D4);}
  else if( s == "UCLineS" ){addWord(0x4795);}
  else if( s == "UnarchiveVar" ){addWord(0x4FDE);}
  else if( s == "UnLineCmd" ){addWord(0x48AF);}
  else if( s == "UnOPExec" ){addWord(0x4672);}
  else if( s == "VertCmd" ){addWord(0x48A9);}
  else if( s == "VerticalLine" ){addWord(0x4D95);}
  else if( s == "VPutMap" ){addWord(0x455E);}
  else if( s == "VPutS" ){addWord(0x4561);}
  else if( s == "VPutSN" ){addWord(0x4564);}
  else if( s == "VtoWHLDE" ){addWord(0x47FB);}
  else if( s == "WaitEnterKey" ){addWord(0x51A0);}
  else if( s == "wDispEOL" ){addWord(0x4525);}
  else if( s == "wPutS" ){addWord(0x451F);}
  else if( s == "wPutSEOL" ){addWord(0x4522);}
  else if( s == "WriteAByte" ){addWord(0x8021);}
  else if( s == "WriteAByteSafe" ){addWord(0x80C6);}
  else if( s == "WriteFlash" ){addWord(0x80C9);}
  else if( s == "WriteFlashUnsafe" ){addWord(0x8087);}
  else if( s == "WriteToFlash" ){addWord(0x50CB);}
  else if( s == "XftoI" ){addWord(0x4804);}
  else if( s == "Xitof" ){addWord(0x47FE);}
  else if( s == "XName" ){addWord(0x4288);}
  else if( s == "XorA" ){addWord(0x808D);}
  else if( s == "XRootY" ){addWord(0x479E);}
  else if( s == "YftoI" ){addWord(0x4801);}
  else if( s == "YName" ){addWord(0x428B);}
  else if( s == "YToX" ){addWord(0x47A1);}
  else if( s == "Zero16D" ){addWord(0x41B0);}
  else if( s == "ZeroOP" ){addWord(0x41CE);}
  else if( s == "ZeroOP1" ){addWord(0x41C5);}
  else if( s == "ZeroOP2" ){addWord(0x41C8);}
  else if( s == "ZeroOP3" ){addWord(0x41CB);}
  else if( s == "ZmDecml" ){addWord(0x484F);}
  else if( s == "ZmFit" ){addWord(0x485B);}
  else if( s == "ZmInt" ){addWord(0x484C);}
  else if( s == "ZmPrev" ){addWord(0x4852);}
  else if( s == "ZmSquare" ){addWord(0x485E);}
  else if( s == "ZmStats" ){addWord(0x47A4);}
  else if( s == "ZmTrig" ){addWord(0x4861);}
  else if( s == "ZmUsr" ){addWord(0x4855);}
  else if( s == "ZooDefault" ){addWord(0x4867);}
  else
    {
      cerr << endl << "*** unknown system call [" << s << "] - compilation failed ***" << endl;
      cerr << "    line number: " << dec << line_number << endl;
      cerr << "    program index: 0x" << hex << uppercase << program_index << " (" << dec << program_index << " decimal)" << endl << endl;
      compilation_failed=1;
      error_count++;
    }
#ifdef DEBUG
  cerr << "Added: 0x" << hex << (int)byte_vector[program_index-3] << " 0x" <<  (int)byte_vector[program_index-2] << " 0x" << (int)byte_vector[program_index-1] << endl;
#endif
    
}

void pushall()
{
  a("push af");
  a("push bc");
  a("push de");
  a("push hl");
}
void popall()
{
  a("pop hl");
  a("pop de");
  a("pop bc");
  a("pop af");

}
void a( string s )
{
  if( s == "ret" ){  addByte( 0xC9 );}
  else if( s == "nop" ){  addByte( 0x00 );}
  else if( s == "ld bc, **" ){  addByte(0x01);}
  else if( s == "ld (bc), a" ){  addByte(0x02);}
  else if( s == "inc bc" ){  addByte(0x03);}
  else if( s == "inc b" ){  addByte(0x04);}
  else if( s == "dec b" ){  addByte(0x05);}
  else if( s == "ld b, *" ){  addByte(0x06);}
  else if( s == "rlca" ){  addByte(0x07);}
  else if( s == "ex af, af'" ){  addByte(0x08);}
  else if( s == "add hl, bc" ){  addByte(0x09);}
  else if( s == "ld a, (bc)" ){  addByte(0x0A);}
  else if( s == "dec bc" ){  addByte(0x0B);}
  else if( s == "inc c" ){  addByte(0x0C);}
  else if( s == "dec c" ){  addByte(0x0D);}
  else if( s == "ld c, *" ){  addByte(0x0E);}
  else if( s == "rrca" ){  addByte(0x0F);}
  else if( s == "djnz *" ){  addByte(0x10);}
  else if( s == "ld de, **" ){  addByte(0x11);}
  else if( s == "ld (de), a" ){  addByte(0x12);}
  else if( s == "inc de" ){  addByte(0x13);}
  else if( s == "inc d" ){  addByte(0x14);}
  else if( s == "dec d" ){  addByte(0x15);}
  else if( s == "ld d, *" ){  addByte(0x16);}
  else if( s == "rla" ){  addByte(0x17);}
  else if( s == "jr *" ){  addByte(0x18);}
  else if( s == "add hl, de" ){  addByte(0x19);}
  else if( s == "ld a, (de)" ){  addByte(0x1A);}
  else if( s == "dec de" ){  addByte(0x1B);}
  else if( s == "inc e" ){  addByte(0x1C);}
  else if( s == "dec e" ){  addByte(0x1D);}
  else if( s == "ld e, *" ){  addByte(0x1E);}
  else if( s == "rra" ){  addByte(0x1F);}
  else if( s == "jr nz, *" ){  addByte(0x20);}
  else if( s == "ld hl, **" ){  addByte(0x21);}
  else if( s == "ld (**), hl" ){  addByte(0x22);}
  else if( s == "inc hl" ){  addByte(0x23);}
  else if( s == "inc h" ){  addByte(0x24);}
  else if( s == "dec h" ){  addByte(0x25);}
  else if( s == "ld h, *" ){  addByte(0x26);}
  else if( s == "daa" ){  addByte(0x27);}
  else if( s == "jr z, *" ){  addByte(0x28);}
  else if( s == "add hl, hl" ){  addByte(0x29);}
  else if( s == "ld hl, (**)" ){  addByte(0x2A);}
  else if( s == "dec hl" ){  addByte(0x2B);}
  else if( s == "inc l" ){  addByte(0x2C);}
  else if( s == "dec l" ){  addByte(0x2D);}
  else if( s == "ld l, *" ){  addByte(0x2E);}
  else if( s == "cpl" ){  addByte(0x2F);}
  else if( s == "jr nc, *" ){  addByte(0x30);}
  else if( s == "ld sp, **" ){  addByte(0x31);}
  else if( s == "ld (**), a" ){  addByte(0x32);}
  else if( s == "inc sp" ){  addByte(0x33);}
  else if( s == "inc (hl)" ){  addByte(0x34);}
  else if( s == "dec (hl)" ){  addByte(0x35);}
  else if( s == "ld (hl), *" ){  addByte(0x36);}
  else if( s == "scf" ){  addByte(0x37);}
  else if( s == "jr c, *" ){  addByte(0x38);}
  else if( s == "add hl, sp" ){  addByte(0x39);}
  else if( s == "ld a, (**)" ){  addByte(0x3A);}
  else if( s == "dec sp" ){  addByte(0x3B);}
  else if( s == "inc a" ){  addByte(0x3C);}
  else if( s == "dec a" ){  addByte(0x3D);}
  else if( s == "ld a, *" ){  addByte(0x3E);}
  else if( s == "ccf" ){  addByte(0x3F);}
  else if( s == "ld b, b" ){  addByte(0x40);}
  else if( s == "ld b, c" ){  addByte(0x41);}
  else if( s == "ld b, d" ){  addByte(0x42);}
  else if( s == "ld b, e" ){  addByte(0x43);}
  else if( s == "ld b, h" ){  addByte(0x44);}
  else if( s == "ld b, l" ){  addByte(0x45);}
  else if( s == "ld b, (hl)" ){  addByte(0x46);}
  else if( s == "ld b, a" ){  addByte(0x47);}
  else if( s == "ld c, b" ){  addByte(0x48);}
  else if( s == "ld c, c" ){  addByte(0x49);}
  else if( s == "ld c, d" ){  addByte(0x4A);}
  else if( s == "ld c, e" ){  addByte(0x4B);}
  else if( s == "ld c, h" ){  addByte(0x4C);}
  else if( s == "ld c, l" ){  addByte(0x4D);}
  else if( s == "ld c, (hl)" ){  addByte(0x4E);}
  else if( s == "ld c, a" ){  addByte(0x4F);}
  else if( s == "ld d, b" ){  addByte(0x50);}
  else if( s == "ld d, c" ){  addByte(0x51);}
  else if( s == "ld d, d" ){  addByte(0x52);}
  else if( s == "ld d, e" ){  addByte(0x53);}
  else if( s == "ld d, h" ){  addByte(0x54);}
  else if( s == "ld d, l" ){  addByte(0x55);}
  else if( s == "ld d, (hl)" ){  addByte(0x56);}
  else if( s == "ld d, a" ){  addByte(0x57);}
  else if( s == "ld e, b" ){  addByte(0x58);}
  else if( s == "ld e, c" ){  addByte(0x59);}
  else if( s == "ld e, d" ){  addByte(0x5A);}
  else if( s == "ld e, e" ){  addByte(0x5B);}
  else if( s == "ld e, h" ){  addByte(0x5C);}
  else if( s == "ld e, l" ){  addByte(0x5D);}
  else if( s == "ld e, (hl)" ){  addByte(0x5E);}
  else if( s == "ld e, a" ){  addByte(0x5F);}
  else if( s == "ld h, b" ){  addByte(0x60);}
  else if( s == "ld h, c" ){  addByte(0x61);}
  else if( s == "ld h, d" ){  addByte(0x62);}
  else if( s == "ld h, e" ){  addByte(0x63);}
  else if( s == "ld h, h" ){  addByte(0x64);}
  else if( s == "ld h, l" ){  addByte(0x65);}
  else if( s == "ld h, (hl)" ){  addByte(0x66);}
  else if( s == "ld h, a" ){  addByte(0x67);}
  else if( s == "ld l, b" ){  addByte(0x68);}
  else if( s == "ld l, c" ){  addByte(0x69);}
  else if( s == "ld l, d" ){  addByte(0x6A);}
  else if( s == "ld l, e" ){  addByte(0x6B);}
  else if( s == "ld l, h" ){  addByte(0x6C);}
  else if( s == "ld l, l" ){  addByte(0x6D);}
  else if( s == "ld l, (hl)" ){  addByte(0x6E);}
  else if( s == "ld l, a" ){  addByte(0x6F);}
  else if( s == "ld (hl), b" ){  addByte(0x70);}
  else if( s == "ld (hl), c" ){  addByte(0x71);}
  else if( s == "ld (hl), d" ){  addByte(0x72);}
  else if( s == "ld (hl), e" ){  addByte(0x73);}
  else if( s == "ld (hl), h" ){  addByte(0x74);}
  else if( s == "ld (hl), l" ){  addByte(0x75);}
  else if( s == "halt" ){  addByte(0x76);}
  else if( s == "ld (hl), a" ){  addByte(0x77);}
  else if( s == "ld a, b" ){  addByte(0x78);}
  else if( s == "ld a, c" ){  addByte(0x79);}
  else if( s == "ld a, d" ){  addByte(0x7A);}
  else if( s == "ld a, e" ){  addByte(0x7B);}
  else if( s == "ld a, h" ){  addByte(0x7C);}
  else if( s == "ld a, l" ){  addByte(0x7D);}
  else if( s == "ld a, (hl)" ){  addByte(0x7E);}
  else if( s == "ld a, a" ){  addByte(0x7F);}
  else if( s == "add a,b" ){addByte(0x80);}
  else if( s == "add a,c" ){addByte(0x81);}
  else if( s == "add a,d" ){addByte(0x82);}
  else if( s == "add a,e" ){addByte(0x83);}
  else if( s == "add a,h" ){addByte(0x84);}
  else if( s == "add a,l" ){addByte(0x85);}
  else if( s == "add a,(hl)" ){addByte(0x86);}
  else if( s == "add a,a" ){addByte(0x87);}
  else if( s == "adc a,b" ){addByte(0x88);}
  else if( s == "adc a,c" ){addByte(0x89);}
  else if( s == "adc a,d" ){addByte(0x8A);}
  else if( s == "adc a,e" ){addByte(0x8B);}
  else if( s == "adc a,h" ){addByte(0x8C);}
  else if( s == "adc a,l" ){addByte(0x8D);}
  else if( s == "adc a,(hl)" ){addByte(0x8E);}
  else if( s == "adc a,a" ){addByte(0x8F);}
  else if( s == "subb" ){addByte(0x90);}
  else if( s == "subc" ){addByte(0x91);}
  else if( s == "subd" ){addByte(0x92);}
  else if( s == "sube" ){addByte(0x93);}
  else if( s == "subh" ){addByte(0x94);}
  else if( s == "subl" ){addByte(0x95);}
  else if( s == "sub(hl)" ){addByte(0x96);}
  else if( s == "suba" ){addByte(0x97);}
  else if( s == "sbc a,b" ){addByte(0x98);}
  else if( s == "sbc a,c" ){addByte(0x99);}
  else if( s == "sbc a,d" ){addByte(0x9A);}
  else if( s == "sbc a,e" ){addByte(0x9B);}
  else if( s == "sbc a,h" ){addByte(0x9C);}
  else if( s == "sbc a,l" ){addByte(0x9D);}
  else if( s == "sbc a,(hl)" ){addByte(0x9E);}
  else if( s == "sbc a,a" ){addByte(0x9F);}
  else if( s == "add a, b" ){addByte(0x80);}
  else if( s == "add a, c" ){addByte(0x81);}
  else if( s == "add a, d" ){addByte(0x82);}
  else if( s == "add a, e" ){addByte(0x83);}
  else if( s == "add a, h" ){addByte(0x84);}
  else if( s == "add a, l" ){addByte(0x85);}
  else if( s == "add a, (hl)" ){addByte(0x86);}
  else if( s == "add a, a" ){addByte(0x87);}
  else if( s == "adc a, b" ){addByte(0x88);}
  else if( s == "adc a, c" ){addByte(0x89);}
  else if( s == "adc a, d" ){addByte(0x8A);}
  else if( s == "adc a, e" ){addByte(0x8B);}
  else if( s == "adc a, h" ){addByte(0x8C);}
  else if( s == "adc a, l" ){addByte(0x8D);}
  else if( s == "adc a, (hl)" ){addByte(0x8E);}
  else if( s == "adc a, a" ){addByte(0x8F);}
  else if( s == "sub b" ){addByte(0x90);}
  else if( s == "sub c" ){addByte(0x91);}
  else if( s == "sub d" ){addByte(0x92);}
  else if( s == "sub e" ){addByte(0x93);}
  else if( s == "sub h" ){addByte(0x94);}
  else if( s == "sub l" ){addByte(0x95);}
  else if( s == "sub (hl)" ){addByte(0x96);}
  else if( s == "sub a" ){addByte(0x97);}
  else if( s == "sbc a, b" ){addByte(0x98);}
  else if( s == "sbc a, c" ){addByte(0x99);}
  else if( s == "sbc a, d" ){addByte(0x9A);}
  else if( s == "sbc a, e" ){addByte(0x9B);}
  else if( s == "sbc a, h" ){addByte(0x9C);}
  else if( s == "sbc a, l" ){addByte(0x9D);}
  else if( s == "sbc a, (hl)" ){addByte(0x9E);}
  else if( s == "sbc a, a" ){addByte(0x9F);}
  else if( s == "and b" ){addByte(0xA0);}
  else if( s == "and c" ){addByte(0xA1);}
  else if( s == "and d" ){addByte(0xA2);}
  else if( s == "and e" ){addByte(0xA3);}
  else if( s == "and h" ){addByte(0xA4);}
  else if( s == "and l" ){addByte(0xA5);}
  else if( s == "and (hl)" ){addByte(0xA6);}
  else if( s == "and a" ){addByte(0xA7);}
  else if( s == "and b" ){addByte(0xA8);}
  else if( s == "xor c" ){addByte(0xA9);}
  else if( s == "xor d" ){addByte(0xAA);}
  else if( s == "xor e" ){addByte(0xAB);}
  else if( s == "xor h" ){addByte(0xAC);}
  else if( s == "xor l" ){addByte(0xAD);}
  else if( s == "xor (hl)" ){addByte(0xAE);}
  else if( s == "xor a" ){addByte(0xAF);}
  else if( s == "or b" ){addByte(0xB0);}
  else if( s == "or c" ){addByte(0xB1);}
  else if( s == "or d" ){addByte(0xB2);}
  else if( s == "or e" ){addByte(0xB3);}
  else if( s == "or h" ){addByte(0xB4);}
  else if( s == "or l" ){addByte(0xB5);}
  else if( s == "or (hl)" ){addByte(0xB6);}
  else if( s == "or a" ){addByte(0xB7);}
  else if( s == "cp b" ){addByte(0xB8);}
  else if( s == "cp c" ){addByte(0xB9);}
  else if( s == "cp d" ){addByte(0xBA);}
  else if( s == "cp e" ){addByte(0xBB);}
  else if( s == "cp h" ){addByte(0xBC);}
  else if( s == "cp l" ){addByte(0xBD);}
  else if( s == "cp (hl)" ){addByte(0xBE);}
  else if( s == "cp a" ){addByte(0xBF);}
  else if( s == "ret nz" ){addByte(0xC0);}
  else if( s == "pop bc" ){addByte(0xC1);}
  else if( s == "jp nz, **" ){addByte(0xC2);}
  else if( s == "jp **" ){addByte(0xC3);}
  else if( s == "call nz, **" ){addByte(0xC4);}
  else if( s == "push bc" ){addByte(0xC5);}
  else if( s == "add a, *" ){addByte(0xC6);}
  else if( s == "rst 0x00" ){addByte(0xC7);}
  else if( s == "ret z" ){addByte(0xC8);}
  else if( s == "jp z, **" ){addByte(0xCA);}
  else if( s == "BITS" )
    {
      //addByte(0xCB);
      error_count++;
      cerr << endl << "*** instruction not implemented yet [" << s << "] - compilation failed ***" << endl;
      cerr << "    line number: " << dec << line_number << endl;

      cerr << "    program index: 0x" << hex << uppercase << program_index << " (" << dec << program_index << " decimal)" << endl << endl;
      compilation_failed=1;


      
    }
  else if( s == "call z, **" ){addByte(0xCC);}
  else if( s == "call **" ){addByte(0xCD);}
  else if( s == "adc a, *" ){addByte(0xCE);}
  else if( s == "rst 0x08" ){addByte(0xCF);}
  else if( s == "ret nc" ){addByte(0xD0);}
  else if( s == "pop de" ){addByte(0xD1);}
  else if( s == "jp nc, **" ){addByte(0xD2);}
  else if( s == "out (*), a" ){addByte(0xD3);}
  else if( s == "call nc, **" ){addByte(0xD4);}
  else if( s == "push de" ){addByte(0xD5);}
  else if( s == "sub *" ){addByte(0xD6);}
  else if( s == "rst 0x10" ){addByte(0xD7);}
  else if( s == "ret c" ){addByte(0xD8);}
  else if( s == "exx" ){addByte(0xD9);}
  else if( s == "jp c, **" ){addByte(0xDA);}
  else if( s == "in a, (*)" ){addByte(0xDB);}
  else if( s == "call c, **" ){addByte(0xDC);}
  else if( s == "IX" )
    {
      //addByte(0xDD);
      error_count++;
      cerr << endl << "*** instruction not implemented yet [" << s << "] - compilation failed ***" << endl;
      cerr << "    line number: " << dec << line_number << endl;

      cerr << "    program index: 0x" << hex << uppercase << program_index << " (" << dec << program_index << " decimal)" << endl << endl;
      compilation_failed=1;
    }
  else if( s == "sbc a, *" ){addByte(0xDE);}
  else if( s == "rst 0x18" ){addByte(0xDF);}
  else if( s == "ret po" ){addByte(0xE0);}
  else if( s == "pop hl" ){addByte(0xE1);}
  else if( s == "jp po, **" ){addByte(0xE2);}
  else if( s == "ex (sp), hl" ){addByte(0xE3);}
  else if( s == "call po, **" ){addByte(0xE4);}
  else if( s == "push hl" ){addByte(0xE5);}
  else if( s == "and *" ){addByte(0xE6);}
  else if( s == "rst 0x20" ){addByte(0xE7);}
  else if( s == "ret pe" ){addByte(0xE8);}
  else if( s == "jp (hl)" ){addByte(0xE9);}
  else if( s == "jp pe, **" ){addByte(0xEA);}
  else if( s == "ex de, hl" ){addByte(0xEB);}
  else if( s == "call pe, **" ){addByte(0xEC);}
  else if( s == "in b, (c)" ){addByte(0xED);addByte(0x40);}
  else if( s == "out (c), b" ){addByte(0xED);addByte(0x41);}
  else if( s == "sbc hl,bc" ){addByte(0xED);addByte(0x42);}
  else if( s == "ld (**),bc" ){addByte(0xED);addByte(0x43);}
  else if( s == "neg" ){addByte(0xED);addByte(0x44);}
  else if( s == "retn" ){addByte(0xED);addByte(0x45);}
  else if( s == "im 0" ){addByte(0xED);addByte(0x46);}
  else if( s == "ld i,a" ){addByte(0xED);addByte(0x47);}
  else if( s == "in c,(c)" ){addByte(0xED);addByte(0x48);}
  else if( s == "out (c),c" ){addByte(0xED);addByte(0x49);}
  else if( s == "adc hl,bc" ){addByte(0xED);addByte(0x4A);}
  else if( s == "ld bc,(**)" ){addByte(0xED);addByte(0x4B);}
  else if( s == "neg"){addByte(0xED);addByte(0x4C);}
  else if( s == "reti"){addByte(0xED);addByte(0x4D);}
  else if( s == "im 0/1"){addByte(0xED);addByte(0x4E);}
  else if( s == "ld r,a" ){addByte(0xED);addByte(0x4F);}
  else if( s == "in d,(c)"){addByte(0xED);addByte(0x50);}
  else if( s == "out (c),d"){addByte(0xED);addByte(0x51);}
  else if( s == "sbc hl,de"){addByte(0xED);addByte(0x52);}
  else if( s == "ld (**),de"){addByte(0xED);addByte(0x53);}
  else if( s == "neg"){addByte(0xED);addByte(0x54);}
  else if( s == "retn"){addByte(0xED);addByte(0x55);}
  else if( s == "im 1"){addByte(0xED);addByte(0x56);}
  else if( s == "ld a,i"){addByte(0xED);addByte(0x57);}
  else if( s == "in e,(c)"){addByte(0xED);addByte(0x58);}
  else if( s == "out (c),e"){addByte(0xED);addByte(0x59);}
  else if( s == "adc hl,de"){addByte(0xED);addByte(0x5A);}
  else if( s == "ld de,(**)"){addByte(0xED);addByte(0x5B);}
  else if( s == "neg"){addByte(0xED);addByte(0x5C);}
  else if( s == "retn"){addByte(0xED);addByte(0x5D);}
  else if( s == "im 2"){addByte(0xED);addByte(0x5E);}
  else if( s == "ld a,r" ){addByte(0xED);addByte(0x5F);}
  else if( s == "in h,(c)"){addByte(0xED);addByte(0x60);}
  else if( s == "out (c),h"){addByte(0xED);addByte(0x61);}
  else if( s == "sbc hl,hl"){addByte(0xED);addByte(0x62);}
  else if( s == "ld (**),hl"){addByte(0xED);addByte(0x63);}
  else if( s == "neg"){addByte(0xED);addByte(0x64);}
  else if( s == "retn"){addByte(0xED);addByte(0x65);}
  else if( s == "im 0"){addByte(0xED);addByte(0x66);}
  else if( s == "rrd"){addByte(0xED);addByte(0x67);}
  else if( s == "in l,(c)"){addByte(0xED);addByte(0x68);}
  else if( s == "out (c),l"){addByte(0xED);addByte(0x69);}
  else if( s == "adc hl,hl"){addByte(0xED);addByte(0x6A);}
  else if( s == "ld hl,(**)"){addByte(0xED);addByte(0x6B);}
  else if( s == "neg"){addByte(0xED);addByte(0x6C);}
  else if( s == "retn"){addByte(0xED);addByte(0x6D);}
  else if( s == "im 0/1"){addByte(0xED);addByte(0x6E);}
  else if( s == "rld" ){addByte(0xED);addByte(0x6F);}
  else if( s == "in (c)"){addByte(0xED);addByte(0x70);}
  else if( s == "out (c),0"){addByte(0xED);addByte(0x71);}
  else if( s == "sbc hl,sp"){addByte(0xED);addByte(0x72);}
  else if( s == "ld (**),sp"){addByte(0xED);addByte(0x73);}
  else if( s == "neg"){addByte(0xED);addByte(0x74);}
  else if( s == "retn"){addByte(0xED);addByte(0x75);}
  else if( s == "im 1"){addByte(0xED);addByte(0x76);}
  else if( s == "" ){addByte(0xED);addByte(0x77);}
  else if( s == "in a,(c)"){addByte(0xED);addByte(0x78);}
  else if( s == "out (c),a"){addByte(0xED);addByte(0x79);}
  else if( s == "adc hl,sp"){addByte(0xED);addByte(0x7A);}
  else if( s == "ld sp,(**)"){addByte(0xED);addByte(0x7B);}
  else if( s == "neg"){addByte(0xED);addByte(0x7C);}
  else if( s == "retn"){addByte(0xED);addByte(0x7D);}
  else if( s == "im 2"){addByte(0xED);addByte(0x7E);}
  else if( s == "ldi"){addByte(0xED);addByte(0xA0);}
  else if( s == "cpi"){addByte(0xED);addByte(0xA1);}
  else if( s == "ini"){addByte(0xED);addByte(0xA2);}
  else if( s == "outi"){addByte(0xED);addByte(0xA3);}
  else if( s == "ldd"){addByte(0xED);addByte(0xA8);}
  else if( s == "cpd"){addByte(0xED);addByte(0xA9);}
  else if( s == "ind"){addByte(0xED);addByte(0xAA);}
  else if( s == "outd"){addByte(0xED);addByte(0xAB);}
  else if( s == "ldir"){addByte(0xED);addByte(0xB0);}
  else if( s == "cpir"){addByte(0xED);addByte(0xB1);}
  else if( s == "inir"){addByte(0xED);addByte(0xB2);}
  else if( s == "otir"){addByte(0xED);addByte(0xB3);}
  else if( s == "lddr"){addByte(0xED);addByte(0xB8);}
  else if( s == "cpdr"){addByte(0xED);addByte(0xB9);}
  else if( s == "indr"){addByte(0xED);addByte(0xBA);}
  else if( s == "otdr" ){addByte(0xED);addByte(0xBB);}
  else if( s == "rlc b"){addByte(0xCB);addByte(0x0);}
  else if( s == "rlc c"){addByte(0xCB);addByte(0x1);}
  else if( s == "rlc d"){addByte(0xCB);addByte(0x2);}
  else if( s == "rlc e"){addByte(0xCB);addByte(0x3);}
  else if( s == "rlc h"){addByte(0xCB);addByte(0x4);}
  else if( s == "rlc l"){addByte(0xCB);addByte(0x5);}
  else if( s == "rlc (hl)"){addByte(0xCB);addByte(0x6);}
  else if( s == "rlc a"){addByte(0xCB);addByte(0x7);}
  else if( s == "rrc b"){addByte(0xCB);addByte(0x8);}
  else if( s == "rrc c"){addByte(0xCB);addByte(0x9);}
  else if( s == "rrc d"){addByte(0xCB);addByte(0xA);}
  else if( s == "rrc e"){addByte(0xCB);addByte(0xB);}
  else if( s == "rrc h"){addByte(0xCB);addByte(0xC);}
  else if( s == "rrc l"){addByte(0xCB);addByte(0xD);}
  else if( s == "rrc (hl)"){addByte(0xCB);addByte(0xE);}
  else if( s == "rrc a" ){addByte(0xCB);addByte(0xF);}
  else if( s == "rl b"){addByte(0xCB);addByte(0x10);}
  else if( s == "rl c"){addByte(0xCB);addByte(0x11);}
  else if( s == "rl d"){addByte(0xCB);addByte(0x12);}
  else if( s == "rl e"){addByte(0xCB);addByte(0x13);}
  else if( s == "rl h"){addByte(0xCB);addByte(0x14);}
  else if( s == "rl l"){addByte(0xCB);addByte(0x15);}
  else if( s == "rl (hl)"){addByte(0xCB);addByte(0x16);}
  else if( s == "rl a"){addByte(0xCB);addByte(0x17);}
  else if( s == "rr b"){addByte(0xCB);addByte(0x18);}
  else if( s == "rr c"){addByte(0xCB);addByte(0x19);}
  else if( s == "rr d"){addByte(0xCB);addByte(0x1A);}
  else if( s == "rr e"){addByte(0xCB);addByte(0x1B);}
  else if( s == "rr h"){addByte(0xCB);addByte(0x1C);}
  else if( s == "rr l"){addByte(0xCB);addByte(0x1D);}
  else if( s == "rr (hl)"){addByte(0xCB);addByte(0x1E);}
  else if( s == "rr a" ){addByte(0xCB);addByte(0x1F);}
  else if( s == "sla b"){addByte(0xCB);addByte(0x20);}
  else if( s == "sla c"){addByte(0xCB);addByte(0x21);}
  else if( s == "sla d"){addByte(0xCB);addByte(0x22);}
  else if( s == "sla e"){addByte(0xCB);addByte(0x23);}
  else if( s == "sla h"){addByte(0xCB);addByte(0x24);}
  else if( s == "sla l"){addByte(0xCB);addByte(0x25);}
  else if( s == "sla (hl)"){addByte(0xCB);addByte(0x26);}
  else if( s == "sla a"){addByte(0xCB);addByte(0x27);}
  else if( s == "sra b"){addByte(0xCB);addByte(0x28);}
  else if( s == "sra c"){addByte(0xCB);addByte(0x29);}
  else if( s == "sra d"){addByte(0xCB);addByte(0x2A);}
  else if( s == "sra e"){addByte(0xCB);addByte(0x2B);}
  else if( s == "sra h"){addByte(0xCB);addByte(0x2C);}
  else if( s == "sra l"){addByte(0xCB);addByte(0x2D);}
  else if( s == "sra (hl)"){addByte(0xCB);addByte(0x2E);}
  else if( s == "sra a" ){addByte(0xCB);addByte(0x2F);}
  else if( s == "sll b"){addByte(0xCB);addByte(0x30);}
  else if( s == "sll c"){addByte(0xCB);addByte(0x31);}
  else if( s == "sll d"){addByte(0xCB);addByte(0x32);}
  else if( s == "sll e"){addByte(0xCB);addByte(0x33);}
  else if( s == "sll h"){addByte(0xCB);addByte(0x34);}
  else if( s == "sll l"){addByte(0xCB);addByte(0x35);}
  else if( s == "sll (hl)"){addByte(0xCB);addByte(0x36);}
  else if( s == "sll a"){addByte(0xCB);addByte(0x37);}
  else if( s == "srl b"){addByte(0xCB);addByte(0x38);}
  else if( s == "srl c"){addByte(0xCB);addByte(0x39);}
  else if( s == "srl d"){addByte(0xCB);addByte(0x3A);}
  else if( s == "srl e"){addByte(0xCB);addByte(0x3B);}
  else if( s == "srl h"){addByte(0xCB);addByte(0x3C);}
  else if( s == "srl l"){addByte(0xCB);addByte(0x3D);}
  else if( s == "srl (hl)"){addByte(0xCB);addByte(0x3E);}
  else if( s == "srl a" ){addByte(0xCB);addByte(0x3F);}
  else if( s == "bit 0, b"){addByte(0xCB);addByte(0x40);}
  else if( s == "bit 0, c"){addByte(0xCB);addByte(0x41);}
  else if( s == "bit 0, d"){addByte(0xCB);addByte(0x42);}
  else if( s == "bit 0, e"){addByte(0xCB);addByte(0x43);}
  else if( s == "bit 0, h"){addByte(0xCB);addByte(0x44);}
  else if( s == "bit 0, l"){addByte(0xCB);addByte(0x45);}
  else if( s == "bit 0, (hl)"){addByte(0xCB);addByte(0x46);}
  else if( s == "bit 0, a"){addByte(0xCB);addByte(0x47);}
  else if( s == "bit 1, b"){addByte(0xCB);addByte(0x48);}
  else if( s == "bit 1, c"){addByte(0xCB);addByte(0x49);}
  else if( s == "bit 1, d"){addByte(0xCB);addByte(0x4A);}
  else if( s == "bit 1, e"){addByte(0xCB);addByte(0x4B);}
  else if( s == "bit 1, h"){addByte(0xCB);addByte(0x4C);}
  else if( s == "bit 1, l"){addByte(0xCB);addByte(0x4D);}
  else if( s == "bit 1, (hl)"){addByte(0xCB);addByte(0x4E);}
  else if( s == "bit 1, a" ){addByte(0xCB);addByte(0x4F);}
  else if( s == "bit 2, b"){addByte(0xCB);addByte(0x50);}
  else if( s == "bit 2, c"){addByte(0xCB);addByte(0x51);}
  else if( s == "bit 2, d"){addByte(0xCB);addByte(0x52);}
  else if( s == "bit 2, e"){addByte(0xCB);addByte(0x53);}
  else if( s == "bit 2, h"){addByte(0xCB);addByte(0x54);}
  else if( s == "bit 2, l"){addByte(0xCB);addByte(0x55);}
  else if( s == "bit 2, (hl)"){addByte(0xCB);addByte(0x56);}
  else if( s == "bit 2, a"){addByte(0xCB);addByte(0x57);}
  else if( s == "bit 3, b"){addByte(0xCB);addByte(0x58);}
  else if( s == "bit 3, c"){addByte(0xCB);addByte(0x59);}
  else if( s == "bit 3, d"){addByte(0xCB);addByte(0x5A);}
  else if( s == "bit 3, e"){addByte(0xCB);addByte(0x5B);}
  else if( s == "bit 3, h"){addByte(0xCB);addByte(0x5C);}
  else if( s == "bit 3, l"){addByte(0xCB);addByte(0x5D);}
  else if( s == "bit 3, (hl)"){addByte(0xCB);addByte(0x5E);}
  else if( s == "bit 3, a" ){addByte(0xCB);addByte(0x5F);}
  else if( s == "bit 4, b"){addByte(0xCB);addByte(0x60);}
  else if( s == "bit 4, c"){addByte(0xCB);addByte(0x61);}
  else if( s == "bit 4, d"){addByte(0xCB);addByte(0x62);}
  else if( s == "bit 4, e"){addByte(0xCB);addByte(0x63);}
  else if( s == "bit 4, h"){addByte(0xCB);addByte(0x64);}
  else if( s == "bit 4, l"){addByte(0xCB);addByte(0x65);}
  else if( s == "bit 4, (hl)"){addByte(0xCB);addByte(0x66);}
  else if( s == "bit 4, a"){addByte(0xCB);addByte(0x67);}
  else if( s == "bit 5, b"){addByte(0xCB);addByte(0x68);}
  else if( s == "bit 5, c"){addByte(0xCB);addByte(0x69);}
  else if( s == "bit 5, d"){addByte(0xCB);addByte(0x6A);}
  else if( s == "bit 5, e"){addByte(0xCB);addByte(0x6B);}
  else if( s == "bit 5, h"){addByte(0xCB);addByte(0x6C);}
  else if( s == "bit 5, l"){addByte(0xCB);addByte(0x6D);}
  else if( s == "bit 5, (hl)"){addByte(0xCB);addByte(0x6E);}
  else if( s == "bit 5, a" ){addByte(0xCB);addByte(0x6F);}
  else if( s == "bit 6, b"){addByte(0xCB);addByte(0x70);}
  else if( s == "bit 6, c"){addByte(0xCB);addByte(0x71);}
  else if( s == "bit 6, d"){addByte(0xCB);addByte(0x72);}
  else if( s == "bit 6, e"){addByte(0xCB);addByte(0x73);}
  else if( s == "bit 6, h"){addByte(0xCB);addByte(0x74);}
  else if( s == "bit 6, l"){addByte(0xCB);addByte(0x75);}
  else if( s == "bit 6, (hl)"){addByte(0xCB);addByte(0x76);}
  else if( s == "bit 6, a"){addByte(0xCB);addByte(0x77);}
  else if( s == "bit 7, b"){addByte(0xCB);addByte(0x78);}
  else if( s == "bit 7, c"){addByte(0xCB);addByte(0x79);}
  else if( s == "bit 7, d"){addByte(0xCB);addByte(0x7A);}
  else if( s == "bit 7, e"){addByte(0xCB);addByte(0x7B);}
  else if( s == "bit 7, h"){addByte(0xCB);addByte(0x7C);}
  else if( s == "bit 7, l"){addByte(0xCB);addByte(0x7D);}
  else if( s == "bit 7, (hl)"){addByte(0xCB);addByte(0x7E);}
  else if( s == "bit 7, a" ){addByte(0xCB);addByte(0x7F);}
  else if( s == "res 0, b"){addByte(0xCB);addByte(0x80);}
  else if( s == "res 0, c"){addByte(0xCB);addByte(0x81);}
  else if( s == "res 0, d"){addByte(0xCB);addByte(0x82);}
  else if( s == "res 0, e"){addByte(0xCB);addByte(0x83);}
  else if( s == "res 0, h"){addByte(0xCB);addByte(0x84);}
  else if( s == "res 0, l"){addByte(0xCB);addByte(0x85);}
  else if( s == "res 0, (hl)"){addByte(0xCB);addByte(0x86);}
  else if( s == "res 0, a"){addByte(0xCB);addByte(0x87);}
  else if( s == "res 1, b"){addByte(0xCB);addByte(0x88);}
  else if( s == "res 1, c"){addByte(0xCB);addByte(0x89);}
  else if( s == "res 1, d"){addByte(0xCB);addByte(0x8A);}
  else if( s == "res 1, e"){addByte(0xCB);addByte(0x8B);}
  else if( s == "res 1, h"){addByte(0xCB);addByte(0x8C);}
  else if( s == "res 1, l"){addByte(0xCB);addByte(0x8D);}
  else if( s == "res 1, (hl)"){addByte(0xCB);addByte(0x8E);}
  else if( s == "res 1, a" ){addByte(0xCB);addByte(0x8F);}
  else if( s == "res 2, b"){addByte(0xCB);addByte(0x90);}
  else if( s == "res 2, c"){addByte(0xCB);addByte(0x91);}
  else if( s == "res 2, d"){addByte(0xCB);addByte(0x92);}
  else if( s == "res 2, e"){addByte(0xCB);addByte(0x93);}
  else if( s == "res 2, h"){addByte(0xCB);addByte(0x94);}
  else if( s == "res 2, l"){addByte(0xCB);addByte(0x95);}
  else if( s == "res 2, (hl)"){addByte(0xCB);addByte(0x96);}
  else if( s == "res 2, a"){addByte(0xCB);addByte(0x97);}
  else if( s == "res 3, b"){addByte(0xCB);addByte(0x98);}
  else if( s == "res 3, c"){addByte(0xCB);addByte(0x99);}
  else if( s == "res 3, d"){addByte(0xCB);addByte(0x9A);}
  else if( s == "res 3, e"){addByte(0xCB);addByte(0x9B);}
  else if( s == "res 3, h"){addByte(0xCB);addByte(0x9C);}
  else if( s == "res 3, l"){addByte(0xCB);addByte(0x9D);}
  else if( s == "res 3, (hl)"){addByte(0xCB);addByte(0x9E);}
  else if( s == "res 3, a" ){addByte(0xCB);addByte(0x9F);}
  else if( s == "res 4, b"){addByte(0xCB);addByte(0xA0);}
  else if( s == "res 4, c"){addByte(0xCB);addByte(0xA1);}
  else if( s == "res 4, d"){addByte(0xCB);addByte(0xA2);}
  else if( s == "res 4, e"){addByte(0xCB);addByte(0xA3);}
  else if( s == "res 4, h"){addByte(0xCB);addByte(0xA4);}
  else if( s == "res 4, l"){addByte(0xCB);addByte(0xA5);}
  else if( s == "res 4, (hl)"){addByte(0xCB);addByte(0xA6);}
  else if( s == "res 4, a"){addByte(0xCB);addByte(0xA7);}
  else if( s == "res 5, b"){addByte(0xCB);addByte(0xA8);}
  else if( s == "res 5, c"){addByte(0xCB);addByte(0xA9);}
  else if( s == "res 5, d"){addByte(0xCB);addByte(0xAA);}
  else if( s == "res 5, e"){addByte(0xCB);addByte(0xAB);}
  else if( s == "res 5, h"){addByte(0xCB);addByte(0xAC);}
  else if( s == "res 5, l"){addByte(0xCB);addByte(0xAD);}
  else if( s == "res 5, (hl)"){addByte(0xCB);addByte(0xAE);}
  else if( s == "res 5, a" ){addByte(0xCB);addByte(0xAF);}
  else if( s == "res 6, b"){addByte(0xCB);addByte(0xB0);}
  else if( s == "res 6, c"){addByte(0xCB);addByte(0xB1);}
  else if( s == "res 6, d"){addByte(0xCB);addByte(0xB2);}
  else if( s == "res 6, e"){addByte(0xCB);addByte(0xB3);}
  else if( s == "res 6, h"){addByte(0xCB);addByte(0xB4);}
  else if( s == "res 6, l"){addByte(0xCB);addByte(0xB5);}
  else if( s == "res 6, (hl)"){addByte(0xCB);addByte(0xB6);}
  else if( s == "res 6, a"){addByte(0xCB);addByte(0xB7);}
  else if( s == "res 7, b"){addByte(0xCB);addByte(0xB8);}
  else if( s == "res 7, c"){addByte(0xCB);addByte(0xB9);}
  else if( s == "res 7, d"){addByte(0xCB);addByte(0xBA);}
  else if( s == "res 7, e"){addByte(0xCB);addByte(0xBB);}
  else if( s == "res 7, h"){addByte(0xCB);addByte(0xBC);}
  else if( s == "res 7, l"){addByte(0xCB);addByte(0xBD);}
  else if( s == "res 7, (hl)"){addByte(0xCB);addByte(0xBE);}
  else if( s == "res 7, a" ){addByte(0xCB);addByte(0xBF);}
  else if( s == "set 0, b"){addByte(0xCB);addByte(0xC0);}
  else if( s == "set 0, c"){addByte(0xCB);addByte(0xC1);}
  else if( s == "set 0, d"){addByte(0xCB);addByte(0xC2);}
  else if( s == "set 0, e"){addByte(0xCB);addByte(0xC3);}
  else if( s == "set 0, h"){addByte(0xCB);addByte(0xC4);}
  else if( s == "set 0, l"){addByte(0xCB);addByte(0xC5);}
  else if( s == "set 0, (hl)"){addByte(0xCB);addByte(0xC6);}
  else if( s == "set 0, a"){addByte(0xCB);addByte(0xC7);}
  else if( s == "set 1, b"){addByte(0xCB);addByte(0xC8);}
  else if( s == "set 1, c"){addByte(0xCB);addByte(0xC9);}
  else if( s == "set 1, d"){addByte(0xCB);addByte(0xCA);}
  else if( s == "set 1, e"){addByte(0xCB);addByte(0xCB);}
  else if( s == "set 1, h"){addByte(0xCB);addByte(0xCC);}
  else if( s == "set 1, l"){addByte(0xCB);addByte(0xCD);}
  else if( s == "set 1, (hl)"){addByte(0xCB);addByte(0xCE);}
  else if( s == "set 1, a" ){addByte(0xCB);addByte(0xCF);}
  else if( s == "set 2, b"){addByte(0xCB);addByte(0xD0);}
  else if( s == "set 2, c"){addByte(0xCB);addByte(0xD1);}
  else if( s == "set 2, d"){addByte(0xCB);addByte(0xD2);}
  else if( s == "set 2, e"){addByte(0xCB);addByte(0xD3);}
  else if( s == "set 2, h"){addByte(0xCB);addByte(0xD4);}
  else if( s == "set 2, l"){addByte(0xCB);addByte(0xD5);}
  else if( s == "set 2, (hl)"){addByte(0xCB);addByte(0xD6);}
  else if( s == "set 2, a"){addByte(0xCB);addByte(0xD7);}
  else if( s == "set 3, b"){addByte(0xCB);addByte(0xD8);}
  else if( s == "set 3, c"){addByte(0xCB);addByte(0xD9);}
  else if( s == "set 3, d"){addByte(0xCB);addByte(0xDA);}
  else if( s == "set 3, e"){addByte(0xCB);addByte(0xDB);}
  else if( s == "set 3, h"){addByte(0xCB);addByte(0xDC);}
  else if( s == "set 3, l"){addByte(0xCB);addByte(0xDD);}
  else if( s == "set 3, (hl)"){addByte(0xCB);addByte(0xDE);}
  else if( s == "set 3, a" ){addByte(0xCB);addByte(0xDF);}
  else if( s == "set 4, b"){addByte(0xCB);addByte(0xE0);}
  else if( s == "set 4, c"){addByte(0xCB);addByte(0xE1);}
  else if( s == "set 4, d"){addByte(0xCB);addByte(0xE2);}
  else if( s == "set 4, e"){addByte(0xCB);addByte(0xE3);}
  else if( s == "set 4, h"){addByte(0xCB);addByte(0xE4);}
  else if( s == "set 4, l"){addByte(0xCB);addByte(0xE5);}
  else if( s == "set 4, (hl)"){addByte(0xCB);addByte(0xE6);}
  else if( s == "set 4, a"){addByte(0xCB);addByte(0xE7);}
  else if( s == "set 5, b"){addByte(0xCB);addByte(0xE8);}
  else if( s == "set 5, c"){addByte(0xCB);addByte(0xE9);}
  else if( s == "set 5, d"){addByte(0xCB);addByte(0xEA);}
  else if( s == "set 5, e"){addByte(0xCB);addByte(0xEB);}
  else if( s == "set 5, h"){addByte(0xCB);addByte(0xEC);}
  else if( s == "set 5, l"){addByte(0xCB);addByte(0xED);}
  else if( s == "set 5, (hl)"){addByte(0xCB);addByte(0xEE);}
  else if( s == "set 5, a" ){addByte(0xCB);addByte(0xEF);}
  else if( s == "set 6, b"){addByte(0xCB);addByte(0xF0);}
  else if( s == "set 6, c"){addByte(0xCB);addByte(0xF1);}
  else if( s == "set 6, d"){addByte(0xCB);addByte(0xF2);}
  else if( s == "set 6, e"){addByte(0xCB);addByte(0xF3);}
  else if( s == "set 6, h"){addByte(0xCB);addByte(0xF4);}
  else if( s == "set 6, l"){addByte(0xCB);addByte(0xF5);}
  else if( s == "set 6, (hl)"){addByte(0xCB);addByte(0xF6);}
  else if( s == "set 6, a"){addByte(0xCB);addByte(0xF7);}
  else if( s == "set 7, b"){addByte(0xCB);addByte(0xF8);}
  else if( s == "set 7, c"){addByte(0xCB);addByte(0xF9);}
  else if( s == "set 7, d"){addByte(0xCB);addByte(0xFA);}
  else if( s == "set 7, e"){addByte(0xCB);addByte(0xFB);}
  else if( s == "set 7, h"){addByte(0xCB);addByte(0xFC);}
  else if( s == "set 7, l"){addByte(0xCB);addByte(0xFD);}
  else if( s == "set 7, (hl)"){addByte(0xCB);addByte(0xFE);}
  else if( s == "set 7, a" ){addByte(0xCB);addByte(0xFF);}
  
  else if( s == "rlc (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0x0);}
  else if( s == "rlc (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0x1);}
  else if( s == "rlc (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0x2);}
  else if( s == "rlc (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0x3);}
  else if( s == "rlc (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0x4);}
  else if( s == "rlc (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0x5);}
  else if( s == "rlc (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x6);}
  else if( s == "rlc (ix+*), a"){addByte(0xDD);addByte(0xCB);addByte(0x7);}
  else if( s == "rrc (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0x8);}
  else if( s == "rrc (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0x9);}
  else if( s == "rrc (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0xA);}
  else if( s == "rrc (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0xB);}
  else if( s == "rrc (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0xC);}
  else if( s == "rrc (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0xD);}
  else if( s == "rrc (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0xE);}
  else if( s == "rrc (ix+*), a" ){addByte(0xDD);addByte(0xCB);addByte(0xF);}
  else if( s == "rl (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0x10);}
  else if( s == "rl (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0x11);}
  else if( s == "rl (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0x12);}
  else if( s == "rl (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0x13);}
  else if( s == "rl (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0x14);}
  else if( s == "rl (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0x15);}
  else if( s == "rl (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x16);}
  else if( s == "rl (ix+*), a"){addByte(0xDD);addByte(0xCB);addByte(0x17);}
  else if( s == "rr (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0x18);}
  else if( s == "rr (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0x19);}
  else if( s == "rr (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0x1A);}
  else if( s == "rr (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0x1B);}
  else if( s == "rr (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0x1C);}
  else if( s == "rr (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0x1D);}
  else if( s == "rr (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x1E);}
  else if( s == "rr (ix+*), a" ){addByte(0xDD);addByte(0xCB);addByte(0x1F);}
  else if( s == "sla (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0x20);}
  else if( s == "sla (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0x21);}
  else if( s == "sla (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0x22);}
  else if( s == "sla (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0x23);}
  else if( s == "sla (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0x24);}
  else if( s == "sla (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0x25);}
  else if( s == "sla (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x26);}
  else if( s == "sla (ix+*), a"){addByte(0xDD);addByte(0xCB);addByte(0x27);}
  else if( s == "sra (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0x28);}
  else if( s == "sra (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0x29);}
  else if( s == "sra (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0x2A);}
  else if( s == "sra (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0x2B);}
  else if( s == "sra (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0x2C);}
  else if( s == "sra (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0x2D);}
  else if( s == "sra (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x2E);}
  else if( s == "sra (ix+*), a" ){addByte(0xDD);addByte(0xCB);addByte(0x2F);}
  else if( s == "sll (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0x30);}
  else if( s == "sll (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0x31);}
  else if( s == "sll (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0x32);}
  else if( s == "sll (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0x33);}
  else if( s == "sll (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0x34);}
  else if( s == "sll (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0x35);}
  else if( s == "sll (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x36);}
  else if( s == "sll (ix+*), a"){addByte(0xDD);addByte(0xCB);addByte(0x37);}
  else if( s == "srl (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0x38);}
  else if( s == "srl (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0x39);}
  else if( s == "srl (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0x3A);}
  else if( s == "srl (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0x3B);}
  else if( s == "srl (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0x3C);}
  else if( s == "srl (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0x3D);}
  else if( s == "srl (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x3E);}
  else if( s == "srl (ix+*), a" ){addByte(0xDD);addByte(0xCB);addByte(0x3F);}
  else if( s == "bit 0, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x40);}
  else if( s == "bit 0, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x41);}
  else if( s == "bit 0, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x42);}
  else if( s == "bit 0, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x43);}
  else if( s == "bit 0, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x44);}
  else if( s == "bit 0, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x45);}
  else if( s == "bit 0, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x46);}
  else if( s == "bit 0, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x47);}
  else if( s == "bit 1, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x48);}
  else if( s == "bit 1, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x49);}
  else if( s == "bit 1, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x4A);}
  else if( s == "bit 1, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x4B);}
  else if( s == "bit 1, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x4C);}
  else if( s == "bit 1, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x4D);}
  else if( s == "bit 1, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x4E);}
  else if( s == "bit 1, (ix+*)" ){addByte(0xDD);addByte(0xCB);addByte(0x4F);}
  else if( s == "bit 2, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x50);}
  else if( s == "bit 2, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x51);}
  else if( s == "bit 2, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x52);}
  else if( s == "bit 2, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x53);}
  else if( s == "bit 2, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x54);}
  else if( s == "bit 2, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x55);}
  else if( s == "bit 2, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x56);}
  else if( s == "bit 2, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x57);}
  else if( s == "bit 3, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x58);}
  else if( s == "bit 3, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x59);}
  else if( s == "bit 3, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x5A);}
  else if( s == "bit 3, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x5B);}
  else if( s == "bit 3, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x5C);}
  else if( s == "bit 3, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x5D);}
  else if( s == "bit 3, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x5E);}
  else if( s == "bit 3, (ix+*)" ){addByte(0xDD);addByte(0xCB);addByte(0x5F);}
  else if( s == "bit 4, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x60);}
  else if( s == "bit 4, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x61);}
  else if( s == "bit 4, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x62);}
  else if( s == "bit 4, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x63);}
  else if( s == "bit 4, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x64);}
  else if( s == "bit 4, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x65);}
  else if( s == "bit 4, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x66);}
  else if( s == "bit 4, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x67);}
  else if( s == "bit 5, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x68);}
  else if( s == "bit 5, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x69);}
  else if( s == "bit 5, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x6A);}
  else if( s == "bit 5, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x6B);}
  else if( s == "bit 5, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x6C);}
  else if( s == "bit 5, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x6D);}
  else if( s == "bit 5, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x6E);}
  else if( s == "bit 5, (ix+*)" ){addByte(0xDD);addByte(0xCB);addByte(0x6F);}
  else if( s == "bit 6, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x70);}
  else if( s == "bit 6, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x71);}
  else if( s == "bit 6, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x72);}
  else if( s == "bit 6, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x73);}
  else if( s == "bit 6, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x74);}
  else if( s == "bit 6, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x75);}
  else if( s == "bit 6, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x76);}
  else if( s == "bit 6, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x77);}
  else if( s == "bit 7, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x78);}
  else if( s == "bit 7, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x79);}
  else if( s == "bit 7, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x7A);}
  else if( s == "bit 7, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x7B);}
  else if( s == "bit 7, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x7C);}
  else if( s == "bit 7, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x7D);}
  else if( s == "bit 7, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x7E);}
  else if( s == "bit 7, (ix+*)" ){addByte(0xDD);addByte(0xCB);addByte(0x7F);}
  else if( s == "res 0, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0x80);}
  else if( s == "res 0, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0x81);}
  else if( s == "res 0, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0x82);}
  else if( s == "res 0, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0x83);}
  else if( s == "res 0, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0x84);}
  else if( s == "res 0, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0x85);}
  else if( s == "res 0, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x86);}
  else if( s == "res 0, (ix+*), a"){addByte(0xDD);addByte(0xCB);addByte(0x87);}
  else if( s == "res 1, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0x88);}
  else if( s == "res 1, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0x89);}
  else if( s == "res 1, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0x8A);}
  else if( s == "res 1, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0x8B);}
  else if( s == "res 1, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0x8C);}
  else if( s == "res 1, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0x8D);}
  else if( s == "res 1, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x8E);}
  else if( s == "res 1, (ix+*), a" ){addByte(0xDD);addByte(0xCB);addByte(0x8F);}
  else if( s == "res 2, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0x90);}
  else if( s == "res 2, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0x91);}
  else if( s == "res 2, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0x92);}
  else if( s == "res 2, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0x93);}
  else if( s == "res 2, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0x94);}
  else if( s == "res 2, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0x95);}
  else if( s == "res 2, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x96);}
  else if( s == "res 2, (ix+*), a"){addByte(0xDD);addByte(0xCB);addByte(0x97);}
  else if( s == "res 3, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0x98);}
  else if( s == "res 3, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0x99);}
  else if( s == "res 3, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0x9A);}
  else if( s == "res 3, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0x9B);}
  else if( s == "res 3, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0x9C);}
  else if( s == "res 3, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0x9D);}
  else if( s == "res 3, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0x9E);}
  else if( s == "res 3, (ix+*), a" ){addByte(0xDD);addByte(0xCB);addByte(0x9F);}
  else if( s == "res 4, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0xA0);}
  else if( s == "res 4, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0xA1);}
  else if( s == "res 4, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0xA2);}
  else if( s == "res 4, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0xA3);}
  else if( s == "res 4, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0xA4);}
  else if( s == "res 4, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0xA5);}
  else if( s == "res 4, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0xA6);}
  else if( s == "res 4, (ix+*), a"){addByte(0xDD);addByte(0xCB);addByte(0xA7);}
  else if( s == "res 5, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0xA8);}
  else if( s == "res 5, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0xA9);}
  else if( s == "res 5, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0xAA);}
  else if( s == "res 5, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0xAB);}
  else if( s == "res 5, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0xAC);}
  else if( s == "res 5, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0xAD);}
  else if( s == "res 5, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0xAE);}
  else if( s == "res 5, (ix+*), a" ){addByte(0xDD);addByte(0xCB);addByte(0xAF);}
  else if( s == "res 6, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0xB0);}
  else if( s == "res 6, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0xB1);}
  else if( s == "res 6, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0xB2);}
  else if( s == "res 6, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0xB3);}
  else if( s == "res 6, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0xB4);}
  else if( s == "res 6, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0xB5);}
  else if( s == "res 6, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0xB6);}
  else if( s == "res 6, (ix+*), a"){addByte(0xDD);addByte(0xCB);addByte(0xB7);}
  else if( s == "res 7, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0xB8);}
  else if( s == "res 7, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0xB9);}
  else if( s == "res 7, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0xBA);}
  else if( s == "res 7, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0xBB);}
  else if( s == "res 7, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0xBC);}
  else if( s == "res 7, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0xBD);}
  else if( s == "res 7, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0xBE);}
  else if( s == "res 7, (ix+*), a" ){addByte(0xDD);addByte(0xCB);addByte(0xBF);}
  else if( s == "set 0, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0xC0);}
  else if( s == "set 0, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0xC1);}
  else if( s == "set 0, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0xC2);}
  else if( s == "set 0, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0xC3);}
  else if( s == "set 0, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0xC4);}
  else if( s == "set 0, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0xC5);}
  else if( s == "set 0, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0xC6);}
  else if( s == "set 0, (ix+*), a"){addByte(0xDD);addByte(0xCB);addByte(0xC7);}
  else if( s == "set 1, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0xC8);}
  else if( s == "set 1, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0xC9);}
  else if( s == "set 1, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0xCA);}
  else if( s == "set 1, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0xCB);}
  else if( s == "set 1, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0xCC);}
  else if( s == "set 1, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0xCD);}
  else if( s == "set 1, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0xCE);}
  else if( s == "set 1, (ix+*), a" ){addByte(0xDD);addByte(0xCB);addByte(0xCF);}
  else if( s == "set 2, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0xD0);}
  else if( s == "set 2, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0xD1);}
  else if( s == "set 2, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0xD2);}
  else if( s == "set 2, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0xD3);}
  else if( s == "set 2, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0xD4);}
  else if( s == "set 2, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0xD5);}
  else if( s == "set 2, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0xD6);}
  else if( s == "set 2, (ix+*), a"){addByte(0xDD);addByte(0xCB);addByte(0xD7);}
  else if( s == "set 3, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0xD8);}
  else if( s == "set 3, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0xD9);}
  else if( s == "set 3, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0xDA);}
  else if( s == "set 3, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0xDB);}
  else if( s == "set 3, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0xDC);}
  else if( s == "set 3, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0xDD);}
  else if( s == "set 3, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0xDE);}
  else if( s == "set 3, (ix+*), a" ){addByte(0xDD);addByte(0xCB);addByte(0xDF);}
  else if( s == "set 4, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0xE0);}
  else if( s == "set 4, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0xE1);}
  else if( s == "set 4, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0xE2);}
  else if( s == "set 4, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0xE3);}
  else if( s == "set 4, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0xE4);}
  else if( s == "set 4, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0xE5);}
  else if( s == "set 4, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0xE6);}
  else if( s == "set 4, (ix+*), a"){addByte(0xDD);addByte(0xCB);addByte(0xE7);}
  else if( s == "set 5, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0xE8);}
  else if( s == "set 5, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0xE9);}
  else if( s == "set 5, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0xEA);}
  else if( s == "set 5, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0xEB);}
  else if( s == "set 5, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0xEC);}
  else if( s == "set 5, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0xED);}
  else if( s == "set 5, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0xEE);}
  else if( s == "set 5, (ix+*), a" ){addByte(0xDD);addByte(0xCB);addByte(0xEF);}
  else if( s == "set 6, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0xF0);}
  else if( s == "set 6, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0xF1);}
  else if( s == "set 6, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0xF2);}
  else if( s == "set 6, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0xF3);}
  else if( s == "set 6, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0xF4);}
  else if( s == "set 6, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0xF5);}
  else if( s == "set 6, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0xF6);}
  else if( s == "set 6, (ix+*), a"){addByte(0xDD);addByte(0xCB);addByte(0xF7);}
  else if( s == "set 7, (ix+*), b"){addByte(0xDD);addByte(0xCB);addByte(0xF8);}
  else if( s == "set 7, (ix+*), c"){addByte(0xDD);addByte(0xCB);addByte(0xF9);}
  else if( s == "set 7, (ix+*), d"){addByte(0xDD);addByte(0xCB);addByte(0xFA);}
  else if( s == "set 7, (ix+*), e"){addByte(0xDD);addByte(0xCB);addByte(0xFB);}
  else if( s == "set 7, (ix+*), h"){addByte(0xDD);addByte(0xCB);addByte(0xFC);}
  else if( s == "set 7, (ix+*), l"){addByte(0xDD);addByte(0xCB);addByte(0xFD);}
  else if( s == "set 7, (ix+*)"){addByte(0xDD);addByte(0xCB);addByte(0xFE);}
  else if( s == "set 7, (ix+*), a" ){addByte(0xDD);addByte(0xCB);addByte(0xFF);}
  else if( s == "rlc (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0x0);}
  else if( s == "rlc (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0x1);}
  else if( s == "rlc (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0x2);}
  else if( s == "rlc (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0x3);}
  else if( s == "rlc (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0x4);}
  else if( s == "rlc (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0x5);}
  else if( s == "rlc (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x6);}
  else if( s == "rlc (iy+*), a"){addByte(0xFD);addByte(0xCB);addByte(0x7);}
  else if( s == "rrc (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0x8);}
  else if( s == "rrc (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0x9);}
  else if( s == "rrc (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0xA);}
  else if( s == "rrc (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0xB);}
  else if( s == "rrc (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0xC);}
  else if( s == "rrc (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0xD);}
  else if( s == "rrc (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0xE);}
  else if( s == "rrc (iy+*), a" ){addByte(0xFD);addByte(0xCB);addByte(0xF);}
  else if( s == "rl (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0x10);}
  else if( s == "rl (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0x11);}
  else if( s == "rl (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0x12);}
  else if( s == "rl (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0x13);}
  else if( s == "rl (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0x14);}
  else if( s == "rl (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0x15);}
  else if( s == "rl (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x16);}
  else if( s == "rl (iy+*), a"){addByte(0xFD);addByte(0xCB);addByte(0x17);}
  else if( s == "rr (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0x18);}
  else if( s == "rr (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0x19);}
  else if( s == "rr (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0x1A);}
  else if( s == "rr (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0x1B);}
  else if( s == "rr (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0x1C);}
  else if( s == "rr (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0x1D);}
  else if( s == "rr (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x1E);}
  else if( s == "rr (iy+*), a" ){addByte(0xFD);addByte(0xCB);addByte(0x1F);}
  else if( s == "sla (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0x20);}
  else if( s == "sla (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0x21);}
  else if( s == "sla (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0x22);}
  else if( s == "sla (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0x23);}
  else if( s == "sla (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0x24);}
  else if( s == "sla (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0x25);}
  else if( s == "sla (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x26);}
  else if( s == "sla (iy+*), a"){addByte(0xFD);addByte(0xCB);addByte(0x27);}
  else if( s == "sra (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0x28);}
  else if( s == "sra (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0x29);}
  else if( s == "sra (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0x2A);}
  else if( s == "sra (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0x2B);}
  else if( s == "sra (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0x2C);}
  else if( s == "sra (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0x2D);}
  else if( s == "sra (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x2E);}
  else if( s == "sra (iy+*), a" ){addByte(0xFD);addByte(0xCB);addByte(0x2F);}
  else if( s == "sll (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0x30);}
  else if( s == "sll (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0x31);}
  else if( s == "sll (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0x32);}
  else if( s == "sll (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0x33);}
  else if( s == "sll (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0x34);}
  else if( s == "sll (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0x35);}
  else if( s == "sll (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x36);}
  else if( s == "sll (iy+*), a"){addByte(0xFD);addByte(0xCB);addByte(0x37);}
  else if( s == "srl (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0x38);}
  else if( s == "srl (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0x39);}
  else if( s == "srl (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0x3A);}
  else if( s == "srl (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0x3B);}
  else if( s == "srl (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0x3C);}
  else if( s == "srl (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0x3D);}
  else if( s == "srl (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x3E);}
  else if( s == "srl (iy+*), a" ){addByte(0xFD);addByte(0xCB);addByte(0x3F);}
  else if( s == "bit 0, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x40);}
  else if( s == "bit 0, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x41);}
  else if( s == "bit 0, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x42);}
  else if( s == "bit 0, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x43);}
  else if( s == "bit 0, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x44);}
  else if( s == "bit 0, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x45);}
  else if( s == "bit 0, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x46);}
  else if( s == "bit 0, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x47);}
  else if( s == "bit 1, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x48);}
  else if( s == "bit 1, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x49);}
  else if( s == "bit 1, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x4A);}
  else if( s == "bit 1, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x4B);}
  else if( s == "bit 1, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x4C);}
  else if( s == "bit 1, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x4D);}
  else if( s == "bit 1, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x4E);}
  else if( s == "bit 1, (iy+*)" ){addByte(0xFD);addByte(0xCB);addByte(0x4F);}
  else if( s == "bit 2, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x50);}
  else if( s == "bit 2, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x51);}
  else if( s == "bit 2, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x52);}
  else if( s == "bit 2, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x53);}
  else if( s == "bit 2, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x54);}
  else if( s == "bit 2, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x55);}
  else if( s == "bit 2, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x56);}
  else if( s == "bit 2, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x57);}
  else if( s == "bit 3, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x58);}
  else if( s == "bit 3, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x59);}
  else if( s == "bit 3, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x5A);}
  else if( s == "bit 3, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x5B);}
  else if( s == "bit 3, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x5C);}
  else if( s == "bit 3, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x5D);}
  else if( s == "bit 3, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x5E);}
  else if( s == "bit 3, (iy+*)" ){addByte(0xFD);addByte(0xCB);addByte(0x5F);}
  else if( s == "bit 4, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x60);}
  else if( s == "bit 4, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x61);}
  else if( s == "bit 4, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x62);}
  else if( s == "bit 4, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x63);}
  else if( s == "bit 4, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x64);}
  else if( s == "bit 4, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x65);}
  else if( s == "bit 4, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x66);}
  else if( s == "bit 4, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x67);}
  else if( s == "bit 5, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x68);}
  else if( s == "bit 5, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x69);}
  else if( s == "bit 5, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x6A);}
  else if( s == "bit 5, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x6B);}
  else if( s == "bit 5, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x6C);}
  else if( s == "bit 5, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x6D);}
  else if( s == "bit 5, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x6E);}
  else if( s == "bit 5, (iy+*)" ){addByte(0xFD);addByte(0xCB);addByte(0x6F);}
  else if( s == "bit 6, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x70);}
  else if( s == "bit 6, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x71);}
  else if( s == "bit 6, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x72);}
  else if( s == "bit 6, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x73);}
  else if( s == "bit 6, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x74);}
  else if( s == "bit 6, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x75);}
  else if( s == "bit 6, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x76);}
  else if( s == "bit 6, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x77);}
  else if( s == "bit 7, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x78);}
  else if( s == "bit 7, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x79);}
  else if( s == "bit 7, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x7A);}
  else if( s == "bit 7, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x7B);}
  else if( s == "bit 7, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x7C);}
  else if( s == "bit 7, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x7D);}
  else if( s == "bit 7, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x7E);}
  else if( s == "bit 7, (iy+*)" ){addByte(0xFD);addByte(0xCB);addByte(0x7F);}
  else if( s == "res 0, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0x80);}
  else if( s == "res 0, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0x81);}
  else if( s == "res 0, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0x82);}
  else if( s == "res 0, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0x83);}
  else if( s == "res 0, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0x84);}
  else if( s == "res 0, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0x85);}
  else if( s == "res 0, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x86);}
  else if( s == "res 0, (iy+*), a"){addByte(0xFD);addByte(0xCB);addByte(0x87);}
  else if( s == "res 1, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0x88);}
  else if( s == "res 1, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0x89);}
  else if( s == "res 1, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0x8A);}
  else if( s == "res 1, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0x8B);}
  else if( s == "res 1, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0x8C);}
  else if( s == "res 1, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0x8D);}
  else if( s == "res 1, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x8E);}
  else if( s == "res 1, (iy+*), a" ){addByte(0xFD);addByte(0xCB);addByte(0x8F);}
  else if( s == "res 2, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0x90);}
  else if( s == "res 2, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0x91);}
  else if( s == "res 2, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0x92);}
  else if( s == "res 2, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0x93);}
  else if( s == "res 2, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0x94);}
  else if( s == "res 2, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0x95);}
  else if( s == "res 2, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x96);}
  else if( s == "res 2, (iy+*), a"){addByte(0xFD);addByte(0xCB);addByte(0x97);}
  else if( s == "res 3, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0x98);}
  else if( s == "res 3, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0x99);}
  else if( s == "res 3, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0x9A);}
  else if( s == "res 3, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0x9B);}
  else if( s == "res 3, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0x9C);}
  else if( s == "res 3, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0x9D);}
  else if( s == "res 3, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0x9E);}
  else if( s == "res 3, (iy+*), a" ){addByte(0xFD);addByte(0xCB);addByte(0x9F);}
  else if( s == "res 4, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0xA0);}
  else if( s == "res 4, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0xA1);}
  else if( s == "res 4, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0xA2);}
  else if( s == "res 4, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0xA3);}
  else if( s == "res 4, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0xA4);}
  else if( s == "res 4, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0xA5);}
  else if( s == "res 4, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0xA6);}
  else if( s == "res 4, (iy+*), a"){addByte(0xFD);addByte(0xCB);addByte(0xA7);}
  else if( s == "res 5, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0xA8);}
  else if( s == "res 5, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0xA9);}
  else if( s == "res 5, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0xAA);}
  else if( s == "res 5, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0xAB);}
  else if( s == "res 5, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0xAC);}
  else if( s == "res 5, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0xAD);}
  else if( s == "res 5, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0xAE);}
  else if( s == "res 5, (iy+*), a" ){addByte(0xFD);addByte(0xCB);addByte(0xAF);}
  else if( s == "res 6, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0xB0);}
  else if( s == "res 6, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0xB1);}
  else if( s == "res 6, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0xB2);}
  else if( s == "res 6, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0xB3);}
  else if( s == "res 6, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0xB4);}
  else if( s == "res 6, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0xB5);}
  else if( s == "res 6, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0xB6);}
  else if( s == "res 6, (iy+*), a"){addByte(0xFD);addByte(0xCB);addByte(0xB7);}
  else if( s == "res 7, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0xB8);}
  else if( s == "res 7, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0xB9);}
  else if( s == "res 7, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0xBA);}
  else if( s == "res 7, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0xBB);}
  else if( s == "res 7, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0xBC);}
  else if( s == "res 7, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0xBD);}
  else if( s == "res 7, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0xBE);}
  else if( s == "res 7, (iy+*), a" ){addByte(0xFD);addByte(0xCB);addByte(0xBF);}
  else if( s == "set 0, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0xC0);}
  else if( s == "set 0, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0xC1);}
  else if( s == "set 0, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0xC2);}
  else if( s == "set 0, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0xC3);}
  else if( s == "set 0, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0xC4);}
  else if( s == "set 0, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0xC5);}
  else if( s == "set 0, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0xC6);}
  else if( s == "set 0, (iy+*), a"){addByte(0xFD);addByte(0xCB);addByte(0xC7);}
  else if( s == "set 1, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0xC8);}
  else if( s == "set 1, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0xC9);}
  else if( s == "set 1, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0xCA);}
  else if( s == "set 1, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0xCB);}
  else if( s == "set 1, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0xCC);}
  else if( s == "set 1, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0xCD);}
  else if( s == "set 1, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0xCE);}
  else if( s == "set 1, (iy+*), a" ){addByte(0xFD);addByte(0xCB);addByte(0xCF);}
  else if( s == "set 2, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0xD0);}
  else if( s == "set 2, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0xD1);}
  else if( s == "set 2, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0xD2);}
  else if( s == "set 2, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0xD3);}
  else if( s == "set 2, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0xD4);}
  else if( s == "set 2, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0xD5);}
  else if( s == "set 2, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0xD6);}
  else if( s == "set 2, (iy+*), a"){addByte(0xFD);addByte(0xCB);addByte(0xD7);}
  else if( s == "set 3, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0xD8);}
  else if( s == "set 3, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0xD9);}
  else if( s == "set 3, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0xDA);}
  else if( s == "set 3, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0xDB);}
  else if( s == "set 3, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0xDC);}
  else if( s == "set 3, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0xDD);}
  else if( s == "set 3, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0xDE);}
  else if( s == "set 3, (iy+*), a" ){addByte(0xFD);addByte(0xCB);addByte(0xDF);}
  else if( s == "set 4, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0xE0);}
  else if( s == "set 4, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0xE1);}
  else if( s == "set 4, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0xE2);}
  else if( s == "set 4, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0xE3);}
  else if( s == "set 4, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0xE4);}
  else if( s == "set 4, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0xE5);}
  else if( s == "set 4, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0xE6);}
  else if( s == "set 4, (iy+*), a"){addByte(0xFD);addByte(0xCB);addByte(0xE7);}
  else if( s == "set 5, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0xE8);}
  else if( s == "set 5, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0xE9);}
  else if( s == "set 5, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0xEA);}
  else if( s == "set 5, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0xEB);}
  else if( s == "set 5, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0xEC);}
  else if( s == "set 5, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0xED);}
  else if( s == "set 5, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0xEE);}
  else if( s == "set 5, (iy+*), a" ){addByte(0xFD);addByte(0xCB);addByte(0xEF);}
  else if( s == "set 6, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0xF0);}
  else if( s == "set 6, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0xF1);}
  else if( s == "set 6, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0xF2);}
  else if( s == "set 6, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0xF3);}
  else if( s == "set 6, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0xF4);}
  else if( s == "set 6, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0xF5);}
  else if( s == "set 6, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0xF6);}
  else if( s == "set 6, (iy+*), a"){addByte(0xFD);addByte(0xCB);addByte(0xF7);}
  else if( s == "set 7, (iy+*), b"){addByte(0xFD);addByte(0xCB);addByte(0xF8);}
  else if( s == "set 7, (iy+*), c"){addByte(0xFD);addByte(0xCB);addByte(0xF9);}
  else if( s == "set 7, (iy+*), d"){addByte(0xFD);addByte(0xCB);addByte(0xFA);}
  else if( s == "set 7, (iy+*), e"){addByte(0xFD);addByte(0xCB);addByte(0xFB);}
  else if( s == "set 7, (iy+*), h"){addByte(0xFD);addByte(0xCB);addByte(0xFC);}
  else if( s == "set 7, (iy+*), l"){addByte(0xFD);addByte(0xCB);addByte(0xFD);}
  else if( s == "set 7, (iy+*)"){addByte(0xFD);addByte(0xCB);addByte(0xFE);}
  else if( s == "set 7, (iy+*), a" ){addByte(0xFD);addByte(0xCB);addByte(0xFF);}

  else if( s == "add ix, bc"){addByte(0xDD);addByte(0x9);}
  else if( s == "add ix, de"){addByte(0xDD);addByte(0x19);}
  else if( s == "ld ix, **"){addByte(0xDD);addByte(0x21);}
  else if( s == "ld (**), ix"){addByte(0xDD);addByte(0x22);}
  else if( s == "inc ix"){addByte(0xDD);addByte(0x23);}
  else if( s == "inc ixh"){addByte(0xDD);addByte(0x24);}
  else if( s == "dec ixh"){addByte(0xDD);addByte(0x25);}
  else if( s == "ld ixh, *"){addByte(0xDD);addByte(0x26);}
  else if( s == "add ix, ix"){addByte(0xDD);addByte(0x29);}
  else if( s == "ld ix, (**)"){addByte(0xDD);addByte(0x2A);}
  else if( s == "dec ix"){addByte(0xDD);addByte(0x2B);}
  else if( s == "inc ixl"){addByte(0xDD);addByte(0x2C);}
  else if( s == "dec ixl"){addByte(0xDD);addByte(0x2D);}
  else if( s == "ld ixl, *"){addByte(0xDD);addByte(0x2E);}
  else if( s == "inc (ix+*)"){addByte(0xDD);addByte(0x34);}
  else if( s == "dec (ix+*)"){addByte(0xDD);addByte(0x35);}
  else if( s == "ld (ix+*), *"){addByte(0xDD);addByte(0x36);}
  else if( s == "add ix, sp"){addByte(0xDD);addByte(0x39);}
  else if( s == "ld b, ixh"){addByte(0xDD);addByte(0x44);}
  else if( s == "ld b, ixl"){addByte(0xDD);addByte(0x45);}
  else if( s == "ld b, (ix+*)"){addByte(0xDD);addByte(0x46);}
  else if( s == "ld c, ixh"){addByte(0xDD);addByte(0x4C);}
  else if( s == "ld c, ixl"){addByte(0xDD);addByte(0x4D);}
  else if( s == "ld c, (ix+*)"){addByte(0xDD);addByte(0x4E);}
  else if( s == "ld d, ixh"){addByte(0xDD);addByte(0x54);}
  else if( s == "ld d, ixl"){addByte(0xDD);addByte(0x55);}
  else if( s == "ld d, (ix+*)"){addByte(0xDD);addByte(0x56);}
  else if( s == "ld e, ixh"){addByte(0xDD);addByte(0x5C);}
  else if( s == "ld e, ixl"){addByte(0xDD);addByte(0x5D);}
  else if( s == "ld e, (ix+*)"){addByte(0xDD);addByte(0x5E);}
  else if( s == "ld ixh, b"){addByte(0xDD);addByte(0x60);}
  else if( s == "ld ixh, c"){addByte(0xDD);addByte(0x61);}
  else if( s == "ld ixh, d"){addByte(0xDD);addByte(0x62);}
  else if( s == "ld ixh, e"){addByte(0xDD);addByte(0x63);}
  else if( s == "ld ixh, ixh"){addByte(0xDD);addByte(0x64);}
  else if( s == "ld ixh, ixl"){addByte(0xDD);addByte(0x65);}
  else if( s == "ld h, (ix+*)"){addByte(0xDD);addByte(0x66);}
  else if( s == "ld ixh, a"){addByte(0xDD);addByte(0x67);}
  else if( s == "ld ixl, b"){addByte(0xDD);addByte(0x68);}
  else if( s == "ld ixl, c"){addByte(0xDD);addByte(0x69);}
  else if( s == "ld ixl, d"){addByte(0xDD);addByte(0x6A);}
  else if( s == "ld ixl, e"){addByte(0xDD);addByte(0x6B);}
  else if( s == "ld ixl, ixh"){addByte(0xDD);addByte(0x6C);}
  else if( s == "ld ixl, ixl"){addByte(0xDD);addByte(0x6D);}
  else if( s == "ld l, (ix+*)"){addByte(0xDD);addByte(0x6E);}
  else if( s == "ld ixl, a" ){addByte(0xDD);addByte(0x6F);}
  else if( s == "ld (ix+*), b"){addByte(0xDD);addByte(0x70);}
  else if( s == "ld (ix+*), c"){addByte(0xDD);addByte(0x71);}
  else if( s == "ld (ix+*), d"){addByte(0xDD);addByte(0x72);}
  else if( s == "ld (ix+*), e"){addByte(0xDD);addByte(0x73);}
  else if( s == "ld (ix+*), h"){addByte(0xDD);addByte(0x74);}
  else if( s == "ld (ix+*), l"){addByte(0xDD);addByte(0x75);}
  else if( s == "ld (ix+*), a"){addByte(0xDD);addByte(0x77);}
  else if( s == "ld a, ixh"){addByte(0xDD);addByte(0x7C);}
  else if( s == "ld a, ixl"){addByte(0xDD);addByte(0x7D);}
  else if( s == "ld a, (ix+*)"){addByte(0xDD);addByte(0x7E);}
  else if( s == "add a, ixh"){addByte(0xDD);addByte(0x84);}
  else if( s == "add a, ixl"){addByte(0xDD);addByte(0x85);}
  else if( s == "add a, (ix+*)"){addByte(0xDD);addByte(0x86);}
  else if( s == "adc a, ixh"){addByte(0xDD);addByte(0x8C);}
  else if( s == "adc a, ixl"){addByte(0xDD);addByte(0x8D);}
  else if( s == "adc a, (ix+*)"){addByte(0xDD);addByte(0x8E);}
  else if( s == "sub ixh"){addByte(0xDD);addByte(0x94);}
  else if( s == "sub ixl"){addByte(0xDD);addByte(0x95);}
  else if( s == "sub (ix+*)"){addByte(0xDD);addByte(0x96);}
  else if( s == "sbc a, ixh"){addByte(0xDD);addByte(0x9C);}
  else if( s == "sbc a, ixl"){addByte(0xDD);addByte(0x9D);}
  else if( s == "sbc a, (ix+*)"){addByte(0xDD);addByte(0x9E);}
  else if( s == "and ixh"){addByte(0xDD);addByte(0xA4);}
  else if( s == "and ixl"){addByte(0xDD);addByte(0xA5);}
  else if( s == "and (ix+*)"){addByte(0xDD);addByte(0xA6);}
  else if( s == "xor ixh"){addByte(0xDD);addByte(0xAC);}
  else if( s == "xor ixl"){addByte(0xDD);addByte(0xAD);}
  else if( s == "xor (ix+*)"){addByte(0xDD);addByte(0xAE);}
  else if( s == "or ixh"){addByte(0xDD);addByte(0xB4);}
  else if( s == "or ixl"){addByte(0xDD);addByte(0xB5);}
  else if( s == "or (ix+*)"){addByte(0xDD);addByte(0xB6);}
  else if( s == "cp ixh"){addByte(0xDD);addByte(0xBC);}
  else if( s == "cp ixl"){addByte(0xDD);addByte(0xBD);}
  else if( s == "cp (ix+*)"){addByte(0xDD);addByte(0xBE);}
  else if( s == "pop ix"){addByte(0xDD);addByte(0xE1);}
  else if( s == "ex (sp), ix"){addByte(0xDD);addByte(0xE3);}
  else if( s == "push ix"){addByte(0xDD);addByte(0xE5);}
  else if( s == "jp (ix)"){addByte(0xDD);addByte(0xE9);}
  else if( s == "ld sp, ix"){addByte(0xDD);addByte(0xF9);}
else if( s == "add iy, bc"){addByte(0xFD);addByte(0x9);}
else if( s == "add iy, de"){addByte(0xFD);addByte(0x19);}
else if( s == "ld iy, **"){addByte(0xFD);addByte(0x21);}
else if( s == "ld (**), iy"){addByte(0xFD);addByte(0x22);}
else if( s == "inc iy"){addByte(0xFD);addByte(0x23);}
else if( s == "inc iyh"){addByte(0xFD);addByte(0x24);}
else if( s == "dec iyh"){addByte(0xFD);addByte(0x25);}
else if( s == "ld iyh, *"){addByte(0xFD);addByte(0x26);}
else if( s == "add iy,iy"){addByte(0xFD);addByte(0x29);}
else if( s == "ld iy, (**)"){addByte(0xFD);addByte(0x2A);}
else if( s == "dec iy"){addByte(0xFD);addByte(0x2B);}
else if( s == "inc iyl"){addByte(0xFD);addByte(0x2C);}
else if( s == "dec iyl"){addByte(0xFD);addByte(0x2D);}
else if( s == "ld iyl, *"){addByte(0xFD);addByte(0x2E);}
else if( s == "inc (iy+*)"){addByte(0xFD);addByte(0x34);}
else if( s == "dec (iy+*)"){addByte(0xFD);addByte(0x35);}
else if( s == "ld (iy+*), *"){addByte(0xFD);addByte(0x36);}
else if( s == "add iy, sp"){addByte(0xFD);addByte(0x39);}
else if( s == "ld b, iyh"){addByte(0xFD);addByte(0x44);}
else if( s == "ld b, iyl"){addByte(0xFD);addByte(0x45);}
else if( s == "ld b, (iy+*)"){addByte(0xFD);addByte(0x46);}
else if( s == "ld c, iyh"){addByte(0xFD);addByte(0x4C);}
else if( s == "ld c, iyl"){addByte(0xFD);addByte(0x4D);}
else if( s == "ld c, (iy+*)"){addByte(0xFD);addByte(0x4E);}
else if( s == "ld d, iyh"){addByte(0xFD);addByte(0x54);}
else if( s == "ld d, iyl"){addByte(0xFD);addByte(0x55);}
else if( s == "ld d, (iy+*)"){addByte(0xFD);addByte(0x56);}
else if( s == "ld e, iyh"){addByte(0xFD);addByte(0x5C);}
else if( s == "ld e, iyl"){addByte(0xFD);addByte(0x5D);}
else if( s == "ld e, (iy+*)"){addByte(0xFD);addByte(0x5E);}
else if( s == "ld iyh, b"){addByte(0xFD);addByte(0x60);}
else if( s == "ld iyh, c"){addByte(0xFD);addByte(0x61);}
else if( s == "ld iyh, d"){addByte(0xFD);addByte(0x62);}
else if( s == "ld iyh, e"){addByte(0xFD);addByte(0x63);}
else if( s == "ld iyh, iyh"){addByte(0xFD);addByte(0x64);}
else if( s == "ld iyh, iyl"){addByte(0xFD);addByte(0x65);}
else if( s == "ld h, (iy+*)"){addByte(0xFD);addByte(0x66);}
else if( s == "ld iyh, a"){addByte(0xFD);addByte(0x67);}
else if( s == "ld iyl, b"){addByte(0xFD);addByte(0x68);}
else if( s == "ld iyl, c"){addByte(0xFD);addByte(0x69);}
else if( s == "ld iyl, d"){addByte(0xFD);addByte(0x6A);}
else if( s == "ld iyl, e"){addByte(0xFD);addByte(0x6B);}
else if( s == "ld iyl, iyh"){addByte(0xFD);addByte(0x6C);}
else if( s == "ld iyl, iyl"){addByte(0xFD);addByte(0x6D);}
else if( s == "ld l, (iy+*)"){addByte(0xFD);addByte(0x6E);}
else if( s == "ld iyl, a" ){addByte(0xFD);addByte(0x6F);}
else if( s == "ld (iy+*), b"){addByte(0xFD);addByte(0x70);}
else if( s == "ld (iy+*), c"){addByte(0xFD);addByte(0x71);}
else if( s == "ld (iy+*), d"){addByte(0xFD);addByte(0x72);}
else if( s == "ld (iy+*), e"){addByte(0xFD);addByte(0x73);}
else if( s == "ld (iy+*), h"){addByte(0xFD);addByte(0x74);}
else if( s == "ld (iy+*), l"){addByte(0xFD);addByte(0x75);}
else if( s == "ld (iy+*), a"){addByte(0xFD);addByte(0x77);}
else if( s == "ld a, iyh"){addByte(0xFD);addByte(0x7C);}
else if( s == "ld a, iyl"){addByte(0xFD);addByte(0x7D);}
else if( s == "ld a, (iy+*)"){addByte(0xFD);addByte(0x7E);}
else if( s == "add a, iyh"){addByte(0xFD);addByte(0x84);}
else if( s == "add a, iyl"){addByte(0xFD);addByte(0x85);}
else if( s == "add a, (iy+*)"){addByte(0xFD);addByte(0x86);}
else if( s == "adc a, iyh"){addByte(0xFD);addByte(0x8C);}
else if( s == "adc a, iyl"){addByte(0xFD);addByte(0x8D);}
else if( s == "adc a, (iy+*)"){addByte(0xFD);addByte(0x8E);}
else if( s == "sub iyh"){addByte(0xFD);addByte(0x94);}
else if( s == "sub iyl"){addByte(0xFD);addByte(0x95);}
else if( s == "sub (iy+*)"){addByte(0xFD);addByte(0x96);}
else if( s == "sbc a, iyh"){addByte(0xFD);addByte(0x9C);}
else if( s == "sbc a, iyl"){addByte(0xFD);addByte(0x9D);}
else if( s == "sbc a, (iy+*)"){addByte(0xFD);addByte(0x9E);}
else if( s == "and iyh"){addByte(0xFD);addByte(0xA4);}
else if( s == "and iyl"){addByte(0xFD);addByte(0xA5);}
else if( s == "and (iy+*)"){addByte(0xFD);addByte(0xA6);}
else if( s == "xor iyh"){addByte(0xFD);addByte(0xAC);}
else if( s == "xor iyl"){addByte(0xFD);addByte(0xAD);}
else if( s == "xor (iy+*)"){addByte(0xFD);addByte(0xAE);}
else if( s == "or iyh"){addByte(0xFD);addByte(0xB4);}
else if( s == "or iyl"){addByte(0xFD);addByte(0xB5);}
else if( s == "or (iy+*)"){addByte(0xFD);addByte(0xB6);}
else if( s == "cp iyh"){addByte(0xFD);addByte(0xBC);}
else if( s == "cp iyl"){addByte(0xFD);addByte(0xBD);}
else if( s == "cp (iy+*)"){addByte(0xFD);addByte(0xBE);}
else if( s == "pop iy"){addByte(0xFD);addByte(0xE1);}
else if( s == "ex (sp), iy"){addByte(0xFD);addByte(0xE3);}
else if( s == "push iy"){addByte(0xFD);addByte(0xE5);}
else if( s == "jp (iy)"){addByte(0xFD);addByte(0xE9);}
else if( s == "ld sp, iy"){addByte(0xFD);addByte(0xF9);}

  
  else if( s == "xor *" ){addByte(0xEE);}
  //else if( s == "rst 0x28" ){addByte(0xEF);}
  else if( s == "ret p" ){addByte(0xF0);}
  else if( s == "pop af" ){addByte(0xF1);}
  else if( s == "jp p, **" ){addByte(0xF2);}
  else if( s == "di" ){addByte(0xF3);}
  else if( s == "call p, **" ){addByte(0xF4);}
  else if( s == "push af" ){addByte(0xF5);}
  else if( s == "or *" ){addByte(0xF6);}
  else if( s == "rst 0x30" ){addByte(0xF7);}
  else if( s == "ret m" ){addByte(0xF8);}
  else if( s == "ld sp, hl" ){addByte(0xF9);}
  else if( s == "jp m, **" ){addByte(0xFA);}
  else if( s == "ei" ){addByte(0xFB);}
  else if( s == "call m, **" ){addByte(0xFC);}
  else if( s == "IY" )
    {
      error_count++;
      cerr << endl << "*** instruction not implemented yet [" << s << "] - compilation failed ***" << endl;
      cerr << "    line number: " << dec << line_number << endl;

      cerr << "    program index: 0x" << hex << uppercase << program_index << " (" << dec << program_index << " decimal)" << endl << endl;
      compilation_failed=1;
      //addByte(0xFD);
    }
  else if( s == "cp *" ){addByte(0xFE);}
  else if( s == "rst 0x38" ){addByte(0xFF);}
  else
    {
      cerr << endl << "*** unknown instruction [" << s << "] - compilation failed ***" << endl;
      cerr << "    line number: " << dec << line_number << endl;
      cerr << "    program index: 0x" << hex << uppercase << program_index << " (" << dec << program_index << " decimal)" << endl << endl;
      compilation_failed=1;
      error_count++;
    }

}

void addFP( string name )
{
  addLabel( name );
  for( int i=0; i<9; i++ ) addByte( 0x00 );// 9 bytes for FP_x2
}

int relativeJump(int i)
{
#ifdef DEBUG
  cerr << "relative jump: " << label_cr_vector[i-1].getMemoryLocation() - memorylocation << " bytes" << endl;
#endif
  return ( label_cr_vector[i-1].getMemoryLocation() - memorylocation -2);
}

string getBetween( string s, char L='(', char R=')' )
{
  int P1=0;
  int P2=0;
  string retVal;
  // returns the string that's between the ()'s
  for( int i=0; i<s.length(); i++ )
    {
      if( s[i] == L ) P1=i;
      if( s[i] == R ) P2=i;
    }
  P1++;
  if( P1 == 0 || P2== 0)
    {
      retVal=s;
    }
  else
    {
      retVal=s.substr(P1,P2);
      retVal.pop_back();
    }
  return retVal;
}

void function_user_input()
{
 
  if( function_ui == 1 ) return;
  function_ui=1;
  //=====================================================================
  addLabel( "user_input" );
  
  pushall();

  // clear out the buffer
  a( "ld hl, **" ); addAddress( "functionUI_text_bfr_start" );  
  a( "ld (**), hl");addAddress( "functionUI_text_bfr_ptr" );
  
  a( "ld bc, **" ); addWord(0x0A00);// The buffer is 10 bytes
  //====================================================================
  addLabel( "functionUI_clear_bfr_top" );

  // Store a zero at hl
  a( "ld (hl), *" ); addByte(0x00);
  a( "inc hl" );
  a( "djnz *" ); addOffset( "functionUI_clear_bfr_top" ); // do this 10 times


  a( "ld hl, **" ); addAddress("functionUI_text_bfr_size");
  a( "ld (hl), *" ); addByte( 0x00 ); // Store a Zero as the buffer size

  // store the address of the buffer
  a( "ld hl, **" ); addAddress( "functionUI_text_bfr_start" );
  a( "ld (**), hl");addAddress( "functionUI_text_bfr_ptr" );

  //=====================================================================
  addLabel("functionUI_top");

  // if buffer is full then return out of the function
  a( "ld a, (**)" );addAddress("functionUI_text_bfr_size");
  a( "cp *" ); addByte( 0x0A );  
  a( "jp nc, **" ); addAddress( "functionUI_return" );
  //a( "jr nc, *" ); addOffset( "functionUI_return" );

  addLabel( "functionUI_getscan" );
  
  sysCall( "GetCSC" );
  
  // While there is no available input, loop back to getscan
  a( "cp *" ); addByte( 0x00 );
  a( "jr z, *" ); addOffset( "functionUI_getscan" );

  
  // Store the entered key here temporarily
  a( "ld (**), a" ); addAddress( "functionUI_key" );
  
  a( "cp *" ); addByte( 0x09 ); // kEnter
  a( "jp z, **");addAddress( "functionUI_cr" );
  a( "cp *" ); addByte( 0x38 ); // Del
  a( "jp z, **");addAddress( "functionUI_delete" );
  a( "cp *" ); addByte( 0x02 ); // kLeft
  a( "jp z, **");addAddress( "functionUI_delete" );

  a( "cp *" ); addByte( 0x11 ); // Negative
  addLabel( "neg_check_plus_1" );
  a( "jp z, **");addAddress( "functionUI_sign" );
  a( "cp *" ); addByte( 0x0B ); // Minus
  addLabel( "min_check_plus_1" );
  a( "jp z, **");addAddress( "functionUI_sign" );

  a( "cp *" ); addByte( 0x21 ); // k0
  a( "jp z, **");addAddress( "functionUI_digit" );
  a( "cp *" ); addByte( 0x22 ); // k1
  a( "jp z, **");addAddress( "functionUI_digit" );
  a( "cp *" ); addByte( 0x1A ); // k2
  a( "jp z, **");addAddress( "functionUI_digit" );
  a( "cp *" ); addByte( 0x12 ); // k3
  a( "jp z, **");addAddress( "functionUI_digit" );
  a( "cp *" ); addByte( 0x23 ); // k4
  a( "jp z, **");addAddress( "functionUI_digit" );
  a( "cp *" ); addByte( 0x1B ); // k5
  a( "jp z, **");addAddress( "functionUI_digit" );
  a( "cp *" ); addByte( 0x13 ); // k6
  a( "jp z, **");addAddress( "functionUI_digit" );
  a( "cp *" ); addByte( 0x24 ); // k7
  a( "jp z, **");addAddress( "functionUI_digit" );
  a( "cp *" ); addByte( 0x1C ); // k8
  a( "jp z, **");addAddress( "functionUI_digit" );
  a( "cp *" ); addByte( 0x14 ); // k9
  a( "jp z, **");addAddress( "functionUI_digit" );
  a( "cp *" ); addByte( 0x19 ); // DecPt
  addLabel( "dec_check_plus_1" );
  
  a( "jp z, **");addAddress( "functionUI_digit" );
  a( "jr *" ); addOffset("functionUI_getscan");

  //===========================
  // Flags to check for - and .
  addLabel( "check_for_decimal_points" );
  pushall();
  a( "ld a, *" ); addByte( 0x19 );
  a( "ld hl, **" ); addAddress( "dec_check_plus_1" );
  a( "dec hl" );
  a( "ld (hl), a" );
  popall();
  a( "ret" );
  addLabel( "dont_check_for_decimal_points" );
  pushall();
  a( "xor a" ); addByte( 0x00 );
  a( "ld hl, **" ); addAddress( "dec_check_plus_1" );
  a( "dec hl" );
  a( "ld (hl), a" );
  popall();
  a( "ret" );
  addLabel( "check_for_negatives" );
  pushall();
  a( "ld a, *" ); addByte( 0x0B );
  a( "ld hl, **" ); addAddress( "min_check_plus_1" );
  a( "dec hl" );
  a( "ld (hl), a" );

  a( "ld a, *" ); addByte( 0x11 );
  a( "ld hl, **" ); addAddress( "neg_check_plus_1" );
  a( "dec hl" );
  a( "ld (hl), a" );
  popall();
  a( "ret" );
  
  addLabel( "dont_check_for_negatives" );
  pushall();
  a( "ld a, *" ); addByte( 0x00 );
  a( "ld hl, **" ); addAddress( "min_check_plus_1" );
  a( "dec hl" );
  a( "ld (hl), a" );

  a( "ld a, *" ); addByte( 0x00 );
  a( "ld hl, **" ); addAddress( "neg_check_plus_1" );
  a( "dec hl" );
  a( "ld (hl), a" );
  popall();
  a( "ret" );

  //=====================================================================

  // A Table to help convert from scan codes to t values
  addLabel("functionUI_lookuptable");
  addByte(0x33);
  addByte(0x36);
  addByte(0x39);
  addByte(0xB0); // negative
  addByte(0xFF);
  addByte(0xFF);
  addByte(0xFF);
  addByte(0x3A);
  addByte(0x32);
  addByte(0x35);
  addByte(0x38);
  addByte(0xFF);
  addByte(0xFF);
  addByte(0xFF);
  addByte(0xFF);
  addByte(0x30);
  addByte(0x31);
  addByte(0x34);
  addByte(0x37);

  // =========================================================
  addLabel( "functionUI_return" );

  // if the user didn't enter anything, then don't convert anything into anything!
  a( "ld a, (**)" ); addAddress( "functionUI_text_bfr_size" );
  a( "cp *" ); addByte( 0x00 );
  a( "jr z, *" ); addOffset( "functionUI_return_dontstore" );

  // now turn it into an equation
  a( "ld hl, **" ); addAddress( "equationName" );
  sysCall( "Mov9ToOP1" );
  sysCall( "FindSym" );
  a( "jr c, *" ); addOffset( "storeEqu" ); 
  sysCall( "DelVar" );

  addLabel("storeEqu" );
  a( "ld a, (**)" ); addAddress( "functionUI_text_bfr_size" );
  a("ld h, *"); addByte( 0x00 );
  a("ld l, a" );

  sysCall( "CreateEqu" );

  // NOW COPY THE DATA INTO THE VAT
  a("inc de" );
  a("inc de" );
  a( "ld hl, **"); addAddress( "functionUI_text_bfr_start" );
  a( "ld a, (**)" ); addAddress( "functionUI_text_bfr_size" );
  a( "ld b, *"); addByte( 0x00 );
  a("ld c, a" );
  a("ldir" );

  // Then turn the equation into OP1 and store it in memory as a floating point value
  a( "ld hl, **" ); addAddress( "equationName" );
  a( "ld de, **" ); addWord( _OP1 );
  a( "ld bc, **" ); addWord( 0x0004 ); // copy only 4 bytes
  a("ldir" );
  sysCall( "ParseInp" );

  // Copy OP1 to FP_user_input
  a("ld hl, **" ); addWord( _OP1 );
  a( "ld de, **" ); addAddress( "FP_user_input" );
  a( "ld bc, **" ); addWord( 0x0009 );
  a("ldir" );

  // Now delete the Variable from memory
  a( "ld hl, **" ); addAddress( "equationName" );
  sysCall( "Mov9ToOP1" );
  sysCall( "FindSym" );
  sysCall( "DelVar" );

  a( "ld hl, **" ); addAddress( "FP_user_input" );
  sysCall( "Mov9ToOP1" );

  //add_disp_op1=1;
  sysCall( "NewLine" );
  //a( "call **" ); addAddress( "disp_op1" );

  addLabel( "functionUI_return_dontstore" );

  popall();

  a( "ret" );

  addLabel("equationName");
  // EquObj, tVarEqu, tY0, 0, 0
  addByte( 0x03 ); addByte( 0x5E ); addByte( 0x05 ); addByte( 0x00 );addByte( 0x00 );addByte( 0x00 );addByte( 0x00 );addByte( 0x00 );addByte( 0x00 );
  
  addFP( "FP_user_input" );
  
  // =========================================================
  addLabel( "functionUI_storeanddisplay" );
  a( "push af" );
  a( "push bc" );
  a( "push hl" );
  
  a( "ld a, (**)" ); addAddress( "functionUI_key" );// store the key that was pressed into "a"
  a( "ld c, *" ); addByte( 0x12 );
  a( "sub c" );
  a( "ld c, a" );
  a( "ld b, *" ); addByte( 0x00 );  
  a( "ld hl, **" ); addAddress( "functionUI_lookuptable" );
  a( "add hl, bc" );
  
  a( "ld a, (hl)" );
  a( "ld hl, (**)" );  addAddress( "functionUI_text_bfr_ptr" );
  a( "ld (hl), a" );

  // check for the negative sign token
  a( "cp *" ); addByte( 0xB0 );
  a( "jr c, *" ); addOffset( "functionUI_storeanddisplay_skip1" );
  a( "ld a, *" ); addByte( '-' );
  a( "jr *" ); addOffset( "functionUI_storeanddisplay_putc" );
  // check for the decimal point token
  addLabel( "functionUI_storeanddisplay_skip1" );
  a( "cp *" ); addByte( 0x3A );
  a( "jr c, *" ); addOffset( "functionUI_storeanddisplay_putc" );
  a( "ld a, *" ); addByte( '.' );
  addLabel( "functionUI_storeanddisplay_putc" );
  sysCall( "PutC" );

  // increase the ptr
  a( "ld hl, **" ); addAddress("functionUI_text_bfr_ptr");
  a( "inc (hl)" ); 

  // increase the size
  a( "ld hl, **"); addAddress("functionUI_text_bfr_size");
  a( "ld a, (hl)" );
  a( "inc a" );
  a( "ld (hl), a");
  
  a( "pop hl" );
  a( "pop bc" );
  a( "pop af" );
  
  a( "ret" );
  //=====================================================================
  addLabel( "functionUI_delete" );
 
  // if( count == 0 ) then go back to top of loop
  a( "ld a, (**)" ); addAddress( "functionUI_text_bfr_size" );
  a( "cp *" ); addByte( 0x00 );
  a( "jp z, **" ); addAddress("functionUI_top");

  // else delete one of the bytes and dec the size
  a( "ld hl, (**)" ); addAddress( "functionUI_text_bfr_ptr" );
  
  a( "dec hl" ); // move back one element
  a( "ld (hl), *" ); addByte( 0x00 ); // Store a in the buffer
  a( "ld (**), hl" ); addAddress( "functionUI_text_bfr_ptr" );
  
  // update the length of the entered string
  a( "ld a, (**)" ); addAddress( "functionUI_text_bfr_size" );
  a( "dec a" );
  a( "ld (**), a" ); addAddress( "functionUI_text_bfr_size" );

  
  // todo erase the previously typed character on the screen
  a( "ld hl, (**)" ); addWord( _CurCol );
  a( "dec hl" );
  a( "ld (**), hl" ); addWord( _CurCol );
  a( "ld a, *" ); addByte( ' ' );
  sysCall( "PutC" );
  a( "ld hl, (**)" ); addWord( _CurCol );
  a( "dec hl" );
  a( "ld (**), hl" ); addWord( _CurCol );
  

  //sysCall( "DispHL" );

  a( "jp **" ); addAddress( "functionUI_top" );
  
  //=====================================================================
  addLabel( "functionUI_sign" );

  // zero out a
  a( "xor a" );
  // if buffersize>0 then go back to the top of the loop
  a( "ld hl, **" ); addAddress( "functionUI_text_bfr_size" );
  a( "ld b, (hl)" );
  a( "cp b" );
  a( "jp c, **" ); addAddress( "functionUI_top");

  a( "ld a, *" ); addByte( 0x15 );
  a( "ld (**), a" ); addAddress( "functionUI_key" );

  a( "call **" ); addAddress( "functionUI_storeanddisplay" );
  // todo: move the cursor
  //a( "ld a, *" ); addByte( '*' );
  //sysCall( "PutC" );

  // TO DO - move the cursor and 
  // go back to the top of the loop
  a( "jp **"); addAddress( "functionUI_top" );
 
  //=====================================================================
  addLabel( "functionUI_digit" );

  a( "call **" ); addAddress( "functionUI_storeanddisplay" );

  a( "jp **" ); addAddress( "functionUI_top" );

  
  //=====================================================================
  addLabel( "functionUI_cr" );

  
  
  a( "jp **" ); addAddress( "functionUI_return" );  
  //=====================================================================
  addLabel( "functionUI_text_bfr_size" ); addByte( 0x00 );
  addLabel( "functionUI_text_bfr_ptr" ); addWord( 0x0000 );
  addLabel( "functionUI_text_bfr_start" );
  for( int i=0; i< 11; i++ ) addByte( 0x00 );
  addLabel( "functionUI_key" ); addByte( 0x00 );


  
}
void function_disp_op1()
{
  if( disp_op1 == 1) return;
  disp_op1=1;
  addLabel( "disp_op1" );  
  pushall();
  a( "ld a, *"); addByte( 0x0A );
  sysCall( "FormReal" );
  a( "ld hl, **" ); addWord( _OP3 );
  sysCall( "PutS" );
  sysCall( "NewLine" );
  popall();
  a( "ret" );

}
void function_store_op1()
{
  if( store_op1 == 1 ) return;
  store_op1=1;
  //
  
  addLabel( "store_op1" );
  // todo push all registers
  pushall();

  a("ld hl, **"); addWord( _OP1 );
  a("ld de, **"); addAddress( "variabledata" );
  a("ld bc, **"); addWord(0x0009);
  a("ldir");
 
  a("ld hl, **" ); addAddress( "variablename");
  sysCall( "Mov9ToOP1");
  sysCall( "FindSym" );
  a("jr c, *"); addOffset("storeVAR");
  sysCall( "DelVar" );  
  addLabel("storeVAR");
  
  sysCall( "FindSym" );
  sysCall( "CreateReal" );
  
  a("ld hl, **");addAddress("variabledata");
  a("ld bc, **");addWord( 0x0009 );
  a("ldir");

  // todo pop all registers
  popall();
  a("ret");

  addLabel("variablename");
  addByte( RealObj );
  addLabel("variabletoken");
  addByte( 0x00 );addByte( 0x00 );addByte( 0x00 );
  addLabel("variabledata");
  addByte( 0x00 );addByte( 0x00 );addByte( 0x00 );addByte( 0x00 );addByte( 0x00 );addByte( 0x00 );addByte( 0x00 );addByte( 0x00 );addByte( 0x00 );
}

int stringToHexValue( string s )
{
  int e=0;
  int intg=0;
  int retVal=0;
  for( int i=s.length()-1; i>1; i-- )
    {
      switch( s[i] )
	{
	case '0':
	  intg=0;
	  break;
	case '1':
	  intg=1;
	  break;
	case '2':
	  intg=2;
	  break;
	case '3':
	  intg=3;
	  break;
	case '4':
	  intg=4;
	  break;
	case '5':
	  intg=5;
	  break;
	case '6':
	  intg=6;
	  break;
	case '7':
	  intg=7;
	  break;
	case '8':
	  intg=8;
	  break;
	case '9':
	  intg=9;
	  break;
	case 'A':
	  intg=10;
	  break;
	case 'B':
	  intg=11;
	  break;
	case 'C':
	  intg=12;
	  break;
	case 'D':
	  intg=13;
	  break;
	case 'E':
	  intg=14;
	  break;
	case 'F':
	  intg=15;
	  break;
	default:
	  break;
	}
      retVal+=intg*pow(16,e);
#ifdef DEBUG
      // cerr << "retval: " << retVal << endl; 
#endif
      e++;
    }
  return retVal;
}

   
int getType(string s)
{
  int retVal=16;
  // 0= offset
  // 1= label
  // 2= word
  // 4= byte
  // 8= character
  // 16 = unknown
  
  if( s[0] == '0' && s[1] == 'x' && s.length() == 4 ) retVal=4;
  else if( s[0] == '0' && s[1] == 'x' && s.length() == 6 ) retVal=2;
  else if( s[0] == '\'' && s[2] == '\'' && s.length()==3 ) retVal=8;
  else if( s.length() == 1 ) retVal=8;
  return retVal;
}

int stringToValue(string s)
{
  return stoi(s);
}

string removeUnwanted( string s )
{
  while( s[0] == ' ' )
    {
      s=s.substr(1,s.length() );
    }
  while( s.back() == ' ')
    {
      s.pop_back();
    }
  s.erase( remove(s.begin(),s.end(),'\t'),s.end());
  return s;
}

int main(int argc, char *argv[])
{
#ifdef DEBUG
  cerr << "Compiling " << argv[1] << " into " << argv[2] <<endl;
#endif
  if( argc < 3 )
    {
      cerr << "you must supply an input and an output filename" << endl;
      cerr << "ex: " << argv[0] << " sourcecode.asm " << " " << "program.8xp" << endl;
      exit(-1);
    }
  if( argv[1] == argv[2] )
    {
      cerr << "you must supply an input and an output filename THAT HAS A DIFFERENT NAME" << endl;
      cerr << "ex: " << argv[0] << " sourcecode.asm " << " " << "program.8xp" << endl;
      exit(-1);


    }
  addLabel( "OP1", 0x8478 );
  addLabel( "OP2", 0x8483 );
  addLabel( "OP3", 0x848E );
  addLabel( "OP4", 0x8499 );
  addLabel( "OP5", 0x84A4 );
  addLabel( "OP6", 0x84AF );
  addLabel( "CurRow", 0x844B );
  addLabel( "CurCol", 0x844C );
  
  
  ifstream file(argv[1]);  // source code

  FILE *binary = fopen(argv[2], "wb");

  addString( "**TI83F*" );
  addByte( 0x1A );
  addByte( 0x0A );
  addByte( 0x00 );
  setComment( "(C)2020 m pellegrino mkpelleg.freshell.org" );
  addByte( 0xFF ); // PROGRAM LENGTH - 57 (L)  (53) (This will get overwritten)
  addByte( 0xFF ); // PROGRAM LENGTH - 57 (H)  (54) (This will get overwritten)
  addByte( 0x0D ); 
  addByte( 0x00 );

  // PROGRAM START
  addByte( 0xFF ); // len of Machine Code + 2 bytes (L) (This will get overwritten)
  addByte( 0xFF ); // len of Machine Code + 2 bytes (H) (This will get overwritten)
  addByte( 0x06 );
  setName( "TI84PROG" ); // MUST BE 8 BYTES!!!
  addByte( 0x00 );
  addByte( 0x00 );
  addByte( 0xFF ); // len of Machine Code + 2 bytes (L)  (This will get overwritten)
  addByte( 0xFF ); // len of Machine Code + 2 bytes (H) (This will get overwritten)
  addByte( 0xFF ); // Machine Code Size (L)  (This will get overwritten)



    
  addByte( 0xFF ); // Machine Code Size (H) (This will get overwritten)

  addByte( 0xBB ); // Machine Code
  addByte( 0x6D );
  startCounting();




  
  // ================================================================================================================================================================================================


  string line;
  if (file.is_open())
    {
      while (getline(file, line))
	{
	  
	  line_number++;
	  // remove tabs and other unwanted characters
	  line=removeUnwanted(line);
	    
	  // 
	  if( line=="" || line[0]==';' ){}
	  else if( line.substr(0,5) == ".name" )
	    {
	      for( int i=0; i<8; i++ )
		{
		  name[i]=toupper(line.substr(6, line.size()-6)[i]);
		  byte_vector[i+60]=name[i];
		}
	    }
	  else if( line.back() == ':' )
	    {
	      // Then we have a label
	      line.pop_back();
	      addLabel(line);
	    }
	  else if( line[0] == '.' )
	    {
#ifdef DEBUG
	      cerr << "directive: [" << line << "]" << endl;
#endif	     
	      // Then we have a directive
	      if( line.substr(0,3)==".db" || line.substr(0,3)==".dw" )
		{
		  // turn the rest into a number or numbers
		  // .db 0x03 -> addByte( 0x03 );
		  // .db 0x03, 0x04, 0x05 -> addByte(0x03);addByte(0x04);addByte(0x05);
		  string tmp = removeUnwanted(line.substr(4, line.length() ));
#ifdef DEBUG
		  cerr << "[data: " << tmp << "]" << endl;
#endif	      
		  if( getType(tmp) == 4 ) addByte( stringToHexValue(tmp) );
		  else if( getType(tmp) == 2) addWord( stringToHexValue(tmp) );
		  else if( getType(tmp) == 8) addByte( (int) tmp[1]);
		}
	      if( line.substr(0,4) ==".str" )
		{
		  string tmp = removeUnwanted(line.substr(5, line.length() ));		  
#ifdef DEBUG
		  cerr << "[string: " << tmp << "]" << endl;
#endif	      
		  // remove quotes
		  tmp.erase( remove(tmp.begin(),tmp.end(),'\"'),tmp.end());

		  addString( tmp );
		}
	      
	       if( line.substr(0,3) ==".fp" )
		{
		  string tmp = removeUnwanted(line.substr(4, line.length() ));		  
#ifdef DEBUG
		  cerr << "[float: " << tmp << "]" << endl;
#endif	      
		  addFP( tmp );
		}
	    }
	  else if( line.substr(0,5) == "bCall" )
	    {
	      string addr=line.substr( 6, line.length() );
	      sysCall( getBetween(line) );
	    }
	  else if( line == "call &user_input" )
	    {
	      add_input=1;
	      a( "call **" ); addAddress( "user_input" );

	    }
	  else if( line == "call &store_op1" )
	    {
	      add_store_op1=1;
	      a( "call **" ); addAddress( "store_op1" );

	    }
	  else if( line == "call &disp_op1" )
	    {
	      add_disp_op1=1;
	      a( "call **" ); addAddress( "disp_op1" );
	    }

	      
	  else
	    {
	      int processed=0;

	      // & Address Label
	      // % Address Offset
	      // # Word
	      // @ Byte
	      
	      // here we have true assembler
	      // find value if there is one
	      // save the value (either a byte or a word)
	      // replace the value with a * for byte or
	      // a ** for word.
	      // that's the mneumonic
	      // then determine if the byte or word is
	      // a direct value, an offset, or an address.
	      size_t found;
	      found = line.find('&');
	      // EXTRACT ADDRESS LABEL ================
	      if (found!=string::npos)
		{
		  int k=-1; string s = line.substr(found,line.length());
		  for( int i=0; i<s.length();i++ ){if( s[i]==' ' || s[i]==')' || s[i]==','){k=i;i=s.length();}}
		  if( k!=-1 ) s=s.substr(0,k);    
		  // Now replace the text with ** replace(9,5,str2);
		  line.replace( found, s.length(), string("**") );
#ifdef DEBUG
		  cerr << endl;
		  cerr << "line of code: " << line << "\taddress: " << s << endl;
		  cerr << endl;
#endif
		  a( line ); processed=1;	  
		  addAddress( s.substr(1,s.length()) );
		  
		}

	      found = line.find('%');
	      if (found!=string::npos)
		{
		  int k=-1; string s = line.substr(found,line.length());
		  for( int i=0; i<s.length();i++ ){if( s[i]==' ' || s[i]==')' || s[i]==','){k=i;i=s.length();}}
		  if( k!=-1 ) s=s.substr(0,k);    
		  // Now replace the text with ** replace(9,5,str2);
		  line.replace( found, s.length(), string("*") );
#ifdef DEBUG
		  cerr << "line of code: " << line << "\toffset: " << s << endl;
#endif
		  a( line );  processed=1;	  
		  addOffset( s.substr(1,s.length() ));
		}
	      found = line.find('#');
	      if (found!=string::npos)
		{
		  int k=-1; string s = line.substr(found,line.length());
		  for( int i=0; i<s.length();i++ ){if( s[i]==' ' || s[i]==')' || s[i]==','){k=i;i=s.length();}}
		  if( k!=-1 ) s=s.substr(0,k);    
		  // Now replace the text with ** replace(9,5,str2);
		  line.replace( found, s.length(), string("**") );
#ifdef DEBUG
		  cerr << "line of code: " << line << "\tWord: " << s << endl;
#endif
		  a( line );  processed=1;

		  
		  addWord( stringToHexValue(s.substr(1,s.length())) );
		}

	      found = line.find('@');
	      if (found!=string::npos)
		{
		  int k=-1; string s = line.substr(found,line.length());
		  for( int i=0; i<s.length();i++ ){if( s[i]==' ' || s[i]==')' || s[i]==','){k=i;i=s.length();}}
		  if( k!=-1 ) s=s.substr(0,k);    
		  // Now replace the text with ** replace(9,5,str2);
		  line.replace( found, s.length(), string("*") );
#ifdef DEBUG
		  cerr << "FOUND @ : line of code: " << line << "\tByte: " << dec << stringToHexValue(s) << endl;
#endif
		  a( line );  processed=1;
		  if( getType(s) == 4 )
		    {
		      // *****
		      addByte( stringToHexValue(s));
		    }
		  else
		    {
		      addByte(stringToHexValue(s));
		      //addByte( (int) s.substr(1,s.length())[0] );
		    }
		}

	      if( !processed ) a( line );
	      
	    }
	  
	}

      file.close();
    }

  // ================================================================================================================================================================================================
      if( add_input==1 ) function_user_input();
      if( add_store_op1==1 ) function_store_op1();
      if( add_disp_op1==1 ) function_disp_op1();



  
  // redo labels
  for( int i=0; i<label_vector.size(); i++ )label_vector[i].process();

  // process offsets
  for( int i=0; i<offset_vector.size(); i++ )offset_vector[i].process();
  
  // Set Program Size
  byte_vector[53]=((byte_vector.size()-55) & 0xFF);
  byte_vector[54]=((byte_vector.size()-55) & 0xFF00)/0xFF;

  // Set Machine Code Sizes
  byte_vector[57]=byte_vector[70]=((byte_vector.size()-72) & 0xFF);
  byte_vector[58]=byte_vector[70]=((byte_vector.size()-72) & 0xFF00)/0xFF;
  // Set Just the Code Size
  byte_vector[72]=((byte_vector.size()-74) & 0xFF);
  byte_vector[73]=((byte_vector.size()-74) & 0xFF00)/0xFF;
  
  // calculate the checksum
  int cs=0;
  for( int i=55; i<byte_vector.size(); i++ ) cs+=byte_vector[i];

  addByte( cs&0xFF );
  addByte( (cs & 0xFF00)/0xFF );

  if( compilation_failed )
    {
      cerr << "Error Count: " << dec << error_count << endl;
      fclose(binary);
      exit(-1);
    }



  // this is where is does the file write
    
  for( int i=0; i<byte_vector.size(); i++ )
    {
      unsigned char byte_to_write = byte_vector[i];
      fwrite(&byte_to_write, 1, sizeof(byte_to_write), binary);
    }

  cerr << endl << "*** Compilation success ***" << endl << endl;
  cerr << "    Filename: " << argv[1] << endl;
  cerr << "     TI Name: " << name << endl;    
  cerr << "        Size: " << dec << byte_vector.size() << " bytes" << endl << endl;
  fclose( binary );
  return 0;
}
