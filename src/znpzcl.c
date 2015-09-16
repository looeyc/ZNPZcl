/*
 * znpzcl.c
 *
 *  Created on: 2014-2-21
 *      Author: chengm
 */
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "debuglog.h"
#include "znplib.h"
#include "profiles.h"
#include "znpzcl.h"
#include "zclglobal.h"
#include "zclmsgfifo.h"

//全局变量
stZclStatus GlobalZclStatus;

#define MAXMSGRECORDNUM		300
#define MAXTIMEINTERVAL		2//单位为秒,即在收到一个Msg之后，如果在MAXTIMEINTERVAL时间内又收到一个相同的，则认为时重复的，否则认为不是重复的
typedef struct{
	char isValid;//0-空闲、else-已占用
	unsigned char TranSeqNum;
	unsigned short SrcAddr;
	unsigned char SrcEP;
	struct timeval RcvTime;
}stRecord_ZclMsg;

//检查ZclMsg是否是重复的
//pZclMsg - 待检查的ZclMsg
//return <0-出错，0-不是重复的，else-是重复的
//判断方法：在MAXTIMEINTERVAL的时间内如果收到SrcAddr、SrcEP、TranSeqNum相同的zcl消息，就认为是重复的消息
//为了控制资源消耗，只能存放MAXMSGRECORDNUM条zcl消息记录作为比较依据，如果在MAXTIMEINTERVAL时间内重复的消息过多，会导致用于存放zcl消息记录的数组被填满，填满后，将无法对新增的消息进行重复性判断
int IsDuplicated_ZclMsg(stZclMsg *pZclMsg)
{
	int i,iFound,iFound_SrcAddr,iFound_SrcEP,iFound_TranSeqNum,isDup=0;
	static stRecord_ZclMsg pRecordList[MAXMSGRECORDNUM];
	static int isInited=0;
	struct timeval NowTime;

	if(!isInited){
		memset(pRecordList,0,sizeof(stRecord_ZclMsg)*MAXMSGRECORDNUM);
		isInited=1;
	}
	if(pZclMsg==NULL){
		return -1;
	}

	//获取当前系统时间备用
	gettimeofday(&NowTime,NULL);
	//是否存在相同的
	iFound_SrcAddr=-1;
	iFound_SrcEP=-1;
	iFound_TranSeqNum=-1;
	for(i=0;i<MAXMSGRECORDNUM;i++){
		if(pRecordList[i].isValid){
			if(pZclMsg->ZclMsgAccessory.SrcAddr==pRecordList[i].SrcAddr){
				iFound_SrcAddr=i;
				if(pZclMsg->ZclMsgAccessory.SrcEP==pRecordList[i].SrcEP){
					iFound_SrcEP=i;
					if(pZclMsg->ZclMsgHdr.TranSeqNum==pRecordList[i].TranSeqNum){
						iFound_TranSeqNum=i;
						break;
					}
				}
			}
		}
	}
	if(iFound_TranSeqNum>=0){//如果存在完全相同的
		//检查接收时间间隔是否超过期限
		if(NowTime.tv_sec<pRecordList[iFound_TranSeqNum].RcvTime.tv_sec+MAXTIMEINTERVAL){//未超过期限,判为重复的
			isDup=1;
		}
		//更新
		pRecordList[iFound_TranSeqNum].TranSeqNum=pZclMsg->ZclMsgHdr.TranSeqNum;
		pRecordList[iFound_TranSeqNum].SrcAddr=pZclMsg->ZclMsgAccessory.SrcAddr;
		pRecordList[iFound_TranSeqNum].SrcEP=pZclMsg->ZclMsgAccessory.SrcEP;
		pRecordList[iFound_TranSeqNum].RcvTime=NowTime;
	}else{//不存在相同的
		//找到第一个空闲的或者时间间隔已经超过期限的记录，替换
		iFound=-1;
		for(i=0;i<MAXMSGRECORDNUM;i++){
			if((NowTime.tv_sec>=pRecordList[i].RcvTime.tv_sec+MAXTIMEINTERVAL)||(pRecordList[i].isValid==0)){
				iFound=i;
				break;
			}
		}
		if(iFound>=0){//找到了,替换
			pRecordList[iFound].isValid=1;
			pRecordList[iFound].TranSeqNum=pZclMsg->ZclMsgHdr.TranSeqNum;
			pRecordList[iFound].SrcAddr=pZclMsg->ZclMsgAccessory.SrcAddr;
			pRecordList[iFound].SrcEP=pZclMsg->ZclMsgAccessory.SrcEP;
			pRecordList[iFound].RcvTime=NowTime;
		}else{//未找到，说明列表已经满了
			DEBUGLOG0(3,"Warning, the RecordList of duplicated AF_INCOMING_MSG checking is full, this may be caused by too many incoming AF_INCOMING_MSG.\n");
			isDup=-3;
		}
	}
	return isDup;
}

