#include "stdafx.h"
#include "DfdInfo.h"
#include "BatStream.h"
#include <stdlib.h>

#ifdef __linux__
#include <string.h>
#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL
#define strncpy_s(strDest,numberOfElements,strSource,kount) (strncpy((strDest),(strSource),(kount)))
#endif

BatStream::BatStream(const batField* ele)
{
	DfdElement    = ele;
	int elSize = FieldTypeSize(ele->fieldType);
	elementSize = 4;
	if( elSize == 1 || elSize == 2 || elSize == 4 || elSize == 8 ) elementSize = elSize;
	bufSize = elementSize * 8192;
	buffer = new unsigned char[bufSize];
	index = 0;
	lastIndex = 0;
}
BatStream::~BatStream()
{
	if( buffer != NULL ) delete []buffer;
	buffer = NULL;
	bufSize = 0;
	index = 0;
}
void BatStream::Add(unsigned char* data)
{
	if( (index + elementSize) >= bufSize ) GrowBuffer();
	memcpy(&buffer[index],data,elementSize);
	lastIndex = index;
	index = index + elementSize;
}
void BatStream::AddString(const char* str)
{
	if( elementSize != 1 ) return;
	if( str != NULL && str[0] != 0 )
	{
		int len = strlen(str);
		if( (index + len + 4) >= bufSize ) GrowBuffer();
		memcpy(&buffer[index],str,len);
		buffer[index+len+0] = 0; // this 0 will replaced by '\n' during disk writing
		buffer[index+len+1] = 0;
		lastIndex = index;
		index = index + len + 1;
		return;
	}
	if( (index + 4) >= bufSize ) GrowBuffer();
	buffer[index+0] = 0; //(unsigned char)0x80;
	buffer[index+1] = 0; // this 0 will replaced by '\n' during disk writing
	buffer[index+2] = 0;
	lastIndex = index;
	index = index + 2;
}
ParamValue BatStream::GetLast()
{
	ParamValue res;
	if( DfdElement->fieldType == dfdType::String ) res.valueStr = (const char *)&buffer[lastIndex];
	else
	{  res.value64 = 0;
		switch(elementSize)
		{
		case 1: res.value8=buffer[lastIndex];break;
		case 2: memcpy(&res.value16,&buffer[lastIndex],2);break;
		case 4: memcpy(&res.value32,&buffer[lastIndex],4);break;
		case 8: memcpy(&res.value64,&buffer[lastIndex],8);break;
		}
	}
	return res;
}
void BatStream::empty()
{
	ParamValue val;
	if( DfdElement->fieldType != dfdType::String )
	{
		switch(elementSize)
		{
		case 1: buffer[lastIndex]=MINU8; break;
		case 2: val.value16=MINU16; memcpy(&buffer[lastIndex],&val.value16,2); break;
		case 4: val.value32=MINU32; memcpy(&buffer[lastIndex],&val.value32,4); break;
		case 8: val.value64=MINU64; memcpy(&buffer[lastIndex],&val.value64,8); break;
		}
		return;
	}
	buffer[lastIndex+0] = 0; //(unsigned char)0x80;
	buffer[lastIndex+1] = 0; // this 0 will replaced by '\n' during disk writing
	buffer[lastIndex+2] = 0;
	index = lastIndex + 2;
}
void BatStream::AddEmpty()
{
	if( (index + 12) >= bufSize ) GrowBuffer();

	lastIndex = index;
	empty();
	if( DfdElement->fieldType != dfdType::String ) 
		index = index + elementSize;
}
void BatStream::SetLast(ParamValue val)
{
	if( DfdElement->fieldType != dfdType::String )
	{
		switch(elementSize)
		{
		case 1: buffer[lastIndex]=val.value8; break;
		case 2: memcpy(&buffer[lastIndex],&val.value16,2); break;
		case 4: memcpy(&buffer[lastIndex],&val.value32,4); break;
		case 8: memcpy(&buffer[lastIndex],&val.value64,8); break;
		}
		return;
	}

	if( val.valueStr != NULL && val.valueStr[0] != 0 )
	{
		int n = strlen(val.valueStr);
		if( (lastIndex + n + 4 ) >= bufSize ) GrowBuffer();
		memcpy(&buffer[lastIndex],val.valueStr,n);
		buffer[lastIndex+n+0] = 0;// this 0 will replaced by '\n' during disk writing
		buffer[lastIndex+n+1] = 0;
		index = lastIndex + n + 1;
	}
	else
	{
		buffer[lastIndex+0] = 0; //(unsigned char)0x80;
		buffer[lastIndex+1] = 0; // this 0 will replaced by '\n' during disk writing
		buffer[lastIndex+2] = 0;
		index = lastIndex + 2;
	}
}
long BatStream::SaveStream(const char* filename)
{
	FILE* fp = NULL;
	fopen_s(&fp,filename,"wb");
	if( fp == NULL ) return 0;
	//---------------------------------
	if( DfdElement->fieldType == dfdType::String )
	for(int i=index-1;i>=0;i--)
	{
		if( buffer[i]==0 && buffer[i-1]==0 && i>0) 
		{
			buffer[i-1]=(unsigned char)0x80;
			buffer[i+0]='\n';
		}
		else if( buffer[i]==0 ) buffer[i]='\n';
	}
	//---------------------------------
	fwrite(buffer,index,1,fp);
	long n = ftell(fp);
	//fflush(fp);
	fclose(fp);
	return n;
}

