/*
 * debuglog.h
 *
 *  Created on: 2012-6-8
 *      Author: chengm
 */

#ifndef DEBUGLOG_H_
#define DEBUGLOG_H_

#include <stdio.h>

#define DEBUGLVEL 		0	//数值越大，打印级别越高，例如：0-所有都打印  3-只打印错误
//#define LOGFILEPATH		"./HostDAL.log"

//带行号的输出
#ifdef  DEBUGLVEL
	#ifdef	LOGFILEPATH
		#define DEBUGLOG0(a,b)				if(a >= DEBUGLVEL){FILE *pLogFile;pLogFile=fopen(LOGFILEPATH,"a");if(pLogFile!=NULL){fprintf(pLogFile,"%s(%d)%s:  ",__FILE__,__LINE__,__func__);	fprintf(pLogFile,b);			fflush(pLogFile);fclose(pLogFile);}	printf("%s(%d)%s:  ",__FILE__,__LINE__,__func__);printf(b);}
		#define DEBUGLOG1(a,b,c)			if(a >= DEBUGLVEL){FILE *pLogFile;pLogFile=fopen(LOGFILEPATH,"a");if(pLogFile!=NULL){fprintf(pLogFile,"%s(%d)%s:  ",__FILE__,__LINE__,__func__);	fprintf(pLogFile,b,c);			fflush(pLogFile);fclose(pLogFile);}	printf("%s(%d)%s:  ",__FILE__,__LINE__,__func__);printf(b,c);}
		#define DEBUGLOG2(a,b,c,d)			if(a >= DEBUGLVEL){FILE *pLogFile;pLogFile=fopen(LOGFILEPATH,"a");if(pLogFile!=NULL){fprintf(pLogFile,"%s(%d)%s:  ",__FILE__,__LINE__,__func__);	fprintf(pLogFile,b,c,d);		fflush(pLogFile);fclose(pLogFile);}	printf("%s(%d)%s:  ",__FILE__,__LINE__,__func__);printf(b,c,d);}
		#define DEBUGLOG3(a,b,c,d,e)		if(a >= DEBUGLVEL){FILE *pLogFile;pLogFile=fopen(LOGFILEPATH,"a");if(pLogFile!=NULL){fprintf(pLogFile,"%s(%d)%s:  ",__FILE__,__LINE__,__func__);	fprintf(pLogFile,b,c,d,e);		fflush(pLogFile);fclose(pLogFile);}	printf("%s(%d)%s:  ",__FILE__,__LINE__,__func__);printf(b,c,d,e);}
		#define DEBUGLOG4(a,b,c,d,e,f)		if(a >= DEBUGLVEL){FILE *pLogFile;pLogFile=fopen(LOGFILEPATH,"a");if(pLogFile!=NULL){fprintf(pLogFile,"%s(%d)%s:  ",__FILE__,__LINE__,__func__);	fprintf(pLogFile,b,c,d,e,f);	fflush(pLogFile);fclose(pLogFile);}	printf("%s(%d)%s:  ",__FILE__,__LINE__,__func__);printf(b,c,d,e,f);}
		#define DEBUGLOG5(a,b,c,d,e,f,g)	if(a >= DEBUGLVEL){FILE *pLogFile;pLogFile=fopen(LOGFILEPATH,"a");if(pLogFile!=NULL){fprintf(pLogFile,"%s(%d)%s:  ",__FILE__,__LINE__,__func__);	fprintf(pLogFile,b,c,d,e,f,g);	fflush(pLogFile);fclose(pLogFile);}	printf("%s(%d)%s:  ",__FILE__,__LINE__,__func__);printf(b,c,d,e,f,g);}
	#else
		#define DEBUGLOG0(a,b)				if(a >= DEBUGLVEL){printf("%s(%d)%s:  ",__FILE__,__LINE__,__func__);printf(b);}
		#define DEBUGLOG1(a,b,c)			if(a >= DEBUGLVEL){printf("%s(%d)%s:  ",__FILE__,__LINE__,__func__);printf(b,c);}
		#define DEBUGLOG2(a,b,c,d)			if(a >= DEBUGLVEL){printf("%s(%d)%s:  ",__FILE__,__LINE__,__func__);printf(b,c,d);}
		#define DEBUGLOG3(a,b,c,d,e)		if(a >= DEBUGLVEL){printf("%s(%d)%s:  ",__FILE__,__LINE__,__func__);printf(b,c,d,e);}
		#define DEBUGLOG4(a,b,c,d,e,f)		if(a >= DEBUGLVEL){printf("%s(%d)%s:  ",__FILE__,__LINE__,__func__);printf(b,c,d,e,f);}
		#define DEBUGLOG5(a,b,c,d,e,f,g)	if(a >= DEBUGLVEL){printf("%s(%d)%s:  ",__FILE__,__LINE__,__func__);printf(b,c,d,e,f,g);}
	#endif
