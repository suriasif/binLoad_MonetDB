#pragma once

System::String^ GetAttrib (System::Xml::XmlNode ^nd, System::String^ name);


class NameHeap
{
public:
	NameHeap() 
	{ 
		Index=0;
		Size = 8192;
		Heap = new char[Size];
		wHeap = new wchar_t[Size];
	}
	~NameHeap()
	{
		if( Heap != NULL ) delete []Heap;
		if( wHeap != NULL ) delete []wHeap;
	}
	int Add(System::String^ name)
	{
		int n = name->Length;
		GrowBuffer(n);
		n = Index;
		Index = Index + CopyString(&Heap[Index],&wHeap[Index],name);
		return n;
	}
	int Add(const wchar_t* str)
	{
		int n = wcslen(str);
		GrowBuffer(n);
		int lstIndex = Index;
		Index = Index + CopyString(&Heap[Index],&wHeap[Index],str,n);
		return lstIndex;
	}
	void StepBack() { if(Index>0) Index--; }
	const char* GetHeap() { return Heap; }
	const wchar_t* wGetHeap() { return wHeap; }
private:
	char*	 Heap;
	wchar_t* wHeap;
	int   Size;
	int   Index;
private:
	int CopyString(char* dest,wchar_t* wdest,System::String^ str);
	int CopyString(char* dest,wchar_t* wdest,const wchar_t* src,int srcLen);
	void GrowBuffer(int addedLen)
	{
		if( (addedLen + Index + 10) > Size )
		{
			char* tmp = new char[Size+8192];
			memcpy(tmp,Heap,Size);
			delete []Heap;
			Heap = tmp;
			//------------------------------
			wchar_t* wtmp = new wchar_t[Size+8192];
			memcpy(wtmp,wHeap,Size*sizeof(wchar_t));
			delete []wHeap;
			wHeap = wtmp;
			//------------------------------
			Size = Size + 8192;
		}
	}
};


class CodeHeap
{
public:
	CodeHeap() 
	{ 
		Index=0;
		Size = 8192;
		Heap = new char[Size];
	}
	~CodeHeap()
	{
		if( Heap != NULL ) delete []Heap;
	}
	int Add(subroutineV*  pSub);
	int Add(subroutine*   pSub, param*  parr,int count);
	const char* GetHeap() { return Heap; }
	int         GetUsedLength() { return Index; }
private:
	char* Heap;
	int   Size;
	int   Index;
};