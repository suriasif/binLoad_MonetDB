// Utils.cpp : Field Decoders.
//
#include "stdafx.h"
#include "DfdInfo.h"
#include "BatStream.h"
#include <stdlib.h>

#ifdef __linux__

#define _atoi64(zVal) (strtoull((zVal),NULL,10))

#endif

void DecodeChar			(const char* Field,BatField *Bat)
{
	char ch=0;
	if( Bat->IsNullable() == 1 ) ch=(unsigned char)0x80;

	if( Field != NULL && Field[0] != 0 )  
		ch = atoi(Field);
	Bat->Add((unsigned char*)&ch);
}
void DecodeTinyInt		(const char* Field,BatField* Bat)
{
	char ch=0;
	if( Bat->IsNullable() == 1 ) ch=(unsigned char)0x80;

	if( Field != NULL && Field[0] != 0 )  
		ch = atoi(Field);
	Bat->Add((unsigned char*)&ch);
}
void DecodeSmallInt		(const char* Field,BatField* Bat)
{
	__int16 ch=0;
	if( Bat->IsNullable() == 1 ) ch=(__uint16)0x8000;

	if( Field != NULL && Field[0] != 0 )  
		ch = atoi(Field);
	Bat->Add((unsigned char*)&ch);
}

void DecodeInt			(const char* Field,BatField* Bat)
{
	__int32 ch=0;
	if( Bat->IsNullable() == 1 ) ch=(__uint32)0x80000000;

	if( Field != NULL && Field[0] != 0 )  
		ch = atoi(Field);
	Bat->Add((unsigned char*)&ch);
}
void DecodeLong			(const char* Field,BatField* Bat)
{
	__int64 ch=0;
	if( Bat->IsNullable() == 1 ) ch=MINU64;

	if( Field != NULL && Field[0] != 0 )  
		ch = _atoi64(Field);
	Bat->Add((unsigned char*)&ch);
}

const __int64 Pow10[19] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 
	10000000000, 100000000000, 1000000000000, 10000000000000, 100000000000000, 1000000000000000, 
	10000000000000000, 100000000000000000, 1000000000000000000 }; 

void DecodeDouble		(const char* Field,BatField* Bat)
{
	__int64 a=0;
	if( Field != NULL && Field[0] != 0 )
	{
		int scale = Bat->fieldScale() + 1;
		int i=0,neg=0,frac=0,flag=1;
		
		while(flag==1 && scale > 0)
		{
			if( Field[i] >= '0' && Field[i] <= '9' ) a = (a * 10 ) +  (Field[i] -'0');
			else if( Field[i] == '.' )	{ if(frac == 0) frac=1; else flag=0; }
			else if( Field[i] == '-' )  { if(i == 0)    neg=1;  else flag=0;  }
			else if( Field[i] == '+' )  { if(i == 0)    neg=0;  else flag=0;  }
			else { flag=0;  }
		
			if( frac == 1 && flag == 1) scale--;
			i++;
		}
		if( frac ==0 ) scale--;
		if( scale > 0 ) a = a * Pow10[scale];
		if( neg == 1) a = a * -1;
	}
	else if( Bat->IsNullable() == 1 ) a = MINU64;
	Bat->Add((unsigned char*)&a);
}
void DecodeString		(const char* Field,BatField* Bat)
{
	Bat->AddString(Field);
}
__int32 todate(int day, int month, int year);
void DecodeTimeStampODBC (const char* Field,BatField* Bat)
{
	//2014-03-31 14:16:52
	__uint64  Dtm = 0;
	int d[6] = {0,0,0,0,0,0};
	if( Field != NULL && Field[0] != 0 ) 
	{
		int flag=1,idx=0,i=0;
		while(flag==1 && idx < 6)
		{
			if( Field[i] >= '0' && Field[i] <= '9' ) d[idx] = (d[idx] * 10 ) +  (Field[i] -'0');
			else if( Field[i] == '-' )  { if(idx <2)			idx++;  else flag=0;  }
			else if( Field[i] == ' ' )  { if(idx == 2)			idx++;  else flag=0;  }
			else if( Field[i] == ':' )  { if(idx >2 && idx <5)  idx++;  else flag=0;  }
			else if( Field[i] == 0   )  { if(idx == 5)			idx++;  else flag=0;  }
			else { flag=0;  }
			if( Field[i] != 0 )i++;
		}
		if( idx == 6 ) //is perfect parsed ?
		{
			__uint32 Date = todate(d[2],d[1],d[0]);
			__uint32 Time = (d[3] * 3600) + (d[4] * 60) + d[5];
			Time = Time * 1000;
			Dtm = Date;
			Dtm = Dtm << 32;
			Dtm = Dtm | Time;
		}
		else Dtm = MINU64;
	}
	else Dtm = MINU64;
	Bat->Add((unsigned char*)&Dtm);
}
__uint32 DecodeIntIP(const char* Field)
{
	static unsigned int m[4] = {0x000000FF,0x0000FF00,0x00FF0000,0xFF000000};
	static unsigned int s[4] = {0,8,16,24}; 

	//static unsigned int m[4] = {0xFF000000,0x00FF0000,0x0000FF00,0x000000FF};
	//static unsigned int s[4] = {24,16,8,0}; 
	
	//192.168.16.5
	unsigned int d[4] = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};

	__uint32 IP=MINU32;
	if( Field != NULL && Field[0] != 0 ) 
	{
		int flag=1,idx=0,i=0;
		while(flag==1 && idx < 4)
		{
			if( Field[i] >= '0' && Field[i] <= '9' ) 
			{ 
				if(d[idx] == 0xFFFFFFFF) d[idx]=0;
				d[idx] = (d[idx] * 10 ) +  (Field[i] -'0');
			}
			else if( Field[i] == '.' )
			{
				if(idx <4 && d[idx] != 0xFFFFFFFF && d[idx] <= 0xFF )
				{
					d[idx] = (d[idx] << s[idx]) & m[idx];
					idx++;  
				} 
				else flag=0;  
			}
			else if( Field[i] == 0   )  
			{ 
				if(idx == 3)
				{
					if(d[idx] != 0xFFFFFFFF && d[idx] <= 0xFF )
					{  
						d[idx] = (d[idx] << s[idx]) & m[idx]; 
						idx++;  
					}
					else flag=0;
				}
				else		 flag=0;  
			}
			else { flag=0;  }
			if( Field[i] != 0 )i++;
		}
		if( idx == 4 && Field[i]==0 && d[3] != 0xFFFFFFFF) //is perfect parsed ?
			 IP = d[0] | d[1] | d[2] | d[3];
		else IP = MINU32;
	}
	else IP = MINU32;
	return IP;
}
void DecodeIntIP        (const char* Field,BatField* Bat)
{
	__uint64 IP64 = MINU64;
	__uint32 IP32 = DecodeIntIP(Field);
	if( IP32 != MINU32 )
	{
		IP64 = IP32;
		IP64 = IP64 | 0x2000000000; //0x20 = 32 = 255.255.255.255
	}
	Bat->Add((unsigned char*)&IP64);
}

