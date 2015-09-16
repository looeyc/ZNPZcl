/*
 * zclglobal.h
 *
 *  Created on: 2014-2-21
 *      Author: root
 */

#ifndef ZCLGLOBAL_H_
#define ZCLGLOBAL_H_

#include <pthread.h>
#include "zclmsg.h"
#include "zclmsgfifo.h"

#define MAXZCLINSTNUM	8

//与EP相关的状态
typedef struct{
	char isValid;//0-未被占用，1-已经被占用
	unsigned char EP;
	unsigned short Profile;//为该EndPoint注册的Profile
	void (*pDefaultCBFxn)(stZclMsg *pZclMsg);//注册默认的ZclMsg回调处理函数，符合Profile的所有未处理ZclMsg将调用该回调进行处理

	stZclMsgFIFOHdl *pZclMsgFIFOHdl;//消息接收队列句柄
}stZclInst;

//Zcl的全局工作参数
typedef struct{
	pthread_mutex_t mutexZclList;//实现对ZclList的互斥访问
	stZclInst ZclInstList[MAXZCLINSTNUM];//ZCL实例列表

	pthread_mutex_t mutexTranSeqNum;
	unsigned char TranSeqNum;//Zcl帧中的传输流水号

	pthread_mutex_t mutexGlobalZclMsgSubsHdlList;//用于ZclMsg订阅
	stZclMsgSubsHdl *pGlobalZclMsgSubsHdlList;
}stZclStatus;
extern stZclStatus GlobalZclStatus;

//获取一个当前唯一的TranSeqNum
unsigned char ZclGenTranSeqNum();

#endif /* ZCLGLOBAL_H_ */