//该回调的功能是接收来自ZNP层的消息，形成ZclMsg并进行分发处理
//所有EP的ZnpMsg都通过该函数进行处理，要注意多线程安全
void ZnpMsgCB_General(unsigned char EP, stZnpMsg *pZnpMsg)
{
	int i,rt;
	stZclFrame ZclFrame;
	stZclMsg *pZclMsg=NULL;
	stZclInst *pZclInst=NULL;

	if(pZnpMsg==NULL){
		return;
	}

	memset(&ZclFrame,0,sizeof(stZclFrame));

	switch(pZnpMsg->MsgType){
	case CMD_AF_INCOMING_MSG://按照ZCL解析
		//找到Zcl对应的注册实例
		pthread_mutex_lock(&GlobalZclStatus.mutexZclList);
		for(i=0;i<MAXZCLINSTNUM;i++){
			if(GlobalZclStatus.ZclInstList[i].isValid){
				if(GlobalZclStatus.ZclInstList[i].EP==EP){
					pZclInst=&GlobalZclStatus.ZclInstList[i];
					break;
				}
			}
		}
		pthread_mutex_unlock(&GlobalZclStatus.mutexZclList);
		if(pZclInst==NULL){//没有找到
			DEBUGLOG1(2,"Warning, can't find any ZclInst registrations for incoming znpmsg target to EP %d, discard.\n",EP);
		}else{
			//形成ZclFrame
			rt=fBytesToSt_ZclFrame(pZnpMsg->un.mAF_INCOMING_MSG.SrcAddr,pZnpMsg->un.mAF_INCOMING_MSG.SrcEndpoint,pZclInst->Profile,pZnpMsg->un.mAF_INCOMING_MSG.ClusterID,pZnpMsg->un.mAF_INCOMING_MSG.Len,pZnpMsg->un.mAF_INCOMING_MSG.Data,&ZclFrame);
			if(rt!=0){
				DEBUGLOG1(3,"Error, fBytesToSt_ZclFrame failed %d.\n",rt);
			}else{
				//形成ZclMsg
				pZclMsg=AllocateZclMsg();
				if(pZclMsg==NULL){
					DEBUGLOG0(3,"Error, AllocateZclMsg failed.\n");
				}else{
					rt=TransZclFrameToZclMsg(&ZclFrame,pZclMsg);
					if(rt!=0){
						DEBUGLOG1(3,"Error, TransZclFrameToZclMsg failed %d.\n",rt);
						DelocateZclMsg(pZclMsg);
					}else{//转换成了ZclMsg，进行分发处理
						//检查是否是重复的Zcl消息,可能会因为发送者没有接收到确认包导致的重发
						rt=IsDuplicated_ZclMsg(pZclMsg);//检查是否是重复的消息
						if(rt>0){//是重复的,丢弃
							DelocateZclMsg(pZclMsg);
							//DEBUGLOG1(0,"Discarding duplicated AF_INCOMING_MSG from 0x%04X.\n",pZnpMsg->un.mAF_INCOMING_MSG.SrcAddr);
						}else{//不是重复的或者检查失败，都送出去
							if(rt<0){//如果检查失败，则输出提示信息
								DEBUGLOG1(2,"Warning, IsDuplicated_ZclMsg failed %d.\n",rt);
							}
							//找到订阅者
							rt=ZclMsgPost(pZclMsg);
							if(rt==0){//未找到订阅者,作为默认消息处理
								rt=ZclMsgFIFOPush(pZclInst->pZclMsgFIFOHdl,pZclMsg);
								if(rt!=0){
									DEBUGLOG1(3,"Error, ZclMsgFIFOPush failed %d.\n",rt);
									DelocateZclMsg(pZclMsg);
								}
							}else if(rt<0){//出错
								DEBUGLOG1(3,"ZclMsgPost failed %d.\n",rt);
								DelocateZclMsg(pZclMsg);
							}
						}
					}
				}
			}
		}
		break;
	default:
		DEBUGLOG1(2,"Warning, discarding unexpected znpmsg 0x%04X.\n",pZnpMsg->MsgType);
		break;
	}
}

