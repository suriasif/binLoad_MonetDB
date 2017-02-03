#include "StdAfx.h"

using namespace System;
using namespace System::Xml;

#include <stdio.h>
#include <string.h>
#include "..\BatCsvReader\DfdInfo.h"
#include "NameHeap.h"

#include "FieldNodes.h"

InputNode::~InputNode()		{	if( Elements != NULL ) delete []Elements; }
ExtendNode::~ExtendNode()	{	if( Elements != NULL ) delete []Elements; }
LocalNode::~LocalNode()     { 	if( Elements != NULL ) delete []Elements; }
OutputNode::~OutputNode()   {	if( Elements != NULL ) delete []Elements; }


InputNode::InputNode(XmlDocument ^doc,void *pHeap)
{
	Elements       = NULL;
	NameHeap &Heap = *((NameHeap*)pHeap);

    XmlNode ^rec = doc->SelectSingleNode(L"/logic/input");
	System::String^ tmp;

	tmp = GetAttrib(rec,L"fieldSep");
	if( tmp->Length > 0 ) fieldSep = tmp[0];
	else				  fieldSep = ',';
	
	tmp = GetAttrib(rec,L"recordSep");
	tmp = tmp->Replace("\\r","\r");
	tmp = tmp->Replace("\\n","\n");
	if( tmp->Length > 1 ) 
	{
		recordSep = tmp[1];
		recordSep = recordSep << 8;
		recordSep = recordSep | (unsigned char)tmp[0];
	}
	else if( tmp->Length == 1 ) recordSep = tmp[0];
	else						recordSep = ('\n' << 8 | '\r');

	System::Xml::XmlNodeList ^List = rec->SelectNodes(L"field");
	if( List != nullptr ) fieldCount = List->Count;
	else                  fieldCount = 0;

	Elements = new batField[ fieldCount + 1 ];
	int Index = 0;
	for each (System::Xml::XmlNode ^fld in List)
	{
		tmp = GetAttrib(fld, "name");
		Elements[Index].fieldName = Heap.Add(tmp);
			
		tmp = GetAttrib(fld, "type");
		Elements[Index].fieldType = GetDfdType(tmp);
			
		tmp = GetAttrib(fld, "nullable");
		Elements[Index].fieldNullable = 0;
		if( tmp->Compare(tmp,L"true",true) == 0 ) 
			Elements[Index].fieldNullable = 1;

		Elements[Index].fieldPrecision = 0;
		Elements[Index].fieldScale = 0;
		if( Elements[Index].fieldType == (__int32)dfdType::Double )
		{
			Elements[Index].fieldPrecision = 18;
			Elements[Index].fieldScale = 0;

			Int32 prec,scale;
			tmp = GetAttrib(fld, "precision");
			Int32::TryParse(tmp,prec);
				
			tmp = GetAttrib(fld, "scale");
			Int32::TryParse(tmp,scale);

			if( prec + scale != 18 )
			{
				Console::Error->WriteLine(L"precision + scale should be 18 for {0}",GetAttrib(fld, "name"));
				throw  ;
			}
			Elements[Index].fieldPrecision = prec;
			Elements[Index].fieldScale = scale;
		}
		else if( Elements[Index].fieldType == (__int32)dfdType::String )
		{
			Elements[Index].fieldPrecision = 100;
			tmp = GetAttrib(fld, "max");
			if( tmp->Length > 0 )
			{
				Int32 max;
				Int32::TryParse(tmp,max);
				if( max > 255 )
				{
					Console::Error->WriteLine(L"string cannot be more than 255 for {0}",GetAttrib(fld, "name"));
					throw  ;
				}
				Elements[Index].fieldPrecision = max;
			}
		}
		Index = Index + 1;
	}
}

