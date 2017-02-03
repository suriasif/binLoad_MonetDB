// BatCsvReader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#include "DfdInfo.h"
#include "BatStream.h"

#ifdef __linux__
#include <string.h>
#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL
#define strncpy_s(strDest,numberOfElements,strSource,kount) (strncpy((strDest),(strSource),(kount)))
#define sprintf_s snprintf
#endif


void DecodeField(const char*,BatField* Bat);
void TransformRecord(txdContext*  pctx);

void TestCodeTimeStamp();
void TestCodeDecimal();
void TestCodeInts();
void TestCodeIntIPs();
void TestCodeIPv6();

//D:\xMonet\dfd.csvd D:\xMonet\csv01.csv D:\xMonet\
/* //0:exe filepath, 1:Dfd_filepath, 2:Csv_filepath, 3:Bat_path */ 
int main(int argc, char* argv[]  ) 
{
	//TestCodeIPv6(); return 0;
	if( argc < 4 )
	{
		fprintf(stderr,"syntax error. BatCsvReader dfdFile CsvFile BatPath\n");
		return 1;
	}
	DfdInfo dfd;
	int err = dfd.Load(argv[1]);
	if( err != 0 )
	{
		fprintf(stderr,"Dfd loading error (%s):(%d).\n",argv[1],err);
		return err;
	}

	FILE* fp = NULL;
	fopen_s(&fp,argv[2],"rt");
	if( fp == NULL )
	{
		fprintf(stderr,"Csv reading error (%d):(20).\n",argv[2]);
		return 20;
	}

	char fSep  = dfd.FieldSep()[0];
	int fld,i,j;
	PtrArray objBatFields ( dfd.inputCount() + dfd.extendCount() );
	PtrArray objLocalDatas( dfd.localCount() );
	//------------------------------------------------------
	BatStream** BatFields  = (BatStream**)objBatFields.Array;
	LocalData** LocalDatas = (LocalData**)objLocalDatas.Array;
	//------------------
	for(i=0;i<dfd.inputCount() ;i++) BatFields[                  i] = new BatStream( dfd.inputField(i)    );
	for(i=0;i<dfd.extendCount();i++) BatFields[dfd.inputCount() +i] = new BatStream( dfd.extendField(i)   );
	for(i=0;i<dfd.localCount() ;i++) LocalDatas[i]					= new LocalData( dfd.localVariable(i) );
	//------------------
	txdContext context;
	context.inputCount	= dfd.inputCount();
	context.inputs		= BatFields;
	context.extendCount = dfd.extendCount();
	context.extends		= &BatFields[context.inputCount];
	context.localCount	= dfd.localCount();
	context.locals		= LocalDatas;
	context.codeLength	= dfd.codeLength();
	context.code		= dfd.codeBytes();
	context.names		= dfd.nameBytes();
	context.currentLineNumber	= 0;
	context.inputFileName		= argv[2];
	j=strlen(argv[2]);
	while(j>0 && argv[2][j] != '.')j--;
	context.inputFileExt		= &argv[2][j+1];
	//------------------
	int fCount = dfd.inputCount();
	char buffer[32768];
	int Line=0;
	while( !feof(fp) )
	{
		Line++;
		buffer[0]=0;
		if( fgets(buffer,32760,fp) == NULL ) break;
		fld =0; i =0;
		while( buffer[i] != 0 && i < 32760 && fld < fCount )
		{
			//if( fld == 13 ) j=i;
			j = i;
			while( buffer[i] != 0 && i < 32760 && buffer[i] != fSep && buffer[i] != '\n' ) i++;
			buffer[i]=0; i++;

			BatStream* pBat = BatFields[fld];
			if( pBat != NULL )
				DecodeField(&buffer[j],pBat);

			fld++;
		}
		if( fld < fCount )
		{
			if( Line == 1)
			{
				fprintf(stderr,"Csv file has incomplete data at line %d having only %d fields.\n",Line,fld);
				//exit(1);
			}
			while(fld < fCount)
			{
				BatStream* pBat = BatFields[fld];
				if( pBat != NULL )
					DecodeField(NULL,pBat);
				fld++;
			}
		}
		//-- executing transformation
		//if( Line == 8228 ) 	i=0;
		context.currentLineNumber = Line;
		TransformRecord(&context);
	}
	fclose(fp);
	//---------------------
	PtrArray objOutArray(dfd.outputCount());
	BatStream** outArray = (BatStream**)objOutArray.Array;
	fCount = dfd.outputCount();
	__int32 NameIdx;
	for(i=0;i<fCount;i++)
	{
		outArray[i]=NULL;
		NameIdx = dfd.outputField(i);
		if( NameIdx != -1 )
		{
			Line=dfd.inputCount() + dfd.extendCount();
			for(j=0;j<Line && outArray[i]==NULL;j++) 
				if( BatFields[j] != NULL && BatFields[j]->DfdElement != NULL && BatFields[j]->DfdElement->fieldName == NameIdx ) 
					outArray[i]=BatFields[j];
		}
	}
	for(i=0;i<fCount;i++)
	{
		BatStream* ptr = outArray[i];
		if( ptr != NULL ) 
		{
			buffer[0]=0;
			//strcpy_s(buffer,32000,argv[3]);
      sprintf_s(buffer,32000,"%s",argv[3]);
      
			j=strlen(buffer);
#ifdef _WIN32      
			if( buffer[j-1] != '\\' ) { buffer[j] = '\\'; j++; buffer[j]=0; }
#endif
#ifdef __linux__
			if( buffer[j-1] != '/' ) { buffer[j] = '/'; j++; buffer[j]=0; }
#endif
			
      sprintf_s(&buffer[j],32000,"%s.%d",&context.names[ptr->DfdElement->fieldName],i);
			if( ptr->SaveStream(buffer) == 0 )
			{
				fprintf(stderr,"Unable to create Bat file (%s).\n",buffer);
				//exit(1);
			}
		}
	}
	//---------------------
	fCount = dfd.inputCount() + dfd.extendCount();
	for(i=0;i<fCount;i++)
	{
		BatStream* ptr = BatFields[i];
		if( ptr != NULL ) delete ptr;
		BatFields[i]=NULL;
	}
	//---------------------
	fCount = dfd.localCount();
	for(i=0;i<fCount;i++)
	{
		LocalData* lData = LocalDatas[i];
		if( lData != NULL ) delete lData;
		LocalDatas[i]=NULL;
	}
	//---------------------
	return 0;
}