//该线程用于接收由ZnpMsgCB_General推送给各EP的Zcl消息
void *Thread_ZnpMsgRcvForEP(void *pArg)
{
	char isValid=0;
	stZclInst *pZclInst=(stZclInst *)pArg;
	stZclMsg *pZclMsg=NULL;

	if(pZclInst==NULL){
		DEBUGLOG0(3,"Error, bad paramter for Thread_ZnpMsgRcvForEP, thread exit.\n");
		pthread_exit(NULL);
	}

	//等待stZclInst注册就绪
	while(1){
		pthread_mutex_lock(&GlobalZclStatus.mutexZclList);
		isValid=pZclInst->isValid;
		pthread_mutex_unlock(&GlobalZclStatus.mutexZclList);

		if(pZclInst->isValid==1){
			break;
		}else{
			DEBUGLOG0(2,"Warning, Thread_ZnpMsgRcvForEP is waiting ZclInst to get ready.\n");
			sleep(1);
		}
	}

	while(1){
		//从消息队列中拉取消息
		pZclMsg=ZclMsgFIFOPop(pZclInst->pZclMsgFIFOHdl);
		if(pZclMsg==NULL){
			DEBUGLOG0(3,"Error, ZclMsgFIFOPop failed, Thread_ZnpMsgRcvForEP exit.\n");
			pthread_exit(NULL);
		}

		//调用注册的回调函数进行处理
		pZclInst->pDefaultCBFxn(pZclMsg);

		//销毁消息
		DelocateZclMsg(pZclMsg);
	}

	return NULL;
}

//初始化zcl
//pZclCfg - 初始化配置参数
//return 0-成功，else-失败
//要求调用者先初始化ZNP，否则结果不可预料
int ZclInit(stZclCfg *pZclCfg)
{
	//参数检查
	if(pZclCfg==NULL){
		return -1;
	}

	//初始化全局参数
	memset(&GlobalZclStatus,0,sizeof(stZclStatus));
	pthread_mutex_init(&GlobalZclStatus.mutexZclList,NULL);
	pthread_mutex_init(&GlobalZclStatus.mutexTranSeqNum,NULL);
	pthread_mutex_init(&GlobalZclStatus.mutexGlobalZclMsgSubsHdlList,NULL);

	return 0;
}

//pRegParam - 注册参数
//return 注册成功时返回Zcl实例的句柄，NULL表示注册失败
stZclInst *ZclRegistInst(stZclInstRegistParam *pRegParam)
{
	int i,Idx,rt;
	int Err=0;
	pthread_t ntid;

	//参数检查
	if(pRegParam==NULL){
		DEBUGLOG0(3,"Error, bad param.\n");
		return NULL;
	}
	if((pRegParam->EndPoint==0)||(pRegParam->pCBFxn==NULL)){
		DEBUGLOG0(3,"Error, bad param.\n");
		return NULL;
	}

	pthread_mutex_lock(&GlobalZclStatus.mutexZclList);
	//尝试在ZCL列表中找到空位
	for(i=0;i<MAXZCLINSTNUM;i++){
		if(GlobalZclStatus.ZclInstList[i].isValid){
			continue;
		}else{
			break;
		}
	}
	Idx=i;
	if(Idx>=MAXZCLINSTNUM){//没有空位
		DEBUGLOG0(3,"Error, can't create more zcl instances.\n");
		Err=1;
	}else{//找到了空位
		//该EP是否已经被注册
		for(i=0;i<MAXZCLINSTNUM;i++){
			if((GlobalZclStatus.ZclInstList[i].isValid)&&(GlobalZclStatus.ZclInstList[i].EP==pRegParam->EndPoint)){
				break;
			}
		}
		if(i<MAXZCLINSTNUM){//已经被注册
			DEBUGLOG2(2,"Warning, EP %d already registed for profile 0x%04X.\n",pRegParam->EndPoint,GlobalZclStatus.ZclInstList[pRegParam->EndPoint].Profile);
			Err=2;
		}else{//没有被注册
			//创建ZclMsg接收队列以及接收线程，该消息队列接收ZnpMsgCB_General分发来的ZclMsg消息，该接收线程从队列中读取消息并调用pRegParam中注册的回调进行处理
			GlobalZclStatus.ZclInstList[Idx].pZclMsgFIFOHdl=ZclMsgFIFOCreate();
			if(GlobalZclStatus.ZclInstList[Idx].pZclMsgFIFOHdl==NULL){
				DEBUGLOG0(3,"Error, ZclMsgFIFOCreate failed.\n");
				Err=3;
			}else{
				//创建线程Thread_ZnpMsgRcvForEP
				rt=pthread_create(&ntid,NULL,Thread_ZnpMsgRcvForEP,(void *)&GlobalZclStatus.ZclInstList[Idx]);
				if(rt!=0){
					DEBUGLOG1(3,"Error, pthread_create Thread_ZnpMsgRcvForEP failed %d.\n",rt);
					ZclMsgFIFODestroy(GlobalZclStatus.ZclInstList[Idx].pZclMsgFIFOHdl);
					Err=4;
				}else{
					//向Znp注册，Zcl对ZnpMsg的处理回调共用一个
					rt=Znp_AF_REGISTER(pRegParam->EndPoint,pRegParam->Profile,0x0000,0x00,0x00,0x00,NULL,0x00,NULL,ZnpMsgCB_General);
					if(rt!=0){
						DEBUGLOG1(3,"Error, Znp_AF_REGISTER failed %d.\n",rt);
						pthread_cancel(ntid);
						ZclMsgFIFODestroy(GlobalZclStatus.ZclInstList[Idx].pZclMsgFIFOHdl);
						Err=5;
					}else{
						GlobalZclStatus.ZclInstList[Idx].Profile=pRegParam->Profile;
						GlobalZclStatus.ZclInstList[Idx].pDefaultCBFxn=pRegParam->pCBFxn;
						GlobalZclStatus.ZclInstList[Idx].EP=pRegParam->EndPoint;
						GlobalZclStatus.ZclInstList[Idx].isValid=1;
					}
				}
			}
		}
	}
	pthread_mutex_unlock(&GlobalZclStatus.mutexZclList);

	if(Err==0){
		return &GlobalZclStatus.ZclInstList[Idx];
	}else{
		return NULL;
	}
}

