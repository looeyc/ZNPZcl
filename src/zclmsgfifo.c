/*
 * zclmsgfifo.c
 *
 *  Created on: 2014-2-24
 *      Author: chengm
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include "debuglog.h"
#include "zclmsg.h"
#include "zclmsgfifo.h"

//创建一个ZclMsg消息队列
//return 创建的消息队列句柄，NULL-创建失败
stZclMsgFIFOHdl *ZclMsgFIFOCreate()
{
	stZclMsgFIFOHdl *pRet=NULL;

	pRet=(stZclMsgFIFOHdl *)malloc(sizeof(stZclMsgFIFOHdl));
	if(pRet==NULL){
		DEBUGLOG0(3,"Error, malloc falied.\n");
		return NULL;
	}
	memset(pRet,0,sizeof(stZclMsgFIFOHdl));

	pthread_mutex_init(&pRet->m,NULL);
	pthread_cond_init(&pRet->c,NULL);

	return pRet;
}

//往ZclMsg消息队列中加入一条消息
//pZclMsgFIFO - 消息队列句柄
//pZclMsg - 需要加入的ZclMsg消息
//return 0-成功，else-失败
//成功后，消息的指针被排入队列，消息数据被FIFO接管
int ZclMsgFIFOPush(stZclMsgFIFOHdl *pZclMsgFIFO, stZclMsg *pZclMsg)
{
	stZclMsgListItem *pScan=NULL;
	stZclMsgListItem *pNewNode=NULL;

	if((pZclMsgFIFO==NULL)||(pZclMsg==NULL)){
		return -1;
	}

	//创建消息队列节点
	pNewNode=(stZclMsgListItem *)malloc(sizeof(stZclMsgListItem));
	if(pNewNode==NULL){
		DEBUGLOG0(3,"Error, malloc failed.\n");
		return -2;
	}
	memset(pNewNode,0,sizeof(stZclMsgListItem));
	pNewNode->pZclMsg=pZclMsg;

	//加入队列
	pthread_mutex_lock(&pZclMsgFIFO->m);
	pScan=pZclMsgFIFO->pZclMsgList;
	if(pScan==NULL){//队列中还没有消息
		pZclMsgFIFO->pZclMsgList=pNewNode;
	}else{//队列中已经有消息了
		while(pScan->pNext!=NULL){//找到队尾
			pScan=pScan->pNext;
		}
		pScan->pNext=pNewNode;
	}
	pZclMsgFIFO->MsgNum++;

	//发出通知，有新消息到来
	pthread_cond_signal(&pZclMsgFIFO->c);
	pthread_mutex_unlock(&pZclMsgFIFO->m);

	return 0;
}

//从ZclMsg消息队列中取出一条消息
//pZclMsgFIFO  - 消息队列句柄
//return 取出的消息指针，NULL表示出错
//成功后，消息数据由调用者接管，消息在使用完毕后要调用DelocateZclMsg进行销毁
stZclMsg *ZclMsgFIFOPop(stZclMsgFIFOHdl *pZclMsgFIFO)
{
	int rt;
	stZclMsgListItem *pFirstNode;
	stZclMsg *pRet=NULL;

	if(pZclMsgFIFO==NULL){
		return NULL;
	}

	pthread_mutex_lock(&pZclMsgFIFO->m);
	if(pZclMsgFIFO->MsgNum==0){//队列空时才阻塞在此等待通知
		while(1){
			rt=pthread_cond_wait(&pZclMsgFIFO->c,&pZclMsgFIFO->m);
			if(rt!=0){
				DEBUGLOG1(3,"Error, pthread_cond_wait failed %d.\n",rt);
				break;
			}
			if(pZclMsgFIFO->MsgNum>0){
				break;
			}
		}
	}
	//取出消息
	if(pZclMsgFIFO->MsgNum>0){
		pFirstNode=pZclMsgFIFO->pZclMsgList;
		pZclMsgFIFO->pZclMsgList=pZclMsgFIFO->pZclMsgList->pNext;
		pRet=pFirstNode->pZclMsg;
		free(pFirstNode);
		pZclMsgFIFO->MsgNum--;
	}
	pthread_mutex_unlock(&pZclMsgFIFO->m);

	return pRet;
}

//销毁一个ZclMsg消息队列
//pZclMsgFIFOHdl - 需销毁的消息队列句柄
void ZclMsgFIFODestroy(stZclMsgFIFOHdl *pZclMsgFIFOHdl)
{
	stZclMsgListItem *pScan=NULL;
	stZclMsgListItem *pNext=NULL;
	int rt;

	if(pZclMsgFIFOHdl==NULL){
		return;
	}

	//销毁队列中的消息
	pthread_mutex_lock(&pZclMsgFIFOHdl->m);
	pScan=pZclMsgFIFOHdl->pZclMsgList;
	while(pScan!=NULL){
		DelocateZclMsg(pScan->pZclMsg);
		pNext=pScan->pNext;
		free(pScan);
		pScan=pNext;
	}
	pthread_mutex_unlock(&pZclMsgFIFOHdl->m);

	//销毁句柄本身
	rt=pthread_mutex_destroy(&pZclMsgFIFOHdl->m);
	if(rt!=0){
		DEBUGLOG1(3,"Warning, pthread_mutex_destroy failed %d.",rt);
	}
	rt=pthread_cond_destroy(&pZclMsgFIFOHdl->c);
	if(rt!=0){
		DEBUGLOG1(3,"Warning, pthread_cond_destroy failed %d.",rt);
	}
	rt=pthread_mutex_destroy(&pZclMsgFIFOHdl->m);
	if(rt!=0){
		DEBUGLOG1(3,"Warning, pthread_mutex_destroy failed %d.",rt);
	}
	free(pZclMsgFIFOHdl);
}