#else
	#define DEBUGLOG0(a,b)
	#define DEBUGLOG1(a,b,c)
	#define DEBUGLOG2(a,b,c,d)
	#define DEBUGLOG3(a,b,c,d,e)
	#define DEBUGLOG4(a,b,c,d,e,f)
	#define DEBUGLOG5(a,b,c,d,e,f,g)
#endif

//不带行号的输出
#ifdef  DEBUGLVEL
	#ifdef	LOGFILEPATH
		#define S_DEBUGLOG0(a,b)			if(a >= DEBUGLVEL){FILE *pLogFile;pLogFile=fopen(LOGFILEPATH,"a");if(pLogFile!=NULL){fprintf(pLogFile,b);			fflush(pLogFile);fclose(pLogFile);}	printf(b);}
		#define S_DEBUGLOG1(a,b,c)			if(a >= DEBUGLVEL){FILE *pLogFile;pLogFile=fopen(LOGFILEPATH,"a");if(pLogFile!=NULL){fprintf(pLogFile,b,c);			fflush(pLogFile);fclose(pLogFile);}	printf(b,c);}
		#define S_DEBUGLOG2(a,b,c,d)		if(a >= DEBUGLVEL){FILE *pLogFile;pLogFile=fopen(LOGFILEPATH,"a");if(pLogFile!=NULL){fprintf(pLogFile,b,c,d);		fflush(pLogFile);fclose(pLogFile);}	printf(b,c,d);}
		#define S_DEBUGLOG3(a,b,c,d,e)		if(a >= DEBUGLVEL){FILE *pLogFile;pLogFile=fopen(LOGFILEPATH,"a");if(pLogFile!=NULL){fprintf(pLogFile,b,c,d,e);		fflush(pLogFile);fclose(pLogFile);}	printf(b,c,d,e);}
		#define S_DEBUGLOG4(a,b,c,d,e,f)	if(a >= DEBUGLVEL){FILE *pLogFile;pLogFile=fopen(LOGFILEPATH,"a");if(pLogFile!=NULL){fprintf(pLogFile,b,c,d,e,f);	fflush(pLogFile);fclose(pLogFile);}	printf(b,c,d,e,f);}
		#define S_DEBUGLOG5(a,b,c,d,e,f,g)	if(a >= DEBUGLVEL){FILE *pLogFile;pLogFile=fopen(LOGFILEPATH,"a");if(pLogFile!=NULL){fprintf(pLogFile,b,c,d,e,f,g);	fflush(pLogFile);fclose(pLogFile);}	printf(b,c,d,e,f,g);}
	#else
		#define S_DEBUGLOG0(a,b)			if(a >= DEBUGLVEL){printf(b);}
		#define S_DEBUGLOG1(a,b,c)			if(a >= DEBUGLVEL){printf(b,c);}
		#define S_DEBUGLOG2(a,b,c,d)		if(a >= DEBUGLVEL){printf(b,c,d);}
		#define S_DEBUGLOG3(a,b,c,d,e)		if(a >= DEBUGLVEL){printf(b,c,d,e);}
		#define S_DEBUGLOG4(a,b,c,d,e,f)	if(a >= DEBUGLVEL){printf(b,c,d,e,f);}
		#define S_DEBUGLOG5(a,b,c,d,e,f,g)	if(a >= DEBUGLVEL){printf(b,c,d,e,f,g);}
	#endif
#else
	#define S_DEBUGLOG0(a,b)
	#define S_DEBUGLOG1(a,b,c)
	#define S_DEBUGLOG2(a,b,c,d)
	#define S_DEBUGLOG3(a,b,c,d,e)
	#define S_DEBUGLOG4(a,b,c,d,e,f)
	#define S_DEBUGLOG5(a,b,c,d,e,f,g)
#endif

#endif /* DEBUGLOG_H_ */