//获取一个当前唯一的TranSeqNum
unsigned char ZclGenTranSeqNum()
{
	unsigned char ret;
	pthread_mutex_lock(&GlobalZclStatus.mutexTranSeqNum);
	ret=GlobalZclStatus.TranSeqNum++;
	pthread_mutex_unlock(&GlobalZclStatus.mutexTranSeqNum);
	return ret;
}

//向指定的ZclInst实例请求发送ZclMsg
//pZclInst - ZclInst实例句柄
//pRoute - 路由，如果为NULL，则忽略该参数
//DevNwkAddr - 目的设备短地址
//EP - 目的设备EndPoint
//pZclMsg - 指向待发送的ZclMsg
//NeedReply - 0-不需要等待应答、1-需要等待控制应答，即发出AF_DATA_REQUEST请求时，是否需要对方在收到后返回AF_DATA_CONFIRM
//return 0-成功，else-失败
int fZclMsgSend(stZclInst *pZclInst, stRoute *pRoute, unsigned short DevNwkAddr, unsigned char EP, stZclMsg *pZclMsg, char NeedReply)
{
	int rt,len;
	stZclFrame ZclFrame;
	unsigned char BufferLen=255;
	unsigned char Buffer[ZCL_MAXPAYLOADLEN];

	//参数检查
	if((pZclInst==NULL)||(pZclMsg==NULL)){
		return -1;
	}
	if(pZclInst->isValid!=1){
		return -1;
	}

	rt=TransZclMsgToZclFrame(pZclMsg, &ZclFrame);
	if(rt!=0){
		DEBUGLOG1(3,"Error, TransZclMsgToZclFrame failed %d.\n",rt);
		return -2;
	}

	len=fStToBytes_ZclFrame(&ZclFrame,BufferLen,Buffer);
	if(len<=0){
		DEBUGLOG1(3,"Error, fStToBytes_ZclFrame failed %d.\n",len);
		return -3;
	}

	if(pRoute==NULL){
		//rt=Znp_AF_DATA_REQUEST(DevNwkAddr,EP,pZclInst->EP,pZclMsg->ZclMsgAccessory.ClusterID,(NeedReply?AF_ACK_REQUEST:0)|AF_DISCV_ROUTE|AF_EN_SECURITY,7,len,Buffer);
		rt=Znp_AF_DATA_REQUEST(DevNwkAddr,EP,pZclInst->EP,pZclMsg->ZclMsgAccessory.ClusterID,(NeedReply?AF_ACK_REQUEST:0)|AF_DISCV_ROUTE,7,len,Buffer);
		if(rt!=0){
			DEBUGLOG1(3,"Error, Znp_AF_DATA_REQUEST failed %d.\n",rt);
			return -4;
		}
	}else{
		//rt=Znp_AF_DATA_REQUEST_SRC_RTG(DevNwkAddr,EP,pZclInst->EP,pZclMsg->ZclMsgAccessory.ClusterID,(NeedReply?AF_ACK_REQUEST:0)|AF_DISCV_ROUTE|AF_EN_SECURITY,7,pRoute->RelayCount,pRoute->RelayList,len,Buffer);
		rt=Znp_AF_DATA_REQUEST_SRC_RTG(DevNwkAddr,EP,pZclInst->EP,pZclMsg->ZclMsgAccessory.ClusterID,(NeedReply?AF_ACK_REQUEST:0)|AF_DISCV_ROUTE,7,pRoute->RelayCount,pRoute->RelayList,len,Buffer);
		if(rt!=0){
			DEBUGLOG1(3,"Error, Znp_AF_DATA_REQUEST_SRC_RTG failed %d.\n",rt);
			return -5;
		}
	}

	return 0;
}

