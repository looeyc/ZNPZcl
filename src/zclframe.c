/*
 * zclframe.c
 *
 *  Created on: 2014-2-21
 *      Author: chengm
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "debuglog.h"
#include "zclframe.h"

//创建一个ZclFrame
//return NULL表示创建失败
//创建的ZclFrame在使用完毕后一定要调用DelocateZclFrame销毁
stZclFrame *AllocateZclFrame()
{
	stZclFrame *pRet;

	pRet=(stZclFrame *)malloc(sizeof(stZclFrame));
	if(pRet==NULL){
		DEBUGLOG0(3,"Error, malloc failed.\n");
		return NULL;
	}
	memset(pRet,0,sizeof(stZclFrame));
	return pRet;
}

//销毁一个ZclFrame
void DelocateZclFrame(stZclFrame *pZclFrame)
{
	if(pZclFrame!=NULL){
		free(pZclFrame);
	}
}

//将stZclFrame结构体转换为字节流
//pZclFrame - 待转换的stZclFrame结构体
//BufferLen - pBuffer指向的缓存区长度
//pBuffer - 调用者提供空间，用于存放转换得到的字节流
//return 转换所得数据长度，<=0表示转换失败
int fStToBytes_ZclFrame(stZclFrame *pZclFrame, unsigned char BufferLen, unsigned char *pBuffer)
{
	unsigned int Len;
	int WritePos=0;
	unsigned char isManufactExist=0;

	//检查参数
	if((pZclFrame==NULL)||(BufferLen==0)||(pBuffer==NULL)){
		return -1;
	}

	isManufactExist=(pZclFrame->FrameCtrl&0x04)>>2;

	//计算长度
	Len=isManufactExist?2:0;
	Len+=3+pZclFrame->PayLoadLen;
	if(Len>BufferLen){
		DEBUGLOG2(3,"Error, not enough buffer space %d<%d.\n",BufferLen,Len);
		return -2;
	}

	//FrameCtrl
	pBuffer[WritePos]=pZclFrame->FrameCtrl;WritePos++;

	//ManufactCode
	if(isManufactExist){
		memcpy(pBuffer+WritePos,&pZclFrame->ManufactCode,2);
		WritePos+=2;
	}

	//Transaction sequence number
	pBuffer[WritePos]=pZclFrame->TranSeqNum;WritePos++;

	//Command identifier
	pBuffer[WritePos]=pZclFrame->CmdID;WritePos++;

	//Frame payload
	memcpy(pBuffer+WritePos,pZclFrame->PayLoad,(unsigned int)(pZclFrame->PayLoadLen));
	WritePos+=pZclFrame->PayLoadLen;

	return WritePos;
}

//将字节流转换为stZclFrame结构体
//SrcAddr - 源地址
//SrcEP - 源EP
//ProfileID - 所属的Profile
//ClusterID - 所属的Cluster
//pDataBuffer - 待转换的字节流缓存区
//DataLen - pDataBuffer指向的缓存区中存放的待转换数据长度
//pZclFrame - 调用者提供空间，用于返回转换得到的结构体
//return 0-成功，else-失败
int fBytesToSt_ZclFrame(unsigned short SrcAddr, unsigned char SrcEP, unsigned short ProfileID, unsigned short ClusterID, unsigned char DataLen, unsigned char *pDataBuffer, stZclFrame *pZclFrame)
{
	unsigned int DataLenLeft=DataLen;
	int ReadPos=0;;
	unsigned char isManufactExist=0;

	if((DataLen==0)||(pDataBuffer==NULL)||(pZclFrame==NULL)){
		return -1;
	}

	memset(pZclFrame,0,sizeof(stZclFrame));

	//附带属性
	pZclFrame->SrcAddr=SrcAddr;
	pZclFrame->SrcEP=SrcEP;
	pZclFrame->ProfileID=ProfileID;
	pZclFrame->ClusterID=ClusterID;

	//FrameCtrl
	pZclFrame->FrameCtrl=pDataBuffer[0];
	DataLenLeft--;ReadPos=1;

	//ManufactCode
	isManufactExist=(pZclFrame->FrameCtrl&0x04)>>2;
	if(isManufactExist){
		if(DataLenLeft<2){
			return -2;
		}else{
			memcpy(&pZclFrame->ManufactCode,pDataBuffer+ReadPos,2);
			DataLenLeft-=2;ReadPos+=2;
		}
	}

	//Transaction sequence number
	if(DataLenLeft<=0){
		return -3;
	}else{
		pZclFrame->TranSeqNum=pDataBuffer[ReadPos];
		DataLenLeft--;ReadPos+=1;
	}

	//Command identifier
	if(DataLenLeft<=0){
		return -4;
	}else{
		pZclFrame->CmdID=pDataBuffer[ReadPos];
		DataLenLeft--;ReadPos+=1;
	}

	//Frame payload
	if(DataLenLeft>ZCL_MAXPAYLOADLEN){
		return -5;
	}else{
		pZclFrame->PayLoadLen=DataLenLeft;
		memcpy(pZclFrame->PayLoad,pDataBuffer+ReadPos,DataLenLeft);
		pZclFrame->PayLoadLen=DataLenLeft;
	}

	return 0;
}



















