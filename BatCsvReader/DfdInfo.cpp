#include "stdafx.h"
#include "DfdInfo.h"
#include <string.h>
#include <stdlib.h>

#ifdef __linux__
//#include <string.h>
#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL
#define sprintf_s snprintf
#define _itoa_s(zVal,zBuff,zMaxChar,zBase) (snprintf((zBuff),(zMaxChar),"%ld",(zVal)))
#define _i64toa_s(zVal,zBuff,zMaxChar,zBase) (snprintf((zBuff),(zMaxChar),"%lld",(zVal)))

//printf("%" PRId64 "\n", my_int);
//#include <inttypes.h>
//void z_i64toa_s(int64_t zVal,char * zBuff,int zMaxChar,int zBase)
//{  snprintf(zBuff,zMaxChar,"%" PRId64 " ",zVal); } 
#endif

union _LogicSignature LogicSignature = { {'C','s','v','L','o','g','i','c'} };

#ifndef _MANAGED
void DfdInfo::ResetFields(void)
{
	inputFields		 = NULL;
	extendFields	 = NULL;
	localFields		 = NULL;
	outputFields	 = NULL;
	nameHeap		 = NULL;
	codeHeap		 = NULL;
	memset(&Header,0,sizeof(Header));
}

DfdInfo::DfdInfo(void)
{
	ResetFields();
	buffer = NULL;
}


DfdInfo::~DfdInfo(void)
{
	if( buffer != NULL ) delete []buffer;
	buffer = NULL;
}

int DfdInfo::Load(const char* filename)
{
	FILE* fp = NULL;
	fopen_s(&fp,filename,"rb");
	if( fp == NULL ) return 10;

	if( buffer != NULL ) delete []buffer;
	buffer = NULL;
	ResetFields();
	fread(&Header,sizeof(Header),1,fp);

	if( Header.signature != LogicSignature.signature  || 
		Header.version != 0x01010101 || 
		Header.headerlength != sizeof(Header) )
	{	
		fclose(fp); 
		ResetFields();
		return 11; 	
	}

	int bufSize = 0;
	bufSize += Header.inputCount  * sizeof(batField);
	bufSize += Header.extendCount * sizeof(batField);
	bufSize += Header.localCount  * sizeof(localField);
	bufSize += Header.outputCount * sizeof(__int32);
	bufSize += Header.heapLength;
	bufSize += Header.codeLength;

	buffer = new unsigned char[bufSize + 4];    
	memset(buffer,0,bufSize);
	fread(buffer,bufSize,1,fp);
	fclose(fp);

	inputFields  = (batField*)  (&buffer[ Header.inputIndex  - Header.headerlength ]);
	extendFields = (batField*)  (&buffer[ Header.extendIndex - Header.headerlength ]);
	localFields  = (localField*)(&buffer[ Header.localIndex  - Header.headerlength ]);
	outputFields = (__int32*)   (&buffer[ Header.outputIndex - Header.headerlength ]);
	nameHeap	 = (char*)      (&buffer[ Header.heapIndex   - Header.headerlength ]);
	codeHeap	 =				 &buffer[ Header.codeIndex   - Header.headerlength ];

	return 0;
}
const batField* DfdInfo::inputField(int index)
{ 
	if( Header.signature != LogicSignature.signature ) return NULL; 
	if( index >= 0 && index < Header.inputCount )		
		return &inputFields[index];
	return NULL;
}
const batField* DfdInfo::extendField(int index)
{ 
	if( Header.signature != LogicSignature.signature ) return NULL; 
	if( index >= 0 && index < Header.extendCount )		
		return &extendFields[index];
	return NULL;
}
const localField* DfdInfo::localVariable(int index)
{
	if( Header.signature != LogicSignature.signature ) return NULL; 
	if( index >= 0 && index < Header.localCount )		
		return &localFields[index];
	return NULL;
}

__int32 DfdInfo::outputField(int index)
{
	if( Header.signature != LogicSignature.signature ) return -1; 
	if( index >= 0 && index < Header.outputCount )		
		return outputFields[index];
	return -1;
}

__int32 DfdInfo::inputNameIndex(int index)
{
	if( Header.signature != LogicSignature.signature ) return -1; 
	if( index >= 0 && index < Header.inputCount )		
		return inputFields[index].fieldName;
	return -1;
}
__int32 DfdInfo::extendNameIndex(int index)
{
	if( Header.signature != LogicSignature.signature ) return -1; 
	if( index >= 0 && index < Header.extendCount )		
		return extendFields[index].fieldName;
	return -1;
}
#endif


DfdTypeInfo dfdTypeInfo[] = {
	{dfdType::Char,				1,L"Char",			L"TINYINT"},
	{dfdType::TinyInt,			1,L"TinyInt",		L"TINYINT"},
	{dfdType::SmallInt,			2,L"SmallInt",		L"SMALLINT"},
	{dfdType::Int,				4,L"Int",			L"INT"},
	{dfdType::IntIP,			8,L"IntIP",			L"INET"},
	{dfdType::Long,				8,L"Long",			L"BIGINT"},
	{dfdType::Double,			8,L"Double",		L"DECIMAL"},
	{dfdType::TimeStampODBC,	8,L"TimeStampODBC",	L"TIMESTAMP"},
	{dfdType::String,			1,L"String",		L"VARCHAR"}
};