//pZclInst - 请求的ZclInst句柄
//pRoute - 指定路由，如果为NULL则不指定路由
//DevNwkAddr - 设备短地址
//EP - Endpoint
//ClusterID - attributes所属的Cluster
//AttriNum - 需要读取的AttriID数量
//pAttriIDList - 需要读取的AttriID列表
//pResault - 调用者提供空间，用于返回读取结果
//return 0-读取操作成功,pResault可用；else-出错，pResault不可用
int ZclAttriRead(stZclInst *pZclInst, stRoute *pRoute, unsigned short DevNwkAddr, unsigned char EP, unsigned short ClusterID, unsigned char AttriNum, unsigned short *pAttriIDList, stZCL_ATTRIREAD_R *pResault)
{
	int rt,Err=0;
	stZclMsg ZclMsg_Req;
	stZclMsg *pZclMsg_Rsp=NULL;
	stZclMsgInfo ZclMsgInfo;
	stZclMsgSubsHdl *pZclMsgSubsHdl=NULL;

	//参数检查
	if((pZclInst==NULL)||(AttriNum==0)||(pResault==NULL)){
		return -1;
	}
	if(pZclInst->isValid!=1){
		return -1;
	}
	if(AttriNum>MAXATTRINUM){
		DEBUGLOG2(3,"Error, the num of attri %d read should not exceed %d.\n",AttriNum,MAXATTRINUM);
		return -1;
	}

	//创建ZclMsg请求消息
	//消息头
	ZclMsg_Req.ZclMsgAccessory.ProfileID=pZclInst->Profile;
	ZclMsg_Req.ZclMsgAccessory.ClusterID=ClusterID;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.FrameType=ZCL_FRAMETYPE_PROFILE;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.Direction=ZCL_DIR_CLIENT_TO_SERVER;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.DisableDefaultRsp=ZCL_DEFAULTRSP_ENABLE;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.Manufact=ZCL_MANUFACT_NOTEXIST;
	ZclMsg_Req.ZclMsgHdr.CmdID=ZCL_GENERAL_ATTRIREAD;
	ZclMsg_Req.ZclMsgHdr.TranSeqNum=ZclGenTranSeqNum();
	//消息内容
	ZclMsg_Req.un.GeneralCmd.mZCL_ATTRIREAD.AttriNum=AttriNum;
	memcpy(ZclMsg_Req.un.GeneralCmd.mZCL_ATTRIREAD.AttriIDList,pAttriIDList,AttriNum*2);

	//订阅应答消息
	ZclMsgInfo.CmdID=ZCL_GENERAL_ATTRIREAD_R;
	ZclMsgInfo.ProfileID=pZclInst->Profile;
	ZclMsgInfo.ClusterID=ZclMsg_Req.ZclMsgAccessory.ClusterID;
	ZclMsgInfo.TranSeqNum=ZclMsg_Req.ZclMsgHdr.TranSeqNum;
	pZclMsgSubsHdl=ZclMsgSubscribe(&ZclMsgInfo);
	if(pZclMsgSubsHdl==NULL){
		DEBUGLOG0(3,"Error, ZclMsgSubscribe failed.\n");
		return -2;
	}

	//发出请求
	rt=fZclMsgSend(pZclInst,pRoute,DevNwkAddr,EP,&ZclMsg_Req,1);
	if(rt!=0){
		DEBUGLOG1(3,"Error, fZclMsgSend failed %d.\n",rt);
		Err=-4;
	}else{
		//等待应答消息
		if(Err==0){
			pZclMsg_Rsp=ZclMsgPend(pZclMsgSubsHdl,5000);
			if(pZclMsg_Rsp==NULL){
				DEBUGLOG0(3,"Error, ZclMsgPend failed.\n");
				Err=1;
			}else{
				//解析应答
				*pResault=pZclMsg_Rsp->un.GeneralCmd.mZCL_ATTRIREAD_R;
			}
		}
	}

	//退订应答消息
	if(pZclMsgSubsHdl!=NULL){
		ZclMsgUnsubscribe(pZclMsgSubsHdl);
	}
	//释放系统资源
	if(pZclMsg_Rsp!=NULL){
		DelocateZclMsg(pZclMsg_Rsp);
	}

	return Err;
}