void DecodeIPv6(const char* Field,__uint64 *Low,__uint64 *High)
{
	static __uint64 m[4] = {0xFFFF000000000000,0x0000FFFF00000000,0x00000000FFFF0000,0x000000000000FFFF};
	static __uint64 s[4] = {48,32,16,0}; 
	//1080:0:0:0:8:800:200C:417A  = 1080::8:800:200C:417A  
	//1080:0:0:8:0:0:0:9          = 1080::8::9
	__uint64 d[8] = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};
	if( Field != NULL && Field[0] != 0 ) 
	{
		int flag=1,idx=0,i=0;
		while(flag==1 && idx < 8)
		{
			if( Field[i] >= '0' && Field[i] <= '9' ) 
			{ 
				if(d[idx] == 0xFFFFFFFF) d[idx]=0;
				d[idx] = (d[idx] << 4 ) |  (Field[i] -'0');
			}
			else if( Field[i] >= 'A' && Field[i] <= 'F' ) 
			{ 
				if(d[idx] == 0xFFFFFFFF) d[idx]=0;
				d[idx] = (d[idx] << 4 ) |  ((Field[i] -'A') + 0x0A);
			}
			else if( Field[i] >= 'a' && Field[i] <= 'f' ) 
			{ 
				if(d[idx] == 0xFFFFFFFF) d[idx]=0;
				d[idx] = (d[idx] << 4 ) |  ((Field[i] -'a') + 0x0A);
			}
			else if( Field[i] == ':' )  
			if(idx <8 && d[idx] != 0xFFFFFFFF && d[idx] <= 0xFFFF )
			{
				if( idx < 4 ) //High
					d[idx] = (d[idx] << s[idx]) & m[idx];
				else //low
					d[idx] = (d[idx] << s[idx-4]) & m[idx-4];
				idx++;
			} 
			else flag=0;  
			else if( Field[i] == 0   )  { if(idx == 7)			idx++;  else flag=0;  }
			else { flag=0;  }
			if( Field[i] != 0 )i++;
		}
		if( idx == 8 && Field[i]==0 && d[7] != 0xFFFFFFFF) //is perfect parsed ?
		{
			 High[0] = d[0] | d[1] | d[2] | d[3];
			 Low[0]  = d[4] | d[5] | d[6] | d[7];
		}
		else 
			 High[0] = Low[0] = MINU64;
	}
	else High[0] = Low[0] = MINU64;
}

void DecodeField(const char* Field,BatField* Bat)
{
	if( Bat == NULL ) return;
	if( Bat->nodeField() == NULL ) return;

	switch(Bat->fieldType())
	{
	case dfdType::Char:			 DecodeChar(Field,Bat);			  break;
	case dfdType::TinyInt:		 DecodeTinyInt(Field,Bat);		  break;
	case dfdType::SmallInt:		 DecodeSmallInt(Field,Bat);		  break;
	case dfdType::Int:			 DecodeInt(Field,Bat);			  break;
	case dfdType::IntIP:         DecodeIntIP(Field,Bat);  	      break;
	case dfdType::Long:			 DecodeLong(Field,Bat);			  break;
	case dfdType::Double:		 DecodeDouble(Field,Bat);		  break;
	case dfdType::String:		 DecodeString(Field,Bat);		  break;
	case dfdType::TimeStampODBC: DecodeTimeStampODBC(Field,Bat);  break;
	}
}

