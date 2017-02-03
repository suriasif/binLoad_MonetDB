// DfdGen.cpp : main project file.
#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "..\BatCsvReader\DfdInfo.h"
#include "NameHeap.h"

using namespace System;
using namespace System::Xml;

#include "FieldNodes.h"


int main(array<System::String ^> ^args)
{
	if( args->Length < 1) 
	{
		Console::Error->WriteLine(L"Syntax Error. dfdGen dfd.xml");
		return 1;
	}

    XmlDocument ^doc = gcnew XmlDocument();
    try
    {
		logicHeader dfd;
		dfd.Init();
        dfd.signature		= LogicSignature.signature;
        dfd.version			= 0x01010101;
        dfd.headerlength	= sizeof(logicHeader);

		NameHeap Heap;

		doc->Load(args[0]);

		InputNode input(doc,&Heap);
		dfd.fieldSep	= input.fieldSep;
		dfd.recordSep	= input.recordSep;
		dfd.inputCount	= input.fieldCount;

		ExtendNode extend(doc,&Heap);
		dfd.extendCount	= extend.fieldCount;
		
		LocalNode local(doc,&Heap);
		dfd.localCount	= local.fieldCount;

		NodeBundle nodeBundle;
		nodeBundle.inputNode  = &input;
		nodeBundle.extendNode = &extend;
		nodeBundle.localNode  = &local;
		nodeBundle.pHeap      = &Heap;

		OutputNode output(doc,&nodeBundle);
		dfd.outputCount	= output.fieldCount;
		output.SaveSchema(args[0]->Replace(L".xml",L".schema"));
		output.SaveLoadCmd(args[0]->Replace(L".xml",L".sql"));

		CodeNode codenode(doc,&nodeBundle);
		dfd.codeLength = codenode.GetUsedLength();

		System::String^ outName = args[0]->Replace(L".xml",L".logic");
		if( ! outName->EndsWith(L".logic") ) outName = outName + L".logic";
		int Index = Heap.Add(outName);
		dfd.heapLength = Index;

		dfd.inputIndex  = sizeof(logicHeader);
		dfd.extendIndex = dfd.inputIndex		+ (dfd.inputCount  * sizeof(batField));
		dfd.localIndex  = dfd.extendIndex		+ (dfd.extendCount * sizeof(batField));
		dfd.outputIndex = dfd.localIndex		+ (dfd.localCount  * sizeof(localField));
		dfd.heapIndex   = dfd.outputIndex		+ (dfd.outputCount * sizeof(__int32));
		dfd.codeIndex   = dfd.heapIndex			+ dfd.heapLength;

		FILE* fp = NULL;
		fopen_s(&fp,&(Heap.GetHeap())[Index],"wb");
		if( fp != NULL )
		{
			fwrite(&dfd,sizeof(dfd),1,fp);
			input.Save(fp);
			extend.Save(fp);
			local.Save(fp);
			output.Save(fp);
			fwrite(Heap.GetHeap(),dfd.heapLength,1,fp);
			fwrite(codenode.GetHeap(),dfd.codeLength,1,fp);
			fclose(fp);
		}
	}
	catch(Exception ^e)
	{
		Console::Error->WriteLine(e->Message);
		Console::Error->WriteLine(e->StackTrace);
	}
    return 0;
}
/*
System::Collections::Hashtable^ getSyscalls()
{
	Array^ arr = Enum::GetValues( Syscall::typeid );
	System::Collections::Hashtable ^hSyscalls  = gcnew System::Collections::Hashtable(arr->Length);
	for ( Int32 i = 0; i < arr->Length; i++ )
	{
		Object^ o = arr->GetValue( i );
		hSyscalls->Add( Enum::Format( Syscall::typeid, o,  "G" ) , (__int32)o);//(__int32)Enum::Format( Syscall::typeid, o,  "D" ) );
	}
	return hSyscalls;
}
System::Collections::Hashtable^ getDfdTypes()
{
	Array^ arr = Enum::GetValues( dfdType::typeid );
	System::Collections::Hashtable ^hDfdTypes  = gcnew System::Collections::Hashtable(arr->Length);
	for ( Int32 i = 0; i < arr->Length; i++ )
	{
		Object^ o = arr->GetValue( i );
		hDfdTypes->Add( Enum::Format( dfdType::typeid, o,  "G" ) , (__int32)o);//(__int32)Enum::Format( Syscall::typeid, o,  "D" ) );
	}
	return hDfdTypes;
}
*/


/*
-----------------------
COPY BINARY INTO TEST_BIN FROM ('/opt/pigdev/data/Field_0', '/opt/pigdev/data/Field_1', '/opt/pigdev/data/Field_2', '/opt/pigdev/data/Field_3',
'/opt/pigdev/data/Field_4', '/opt/pigdev/data/Field_5', '/opt/pigdev/data/Field_6', '/opt/pigdev/data/Field_7', '/opt/pigdev/data/Field_8', 
'/opt/pigdev/data/Field_9', '/opt/pigdev/data/Field_10', '/opt/pigdev/data/Field_11' );
---------------------------------
*/
