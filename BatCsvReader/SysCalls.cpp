#include "stdafx.h"
#include <stdlib.h>
#include "DfdInfo.h"
#include "BatStream.h"

#ifdef __linux__
#include <string.h>
#endif

void DecodeField(const char*,BatField* Bat);

ParamValue GetParamValue(param* p,txdContext*  ctx)
{
	switch(p->storage)
	{
	case NodeTypes::inputNode:  
		p->type = ctx->inputs [p->value]->DfdElement->fieldType;
		return ctx->inputs[p->value]->GetLast();
	
	case NodeTypes::extendNode: 
		p->type = ctx->extends[p->value]->DfdElement->fieldType;
		return ctx->extends[p->value]->GetLast();

	case NodeTypes::localNode:	
		p->type = ctx->locals [p->value]->DfdElement->fieldType;
		return ctx->locals[p->value]->Get();

	case NodeTypes::valueNode:
		return I64ToParamNumber(p->type,p->value);
	}
	ParamValue r;
	r.value64 = 0;
	r.valueStr = NULL;
	return r;
}
void SetParamValue(param* p,txdContext*  ctx,ParamValue val)
{
	switch(p->storage)
	{
	case NodeTypes::inputNode:  ctx->inputs[p->value]->SetLast(val);  break;
	case NodeTypes::extendNode: ctx->extends[p->value]->SetLast(val); break;
	case NodeTypes::localNode:	ctx->locals[p->value]->Set(val);      break;
	}
}
void EmptyParam(param* p,txdContext*  ctx)
{
	switch(p->storage)
	{
	case NodeTypes::inputNode:  ctx->inputs[p->value]->empty();  break;
	case NodeTypes::extendNode: ctx->extends[p->value]->empty(); break;
	case NodeTypes::localNode:	ctx->locals[p->value]->empty();  break;
	}
}


int Internal_AssignValue(param* pout,param* pin,ParamValue val,txdContext*  ctx)
{	
	char tmp[MaxIntStr];
	ParamValue res;
	if( pout->type == dfdType::String && pin->type != dfdType::String )
	{
		res.valueStr = IntToStr(val,pin->type,tmp);
		//SetParamValue(pout,ctx,val);
		SetParamValue(pout,ctx,res);
	}
	else if( pout->type != dfdType::String && pin->type == dfdType::String )
	{
		BatWrap Wrap(NULL);
		switch(pout->storage)
		{
		case NodeTypes::inputNode:  DecodeField(val.valueStr,Wrap(ctx->inputs[pout->value])); break;
		case NodeTypes::extendNode: DecodeField(val.valueStr,Wrap(ctx->extends[pout->value])); break;
		case NodeTypes::localNode:  DecodeField(val.valueStr,ctx->extends[pout->value]); break;
		}
	}
	else
	{
		SetParamValue(pout,ctx,val);
	}
	return 0; 
}