ExtendNode::ExtendNode(XmlDocument ^doc,void *pHeap)
{
	Elements       = NULL;
	NameHeap &Heap = *((NameHeap*)pHeap);

    XmlNode ^rec = doc->SelectSingleNode(L"/logic/extented");
	System::String^ tmp;

	System::Xml::XmlNodeList ^List = rec->SelectNodes(L"field");
	if( List != nullptr ) fieldCount = List->Count;
	else                  fieldCount = 0;

	Elements = new batField[ fieldCount + 1 ];
	int Index = 0;
	for each (System::Xml::XmlNode ^fld in List)
	{
		tmp = GetAttrib(fld, "name");
		Elements[Index].fieldName = Heap.Add(tmp);
			
		tmp = GetAttrib(fld, "type");
		Elements[Index].fieldType = GetDfdType(tmp);

			
		tmp = GetAttrib(fld, "nullable");
		Elements[Index].fieldNullable = 0;
		if( tmp->Compare(tmp,L"true",true) == 0 ) 
			Elements[Index].fieldNullable = 1;

		Elements[Index].fieldPrecision = 0;
		Elements[Index].fieldScale = 0;
		if( Elements[Index].fieldType == (__int32)dfdType::Double )
		{
			Elements[Index].fieldPrecision = 18;
			Elements[Index].fieldScale = 0;

			Int32 prec,scale;
			tmp = GetAttrib(fld, "precision");
			Int32::TryParse(tmp,prec);
				
			tmp = GetAttrib(fld, "scale");
			Int32::TryParse(tmp,scale);

			if( prec + scale != 18 )
			{
				Console::Error->WriteLine(L"precision + scale should be 18 for {0}",GetAttrib(fld, "name"));
				throw  ;
			}
			Elements[Index].fieldPrecision = prec;
			Elements[Index].fieldScale = scale;
		}
		else if( Elements[Index].fieldType == (__int32)dfdType::String )
		{
			Elements[Index].fieldPrecision = 100;
			tmp = GetAttrib(fld, "max");
			if( tmp->Length > 0 )
			{
				Int32 max;
				Int32::TryParse(tmp,max);
				if( max > 255 )
				{
					Console::Error->WriteLine(L"string cannot be more than 255 for {0}",GetAttrib(fld, "name"));
					throw  ;
				}
				Elements[Index].fieldPrecision = max;
			}
		}
		Index = Index + 1;
	}
}