void TestCodeInts()
{
	batField e = {0,dfdType::Long,0,0,0};
	BatStream  bat(&e);
	e.fieldNullable = 1;

	DecodeField("0",&bat);
	DecodeField("12",&bat);
	DecodeField("-1",&bat);
	DecodeField("-0",&bat);
	DecodeField("",&bat);
	DecodeField(NULL,&bat);
	DecodeField("130",&bat);
	DecodeField("-130",&bat);


}

void TestCodeIntIPs()
{
	batField e = {0,dfdType::IntIP,0,0,0};
	BatStream  bat(&e);
	e.fieldNullable = 1;

	DecodeField("192.168.0.1",&bat);
	DecodeField("127.0.0.1",&bat);
	DecodeField("10.1.108.8",&bat);
	DecodeField("212.179.128.63",&bat);
	DecodeField("",&bat);
	DecodeField("212..179.128.63",&bat);
	DecodeField("212.179.128.",&bat);
	DecodeField("212.179.128",&bat);
	DecodeField("300.280.1.1",&bat);

}

void DecodeIPv6(const char* Field,__uint64 *Low,__uint64 *High);
void TestCodeIPv6()
{
	__uint64 low,high;
	DecodeIPv6("FEDC:BA98:7654:3210:FEDC:BA98:7654:3210",&low,&high);
	DecodeIPv6("1080:0:0:0:8:800:200C:417A",&low,&high);
	DecodeIPv6("1080::8:800:200C:417A",&low,&high);
}

void TestCodeTimeStamp()
{

	batField e = {0,dfdType::TimeStampODBC,0,0,0};
	BatStream  bat(&e);

	DecodeField("2014-03-31 14:16:52",&bat);
	DecodeField("2014-03-31:14:16:52",&bat);
	DecodeField("1970-01-01 00:00:00",&bat);
	DecodeField("0001-01-01 00:00:00",&bat);
}


void TestCodeDecimal()
{
	batField e = {0,dfdType::Double,0,15,3};
	BatStream  bat(&e);

	DecodeField(".56",&bat);
	DecodeField(".7258",&bat);
	DecodeField(".987",&bat);

	DecodeField("123.561",&bat);
	DecodeField("123.27",&bat);
	DecodeField("123.3",&bat);
	DecodeField("123.7258",&bat);
	DecodeField("9213123.17258",&bat);
	DecodeField("92131.23.17258",&bat);

	DecodeField("+123.2",&bat);
	DecodeField("-123.2",&bat);
	DecodeField("-123.+2",&bat);
	DecodeField("-123.",&bat);
	DecodeField("92131",&bat);
	DecodeField("0.543",&bat);
	DecodeField("0.0",&bat);
	DecodeField("0.1",&bat);
}


//this is end of file
