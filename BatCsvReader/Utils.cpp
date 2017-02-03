// Utils.cpp : Utility functions.
//
#include "stdafx.h"


typedef __int32 date;
#define date_nil ((date) 0)

int NODAYS[13] =  { 0, 31, 29, 31,  30,  31,  30,  31,  31,  30,  31,  30,  31 };
int CUMDAYS[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

date DATE_MAX, DATE_MIN;	/* often used dates; computed once */

#define YEAR_MAX	5867411
#define YEAR_MIN	-YEAR_MAX
#define MONTHDAYS(m,y)	(((m)!=2)?NODAYS[m]:leapyear(y)?29:28)
#define YEARDAYS(y)	(leapyear(y)?366:365)
#define LEAPYEARS(y)	(leapyears(y)+((y)>=0))
#define DATE(d,m,y)	((m)>0&&(m)<=12&&(d)>0&&(y)!=0&&(y)>=YEAR_MIN&&(y)<=YEAR_MAX&&(d)<=MONTHDAYS(m,y))
#define TIME(h,m,s,x)	((h)>=0&&(h)<24&&(m)>=0&&(m)<60&&(s)>=0&&(s)<60&&(x)>=0 &&(x)<1000)
#define LOWER(c)	(((c) >= 'A' && (c) <= 'Z') ? (c)+'a'-'A' : (c))

int leapyear(int year)
{
	return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}
int leapyears(int year)
{
	/* count the 4-fold years that passed since jan-1-0 */
	int y4 = year / 4;

	/* count the 100-fold years */
	int y100 = year / 100;

	/* count the 400-fold years */
	int y400 = year / 400;

	return y4 + y400 - y100;	/* may be negative */
}
date todate(int day, int month, int year)
{
	date n = 0;

	if (DATE(day, month, year)) {
		if (year < 0)
			year++;	/* HACK: hide year 0 */
		n = (date) (day-1 );
		if ( month > 2 && leapyear(year)) n++;
		n+= CUMDAYS[month-1];
			;
		/* current year does not count as leapyear */
		n += 365 * year + LEAPYEARS(year >= 0 ? year - 1 : year);
	}
	return n;
}

/* multi line comments
 chararray
 datetime
 double
 int
 long
 
 
D8 C1 B3 02 AB 3A 0B 00
F8 9D C6 02 AB 3A 0B 00

00 0b 3a ab 02 b3 c1 d8  =  2014-11-13 12:35:35.000000 
00 0b 3a ab 02 c6 9d f8  =  2014-11-13 12:56:11.000000

(00 0b 3a ab :: 02 b3 c1 d8 ) =  735915 :: 45335000 /1000  = 735915 :: 45335
45335 / 3600 = 12
45335 % 3600 = 2135 / 60 = 35
2135 % 60 = 35 
12:35:35

02 b3 c1 d8
05 26 5C 00 
 



select (epoch(cast(current_timestamp as timestamp))-epoch(timestamp '2013-04-25 11:49:00') 

0b 3a ab 02 b3 c1 d8 = 3160730902970840 / 1000 =  3160730902970
0b 3a ab 02 c6 9d f8 = 3160730904206840 / 1000 =  3160730904206

3160730904206 - 3160730902970 = 1236
2014-11-13 12:56:11.000000 - 2014-11-13 12:35:35.000000   = 1236 seconds

3160730902970 /

*/