LocalNode::LocalNode(XmlDocument ^doc,void *pHeap)
{
	Elements       = NULL;
	NameHeap &Heap = *((NameHeap*)pHeap);

    XmlNode ^rec = doc->SelectSingleNode(L"/logic/local");
	System::String^ tmp;

	System::Xml::XmlNodeList ^List = rec->SelectNodes(L"field");
	if( List != nullptr ) fieldCount = List->Count;
	else                  fieldCount = 0;


	Elements = new localField[ fieldCount + 1 ];
	int Index = 0;
	for each (System::Xml::XmlNode ^fld in List)
	{
		tmp = GetAttrib(fld, "name");
		Elements[Index].fieldName = Heap.Add(tmp);
			
		tmp = GetAttrib(fld, "type");
		Elements[Index].fieldType = GetDfdType(tmp);
			
		Elements[Index].defaultValue = MINU64;
		tmp = GetAttrib(fld, "value");
		if( tmp->Length > 0 )
		{
			if( Elements[Index].fieldType == (__int32)dfdType::String )
				Elements[Index].defaultValue = Heap.Add(tmp);
			else
			{
				Int64 val;
				Int64::TryParse(tmp,val);
				Elements[Index].defaultValue =  I64ToParamNumber(Elements[Index].fieldType,val).value64;
			}
		}

		Elements[Index].maxLength = 0;
		if( Elements[Index].fieldType == (__int32)dfdType::String )
		{
			Elements[Index].maxLength = 100;
			tmp = GetAttrib(fld, "max");
			if( tmp->Length > 0 )
			{
				Int32 max;
				Int32::TryParse(tmp,max);
				if( max > 255 )
				{
					Console::Error->WriteLine(L"string cannot be more than 255 for {0}",GetAttrib(fld, "name"));
					throw  ;
				}
				Elements[Index].maxLength = max;
			}
		}
		Index = Index + 1;
	}
}
void OutputNode::EmmitElementSchema(batField* bat,__int32 diff)
{
  static int FldCount=0;

	Schema.Add( GetDfdDBName(bat->fieldType) );
	//Schema.StepBack();
	if( bat->fieldType == dfdType::String )
	{
		Schema.Add( L"(" + Int32(bat->fieldPrecision).ToString() +  L")" );
		Schema.StepBack();
	}
	else if( bat->fieldType == dfdType::Double )
	{
		Schema.Add( L"(" + Int32(bat->fieldPrecision).ToString() + L"," + Int32(bat->fieldScale).ToString() + L")" );
		Schema.StepBack();
	}
	if(diff > 1 ) { Schema.Add( L"," ); LoadCmd.Add( L"," ); }
	FldCount++;
	if( FldCount >= 3 ) 
	{
		Schema.Add( L"\n" );
		Schema.StepBack();
		LoadCmd.Add( L"\n" );
		LoadCmd.StepBack();
		FldCount=0;
	}

}
OutputNode::OutputNode(XmlDocument ^doc,pNodeBundle nodeBundle)
{
	fieldCount     = 0;
	SchemaLength   = 0;
	LoadCmdLength  = 0;
	Elements       = NULL;
	if( nodeBundle == NULL ) return;
	NameHeap &Heap = *((NameHeap*)nodeBundle->pHeap);
	
    XmlNode ^rec = doc->SelectSingleNode(L"/logic/output");
	System::String^ tableName = GetAttrib(rec,"tableName");
	System::String^ baseDir   = GetAttrib(rec,"baseDir");

	Schema.Add(L"CREATE TABLE " + tableName + L" (\n");
	Schema.StepBack();

	LoadCmd.Add(L"COPY BINARY INTO " + tableName + L" FROM (\n");
	LoadCmd.StepBack();

	System::Xml::XmlNodeList ^List = rec->SelectNodes(L"field");
	if( List != nullptr ) fieldCount = List->Count;
	else                  fieldCount = 0;

	System::String^ tmp;
	Elements = new __int32[ fieldCount + 1 ];
	int n, Index = 0;
	batField* Inputs  = (batField* )nodeBundle->inputNode->GetElements();
	batField* Extends = (batField* )nodeBundle->extendNode->GetElements();
	for each (System::Xml::XmlNode ^fld in List)
	{
		tmp = GetAttrib(fld, "name");
		LoadCmd.Add(L"'" + baseDir + tmp + "." + Int32(Index).ToString() + L"'" );
		LoadCmd.StepBack();
		//
		n = FindBat(NodeTypes::inputNode,tmp,nodeBundle);
		if( n >= 0 ) 
		{	
			Elements[Index] = Inputs[n].fieldName;
			Schema.Add(tmp);
			EmmitElementSchema(&Inputs[n],(fieldCount-Index));
		}
		else
		{
			n = FindBat(NodeTypes::extendNode,tmp,nodeBundle);
			if( n >= 0 )
			{ 
				Elements[Index] = Extends[n].fieldName;  
				Schema.Add(tmp);
				EmmitElementSchema(&Extends[n],(fieldCount-Index));
			}
			else
				Elements[Index] = -1;
		}
		Index = Index + 1;
	}
	Schema.Add(L");\n");
	SchemaLength = Schema.Add(L" ");

	LoadCmd.Add(L");\n");
	LoadCmdLength = LoadCmd.Add(L" ");

	char *sql = (char *)Schema.GetHeap();
	for(n=0;n<SchemaLength;n++) if( sql[n]==0 ) sql[n]=32;
	sql[SchemaLength]=0;

	sql = (char *)LoadCmd.GetHeap();
	for(n=0;n<LoadCmdLength;n++) if( sql[n]==0 ) sql[n]=32;
	sql[LoadCmdLength]=0;
}
void OutputNode::SaveSchema(System::String^ outName)
{
	if( ! outName->EndsWith(L".schema") ) outName = outName + L".schema";
	int len = outName->Length;
	char *tmp = new char[len+1];
	for(int i=0;i<len;i++) tmp[i] = (char)(outName[i]);
	tmp[len]=0;

	FILE* fp = NULL;
	fopen_s(&fp,tmp,"wb");
	delete []tmp;
	if( fp != NULL )
	{
		fwrite(Schema.GetHeap(),SchemaLength,1,fp);
		fclose(fp);
	}
}
void OutputNode::SaveLoadCmd(System::String^ outName)
{
	if( ! outName->EndsWith(L".sql") ) outName = outName + L".sql";
	int len = outName->Length;
	char *tmp = new char[len+1];
	for(int i=0;i<len;i++) tmp[i] = (char)(outName[i]);
	tmp[len]=0;

	FILE* fp = NULL;
	fopen_s(&fp,tmp,"wb");
	delete []tmp;
	if( fp != NULL )
	{
		fwrite(LoadCmd.GetHeap(),LoadCmdLength,1,fp);
		fclose(fp);
	}
}
void SetParam(param* pParam,int nodeType,__int64 val,__int32 direction)
{
	pParam->direction = direction;
	pParam->value   = val;
	pParam->storage = nodeType;
	pParam->type    = (__int32)dfdType::nonType;
	if( nodeType == NodeTypes::valueNode ) 
		pParam->type    = (__int32)dfdType::Long;
}

