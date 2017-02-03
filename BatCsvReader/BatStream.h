#pragma once

class BatField
{
public:
	BatField(){}
	virtual ~BatField(){}
	virtual void Add(unsigned char* data)=0;
	virtual void AddString(const char* data)=0;
	virtual int  IsNullable()=0;
	virtual int  fieldScale()=0;
	virtual int  fieldType()=0;
	virtual void* nodeField()=0;
};

class BatStream : public  BatField
{
public:
	BatStream(const batField* ele);
	~BatStream();
	void Add      (unsigned char* data);
	void AddString(const char* str);
	long SaveStream(const char* filename);
	ParamValue	GetLast();
	void		SetLast(ParamValue val);
	void		empty();
	void        AddEmpty();

	const batField*			DfdElement;	
	int  IsNullable() { return DfdElement->fieldNullable; }
	int  fieldScale() { return DfdElement->fieldScale; }
	int  fieldType()  { return DfdElement->fieldType; }
	void* nodeField() { return (void*)DfdElement; }
private:
	int				elementSize;
	int             index;
	int				lastIndex;
	int             bufSize;
	unsigned char*	buffer;
	
	void GrowBuffer();
	BatStream(){}
	BatStream(const BatStream&){}
};

class BatWrap : public  BatField
{
public:
	BatWrap(BatStream* Bat) { batStream = Bat; }
	BatWrap* operator ()(BatStream* Bat) { batStream = Bat; return this;}

	void Add      (unsigned char* data);
	void AddString(const char* str);

	int  IsNullable() { return batStream->DfdElement->fieldNullable; }
	int  fieldScale() { return batStream->DfdElement->fieldScale; }
	int  fieldType()  { return batStream->DfdElement->fieldType; }
	void* nodeField() { return (void*)(batStream->DfdElement); }
private:
	BatStream* batStream;
	BatWrap(){};
	BatWrap(const BatWrap&){}
};

class LocalData : public  BatField
{
public:
	LocalData(const localField* ele);
	~LocalData();
	void Add      (unsigned char* data);
	void AddString(const char* str);

	void	reset(const char* NamesHeap);
	void    empty();
	ParamValue	Get();
	void		Set(ParamValue val);

	const	localField*	DfdElement;	
	int  IsNullable() { return 1; }
	int  fieldScale() { return 0; }
	int  fieldType()  { return DfdElement->fieldType; }
	void* nodeField() { return (void*)DfdElement; }
private:
	int		elementSize;
	ParamValue  value;
	char*		valueStr;
	LocalData(){}
	LocalData(const LocalData&){}
};

typedef struct _txdContext
{
	int         inputCount;  
	BatStream** inputs;

	int         extendCount; 
	BatStream** extends;

	int			localCount;
	LocalData**	locals;

	int         codeLength; 
	const char* code;

	const char* names;

	const char* inputFileName;
	const char* inputFileExt;
	__uint64    currentLineNumber;
	
} txdContext;

class MultiParams
{
public:
	MultiParams(subroutineV* subInfo,txdContext*  ctx)
	{
		int n = subInfo->sub.inputCount + subInfo->sub.outputCount;
		args =   new param*[n+2];
		values = new ParamValue[n+2];
		pin  = 0;
		pout = subInfo->sub.inputCount+1;
		pin_count  = subInfo->sub.inputCount;
		pout_count = subInfo->sub.outputCount;
		InitParams(subInfo,ctx);
		//
		emptyValue.value64 = MINU64;
		emptyParam.direction = ParamDir::nonDirection;
		emptyParam.storage   = NodeTypes::invalidNode;
		emptyParam.type      = dfdType::nonType;
		emptyParam.value     = MINU64; 

	}
	~MultiParams()
	{
		if( args   != NULL ) delete []args;
		if( values != NULL ) delete []values;
	}
	param* inParam (int idx) { if(idx>=0&&idx<pin_count)  return args[pin+idx];  else return &emptyParam;} 
	param* outParam(int idx) { if(idx>=0&&idx<pout_count) return args[pout+idx]; else return &emptyParam;} 
	ParamValue inValue(int idx)  { if(idx>=0&&idx<pin_count)  return values[pin+idx];  else return emptyValue;} 
	ParamValue outValue(int idx) { if(idx>=0&&idx<pout_count) return values[pout+idx]; else return emptyValue;} 

private:
	void InitParams(subroutineV* subInfo,txdContext*  ctx);
	MultiParams(){}
	MultiParams(const MultiParams&){}

	int pin;
	int pout;
	int pin_count;
	int pout_count;

	param** args;
	ParamValue* values;

	param       emptyParam;
	ParamValue  emptyValue;
};

class PtrArray
{
public:
	PtrArray(int len);
	~PtrArray();
	void** Array;
private:
	PtrArray(){}
	PtrArray(const PtrArray&){}
	int         count;
};

