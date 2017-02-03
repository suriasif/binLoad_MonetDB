#pragma once

#define MaxIntStr 32

typedef struct _DfdTypeInfo
{
	__int32			type;
	__int32			size;
	const wchar_t*  wName;
	const wchar_t*  wDbName;
} DfdTypeInfo;

typedef int (*pSysCallVoid)(void*,void*);
typedef struct _SysCallInfo
{
	__int32			functionID;
	__int32			type;
	const wchar_t*  wName;
	pSysCallVoid	function;
} SysCallInfo;


typedef union _ParamValue
{
	__int64     value64;
	__int32     value32;
	__int16     value16;
	__int8      value8;
	const char* valueStr;
} ParamValue;


__int64		ParamNumberToI64(int type,	ParamValue val);
ParamValue	I64ToParamNumber(int type,	__int64 value);
const char* IntToStr		(ParamValue val,int type,char* buff);

#ifdef _MANAGED
	__int32			GetDfdType(System::String^ name);
	const wchar_t*	GetDfdDBName(__int32 type);
	__int32			GetFunctionID(System::String^ name);
#endif


//#pragma pack (show)
#pragma pack (push)
#pragma pack (4)

struct dfdType   { enum Type   { nonType = 0, Char, SmallInt, TinyInt, Int, IntIP, Long, Double, String, TimeStampODBC }; }; 
struct NodeTypes { enum ndTypes { invalidNode = 0, inputNode, extendNode, localNode, valueNode,  outputNode }; }; 
struct ParamDir  { enum Dir     { nonDirection = 0, input, output }; };
struct ExecCond  { enum Cond    { condAlways = 0, condNever, condTrue, condFalse }; };
struct Syscall   { enum function
	 {  
	  nopFunction = 0,
	  equal       ,     notEqual,     lessEqual, greatEqual, lessThan,   greaterThan,
	  AssignValue ,     StringLength, StringSlice,
	  IPv6ToLongLong,   FileLineID,   FileExtension,
	  maxFunction
	 };
 };

#define MINU64 (__uint64)0x8000000000000000
#define MINU32 (__uint32)0x80000000
#define MINU16 (__uint16)0x8000
#define MINU8  (__uint8 )0x80

int	FieldTypeSize(int type);

union _LogicSignature 
{
	char	ascii[8];
	__int64 signature;
}; 
extern union _LogicSignature LogicSignature;

typedef struct _logicHeader
{
	__int64 signature;
	__int32 version;
	__int32 headerlength;
	//-------------------
	__int32 fieldSep;
	__int32 recordSep;
	//-------------------
	__int32 inputIndex;
	__int32 inputCount;
	//-------------------
	__int32 extendIndex;
	__int32 extendCount;
	//-------------------
	__int32 outputIndex;
	__int32 outputCount;
	//-------------------
	__int32 localIndex;
	__int32 localCount;
	//-------------------
	__int32 heapIndex;
	__int32 heapLength;
	//-------------------
	__int32 codeIndex;
	__int32 codeLength;
	void Init() { signature=0;version=headerlength=fieldSep=recordSep=inputIndex=inputCount=extendIndex=extendCount=0;
	outputIndex=outputCount=localIndex=localCount=heapIndex=heapLength=codeIndex=codeLength=0; }
} logicHeader;

typedef struct _batField
{
	__int32 fieldName;
	__int32 fieldType;
	__int32 fieldNullable;
	__int32 fieldPrecision;
	__int32 fieldScale;
} batField;

//output names is array of __int32 Names in heap;

typedef struct _localField
{
	__int32		fieldName;
	__int32		fieldType;
	__int32		maxLength;
	__int64		defaultValue; // char,smallint,long,double is direct, string  are index to heap
} localField;

typedef struct _param
{
	__int32 type;
	__int32 storage;
	__int32 direction;
	__int64 value;   //index to bat , or local or direct immidiate value
} param;

typedef struct _subroutine
{
	__int32 functionID;
	__int32 ruleID;
	__int32 execCondition; //true or false;
	__int32 inputCount;
	__int32 outputCount;
} subroutine;

typedef struct _subroutineV
{
	subroutine sub;
	param      args[2];
}subroutineV;

class DfdInfo
{
public:
	DfdInfo(void);
	~DfdInfo(void);
	int Load(const char* filename);
	//--------------------------------------------------------------------
	const char* FieldSep()		{ return (const char*)&Header.fieldSep;  }
	const char* RecordSep()		{ return (const char*)&Header.recordSep; }
	int         inputCount()	{ return Header.inputCount; }
	int         extendCount()	{ return Header.extendCount; }
	int         outputCount()	{ return Header.outputCount; }
	int         localCount()	{ return Header.localCount; }
	int         codeLength()	{ return Header.codeLength; }
	const char* codeBytes()		{ return (const char*)codeHeap; }
	const char* nameBytes()		{ return (const char*)nameHeap; }
	//--------------------------------------------------------------------
	//const char*			FieldName(int index);     
	const batField*		inputField(int index);
	const batField*		extendField(int index);
	const localField*	localVariable(int index);
	__int32             outputField(int index); 
	__int32             inputNameIndex(int index);
	__int32             extendNameIndex(int index);
	//
	
private:
	void ResetFields(void);

	logicHeader		Header;
	unsigned char*  buffer;

	batField*	inputFields;
	batField*	extendFields;
	localField* localFields;
	__int32*    outputFields;

	char*			nameHeap;
	unsigned char*  codeHeap;
};

//#pragma pack (show)
#pragma pack (pop)
//#pragma pack (show)




