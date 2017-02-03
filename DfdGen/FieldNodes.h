#pragma once



class InputNode
{
public:
	InputNode(XmlDocument ^doc,void *pHeap);
	~InputNode();

	__int32	fieldSep;
	__int32	recordSep;
	__int32 fieldCount;

	void*  GetElements() { return (void*)Elements; }
	void   Save(FILE* fp) { fwrite(Elements,fieldCount * sizeof(batField),1,fp); }
private:
	batField* Elements;
private:
	InputNode(){}
	InputNode(const InputNode&){}
};

class ExtendNode
{
public:
	ExtendNode(XmlDocument ^doc,void *pHeap);
	~ExtendNode();

	__int32 fieldCount;
	void*  GetElements() { return (void*)Elements; }
	void   Save(FILE* fp) { fwrite(Elements,fieldCount * sizeof(batField),1,fp); }

private:
	batField* Elements;
private:
	ExtendNode(){}
	ExtendNode(const ExtendNode&){}
};

class LocalNode
{
public:
	LocalNode(XmlDocument ^doc,void *pHeap);
	~LocalNode();

	__int32 fieldCount;
	void*  GetElements() { return (void*)Elements; }
	void   Save(FILE* fp) { fwrite(Elements,fieldCount * sizeof(localField),1,fp); }

private:
	localField* Elements;
private:
	LocalNode(){}
	LocalNode(const LocalNode&){}
};

typedef struct _NodeBundle
{
	InputNode*	inputNode;
	ExtendNode*	extendNode;
	LocalNode*	localNode;
	void *		pHeap;
} NodeBundle, *pNodeBundle;
__int32 FindBat(int nodeType,System::String^ name,pNodeBundle nodeBundle);


class OutputNode
{
public:
	OutputNode(XmlDocument ^doc,pNodeBundle nodeBundle);
	~OutputNode();

	__int32 fieldCount;
	//void*  GetElements() { return (void*)Elements; }
	void   Save(FILE* fp) { fwrite(Elements,fieldCount * sizeof(__int32),1,fp); }
	void   SaveSchema(System::String^ outName);
	void   SaveLoadCmd(System::String^ outName);

private:
	__int32* Elements;
	NameHeap Schema;
	NameHeap LoadCmd;
	__int32  SchemaLength;
	__int32  LoadCmdLength;
private:
	void EmmitElementSchema(batField* bat,__int32 diff);
	OutputNode(){}
	OutputNode(const OutputNode&){}
};


class CodeNode
{
public:
	CodeNode(XmlDocument ^doc,pNodeBundle nodeBundle);
	~CodeNode(){}
	int GetUsedLength() { return codeHeap.GetUsedLength(); }
	const char* GetHeap() { return codeHeap.GetHeap(); }

private:
	CodeHeap codeHeap;
	NodeBundle nodebundle;

private:
	CodeNode(){}
	CodeNode(const CodeNode&){}
	int ParseCondition(System::String^ cond,subroutineV* pFunc);
};

