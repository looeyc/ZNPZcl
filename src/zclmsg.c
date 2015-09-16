/*
 * zclmsg.c
 *
 *  Created on: 2014-2-21
 *      Author: chengm
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>
#include "debuglog.h"
#include "alige.h"
#include "znplib.h"
#include "profiles.h"
#include "zclstatus.h"
#include "zclglobal.h"
#include "zclmsg.h"

//创建一个stZclMsg
//return NULL表示创建失败
//创建的stZclMsg在使用完毕后一定要调用DelocateZclMsg销毁
stZclMsg *AllocateZclMsg()
{
	stZclMsg *pRet=NULL;

	pRet=(stZclMsg *)malloc(sizeof(stZclMsg));
	if(pRet==NULL){
		return NULL;
	}
	memset(pRet,0,sizeof(stZclMsg));
	return pRet;
}

//销毁一个ZclFrame
void DelocateZclMsg(stZclMsg *pZclMsg)
{
	if(pZclMsg!=NULL){
		free(pZclMsg);
	}
}

//生成ZclFrame中的传输流水号TranSeqNum
unsigned char GenZclMsgTranSeqNum()
{
	unsigned char Ret;

	pthread_mutex_lock(&GlobalZclStatus.mutexTranSeqNum);
	Ret=GlobalZclStatus.TranSeqNum++;
	pthread_mutex_unlock(&GlobalZclStatus.mutexTranSeqNum);

	return Ret;
}

//转换ZclMsg -> ZclFrame
//pZclMsg - 待转换的pZclMsg
//pZclFrame - 调用者提供空间，用于返回转换结果
//return 0-成功，else-失败
int TransZclMsgToZclFrame(stZclMsg *pZclMsg, stZclFrame *pZclFrame)
{
	int i,Err=0;
	int iWrite=0;

	if((pZclMsg==NULL)||(pZclFrame==NULL)){
		return -1;
	}

	memset(pZclFrame,0,sizeof(stZclFrame));

	//附带属性
	pZclFrame->SrcAddr=pZclMsg->ZclMsgAccessory.SrcAddr;
	pZclFrame->SrcEP=pZclMsg->ZclMsgAccessory.SrcEP;
	pZclFrame->ProfileID=pZclMsg->ZclMsgAccessory.ProfileID;
	pZclFrame->ClusterID=pZclMsg->ZclMsgAccessory.ClusterID;

	//共同部分
	pZclFrame->FrameCtrl=((pZclMsg->ZclMsgHdr.FrameCtrl.DisableDefaultRsp<<4)&0x10)|((pZclMsg->ZclMsgHdr.FrameCtrl.Direction<<3)&0x08)|((pZclMsg->ZclMsgHdr.FrameCtrl.Manufact<<2)&0x4)|((pZclMsg->ZclMsgHdr.FrameCtrl.FrameType)&0x03);
	pZclFrame->ManufactCode=pZclMsg->ZclMsgHdr.ManufactCode;
	pZclFrame->TranSeqNum=pZclMsg->ZclMsgHdr.TranSeqNum;
	pZclFrame->CmdID=pZclMsg->ZclMsgHdr.CmdID;

	//Payload部分
	switch(pZclMsg->ZclMsgHdr.FrameCtrl.FrameType){
	case ZCL_FRAMETYPE_PROFILE:
		switch(pZclMsg->ZclMsgHdr.CmdID){
		case ZCL_GENERAL_ATTRIREAD:
			if(pZclMsg->un.GeneralCmd.mZCL_ATTRIREAD.AttriNum>MAXATTRINUM){
				DEBUGLOG2(3,"Error, too many attributes to read %d>%d.\n",pZclMsg->un.GeneralCmd.mZCL_ATTRIREAD.AttriNum,MAXATTRINUM);
				Err=-1;
			}else{
				pZclFrame->PayLoadLen=pZclMsg->un.GeneralCmd.mZCL_ATTRIREAD.AttriNum*2;
				memcpy(pZclFrame->PayLoad,pZclMsg->un.GeneralCmd.mZCL_ATTRIREAD.AttriIDList,pZclFrame->PayLoadLen);
			}
			break;
		case ZCL_GENERAL_ATTRIWRITE:
			if(pZclMsg->un.GeneralCmd.mZCL_ATTRIWRITE.RecordNum>MAXATTRINUM){
				DEBUGLOG2(3,"Error, too many attributes to write %d>%d.\n",pZclMsg->un.GeneralCmd.mZCL_ATTRIWRITE.RecordNum,MAXATTRINUM);
				Err=-1;
			}else{
				for(i=0;i<pZclMsg->un.GeneralCmd.mZCL_ATTRIWRITE.RecordNum;i++){
					UNALIGEWRITE_16BIT(pZclFrame->PayLoad+iWrite,pZclMsg->un.GeneralCmd.mZCL_ATTRIWRITE.Records[i].AttriID);iWrite+=2;
					pZclFrame->PayLoad[iWrite]=pZclMsg->un.GeneralCmd.mZCL_ATTRIWRITE.Records[i].AttriData.ZclDataType;iWrite++;
					memcpy(pZclFrame->PayLoad+iWrite,pZclMsg->un.GeneralCmd.mZCL_ATTRIWRITE.Records[i].AttriData.Data,pZclMsg->un.GeneralCmd.mZCL_ATTRIWRITE.Records[i].AttriData.DataSize);iWrite+=pZclMsg->un.GeneralCmd.mZCL_ATTRIWRITE.Records[i].AttriData.DataSize;
				}
				pZclFrame->PayLoadLen=iWrite;
				if(pZclFrame->PayLoadLen>ZCL_MAXPAYLOADLEN){
					DEBUGLOG2(3,"Error, payload data lenth of ZclFrame is too long %d>%d.\n",pZclFrame->PayLoadLen,ZCL_MAXPAYLOADLEN);
					Err=-1;
				}
			}
			break;
		//case ZCL_GENERAL_ATTRIWRITEUNDIV:
		//	break;
		case ZCL_GENERAL_CFGREPORT:
			if(pZclMsg->un.GeneralCmd.mZCL_CFGREPORT.RecordNum>MAXATTRINUM){
				DEBUGLOG0(3,"Error, too many attributes to report.\n");
				Err=-1;
			}else{
				for(i=0;i<pZclMsg->un.GeneralCmd.mZCL_CFGREPORT.RecordNum;i++){
					pZclFrame->PayLoad[iWrite]=pZclMsg->un.GeneralCmd.mZCL_CFGREPORT.AttriReportCfgRecord[i].Direction;iWrite++;
					UNALIGEWRITE_16BIT(pZclFrame->PayLoad+iWrite,pZclMsg->un.GeneralCmd.mZCL_CFGREPORT.AttriReportCfgRecord[i].AttriID);iWrite+=2;
					if(pZclMsg->un.GeneralCmd.mZCL_CFGREPORT.AttriReportCfgRecord[i].Direction==0x00){
						pZclFrame->PayLoad[iWrite]=pZclMsg->un.GeneralCmd.mZCL_CFGREPORT.AttriReportCfgRecord[i].DataType;iWrite++;
						UNALIGEWRITE_16BIT(pZclFrame->PayLoad+iWrite,pZclMsg->un.GeneralCmd.mZCL_CFGREPORT.AttriReportCfgRecord[i].MinInterval);iWrite+=2;
						UNALIGEWRITE_16BIT(pZclFrame->PayLoad+iWrite,pZclMsg->un.GeneralCmd.mZCL_CFGREPORT.AttriReportCfgRecord[i].MaxInterval);iWrite+=2;
						memcpy(pZclFrame->PayLoad+iWrite,pZclMsg->un.GeneralCmd.mZCL_CFGREPORT.AttriReportCfgRecord[i].ReportableChange.Data,pZclMsg->un.GeneralCmd.mZCL_CFGREPORT.AttriReportCfgRecord[i].ReportableChange.DataSize);iWrite+=pZclMsg->un.GeneralCmd.mZCL_CFGREPORT.AttriReportCfgRecord[i].ReportableChange.DataSize;
					}else{
						UNALIGEWRITE_16BIT(pZclFrame->PayLoad+iWrite,pZclMsg->un.GeneralCmd.mZCL_CFGREPORT.AttriReportCfgRecord[i].TimeoutPeriod);iWrite+=2;
					}
				}
				pZclFrame->PayLoadLen=iWrite;
			}
			break;
		//case ZCL_GENERAL_REPORTCFGREAD:
		//	break;
		case ZCL_GENERAL_DISCVATTRI:
			if(pZclMsg->un.GeneralCmd.mZCL_ATTRIDISCV.MaxAttriIDNum>MAXATTRIDISVNUM){
				DEBUGLOG2(3,"Error, too many attributes to discover %d>%d.\n",pZclMsg->un.GeneralCmd.mZCL_ATTRIDISCV.MaxAttriIDNum,MAXATTRIDISVNUM);
				Err=-1;
			}else{
				pZclFrame->PayLoadLen=3;
				UNALIGEWRITE_16BIT(pZclFrame->PayLoad,pZclMsg->un.GeneralCmd.mZCL_ATTRIDISCV.StartAttriID);
				pZclFrame->PayLoad[2]=pZclMsg->un.GeneralCmd.mZCL_ATTRIDISCV.MaxAttriIDNum;
			}
			break;
		default:
			DEBUGLOG1(3,"Error, unsupported general cmd 0x%02X.\n",pZclMsg->ZclMsgHdr.CmdID);
			Err=-2;
			break;
		}
		break;
	case ZCL_FRAMETYPE_CLUSTER:
		switch(pZclMsg->ZclMsgAccessory.ClusterID){
		case CLUSTER_ONOFF://无Payload
			break;
		case CLUSTER_ALARMS:
			switch(pZclMsg->ZclMsgHdr.CmdID){
			case CMD_ALARM_RESETALLALARMS:
			case CMD_ALARM_GETALARM:
			case CMD_ALARM_RESETALLALARMLOG:
				break;//无Payload
			case CMD_ALARM_RESETALARM:
				pZclFrame->PayLoadLen=3;
				pZclFrame->PayLoad[0]=pZclMsg->un.ClusterCmd.unClusterCmd_Alarm.mCMD_ALARM_RESETALARM.AlarmCode;
				UNALIGEWRITE_16BIT(pZclFrame->PayLoad+1,pZclMsg->un.ClusterCmd.unClusterCmd_Alarm.mCMD_ALARM_RESETALARM.ClusterID);
				break;
			default:
				DEBUGLOG1(3,"Error, unsupported cmd %d for CLUSTER_ALARMS.\n",pZclMsg->ZclMsgHdr.CmdID);
				Err=-3;
				break;
			}
			break;
		case CLUSTER_IDENTIFY:
			switch(pZclMsg->ZclMsgHdr.CmdID){
			case CMD_IDENTIFY_IDENTIFY:
				pZclFrame->PayLoadLen=2;
				UNALIGEWRITE_16BIT(pZclFrame->PayLoad,pZclMsg->un.ClusterCmd.unClusterCmd_Identify.mCMD_IDENTIFY_IDENTIFY.IdentifyTime);
				break;
			case CMD_IDENTIFY_IDENTIFYQUERY:
				break;//无Payload
			default:
				DEBUGLOG1(3,"Error, unsupported cmd %d for CLUSTER_IDENTIFY.\n",pZclMsg->ZclMsgHdr.CmdID);
				Err=-3;
				break;
			}
			break;
		//自定义Cluster
		case CLUSTER_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG:
			switch(pZclMsg->ZclMsgHdr.CmdID){
			case CMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_ADJUST:
				break;
			case CMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG:
				pZclFrame->PayLoadLen=21;
				UNALIGEWRITE_16BIT(pZclFrame->PayLoad,pZclMsg->un.ClusterCmd.unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG.TS);iWrite+=2;
				UNALIGEWRITE_16BIT(pZclFrame->PayLoad+iWrite,pZclMsg->un.ClusterCmd.unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG.TDMA);iWrite+=2;
				UNALIGEWRITE_16BIT(pZclFrame->PayLoad+iWrite,pZclMsg->un.ClusterCmd.unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG.TDME);iWrite+=2;
				UNALIGEWRITE_16BIT(pZclFrame->PayLoad+iWrite,pZclMsg->un.ClusterCmd.unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG.TBME);iWrite+=2;
				UNALIGEWRITE_16BIT(pZclFrame->PayLoad+iWrite,pZclMsg->un.ClusterCmd.unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG.TPZ);iWrite+=2;
				UNALIGEWRITE_16BIT(pZclFrame->PayLoad+iWrite,pZclMsg->un.ClusterCmd.unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG.TP3);iWrite+=2;
				UNALIGEWRITE_16BIT(pZclFrame->PayLoad+iWrite,pZclMsg->un.ClusterCmd.unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG.TPVS);iWrite+=2;
				UNALIGEWRITE_16BIT(pZclFrame->PayLoad+iWrite,pZclMsg->un.ClusterCmd.unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG.TPVL);iWrite+=2;
				pZclFrame->PayLoad[iWrite]=pZclMsg->un.ClusterCmd.unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG.WMODE;iWrite++;
				pZclFrame->PayLoad[iWrite]=pZclMsg->un.ClusterCmd.unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG.RHMN;iWrite++;
				pZclFrame->PayLoad[iWrite]=pZclMsg->un.ClusterCmd.unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG.PMF;iWrite++;
				pZclFrame->PayLoad[iWrite]=pZclMsg->un.ClusterCmd.unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG.PMN;iWrite++;
				pZclFrame->PayLoad[iWrite]=pZclMsg->un.ClusterCmd.unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag.mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG.PRPT;iWrite++;
				break;
			default:
				DEBUGLOG1(3,"Error, unsupported cmd %d for CLUSTER_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG.\n",pZclMsg->ZclMsgHdr.CmdID);
				Err=-3;
				break;
			}
			break;//无Payload
		case CLUSTER_SELFINTRODUCTION:
			break;//无Payload
		default:
			DEBUGLOG2(3,"Error, Cluster 0x%04X is not supported in profile 0x%04X yet.\n",pZclMsg->ZclMsgAccessory.ClusterID,pZclMsg->ZclMsgAccessory.ProfileID);
			Err=-4;
			break;
		}
		break;
	default:
		DEBUGLOG1(3,"Error, illegal ZclMsg FrameType %d.\n",pZclMsg->ZclMsgHdr.FrameCtrl.FrameType);
		Err=-5;
		break;
	}

	return Err;
}

//转换ZclFrame -> ZclMsg
//pZclFrame - 待转换的ZclFrame
//pZclMsg - 调用者提供空间，用于返回转换结果
//return 0-成功，else-失败
int TransZclFrameToZclMsg(stZclFrame *pZclFrame, stZclMsg *pZclMsg)
{
	int i=0,len=0,ReadPos=0;
	unsigned short AttriID=0;
	unsigned char DataType=0;
	unsigned char DataLeftLen=0;
	unsigned char Status=0;
	stZclDataUnit ZclDataUnit;

	if((pZclMsg==NULL)||(pZclFrame==NULL)){
		return -1;
	}
	memset(pZclMsg,0,sizeof(stZclMsg));

	//附带属性
	pZclMsg->ZclMsgAccessory.SrcAddr=pZclFrame->SrcAddr;
	pZclMsg->ZclMsgAccessory.SrcEP=pZclFrame->SrcEP;
	pZclMsg->ZclMsgAccessory.ProfileID=pZclFrame->ProfileID;
	pZclMsg->ZclMsgAccessory.ClusterID=pZclFrame->ClusterID;

	//共同部分
	pZclMsg->ZclMsgHdr.FrameCtrl.DisableDefaultRsp=(pZclFrame->FrameCtrl&0x10)>>4;
	pZclMsg->ZclMsgHdr.FrameCtrl.Direction=(pZclFrame->FrameCtrl&0x08)>>3;
	pZclMsg->ZclMsgHdr.FrameCtrl.Manufact=(pZclFrame->FrameCtrl&0x04)>>2;
	pZclMsg->ZclMsgHdr.FrameCtrl.FrameType=pZclFrame->FrameCtrl&0x03;
	pZclMsg->ZclMsgHdr.ManufactCode=pZclFrame->ManufactCode;
	pZclMsg->ZclMsgHdr.TranSeqNum=pZclFrame->TranSeqNum;
	pZclMsg->ZclMsgHdr.CmdID=pZclFrame->CmdID;

	DataLeftLen=pZclFrame->PayLoadLen;

	switch(pZclMsg->ZclMsgHdr.FrameCtrl.FrameType){
	case ZCL_FRAMETYPE_PROFILE:
		switch(pZclMsg->ZclMsgHdr.CmdID){
		case ZCL_GENERAL_ATTRIREAD_R:
			if(DataLeftLen<3){
				return -3;
			}
			while(1){
				if(DataLeftLen<3){
					break;
				}
				AttriID=UNALIGEREAD_16BIT(pZclFrame->PayLoad+ReadPos);ReadPos+=2;DataLeftLen-=2;
				Status=pZclFrame->PayLoad[ReadPos];ReadPos++;DataLeftLen--;
				if(Status==0){
					DataType=pZclFrame->PayLoad[ReadPos];ReadPos++;DataLeftLen--;
					len=ZclDataTypeRead(DataType,DataLeftLen,pZclFrame->PayLoad+ReadPos,&ZclDataUnit);
					if(len<0){//读取出错
						DEBUGLOG1(3,"Error, ZclDataTypeRead failed %d.\n",len);
						break;
					}else if(len==0){//没有读取到数据，说明没有更多数据了，读取结束
						break;
					}else{//读取成功
						ReadPos+=len;DataLeftLen-=len;
					}
				}
				pZclMsg->un.GeneralCmd.mZCL_ATTRIREAD_R.RecordNum++;
				pZclMsg->un.GeneralCmd.mZCL_ATTRIREAD_R.Records[i].AttriID=AttriID;
				pZclMsg->un.GeneralCmd.mZCL_ATTRIREAD_R.Records[i].Status=Status;
				pZclMsg->un.GeneralCmd.mZCL_ATTRIREAD_R.Records[i].AttriData=ZclDataUnit;
				i++;
			}
			break;
		case ZCL_GENERAL_ATTRIWRITE_R:
			if(DataLeftLen<1){
				return -3;
			}
			if(DataLeftLen==1){//成功时只有一个status字节
				Status=pZclFrame->PayLoad[0];
				pZclMsg->un.GeneralCmd.mZCL_ATTRIWRITE_R.RecordNum=1;
			}else{
				while(1){
					if(DataLeftLen<3){
						break;
					}
					Status=pZclFrame->PayLoad[ReadPos];ReadPos++;DataLeftLen--;
					AttriID=UNALIGEREAD_16BIT(pZclFrame->PayLoad+ReadPos);ReadPos+=2;DataLeftLen-=2;
					pZclMsg->un.GeneralCmd.mZCL_ATTRIWRITE_R.Records[i].AttriID=AttriID;
					pZclMsg->un.GeneralCmd.mZCL_ATTRIWRITE_R.Records[i].Status=Status;
					pZclMsg->un.GeneralCmd.mZCL_ATTRIWRITE_R.RecordNum++;
				}
			}
			break;
		case ZCL_GENERAL_CFGREPORT_R:
			while(1){
				if(DataLeftLen<1){
					break;
				}
				Status=pZclFrame->PayLoad[ReadPos];ReadPos++;DataLeftLen--;
				if(Status!=0){
					pZclMsg->un.GeneralCmd.mZCL_CFGREPORT_R.AttriStatusRecord[i].Direction=pZclFrame->PayLoad[ReadPos];ReadPos++;DataLeftLen--;
					pZclMsg->un.GeneralCmd.mZCL_CFGREPORT_R.AttriStatusRecord[i].AttriID=UNALIGEREAD_16BIT(pZclFrame->PayLoad+ReadPos);ReadPos+=2;DataLeftLen-=2;
				}
				pZclMsg->un.GeneralCmd.mZCL_CFGREPORT_R.AttriStatusRecord[i].Status=Status;
				pZclMsg->un.GeneralCmd.mZCL_CFGREPORT_R.RecordNum++;
				i++;
			}
			break;
		//case ZCL_GENERAL_REPORTCFGREAD_R:
		//	break;
		case ZCL_GENERAL_ATTRIREPORT:
			if(DataLeftLen<3){
				return -3;
			}
			while(1){
				AttriID=UNALIGEREAD_16BIT(pZclFrame->PayLoad+ReadPos);ReadPos+=2;DataLeftLen-=2;
				DataType=pZclFrame->PayLoad[ReadPos];ReadPos++;DataLeftLen--;
				len=ZclDataTypeRead(DataType,DataLeftLen,pZclFrame->PayLoad+ReadPos,&ZclDataUnit);
				if(len<0){//读取出错
					DEBUGLOG1(3,"Error, ZclDataTypeRead failed %d.\n",len);
					return -4;
				}else if(len==0){//读取完成
					break;
				}else{
					pZclMsg->un.GeneralCmd.mZCL_ATTRIREPORT.AttriReportRecord[i].AttriID=AttriID;
					pZclMsg->un.GeneralCmd.mZCL_ATTRIREPORT.AttriReportRecord[i].AttriData=ZclDataUnit;
					pZclMsg->un.GeneralCmd.mZCL_ATTRIREPORT.AttriNum++;
					i++;
					ReadPos+=len;DataLeftLen-=len;
				}
			}
			break;
		case ZCL_GENERAL_DEFAULTRSP:
			if(pZclFrame->PayLoadLen<2){
				DEBUGLOG1(3,"Error, bad PayLoadLen %d!\n",pZclFrame->PayLoadLen);
				return -5;
			}
			pZclMsg->un.GeneralCmd.mZCL_DEFAULTRSP.CmdID=pZclFrame->PayLoad[0];
			pZclMsg->un.GeneralCmd.mZCL_DEFAULTRSP.Status=pZclFrame->PayLoad[1];
			break;
		case ZCL_GENERAL_DISCVATTRI_R:
			if(DataLeftLen<1){
				DEBUGLOG1(3,"Error, bad PayLoadLen %d!\n",pZclFrame->PayLoadLen);
				return -6;
			}
			pZclMsg->un.GeneralCmd.mZCL_ATTRIDISCV_R.isComplete=pZclFrame->PayLoad[0];ReadPos++;DataLeftLen--;
			if(DataLeftLen>=3){
				while(1){
					pZclMsg->un.GeneralCmd.mZCL_ATTRIDISCV_R.AttriInfo[i].AttriID=UNALIGEREAD_16BIT(pZclFrame->PayLoad+ReadPos);ReadPos+=2;DataLeftLen-=2;
					pZclMsg->un.GeneralCmd.mZCL_ATTRIDISCV_R.AttriInfo[i].AttriDataType=pZclFrame->PayLoad[ReadPos];ReadPos++;DataLeftLen--;
					i++;
					if((DataLeftLen<3)||(i>=MAXATTRIDISVNUM)){
						break;
					}
				}
			}
			break;
		//case ZCL_GENERAL_ATTRIREADSTR:
		//	break;
		//case ZCL_GENERAL_ATTRIWRITESTR:
		//	break;
		//case ZCL_GENERAL_ATTRIWRITESTR_R:
		//	break;
		default:
			DEBUGLOG1(3,"Error, unsupported general cluster cmd 0x%02X!\n",pZclFrame->CmdID);
			return -7;
			break;
		}
		break;
	case ZCL_FRAMETYPE_CLUSTER:
		switch(pZclFrame->ClusterID){
		case CLUSTER_IASZONE:
			switch(pZclFrame->CmdID){
			case CMD_IASZONE_ZONESTATUSCHANGENOTIF:
				if(DataLeftLen<3){
					return -12;
				}
				pZclMsg->un.ClusterCmd.unClusterCmd_IASZone.mCMD_IASZONE_ZONESTATUSCHANGENOTIF.ZoneStatus=UNALIGEREAD_16BIT(pZclFrame->PayLoad+ReadPos);ReadPos+=2;DataLeftLen-=2;
				pZclMsg->un.ClusterCmd.unClusterCmd_IASZone.mCMD_IASZONE_ZONESTATUSCHANGENOTIF.ExtStatus=pZclFrame->PayLoad[ReadPos];ReadPos++;DataLeftLen--;
				break;
			default:
				DEBUGLOG1(3,"Error, unsupported cmd %d for HA_IAS_ZONE cluster.\n",pZclFrame->CmdID);
				return -11;
				break;
			}
			break;
		case CLUSTER_ALARMS:
			switch(pZclFrame->CmdID){
			case CMD_ALARM_ALARM:
				pZclMsg->un.ClusterCmd.unClusterCmd_Alarm.mCMD_ALARM_ALARM.AlarmCode=pZclFrame->PayLoad[0];
				pZclMsg->un.ClusterCmd.unClusterCmd_Alarm.mCMD_ALARM_ALARM.ClusterID=UNALIGEREAD_16BIT(pZclFrame->PayLoad+1);
				break;
			case CMD_ALARM_GETALARMRESPONSE:
				pZclMsg->un.ClusterCmd.unClusterCmd_Alarm.mCMD_ALARM_GETALARMRESPONSE.Status=pZclFrame->PayLoad[0];
				if(pZclMsg->un.ClusterCmd.unClusterCmd_Alarm.mCMD_ALARM_GETALARMRESPONSE.Status==0){
					pZclMsg->un.ClusterCmd.unClusterCmd_Alarm.mCMD_ALARM_GETALARMRESPONSE.AlarmCode=pZclFrame->PayLoad[1];
					pZclMsg->un.ClusterCmd.unClusterCmd_Alarm.mCMD_ALARM_GETALARMRESPONSE.ClusterID=UNALIGEREAD_16BIT(pZclFrame->PayLoad+2);
					pZclMsg->un.ClusterCmd.unClusterCmd_Alarm.mCMD_ALARM_GETALARMRESPONSE.TimeStamp=UNALIGEREAD_16BIT(pZclFrame->PayLoad+4);
				}
				break;
			default:
				DEBUGLOG1(3,"Error, unsupported cmd %d for HA_GEN_POWERCFG cluster.\n",pZclFrame->CmdID);
				return -11;
				break;
			}
			break;
		case CLUSTER_IDENTIFY:
			switch(pZclFrame->CmdID){
			case CMD_IDENTIFY_IDENTIFYQUERYRSP:
				pZclMsg->un.ClusterCmd.unClusterCmd_Identify.mCMD_IDENTIFY_IDENTIFYQUERYRSP.Timeout=UNALIGEREAD_16BIT(pZclFrame->PayLoad);
				break;
			default:
				DEBUGLOG1(3,"Error, unsupported cmd %d for HA_GEN_IDENTIFY cluster.\n",pZclFrame->CmdID);
				return -11;
				break;
			}
			break;
		case CLUSTER_SELFINTRODUCTION:
			switch(pZclFrame->CmdID){
			case CMD_SELFINTRODUCTION_INTRODUCE_RSP:
				if(DataLeftLen<33){
					DEBUGLOG0(3,"Error, bad zcl frame data.\n");
					return -11;
				}
				ReadPos=0;
				memcpy(pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.IEEEAddr,pZclFrame->PayLoad,8);					ReadPos+=8;
				memcpy(pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.UserDescriptor,pZclFrame->PayLoad+ReadPos,16);	ReadPos+=16;
				memcpy(&pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.NwkAddr,pZclFrame->PayLoad+ReadPos,2);			ReadPos+=2;
				memcpy(&pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.ProfileID,pZclFrame->PayLoad+ReadPos,2);		ReadPos+=2;
				memcpy(&pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.DeviceID,pZclFrame->PayLoad+ReadPos,2);		ReadPos+=2;
				memcpy(&pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.EP,pZclFrame->PayLoad+ReadPos,1);				ReadPos+=1;
				memcpy(&pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.NumInClusters,pZclFrame->PayLoad+ReadPos,1);	ReadPos+=1;
				memcpy(&pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.NumOutClusters,pZclFrame->PayLoad+ReadPos,1);	ReadPos+=1;

				DataLeftLen-=33;
				if(DataLeftLen<pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.NumInClusters*2+pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.NumOutClusters*2){
					DEBUGLOG0(3,"Error, bad zcl frame data.\n");
					return -11;
				}
				memcpy(pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.InClusterIDList,pZclFrame->PayLoad+ReadPos,pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.NumInClusters*2);ReadPos+=pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.NumInClusters*2;
				memcpy(pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.OutClusterIDList,pZclFrame->PayLoad+ReadPos,pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.NumOutClusters*2);ReadPos+=pZclMsg->un.ClusterCmd.unClusterCmd_SelfIntroduction.mCMD_SELFINTRODUCTION_INTRODUCE_RSP.NumOutClusters*2;
				break;
			default:
				DEBUGLOG1(3,"Error, unsupported cmd %d for CLUSTER_SELFINTRODUCTION cluster.\n",pZclFrame->CmdID);
				return -11;
				break;
			}
			break;
		default:
			DEBUGLOG1(3,"Error, unsupported cluster 0x%04X for HA profile.\n",pZclFrame->ClusterID);
			return -10;
			break;
		}
		break;
	default:
		DEBUGLOG1(3,"Error, illegal ZclFrame FrameType %d.\n",pZclMsg->ZclMsgHdr.FrameCtrl.FrameType);
		return -8;
		break;
	}
	return 0;
}

//pZclMsgInfo 需要订阅的Zcl消息特征
stZclMsgSubsHdl *ZclMsgSubscribe(stZclMsgInfo *pZclMsgInfo)
{
	stZclMsgSubsHdl *pRet=NULL;
	stZclMsgSubsHdl *pScan=NULL;

	pRet=(stZclMsgSubsHdl *)malloc(sizeof(stZclMsgSubsHdl));
	if(pRet==NULL){
		DEBUGLOG0(3,"Error, malloc failed.\n");
		return NULL;
	}
	memset(pRet,0,sizeof(stZclMsgSubsHdl));
	pRet->ZclMsgInfo=*pZclMsgInfo;
	pthread_mutex_init(&pRet->m,NULL);
	pthread_cond_init(&pRet->c,NULL);

	//加入全局订阅列表
	pthread_mutex_lock(&GlobalZclStatus.mutexGlobalZclMsgSubsHdlList);
	if(GlobalZclStatus.pGlobalZclMsgSubsHdlList==NULL){
		GlobalZclStatus.pGlobalZclMsgSubsHdlList=pRet;
	}else{
		pScan=GlobalZclStatus.pGlobalZclMsgSubsHdlList;
		while(pScan->pNext!=NULL){
			pScan=pScan->pNext;
		}
		pScan->pNext=pRet;
		pRet->pPrev=pScan;
	}
	pthread_mutex_unlock(&GlobalZclStatus.mutexGlobalZclMsgSubsHdlList);

	return pRet;
}

//等待订阅的消息
//pZnpMsgSubsHdl 订阅句柄
//Timeout 最大等待时间（毫秒 ms）
//return 成功时，返回得到的消息，失败时为NULL
stZclMsg *ZclMsgPend(stZclMsgSubsHdl *pZclMsgSubsHdl, int Timeout)
{
	int rt;
	struct timespec tv;
	struct timeval now;
	stZclMsg *pRet=NULL;

	if((pZclMsgSubsHdl==NULL)||(Timeout<0)){
		return NULL;
	}

	pthread_mutex_lock(&pZclMsgSubsHdl->m);
	if(pZclMsgSubsHdl->isMsgValid==0){
		gettimeofday(&now,NULL);
		tv.tv_sec = now.tv_sec + Timeout/1000;
		tv.tv_nsec = now.tv_usec*1000 + (Timeout%1000)*1000*1000;
		while(1){
			rt=pthread_cond_timedwait(&pZclMsgSubsHdl->c,&pZclMsgSubsHdl->m,&tv);
			if(rt == 0){//触发
				if(pZclMsgSubsHdl->isMsgValid != 0){//确实触发了
					break;
				}
			}else if(rt == ETIMEDOUT){//超时
				DEBUGLOG0(2,"Warning, ZclMsgPend timed out.\n");
				break;
			}else{//出错
				DEBUGLOG1(3,"Error, pthread_cond_timedwait failed %d.\n",rt);
				break;
			}
		}
	}
	//当消息确实到来时，提取消息
	if(pZclMsgSubsHdl->isMsgValid){
		pRet=pZclMsgSubsHdl->pZclMsg;
		pZclMsgSubsHdl->isMsgValid=0;//消息已经被提取，将相应标志位清空
		pZclMsgSubsHdl->pZclMsg=NULL;
	}
	pthread_mutex_unlock(&pZclMsgSubsHdl->m);

	return pRet;
}

//寄出消息
//在消息订阅列表中查找订阅者，并向其寄出消息
//pZclMsg - 需要寄出的消息
//return 0-表示该消息当前无订阅者, >0-表示该消息当前的订阅者数量, <0-出错
//当前只允许一条消息有一个订阅者
int ZclMsgPost(stZclMsg *pZclMsg)
{
	stZclMsgSubsHdl *pScan=NULL;
	int isFound=0,Ret=0;

	if(pZclMsg==NULL){
		return -1;
	}
	pthread_mutex_lock(&GlobalZclStatus.mutexGlobalZclMsgSubsHdlList);
	pScan=GlobalZclStatus.pGlobalZclMsgSubsHdlList;
	while(pScan!=NULL){
		if((pScan->ZclMsgInfo.ProfileID==pZclMsg->ZclMsgAccessory.ProfileID)&&(pScan->ZclMsgInfo.ClusterID==pZclMsg->ZclMsgAccessory.ClusterID)&&(pScan->ZclMsgInfo.CmdID==pZclMsg->ZclMsgHdr.CmdID)&&(pScan->ZclMsgInfo.TranSeqNum==pZclMsg->ZclMsgHdr.TranSeqNum)){
			isFound=1;
			break;
		}else{
			pScan=pScan->pNext;
		}
	}
	if((isFound)&&(pScan!=NULL)){
		if(pScan->isMsgValid!=0){
			DEBUGLOG0(2,"Warning, discarding duplicated ZclMsg.\n");
			DelocateZclMsg(pZclMsg);
			Ret=1;
		}else{
			pthread_mutex_lock(&pScan->m);
			pScan->pZclMsg=pZclMsg;
			pScan->isMsgValid=1;
			pthread_cond_signal(&pScan->c);
			pthread_mutex_unlock(&pScan->m);
			Ret=1;
		}
	}
	pthread_mutex_unlock(&GlobalZclStatus.mutexGlobalZclMsgSubsHdlList);
	return Ret;
}

//退订消息
void ZclMsgUnsubscribe(stZclMsgSubsHdl *pZclMsgSubsHdl)
{
	int rt;

	if(pZclMsgSubsHdl==NULL){
		return;
	}
	if(GlobalZclStatus.pGlobalZclMsgSubsHdlList==NULL){//This should not happen when pZnpMsgSubsHdl!=NULL.
		return;
	}

	pthread_mutex_lock(&GlobalZclStatus.mutexGlobalZclMsgSubsHdlList);
	if(pZclMsgSubsHdl==GlobalZclStatus.pGlobalZclMsgSubsHdlList){//如果是队首
		GlobalZclStatus.pGlobalZclMsgSubsHdlList=GlobalZclStatus.pGlobalZclMsgSubsHdlList->pNext;
	}else if(pZclMsgSubsHdl->pNext==NULL){//如果是队尾
		pZclMsgSubsHdl->pPrev->pNext=NULL;
	}else{//如果是中间元素
		pZclMsgSubsHdl->pPrev->pNext=pZclMsgSubsHdl->pNext;
		pZclMsgSubsHdl->pNext->pPrev=pZclMsgSubsHdl->pPrev;
	}
	//释放申请的系统资源
	rt=pthread_mutex_destroy(&pZclMsgSubsHdl->m);
	if(rt!=0){
		DEBUGLOG1(3,"Warning, pthread_mutex_destroy failed %d.\n",rt);
	}
	rt=pthread_cond_destroy(&pZclMsgSubsHdl->c);
	if(rt!=0){
		DEBUGLOG1(3,"Warning, pthread_cond_destroy failed %d.\n",rt);
	}
	pthread_mutex_unlock(&GlobalZclStatus.mutexGlobalZclMsgSubsHdlList);

	//销毁其中可能存在的消息,pend超时返回到消息退订之间的时间空隙可能碰巧会有消息到来
	if(pZclMsgSubsHdl->isMsgValid){
		DelocateZclMsg(pZclMsgSubsHdl->pZclMsg);
	}
	free(pZclMsgSubsHdl);
}