//写属性数据
//pZclInst - 请求的ZclInst句柄
//pRoute - 指定路由，如果为NULL则不指定路由
//DevNwkAddr - 设备短地址
//EP - Endpoint
//Cluster - attributes所属的Cluster
//AttriNum - 需要写的AttriID数量
//pAttriWriteRecordList - 需要写的Attri列表
//pResault - 调用者提供空间，用于返回读取结果
//return 0-读取操作成功,pResault可用；else-出错，pResault不可用
//如果pResault中只有一项记录，并且Status是0（成功），则表示所有的写操作都成功了，这时这唯一的一项记录中的AttriID是无效的
int ZclAttriWrite(stZclInst *pZclInst, stRoute *pRoute, unsigned short DevNwkAddr, unsigned char EP, unsigned short ClusterID, unsigned char AttriNum, stAttriWriteRecord *pAttriWriteRecordList, stZCL_ATTRIWRITE_R *pResault)
{
	int i,rt,Err=0;
	stZclMsg ZclMsg_Req;
	stZclMsg *pZclMsg_Rsp=NULL;
	stZclMsgInfo ZclMsgInfo;
	stZclMsgSubsHdl *pZclMsgSubsHdl=NULL;

	//参数检查
	if((pZclInst==NULL)||(AttriNum==0)||(pAttriWriteRecordList==NULL)||(pResault==NULL)){
		return -1;
	}
	if(pZclInst->isValid!=1){
		return -1;
	}
	if(AttriNum>MAXATTRINUM){
		DEBUGLOG2(3,"Error, the num of attri %d read should not exceed %d.\n",AttriNum,MAXATTRINUM);
		return -1;
	}

	//创建ZclMsg请求消息
	//消息头
	ZclMsg_Req.ZclMsgAccessory.ProfileID=pZclInst->Profile;
	ZclMsg_Req.ZclMsgAccessory.ClusterID=ClusterID;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.FrameType=ZCL_FRAMETYPE_PROFILE;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.Direction=ZCL_DIR_CLIENT_TO_SERVER;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.DisableDefaultRsp=ZCL_DEFAULTRSP_ENABLE;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.Manufact=ZCL_MANUFACT_NOTEXIST;
	ZclMsg_Req.ZclMsgHdr.CmdID=ZCL_GENERAL_ATTRIWRITE;
	ZclMsg_Req.ZclMsgHdr.TranSeqNum=ZclGenTranSeqNum();
	//消息内容
	ZclMsg_Req.un.GeneralCmd.mZCL_ATTRIWRITE.RecordNum=AttriNum;
	for(i=0;i<AttriNum;i++){
		ZclMsg_Req.un.GeneralCmd.mZCL_ATTRIWRITE.Records[i]=pAttriWriteRecordList[i];
	}

	//订阅应答消息
	ZclMsgInfo.CmdID=ZCL_GENERAL_ATTRIWRITE_R;
	ZclMsgInfo.ProfileID=pZclInst->Profile;
	ZclMsgInfo.ClusterID=ZclMsg_Req.ZclMsgAccessory.ClusterID;
	ZclMsgInfo.TranSeqNum=ZclMsg_Req.ZclMsgHdr.TranSeqNum;
	pZclMsgSubsHdl=ZclMsgSubscribe(&ZclMsgInfo);
	if(pZclMsgSubsHdl==NULL){
		DEBUGLOG0(3,"Error, ZclMsgSubscribe failed.\n");
		return -2;
	}

	//发出请求
	rt=fZclMsgSend(pZclInst,pRoute,DevNwkAddr,EP,&ZclMsg_Req,1);
	if(rt!=0){
		DEBUGLOG1(3,"Error, fZclMsgSend failed %d.\n",rt);
		Err=-4;
	}else{
		//等待应答消息
		if(Err==0){
			pZclMsg_Rsp=ZclMsgPend(pZclMsgSubsHdl,5000);
			if(pZclMsg_Rsp==NULL){
				DEBUGLOG0(3,"Error, ZclMsgPend failed.\n");
				Err=1;
			}else{
				//解析应答
				*pResault=pZclMsg_Rsp->un.GeneralCmd.mZCL_ATTRIWRITE_R;
			}
		}
	}

	//退订应答消息
	if(pZclMsgSubsHdl!=NULL){
		ZclMsgUnsubscribe(pZclMsgSubsHdl);
	}
	//释放系统资源
	if(pZclMsg_Rsp!=NULL){
		DelocateZclMsg(pZclMsg_Rsp);
	}

	return Err;
}

