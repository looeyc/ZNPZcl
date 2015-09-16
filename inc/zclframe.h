/*
 * zclframe.h
 *
 *  Created on: 2014-2-21
 *      Author: chengm
 */

#ifndef ZCLFRAME_H_
#define ZCLFRAME_H_

#define ZCL_MAXPAYLOADLEN			256

//ZclFrame结构
typedef struct{
	//附带属性
	unsigned short SrcAddr;//源地址
	unsigned char SrcEP;//源EP
	unsigned short ProfileID;
	unsigned short ClusterID;

	//Frame各字段
	unsigned char FrameCtrl;
	unsigned short ManufactCode;
	unsigned char TranSeqNum;
	unsigned char CmdID;
	unsigned char PayLoadLen;					//Frame payload 长度
	unsigned char PayLoad[ZCL_MAXPAYLOADLEN];	//Frame payload
}stZclFrame;

//创建一个ZclFrame
//return NULL表示创建失败
//创建的ZclFrame在使用完毕后一定要调用DelocateZclFrame销毁
stZclFrame *AllocateZclFrame();
//销毁一个ZclFrame
void DelocateZclFrame(stZclFrame *pZclFrame);

//将stZclFrame结构体转换为字节流
//pZclFrame - 待转换的stZclFrame结构体
//BufferLen - pBuffer指向的缓存区长度
//pBuffer - 调用者提供空间，用于存放转换得到的字节流
//return 转换所得数据长度，<=0表示转换失败
int fStToBytes_ZclFrame(stZclFrame *pZclFrame, unsigned char BufferLen, unsigned char *pBuffer);

//将字节流转换为stZclFrame结构体
//SrcAddr - 源地址
//SrcEP - 源EP
//ProfileID - 所属的Profile
//ClusterID - 所属的Cluster
//pDataBuffer - 待转换的字节流缓存区
//DataLen - pDataBuffer指向的缓存区中存放的待转换数据长度
//pZclFrame - 调用者提供空间，用于返回转换得到的结构体
//return 0-成功，else-失败
int fBytesToSt_ZclFrame(unsigned short SrcAddr, unsigned char SrcEP, unsigned short ProfileID, unsigned short ClusterID, unsigned char DataLen, unsigned char *pDataBuffer, stZclFrame *pZclFrame);

#endif /* ZCLFRAME_H_ */