void BatStream::GrowBuffer()
{
	int n = bufSize + bufSize ;
	unsigned char* tmp = new unsigned char[n];
	if( tmp == NULL )  { fprintf(stderr,"##memory allocation error##"); exit(1); }
	memcpy(tmp,buffer,bufSize);
	delete []buffer;
	buffer = tmp;
	bufSize = n;
}

#define MaxLocalString 255
LocalData::LocalData(const localField* ele)
{
	DfdElement    = ele;
	int elSize = FieldTypeSize(ele->fieldType);
	elementSize = 4;
	if( elSize == 1 || elSize == 2 || elSize == 4 || elSize == 8 ) elementSize = elSize;

	value.value64 = MINU64;
	valueStr      = NULL;
	if( ele->fieldType == dfdType::String )
	{
		int n = ele->maxLength;
		if( n <= 0 || n > MaxLocalString ) n = MaxLocalString;
		valueStr = new char[n+1];
		valueStr[0] = 0; 
	}
}
LocalData::~LocalData() 
{
	if( valueStr != NULL )  delete []valueStr;
}
void LocalData::reset(const char* NamesHeap)
{
	if( DfdElement->fieldType == dfdType::String )
	{
		valueStr[0] = 0; 
		if( NamesHeap != NULL && DfdElement->defaultValue > 0 && DfdElement->defaultValue != (MINU64) )
		{
			const char *defStr = &NamesHeap[DfdElement->defaultValue];
			if( defStr != NULL && defStr[0] != 0 )
			{
				int n = DfdElement->maxLength;
				if( n <= 0 || n > MaxLocalString ) n = MaxLocalString;
				strncpy_s(valueStr,n+1,defStr,n);
			}
		}
	}
	else 
	{   
		value.value64 = 0;
		switch(elementSize)
		{
			case 1: value.value8  = (__int8)DfdElement->defaultValue; break;
			case 2: value.value16 = (__int16)DfdElement->defaultValue; break;
			case 4: value.value32 = (__int32)DfdElement->defaultValue; break;
			case 8: value.value64 = (__int64)DfdElement->defaultValue; break;
		}
	}
}
ParamValue LocalData::Get()
{
	ParamValue res;
	if( DfdElement->fieldType == dfdType::String ) res.valueStr = valueStr;
	else res.value64 = value.value64;
	return res;
}
void LocalData::empty()
{
	if( DfdElement->fieldType != dfdType::String ) 
	{
		value.value64 = 0;
		switch(elementSize)
		{
		case 1: value.value8  = MINU8;  break;
		case 2: value.value16 = MINU16; break;
		case 4: value.value32 = MINU32; break;
		case 8: value.value64 = MINU64; break;
		}
		return;
	}
	valueStr[0] = 0; 
}
void LocalData::Set(ParamValue val)
{
	if( DfdElement->fieldType != dfdType::String ) 
	{
		value.value64 = 0;
		switch(elementSize)
		{
		case 1: value.value8  = val.value8;  break;
		case 2: value.value16 = val.value16; break;
		case 4: value.value32 = val.value32; break;
		case 8: value.value64 = val.value64; break;
		}
		return;
	}
	valueStr[0] = 0; 
	if( val.valueStr != NULL && val.valueStr[0] != 0 )
	{
		int n = DfdElement->maxLength;
		if( n <= 0 || n > MaxLocalString ) n = MaxLocalString;
		strncpy_s(valueStr,n+1,val.valueStr,n);
	}
}

void LocalData::Add(unsigned char* data)
{
	ParamValue* ptr = (ParamValue*)data;
	Set( ptr[0] );	
}
void LocalData::AddString(const char* str)
{
	ParamValue val;
	val.valueStr = str;
	Set(val);
}

void BatWrap::Add(unsigned char* data)
{
	ParamValue* ptr = (ParamValue*)data;
	batStream->SetLast( ptr[0] );	
}
void BatWrap::AddString(const char* str)
{
	ParamValue val;
	val.valueStr = str;
	batStream->SetLast(val);
}

ParamValue GetParamValue(param* p,txdContext*  ctx);
void MultiParams::InitParams(subroutineV* subInfo,txdContext*  ctx)
{
	int i,j;
	for(i=0,j=0; i<(pout_count + pin_count) && j<pin_count; i++)
	if( subInfo->args[i].direction == ParamDir::input   ) 
	{
		args[j + pin]   =  &subInfo->args[i];
		values[j + pin] =  GetParamValue(&subInfo->args[i],ctx);
		j++;
	}
	for(i=0,j=0; i<(pout_count + pin_count) && j<pout_count; i++)
	if( subInfo->args[i].direction == ParamDir::output   ) 
	{
		args[j + pout]   =  &subInfo->args[i];
		values[j + pout] =  GetParamValue(&subInfo->args[i],ctx);
		j++;
	}
}

PtrArray::PtrArray(int len)
{
	if( len < 0 ) len = 0;
	count = len;
	Array = new void* [len+1];
	for(int i=0;i<=len;i++) Array[i]=NULL;
}

PtrArray::~PtrArray()
{
	for(int i=0;i<=count;i++) Array[i]=NULL;
	delete []Array;
}