int CodeNode::ParseCondition(System::String^ cond,subroutineV* pFunc)
{
	System::String^ condRegex = L"(?<wsp>\\s*)(" +
    L"(?<num>[0-9]+)|" +
    L"(?<variable>[a-zA-Z_][a-zA-Z0-9_]*)|" +
    L"(?<equal>==)|" +
    L"(?<notEqual>!=)|" +
    L"(?<lessEqual>\\<=)|" +
    L"(?<greatEqual>\\>=)|" +
    L"(?<lessThan>\\<)|" +
    L"(?<greaterThan>\\>)" +
    L")";
	System::Text::RegularExpressions::Regex^ regexPattern = gcnew System::Text::RegularExpressions::Regex(condRegex);
	
	int idx =0;
	int n;

	System::Text::RegularExpressions::MatchCollection^ matches = regexPattern->Matches(cond);
	for each (System::Text::RegularExpressions::Match^ match in matches)
	{
		int j,i = 0;
		for each (System::Text::RegularExpressions::Group^ group in match->Groups)
		{
			System::String^ matchValue = group->Value;
			bool success = group->Success;
			if (success && i > 1)
			{
				System::String^ groupName = regexPattern->GroupNameFromNumber(i);
				if (groupName->Compare(groupName,"wsp")!=0)
				{
					if( idx==0 || idx==2 ) //l-Value and r-Value
					{
						if( System::String::Compare(groupName,"variable") == 0 ) //variable node
						for(j=NodeTypes::inputNode;j<=NodeTypes::localNode;j++)
						{
							n = FindBat(j,matchValue,&nodebundle);
							if( n >= 0 && idx == 0 )	  { SetParam(&pFunc->args[0],j,n,ParamDir::input);  idx++; }
							else if( n >= 0 && idx == 2 ) { SetParam(&pFunc->args[1],j,n,ParamDir::input); idx++; }
						}
						else if( System::String::Compare(groupName,"num") == 0 )  //Value node
						{
							Int64 num;
							Int64::TryParse(matchValue,num);
							if( idx == 0 )		{ SetParam(&pFunc->args[0],NodeTypes::valueNode,num,ParamDir::input);  idx++; }
							else if( idx == 2)  { SetParam(&pFunc->args[1],NodeTypes::valueNode,num,ParamDir::input);  idx++; }
						}
					}
					else if ( idx == 1 ) //operation
					{
						//pcond->operation = Syscall::nopFunction;
						pFunc->sub.functionID = GetFunctionID(groupName);
						if( pFunc->sub.functionID != Syscall::nopFunction ) idx++;
					}
				}
			}
			i++;
		}
	}
	return idx;
}

__int32 FindBat(int nodeType,System::String^ name,pNodeBundle nodeBundle)
{
	int i;
	batField* Ele;
	localField* local;
	NodeBundle& nodebundle = *nodeBundle;
	NameHeap* heap = (NameHeap*)nodebundle.pHeap;
	const wchar_t* Names = heap->wGetHeap();

	switch(nodeType)
	{
	case NodeTypes::inputNode:
		Ele = (batField*)nodebundle.inputNode->GetElements();
		for(i=0;i<nodebundle.inputNode->fieldCount;i++) 
			if( System::String::Compare(name, gcnew String(&Names[Ele[i].fieldName]) ) == 0 ) 
				return i;
		break;
	case NodeTypes::extendNode: 
		Ele = (batField*)nodebundle.extendNode->GetElements();
		for(i=0;i<nodebundle.extendNode->fieldCount;i++) 
			if( System::String::Compare(name, gcnew String(&Names[Ele[i].fieldName]) ) == 0 ) 
				return i;
		break;
	case NodeTypes::localNode:
		local = (localField*)nodebundle.localNode->GetElements();
		for(i=0;i<nodebundle.localNode->fieldCount;i++) 
			if( System::String::Compare(name, gcnew String(&Names[local[i].fieldName]) ) == 0 ) 
				return i;
		break;
	}

	return -1;
}

