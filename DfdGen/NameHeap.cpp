#include "StdAfx.h"
#include <string.h>
#include "..\BatCsvReader\DfdInfo.h"
#include "NameHeap.h"

int NameHeap::CopyString(char* dest,wchar_t* wdest,System::String^ str)
{
	int n = str->Length;
	for(int i=0;i<n;i++)
	{
		dest[i] = (char)str[i];
		wdest[i] = str[i];
	}
	dest[n]=0;
	wdest[n]=0;
	return n+1;
}
int NameHeap::CopyString(char* dest,wchar_t* wdest,const wchar_t* src,int srcLen)
{
	for(int i=0;i<srcLen;i++)
	{
		dest[i] = (char)src[i];
		wdest[i] = src[i];
	}
	dest[srcLen]=0;
	wdest[srcLen]=0;
	return srcLen+1;
}

System::String^ GetAttrib(System::Xml::XmlNode ^nd, System::String^ name)
{
	System::Xml::XmlAttribute ^attrib = nd->Attributes[name];
	if( attrib == nullptr ) return L"";

	System::String^ value = attrib->Value;
	return value;
}


int CodeHeap::Add(subroutineV*  func)
{ 
	if( (func->sub.inputCount + func->sub.outputCount) > 2 ) throw L"subroutineV cannot have more than 2 params";
	if( (int)(sizeof(subroutineV) + Index + 10) > Size )
	{
		char* tmp = new char[Size+8192];
		memcpy(tmp,Heap,Size);
		delete []Heap;
		Heap = tmp;
		Size = Size + 8192;
	}
	memcpy(&Heap[Index],func,sizeof(subroutineV));
	Index = Index + sizeof(subroutineV);
	return Index - sizeof(subroutineV);
}

int CodeHeap::Add(subroutine*   pSub, param*  parr,int count)
{
	if( count <= 0 ) return Index;
	int n = (sizeof(param) * count) + sizeof(subroutine);

	if( (n + Index + 10) > Size )
	{
		char* tmp = new char[Size+8192];
		memcpy(tmp,Heap,Size);
		delete []Heap;
		Heap = tmp;
		Size = Size + 8192;
	}
	n = Index;
	memcpy(&Heap[Index],pSub,sizeof(subroutine));
	Index = Index + sizeof(subroutine);
	memcpy(&Heap[Index],parr,(sizeof(param) * count));
	Index = Index + (sizeof(param) * count);
	return n;
}