//Configure Reporting Command
//pZclInst - 请求的ZclInst句柄
//pRoute - 指定路由，如果为NULL则不指定路由
//DevNwkAddr - 设备短地址
//EP - Endpoint
//Cluster - 设置的Cluster
//pCfg - 配置参数
//pRet - 配置结果
int ZclCfgAttriReport(stZclInst *pZclInst, stRoute *pRoute, unsigned short DevNwkAddr, unsigned char EP, unsigned short ClusterID, stZCL_CFGREPORT *pCfg, stZCL_CFGREPORT_R *pResault)
{
	int rt,Err=0;
	stZclMsg ZclMsg_Req;
	stZclMsg *pZclMsg_Rsp=NULL;
	stZclMsgInfo ZclMsgInfo;
	stZclMsgSubsHdl *pZclMsgSubsHdl=NULL;

	//参数检查
	if((pZclInst==NULL)||(pCfg==NULL)||(pResault==NULL)){
		return -1;
	}
	if(pZclInst->isValid!=1){
		return -1;
	}

	//创建ZclMsg请求消息
	//消息头
	ZclMsg_Req.ZclMsgAccessory.ProfileID=pZclInst->Profile;
	ZclMsg_Req.ZclMsgAccessory.ClusterID=ClusterID;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.FrameType=ZCL_FRAMETYPE_PROFILE;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.Direction=ZCL_DIR_CLIENT_TO_SERVER;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.DisableDefaultRsp=ZCL_DEFAULTRSP_ENABLE;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.Manufact=ZCL_MANUFACT_NOTEXIST;
	ZclMsg_Req.ZclMsgHdr.CmdID=ZCL_GENERAL_CFGREPORT;
	ZclMsg_Req.ZclMsgHdr.TranSeqNum=ZclGenTranSeqNum();
	//消息内容
	ZclMsg_Req.un.GeneralCmd.mZCL_CFGREPORT=*pCfg;

	//订阅应答消息
	ZclMsgInfo.CmdID=ZCL_GENERAL_CFGREPORT_R;
	ZclMsgInfo.ProfileID=pZclInst->Profile;
	ZclMsgInfo.ClusterID=ZclMsg_Req.ZclMsgAccessory.ClusterID;
	ZclMsgInfo.TranSeqNum=ZclMsg_Req.ZclMsgHdr.TranSeqNum;
	pZclMsgSubsHdl=ZclMsgSubscribe(&ZclMsgInfo);
	if(pZclMsgSubsHdl==NULL){
		DEBUGLOG0(3,"Error, ZclMsgSubscribe failed.\n");
		return -4;
	}

	//发出请求
	rt=fZclMsgSend(pZclInst,pRoute,DevNwkAddr,EP,&ZclMsg_Req,1);
	if(rt!=0){
		DEBUGLOG1(3,"Error, fZclMsgSend failed %d.\n",rt);
		Err=-5;
	}else{
		//等待应答消息
		if(Err==0){
			pZclMsg_Rsp=ZclMsgPend(pZclMsgSubsHdl,5000);
			if(pZclMsg_Rsp==NULL){
				DEBUGLOG0(3,"Error, ZclMsgPend failed.\n");
				Err=1;
			}else{
				//返回应答消息
				*pResault=pZclMsg_Rsp->un.GeneralCmd.mZCL_CFGREPORT_R;
			}
		}
	}

	//退订应答消息
	if(pZclMsgSubsHdl!=NULL){
		ZclMsgUnsubscribe(pZclMsgSubsHdl);
	}
	//释放系统资源
	if(pZclMsg_Rsp!=NULL){
		DelocateZclMsg(pZclMsg_Rsp);
	}
	return Err;
}