CodeNode::CodeNode(XmlDocument ^doc,pNodeBundle nodeBundle)
{
	if( nodeBundle == NULL ) return;
	nodebundle.inputNode  = nodeBundle->inputNode;
	nodebundle.extendNode = nodeBundle->extendNode;
	nodebundle.localNode  = nodeBundle->localNode;
	nodebundle.pHeap      = nodeBundle->pHeap;


	//codeHeap

    XmlNode ^code = doc->SelectSingleNode(L"/logic/code");
	System::String^ tmp;
	subroutineV Func;
		
	System::Xml::XmlNodeList ^List = code->SelectNodes(L"rule");
	int Index = 0;
	int n,j,dir;
	int ruleID = 1;

	for each (System::Xml::XmlNode ^rule in List)
	{
		Func.sub.ruleID = ruleID++;
		tmp = GetAttrib(rule, "condition");
		if( tmp->Length > 0 ) //contitional rule
		{
			Func.sub.inputCount = 1;
			Func.sub.outputCount = 1;
			n = ParseCondition(tmp,&Func);
			if(  n != 3  || Func.sub.functionID == (__int32)Syscall::nopFunction ) throw;
			Func.sub.execCondition = ExecCond::condAlways;
			codeHeap.Add(&Func);
		}
		for each(System::Xml::XmlNode ^Function in rule->SelectNodes(L"function"))
		{
			tmp = GetAttrib(Function, "name");
			Func.sub.functionID = GetFunctionID(tmp);
			Func.sub.execCondition = ExecCond::condAlways;
			tmp = GetAttrib(Function, "condition");
			if( tmp->Length > 0 )
			{	
				if( System::String::Compare(tmp,"true") == 0 ) Func.sub.execCondition = ExecCond::condTrue;
				else if( System::String::Compare(tmp,"false") == 0 ) Func.sub.execCondition = ExecCond::condFalse;
			}
			System::Xml::XmlNodeList ^Params = Function->SelectNodes(L"param");
			int pCount = 0;
			if( Params != nullptr ) pCount = Params->Count;
			param* p = new param[pCount+1];
			pCount = 0;
			Func.sub.inputCount = 0;
			Func.sub.outputCount = 0;
			for each(System::Xml::XmlNode ^Param in Params)
			{
				p[pCount].type = (__int32)dfdType::nonType;
				tmp = GetAttrib(Param, "storage");
				if( System::String::Compare(tmp,L"Value") == 0 )
				{
					p[pCount].storage = NodeTypes::valueNode;
					tmp = Param->InnerText;
					Int64 num;
					Int64::TryParse(tmp,num);
					p[pCount].value = num;
					tmp = GetAttrib(Param, "type");
					p[pCount].type = GetDfdType(tmp);
					p[pCount].direction = ParamDir::input;
					Func.sub.inputCount++;
				}
				else
				{
					tmp = GetAttrib(Param, "direction");
					if ( tmp[0] == L'O' || tmp[0] == L'o' ) 
					{
						dir = ParamDir::output;
						Func.sub.outputCount++;
					}
					else
					{
						dir = ParamDir::input;
						Func.sub.inputCount++;
					}

					tmp = GetAttrib(Param, "name");
					for(j=NodeTypes::inputNode;j<=NodeTypes::localNode;j++)
					{
						n = FindBat(j,tmp,&nodebundle);
						if( n >= 0 )
							SetParam(&p[pCount],j,n,dir);
					}
				}
				pCount++;
			}
			codeHeap.Add(&Func.sub,p,pCount);
			delete []p;
		}
	}
}