int SysCallVoid_Nop(void* a,void* b) { return 0; }
SysCallInfo sysCallInfo[] = {
	{Syscall::nopFunction,				0,L"nopFunction",			SysCallVoid_Nop},
	{Syscall::equal,					1,L"equal",					SysCallVoid_Nop},
	{Syscall::notEqual,					1,L"notEqual",				SysCallVoid_Nop},
	{Syscall::lessEqual,				1,L"lessEqual",				SysCallVoid_Nop},
	{Syscall::greatEqual,				1,L"greatEqual",			SysCallVoid_Nop},
	{Syscall::lessThan,					1,L"lessThan",				SysCallVoid_Nop},
	{Syscall::greaterThan,				1,L"greaterThan",			SysCallVoid_Nop},
	{Syscall::AssignValue,				2,L"AssignValue",			SysCallVoid_Nop},
	{Syscall::IPv6ToLongLong,			2,L"IPv6ToLongLong",		SysCallVoid_Nop},
	{Syscall::FileLineID,				2,L"FileLineID",			SysCallVoid_Nop},
	{Syscall::FileExtension,			2,L"FileExtension",			SysCallVoid_Nop},
	{Syscall::StringLength,				3,L"StringLength",			SysCallVoid_Nop},
	{Syscall::StringSlice,				3,L"StringSlice",			SysCallVoid_Nop},
	{Syscall::maxFunction,				0,L"maxFunction",			SysCallVoid_Nop},
};

#ifdef _MANAGED
__int32 GetDfdType(System::String^ name)
{
	static int len = sizeof(dfdTypeInfo) / sizeof(DfdTypeInfo);

	int i,n = name->Length;
	wchar_t* tmp = new wchar_t[n+1];
	for(i=0;i<n;i++) tmp[i] = name[i];
	tmp[n]=0;

	for(int i=0;i<len;i++)
	if( _wcsicmp(tmp,dfdTypeInfo[i].wName) == 0 )
	{
		delete []tmp;
		return dfdTypeInfo[i].type;
	}
	delete []tmp;
	return dfdType::nonType;
}
const wchar_t* GetDfdDBName(__int32 type)
{
	static int len = sizeof(dfdTypeInfo) / sizeof(DfdTypeInfo);
	for(int i=0;i<len;i++)
		if( dfdTypeInfo[i].type == type ) 
			return dfdTypeInfo[i].wDbName;
	return NULL;
}
__int32 GetFunctionID(System::String^ name)
{
	static int len = sizeof(sysCallInfo) / sizeof(SysCallInfo);

	int i,n = name->Length;
	wchar_t* tmp = new wchar_t[n+1];
	for(i=0;i<n;i++) tmp[i] = name[i];
	tmp[n]=0;

	for(int i=0;i<len;i++)
	if( _wcsicmp(tmp,sysCallInfo[i].wName) == 0 )
	{
		delete []tmp;
		return sysCallInfo[i].functionID;
	}
	delete []tmp;
	return dfdType::nonType;
}
#endif

int FieldTypeSize(int type)
{
	static int len = sizeof(dfdTypeInfo) / sizeof(DfdTypeInfo);
	for(int i=0;i<len;i++)
		if( dfdTypeInfo[i].type == type ) 
			return dfdTypeInfo[i].size;
	return 0;
}

#ifdef _MANAGED
const char* IntIPToStr(__int64 value64,char* buff)
{
	//if( value64 == MINU64 ) 
	return "0.0.0.0";
	//const unsigned char* ptr = (const unsigned char*)&value64;
	//sprintf_s(buff,MaxIntStr,"%d.%d.%d.%d",ptr[3],ptr[2],ptr[1],ptr[0]);
	//return (const char*)buff;
}
#else
const char* IntIPToStr(__int64 value64,char* buff)
{
	if( value64 == MINU64 ) return "0.0.0.0";
	const unsigned char* ptr = (const unsigned char*)&value64;
	sprintf_s(buff,MaxIntStr,"%d.%d.%d.%d",ptr[3],ptr[2],ptr[1],ptr[0]);
	return (const char*)buff;
}
#endif

const char* IntToStr(ParamValue val,int type,char* buff)
{
	buff[0]=0;
	switch(type)
	{
	case dfdType::TinyInt:
	case dfdType::Char:		_itoa_s(val.value8,buff,MaxIntStr,16); break;
	
	case dfdType::SmallInt: _itoa_s(val.value16,buff,MaxIntStr,16); break;
	case dfdType::Int:		_itoa_s(val.value32,buff,MaxIntStr,16); break;

	case dfdType::Double:
	case dfdType::TimeStampODBC:
	case dfdType::Long:		_i64toa_s(val.value64,buff,MaxIntStr,16); break;

	case dfdType::IntIP:	return IntIPToStr(val.value64,buff);
	}
	return (const char*)buff;
}


__int64 ParamNumberToI64(int type,ParamValue val)
{
	switch(type)
	{
	case dfdType::TinyInt:
	case dfdType::Char:		return val.value8;
	case dfdType::SmallInt: return val.value16;
	case dfdType::Int:		return val.value32;

	case dfdType::IntIP:	
	case dfdType::Double:
	case dfdType::TimeStampODBC: 
	case dfdType::Long:		return val.value64;
	}
	return val.value64;
}

ParamValue I64ToParamNumber(int type,__int64 num)
{
	ParamValue val;
	val.value64=0;
	switch(type)
	{
		case dfdType::TinyInt:
		case dfdType::Char:		val.value8  = static_cast<__int8>(num);  break;
		case dfdType::SmallInt: val.value16 = static_cast<__int16>(num); break;
		case dfdType::Int:		val.value32 = static_cast<__int32>(num); break;
		case dfdType::IntIP:	
		case dfdType::Double:
		case dfdType::TimeStampODBC: 
		case dfdType::Long:		
		default:				val.value64 = num; break;
	}
	return val;	
}
