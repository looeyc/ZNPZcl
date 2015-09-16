/*
 * zclmsgfifo.h
 *
 *  Created on: 2014-2-24
 *      Author: chengm
 */

#ifndef ZCLMSGFIFO_H_
#define ZCLMSGFIFO_H_

#include <pthread.h>
#include "zclmsg.h"

//ZclMsg消息队列
typedef struct stZclMsgListItem_ stZclMsgListItem;
struct stZclMsgListItem_{
	stZclMsgListItem *pNext;
	//消息
	stZclMsg *pZclMsg;
};

//ZclMsg消息队列句柄
typedef struct{
	pthread_mutex_t m;
	pthread_cond_t c;
	stZclMsgListItem *pZclMsgList;//消息队列指针
	int MsgNum;//队列中的消息数量
}stZclMsgFIFOHdl;

//创建一个ZclMsg消息队列
//return 创建的消息队列句柄，NULL-创建失败
stZclMsgFIFOHdl *ZclMsgFIFOCreate();

//往ZclMsg消息队列中加入一条消息
//pZclMsgFIFO - 消息队列句柄
//pZclMsg - 需要加入的ZclMsg消息，消息必须通过是AllocateZnpMsg创建的，因为队列会接管该消息的存储区
//pZclMsgInfo - ZclMsg的附带属性，附带属性参数将被拷贝到队列里，队列不会接管该参数的存储区
//return 0-成功，else-失败
//成功后，消息的指针被排入队列，消息数据被FIFO接管
int ZclMsgFIFOPush(stZclMsgFIFOHdl *pZclMsgFIFO, stZclMsg *pZclMsg);

//从ZclMsg消息队列中取出一条消息
//pZclMsgFIFO  - 消息队列句柄
//pZclMsgInfo - 调用者提供空间，用于返回ZclMsg的附带属性，如果为NULL表示忽略附带属性，不需要返回
//return 取出的消息指针，NULL表示出错
//成功后，消息数据由调用者接管，消息在使用完毕后要调用DelocateZclMsg进行销毁
stZclMsg *ZclMsgFIFOPop(stZclMsgFIFOHdl *pZclMsgFIFO);

//销毁一个ZclMsg消息队列
//pZclMsgFIFOHdl - 需销毁的消息队列句柄
//如果队列中有消息，将销毁所有消息
void ZclMsgFIFODestroy(stZclMsgFIFOHdl *pZclMsgFIFOHdl);

#endif /* ZCLMSGFIFO_H_ */