//对指定设备的指定Cluster进行控制----------------------------------------------------------------------------------------------------------------
//pZclInst - 请求的ZclInst句柄，这里包含有Profile信息
//pRoute - 指定路由，如果为NULL则不指定路由
//DevNwkAddr - 目的设备短地址
//EP - 目的Endpoint
//Cluster - 目标Cluster
//pClusterCmd - 需要执行的Cluster命令
//NeedReply - 0-不需要等待控制应答、1-需要等待控制应答
//return 0-成功，else-失败
//说明：如果控制需要等待应答，以确保控制执行到位，则NeedReply填1，否则填0.对于某些出于休眠模式的EndDevice，控制命令是由其父节点缓存起来的，这类命令如果等待应答的话就会超时，所以这种情况应答将NeedReply填0，即无需应答。
int ZclClusterCtrl(stZclInst *pZclInst, stRoute *pRoute, unsigned short DevNwkAddr, unsigned char EP, unsigned short ClusterID, unsigned char Cmd, unClusterCmd *pClusterCmd, char NeedReply)
{
	int rt,Err=0;
	stZclMsg ZclMsg_Req;
	stZclMsg *pZclMsg_Rsp=NULL;
	stZclMsgInfo ZclMsgInfo;
	stZclMsgSubsHdl *pZclMsgSubsHdl=NULL;

	//参数检查
	if(pZclInst==NULL){
		return -1;
	}
	if(pZclInst->isValid!=1){
		return -1;
	}

	//创建ZclMsg请求消息
	//消息头
	ZclMsg_Req.ZclMsgAccessory.ProfileID=pZclInst->Profile;
	ZclMsg_Req.ZclMsgAccessory.ClusterID=ClusterID;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.FrameType=ZCL_FRAMETYPE_CLUSTER;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.Direction=ZCL_DIR_CLIENT_TO_SERVER;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.DisableDefaultRsp=ZCL_DEFAULTRSP_ENABLE;
	ZclMsg_Req.ZclMsgHdr.FrameCtrl.Manufact=ZCL_MANUFACT_NOTEXIST;
	ZclMsg_Req.ZclMsgHdr.CmdID=Cmd;
	ZclMsg_Req.ZclMsgHdr.TranSeqNum=ZclGenTranSeqNum();
	//消息内容
	switch(ClusterID){
	case CLUSTER_IDENTIFY:
		switch(Cmd){
		case CMD_IDENTIFY_IDENTIFY:
			if(pClusterCmd==NULL){
				return -1;
			}
			ZclMsg_Req.un.ClusterCmd.unClusterCmd_Identify.mCMD_IDENTIFY_IDENTIFY=pClusterCmd->unClusterCmd_Identify.mCMD_IDENTIFY_IDENTIFY;
			break;
		default:
			DEBUGLOG1(3,"Error, undefined cmd %d for CLUSTER_IDENTIFY.\n",Cmd);
			return -3;
			break;
		}
		break;
	case CLUSTER_ONOFF://该Cluster的控制命令没有内容部分
		break;
	case CLUSTER_ALARMS:
		switch(Cmd){
		case CMD_ALARM_RESETALLALARMS://没有内容部分
			break;
		default:
			DEBUGLOG1(3,"Error, undefined cmd %d for cluster CLUSTER_ALARMS.\n",Cmd);
			return -3;
			break;
		}
		break;
	//自定义Cluster
	case CLUSTER_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG:
		switch(Cmd){
		case CMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_ADJUST:
			break;
		case CMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG:
			ZclMsg_Req.un.ClusterCmd.unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG=pClusterCmd->unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG;
			break;
		default:
			DEBUGLOG1(3,"Error, undefined cmd %d for cluster CLUSTER_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG.\n",Cmd);
			return -3;
			break;
		}
		break;
	case CLUSTER_SELFINTRODUCTION:
		break;
	default:
		DEBUGLOG1(3,"Error, undefined cluster type %d.\n",ClusterID);
		return -3;
		break;
	}

	if(NeedReply){//需要等待应答
		//订阅应答消息
		ZclMsgInfo.CmdID=ZCL_GENERAL_DEFAULTRSP;
		ZclMsgInfo.ProfileID=pZclInst->Profile;
		ZclMsgInfo.ClusterID=ZclMsg_Req.ZclMsgAccessory.ClusterID;
		ZclMsgInfo.TranSeqNum=ZclMsg_Req.ZclMsgHdr.TranSeqNum;
		pZclMsgSubsHdl=ZclMsgSubscribe(&ZclMsgInfo);
		if(pZclMsgSubsHdl==NULL){
			DEBUGLOG0(3,"Error, ZclMsgSubscribe failed.\n");
			return -4;
		}

		//发出请求
		rt=fZclMsgSend(pZclInst,pRoute,DevNwkAddr,EP,&ZclMsg_Req,1);
		if(rt!=0){
			DEBUGLOG1(3,"Error, fZclMsgSend failed %d.\n",rt);
			Err=-5;
		}else{
			//等待应答消息
			if(Err==0){
				pZclMsg_Rsp=ZclMsgPend(pZclMsgSubsHdl,5000);
				if(pZclMsg_Rsp==NULL){
					DEBUGLOG0(3,"Error, ZclMsgPend failed.\n");
					Err=1;
				}else{
					//解析应答消息
					if(pZclMsg_Rsp->un.GeneralCmd.mZCL_DEFAULTRSP.CmdID!=Cmd){
						DEBUGLOG0(3,"Error, bad CmdID of default rsp.\n");
						Err=2;
					}else{
						if(pZclMsg_Rsp->un.GeneralCmd.mZCL_DEFAULTRSP.Status!=0){
							Err=3;
						}
					}
				}
			}
		}

		//退订应答消息
		if(pZclMsgSubsHdl!=NULL){
			ZclMsgUnsubscribe(pZclMsgSubsHdl);
		}
		//释放系统资源
		if(pZclMsg_Rsp!=NULL){
			DelocateZclMsg(pZclMsg_Rsp);
		}
	}else{//不需要等待应答
		//发出请求
		rt=fZclMsgSend(pZclInst,pRoute,DevNwkAddr,EP,&ZclMsg_Req,0);
		if(rt!=0){
			DEBUGLOG1(3,"Error, fZclMsgSend failed %d.\n",rt);
			Err=-5;
		}
	}

	return Err;
}