int SysCall_Nop(subroutineV* subInfo,txdContext*  ctx)	{	return 0; }
int SysCall_equal(subroutineV* subInfo,txdContext*  ctx)
{
	char tmp[MaxIntStr];
	ParamValue lval = GetParamValue(&subInfo->args[0],ctx);
	ParamValue rval = GetParamValue(&subInfo->args[1],ctx);
	if( (subInfo->args[0].type == dfdType::String || subInfo->args[1].type == dfdType::String) && 
		subInfo->args[0].type != subInfo->args[1].type ) //we have mix string and non string, let upgrade non string
	{
		if( subInfo->args[0].type == dfdType::String ) return strcmp(lval.valueStr,IntToStr(rval,subInfo->args[1].type,tmp))==0?(ExecCond::condTrue):(ExecCond::condFalse);
		if( subInfo->args[1].type == dfdType::String ) return strcmp(rval.valueStr,IntToStr(lval,subInfo->args[0].type,tmp))==0?(ExecCond::condTrue):(ExecCond::condFalse);
		return ExecCond::condNever;
	}
	return (ParamNumberToI64(subInfo->args[0].type,lval) == ParamNumberToI64(subInfo->args[1].type,rval))?(ExecCond::condTrue):(ExecCond::condFalse);
}
int SysCall_notEqual(subroutineV* subInfo,txdContext*  ctx)
{	
	return !SysCall_equal(subInfo,ctx);
}
int SysCall_lessEqual(subroutineV* subInfo,txdContext*  ctx)
{	
	char tmp[MaxIntStr];
	ParamValue lval = GetParamValue(&subInfo->args[0],ctx);
	ParamValue rval = GetParamValue(&subInfo->args[1],ctx);
	if( (subInfo->args[0].type == dfdType::String || subInfo->args[1].type == dfdType::String) && 
		subInfo->args[0].type != subInfo->args[1].type ) //we have mix string and non string, let upgrade non string
	{
		if( subInfo->args[0].type == dfdType::String ) return strcmp(lval.valueStr,IntToStr(rval,subInfo->args[1].type,tmp))<=0?(ExecCond::condTrue):(ExecCond::condFalse);
		if( subInfo->args[1].type == dfdType::String ) return strcmp(rval.valueStr,IntToStr(lval,subInfo->args[0].type,tmp))<=0?(ExecCond::condTrue):(ExecCond::condFalse);
		return ExecCond::condNever;
	}
	return (ParamNumberToI64(subInfo->args[0].type,lval) <= ParamNumberToI64(subInfo->args[1].type,rval))?(ExecCond::condTrue):(ExecCond::condFalse);
}
int SysCall_greatEqual(subroutineV* subInfo,txdContext*  ctx)
{	
	char tmp[MaxIntStr];
	ParamValue lval = GetParamValue(&subInfo->args[0],ctx);
	ParamValue rval = GetParamValue(&subInfo->args[1],ctx);
	if( (subInfo->args[0].type == dfdType::String || subInfo->args[1].type == dfdType::String) && 
		subInfo->args[0].type != subInfo->args[1].type ) //we have mix string and non string, let upgrade non string
	{
		if( subInfo->args[0].type == dfdType::String ) return strcmp(lval.valueStr,IntToStr(rval,subInfo->args[1].type,tmp))>=0?(ExecCond::condTrue):(ExecCond::condFalse);
		if( subInfo->args[1].type == dfdType::String ) return strcmp(rval.valueStr,IntToStr(lval,subInfo->args[0].type,tmp))>=0?(ExecCond::condTrue):(ExecCond::condFalse);
		return ExecCond::condNever;
	}
	return (ParamNumberToI64(subInfo->args[0].type,lval) >= ParamNumberToI64(subInfo->args[1].type,rval))?(ExecCond::condTrue):(ExecCond::condFalse);
}
int SysCall_lessThan(subroutineV* subInfo,txdContext*  ctx)
{	
	char tmp[MaxIntStr];
	ParamValue lval = GetParamValue(&subInfo->args[0],ctx);
	ParamValue rval = GetParamValue(&subInfo->args[1],ctx);
	if( (subInfo->args[0].type == dfdType::String || subInfo->args[1].type == dfdType::String) && 
		subInfo->args[0].type != subInfo->args[1].type ) //we have mix string and non string, let upgrade non string
	{
		if( subInfo->args[0].type == dfdType::String ) return strcmp(lval.valueStr,IntToStr(rval,subInfo->args[1].type,tmp))<0?(ExecCond::condTrue):(ExecCond::condFalse);
		if( subInfo->args[1].type == dfdType::String ) return strcmp(rval.valueStr,IntToStr(lval,subInfo->args[0].type,tmp))<0?(ExecCond::condTrue):(ExecCond::condFalse);
		return ExecCond::condNever;
	}
	return (ParamNumberToI64(subInfo->args[0].type,lval) < ParamNumberToI64(subInfo->args[1].type,rval))?(ExecCond::condTrue):(ExecCond::condFalse);
}
int SysCall_greaterThan(subroutineV* subInfo,txdContext*  ctx)
{	
	char tmp[MaxIntStr];
	ParamValue lval = GetParamValue(&subInfo->args[0],ctx);
	ParamValue rval = GetParamValue(&subInfo->args[1],ctx);
	if( (subInfo->args[0].type == dfdType::String || subInfo->args[1].type == dfdType::String) && 
		subInfo->args[0].type != subInfo->args[1].type ) //we have mix string and non string, let upgrade non string
	{
		if( subInfo->args[0].type == dfdType::String ) return strcmp(lval.valueStr,IntToStr(rval,subInfo->args[1].type,tmp))>0?(ExecCond::condTrue):(ExecCond::condFalse);
		if( subInfo->args[1].type == dfdType::String ) return strcmp(rval.valueStr,IntToStr(lval,subInfo->args[0].type,tmp))>0?(ExecCond::condTrue):(ExecCond::condFalse);
		return ExecCond::condNever;
	}
	return (ParamNumberToI64(subInfo->args[0].type,rval) > ParamNumberToI64(subInfo->args[1].type,rval))?(ExecCond::condTrue):(ExecCond::condFalse);
}
int SysCall_AssignValue(subroutineV* subInfo,txdContext*  ctx)
{	
	param* pin  = NULL;
	param* pout = NULL;
	if( subInfo->args[0].direction == ParamDir::input  ) pin  = &subInfo->args[0];
	if( subInfo->args[0].direction == ParamDir::output ) pout = &subInfo->args[0];
	if( subInfo->args[1].direction == ParamDir::input )  pin  = &subInfo->args[1];
	if( subInfo->args[1].direction == ParamDir::output ) pout = &subInfo->args[1];
	if( pin == NULL || pout == NULL || pin == pout ) return 0;

	ParamValue rval = GetParamValue(pin,ctx);
	ParamValue val = GetParamValue(pout,ctx); //just to init type

	return Internal_AssignValue(pout,pin,rval,ctx);
}
int SysCall_StringLength(subroutineV* subInfo,txdContext*  ctx)
{	
	param* pin  = NULL;
	param* pout = NULL;
	if( subInfo->args[0].direction == ParamDir::input  ) pin  = &subInfo->args[0];
	if( subInfo->args[0].direction == ParamDir::output ) pout = &subInfo->args[0];
	if( subInfo->args[1].direction == ParamDir::input )  pin  = &subInfo->args[1];
	if( subInfo->args[1].direction == ParamDir::output ) pout = &subInfo->args[1];
	if( pin == NULL || pout == NULL || pin == pout ) return 0;

	ParamValue rval = GetParamValue(pin,ctx);
	ParamValue val = GetParamValue(pout,ctx); //just to init type
	
	char tmp[MaxIntStr];
	int len = 0;
	if( pin->type != dfdType::String )
	{
		IntToStr(rval,pin->type,tmp);
		len = strlen(tmp);
	}
	else len = strlen(rval.valueStr);
	param		p;
	ParamValue  v;
	p.type      = dfdType::Int;
	p.direction = ParamDir::input;
	p.storage   = NodeTypes::valueNode;
	p.value     = len;
	v.value64   = len;
	return Internal_AssignValue(pout,&p,v,ctx);
}
int SysCall_StringSlice(subroutineV* subInfo,txdContext*  ctx)
{	
	char tmp[MaxIntStr];
	MultiParams args(subInfo,ctx);
	char* str;
	if( args.inParam(0)->type != dfdType::String )
		str = (char*)IntToStr(args.inValue(0),args.inParam(0)->type,tmp);
	else
		str = (char*)args.inValue(0).valueStr;

	int i,n = subInfo->sub.outputCount;
	for(i=0;i<n;i++) EmptyParam(args.outParam(i),ctx);
		
	
	if( str == NULL || str[0] == 0 ) return 0; //|| (unsigned char)str[0] == 0x80 
	int len = strlen(str);
	n = subInfo->sub.inputCount;
	int s,idx=0;
	char ch;
	ParamValue val;
	param		p;
	p.type      = dfdType::String;
	p.direction = ParamDir::input;
	p.storage   = NodeTypes::valueNode;
	p.value     = 0;

	for(i=1;i<n && idx<len;i++)
	{
		if( args.inParam(i)->type  < dfdType::Char || args.inParam(i)->type > dfdType::Long) continue;
		//if( args.outParam(i-1)->type != dfdType::String ) continue;
		s = static_cast<__int32>(ParamNumberToI64(args.inParam(i)->type,args.inValue(i))); //slice
		if( s > 0 )
		{
			if( (s+idx) < len )
			{
				ch = str[idx+s];
				str[idx+s]=0;
				val.valueStr = &str[idx];
				Internal_AssignValue(args.outParam(i-1),&p,val,ctx);
				str[idx+s]=ch;
			}
			else
			{
				val.valueStr = &str[idx];
				Internal_AssignValue(args.outParam(i-1),&p,val,ctx);
			}
			idx = idx +s;
			if( idx > len ) idx=len;
		}
	}
	return 0; 
}
void DecodeIPv6(const char* Field,__uint64 *Low,__uint64 *High);
__uint32 DecodeIntIP(const char* Field);
int SysCall_IPv6ToLongLong(subroutineV* subInfo,txdContext*  ctx)
{
	MultiParams args(subInfo,ctx);
	char* str = NULL;
	if( args.inParam(0)->type == dfdType::String )
		str = (char*)args.inValue(0).valueStr;

	__uint64 Low=0,High=0,Ipv4=MINU64;
	DecodeIPv6(str,&Low,&High);
	if( Low == MINU64 && High == MINU64)
	{
		__uint32 ip32 = DecodeIntIP(str);
		if( ip32 != MINU32 )
		{
			Ipv4 = ip32;
			Ipv4 = Ipv4 | 0x2000000000; //0x20 = 32 = 255.255.255.255
		}
	}

	int i,n = subInfo->sub.outputCount;
	for(i=0;i<n;i++) EmptyParam(args.outParam(i),ctx);

	ParamValue val;
	param		p;
	p.type      = dfdType::Long;
	p.direction = ParamDir::input;
	p.storage   = NodeTypes::valueNode;
	p.value     = 0;
	val.value64 = Low;
	Internal_AssignValue(args.outParam(0),&p,val,ctx);
	val.value64 = High;
	Internal_AssignValue(args.outParam(1),&p,val,ctx);
	
	
	val.value64 = Ipv4;
	if( args.outParam(2)->type == dfdType::String )	p.type = dfdType::IntIP;
	Internal_AssignValue(args.outParam(2),&p,val,ctx);

	return 0;
}
int SysCall_FileLineID(subroutineV* subInfo,txdContext*  ctx)
{
	MultiParams args(subInfo,ctx);
	EmptyParam(args.outParam(0),ctx);

	ParamValue val;
	param		p;
	p.type      = dfdType::Long;
	p.direction = ParamDir::input;
	p.storage   = NodeTypes::valueNode;
	p.value     = 0;
	val.value64 = ctx->currentLineNumber;
	Internal_AssignValue(args.outParam(0),&p,val,ctx);
	return 0;
}
int SysCall_FileExtension(subroutineV* subInfo,txdContext*  ctx)
{
	MultiParams args(subInfo,ctx);
	EmptyParam(args.outParam(0),ctx);

	ParamValue val;
	param		p;
	p.type      = dfdType::String;
	p.direction = ParamDir::input;
	p.storage   = NodeTypes::valueNode;
	p.value     = 0;
	val.valueStr = ctx->inputFileExt;
	Internal_AssignValue(args.outParam(0),&p,val,ctx);
	return 0;
}


typedef int (*pSysCall)(subroutineV* subInfo,txdContext*  ctx);
pSysCall SysCalls[Syscall::maxFunction]; 
void InitTransform()
{
	SysCalls[Syscall::nopFunction]	= SysCall_Nop;
	SysCalls[Syscall::equal]		= SysCall_equal;
	SysCalls[Syscall::notEqual]		= SysCall_notEqual;
	SysCalls[Syscall::lessEqual]	= SysCall_lessEqual;
	SysCalls[Syscall::greatEqual]	= SysCall_greatEqual;
	SysCalls[Syscall::lessThan]		= SysCall_lessThan;
	SysCalls[Syscall::greaterThan]	= SysCall_greaterThan;
	//----------------------------------------------
	SysCalls[Syscall::AssignValue]		= SysCall_AssignValue;
	SysCalls[Syscall::StringLength]		= SysCall_StringLength;
	SysCalls[Syscall::StringSlice]		= SysCall_StringSlice;
	SysCalls[Syscall::IPv6ToLongLong]	= SysCall_IPv6ToLongLong;
	SysCalls[Syscall::FileLineID]		= SysCall_FileLineID;
	SysCalls[Syscall::FileExtension]	= SysCall_FileExtension;
}

void TransformRecord(txdContext*  ctx)
{
	InitTransform();
	int i;
	subroutineV* sub;
	int ruleId = 0;
	int cond = ExecCond::condAlways;
	for(i=0;i<ctx->localCount; i++) ctx->locals[i]->reset(ctx->names);
	for(i=0;i<ctx->extendCount;i++) 
		ctx->extends[i]->AddEmpty();
	

	i=0;
	while( i < ctx->codeLength )
	{
		sub = (subroutineV*)&ctx->code[i];
		if( sub->sub.ruleID != ruleId ) 
		{
			ruleId = sub->sub.ruleID;
			cond = ExecCond::condAlways;
		}

		if( sub->sub.functionID >= Syscall::equal && sub->sub.functionID <= Syscall::greaterThan) //logical condition
		{
			cond = SysCalls[sub->sub.functionID](sub,ctx);
		}
		else if( sub->sub.functionID > Syscall::nopFunction &&  sub->sub.functionID < Syscall::maxFunction )
		{
			if( sub->sub.execCondition == ExecCond::condAlways || sub->sub.execCondition == cond )
				SysCalls[sub->sub.functionID](sub,ctx);
		}
		else 
		{
			fprintf(stderr,"invalid code function (%d:%d).\n",i,sub->sub.functionID);
			exit(1);
		}
		i = i + sizeof(subroutine);
		i = i + ((sub->sub.inputCount + sub->sub.outputCount ) * sizeof(param));
	}
}


