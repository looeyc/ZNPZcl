/*
 * znpzcl_test.c
 *
 *  Created on: 2014-2-24
 *      Author: root
 */
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include "znplib.h"
#include "znpzcl.h"

//#define ZNPCOMDEVPATH "/dev/ttySAC1"	//for arm linux
#define ZNPCOMDEVPATH "/dev/ttyS0"	//for ubuntu
//#define ZNPCOMDEVPATH "/dev/ttyAMA0" //for Rpi

void pCB_ZnpMsgDefaultCB(stZnpMsg *pZnpMsg)
{
	printf("pCB_ZnpMsgDefaultCB get a msg with type 0x%04X.\n",pZnpMsg->MsgType);
}

void pCB_ZclMsgDefaultCB(stZclMsg *pZclMsg)
{
	switch(pZclMsg->ZclMsgHdr.FrameCtrl.FrameType){
	case ZCL_FRAMETYPE_PROFILE:
		switch(pZclMsg->ZclMsgHdr.CmdID){
		case ZCL_GENERAL_ATTRIREPORT:
			printf("pCB_ZclMsgDefaultCB get a ZclMsg: ZCL_GENERAL_ATTRIREPORT .\n");
			break;
		default:
			printf("pCB_ZclMsgDefaultCB get a ZclMsg: unknown cmd of ZCL_FRAMETYPE_PROFILE %d.\n",pZclMsg->ZclMsgHdr.CmdID);
			break;
		}
		break;
	case ZCL_FRAMETYPE_CLUSTER:
	default:
		printf("pCB_ZclMsgDefaultCB get a ZclMsg: unknown FrameType %d.\n",pZclMsg->ZclMsgHdr.FrameCtrl.FrameType);
		break;
	}
}

int main(int argc, char *argv[])
{
	int rt;
	stZnpCfg ZnpCfg;
	stZclCfg ZclCfg;
	stZclInstRegistParam ZclInstRegistParam;
	stZclInst *pZclInst;
	unsigned short TestDevAddr=0x0000;

	//初始化ZNP
	ZnpCfg.pDefaultZnpMsgCB=pCB_ZnpMsgDefaultCB;
	ZnpCfg.pZNPComPath=ZNPCOMDEVPATH;
	rt=ZnpInit(&ZnpCfg);
	if(rt!=0){
		printf("Error, ZnpInit failed.\n");
	}
	//初始化Zcl
	memset(&ZclCfg,0,sizeof(stZclCfg));
	rt=ZclInit(&ZclCfg);
	if(rt!=0){
		printf("Error, ZclInit failed.\n");
	}
	//注册Zcl实例
	ZclInstRegistParam.EndPoint=33;
	ZclInstRegistParam.Profile=0x0104;
	ZclInstRegistParam.pCBFxn=pCB_ZclMsgDefaultCB;
	pZclInst=ZclRegistInst(&ZclInstRegistParam);
	if(pZclInst==NULL){
		printf("Error, ZclRegistInst failed.\n");
	}

	//找一个设备
	{
		unsigned char pValue[8]={0xf0,0xd8,0xcb,0x02,0x00,0x4b,0x12,0x00};
		stZB_FIND_DEVICE_CONFIRM mZB_FIND_DEVICE_CONFIRM;

		rt=Znp_ZB_FIND_DEVICE_REQUEST(pValue, &mZB_FIND_DEVICE_CONFIRM);
		if(rt!=0){
			printf("Error, Znp_ZB_FIND_DEVICE_REQUEST failed %d.\n",rt);
		}else{
			memcpy(&TestDevAddr,mZB_FIND_DEVICE_CONFIRM.SearchKey,2);
		}
	}

	{
		stZCL_CFGREPORT mZCL_CFGREPORT;
		stZCL_CFGREPORT_R mZCL_CFGREPORT_R;

		memset(&mZCL_CFGREPORT,0,sizeof(stZCL_CFGREPORT));
		mZCL_CFGREPORT.RecordNum=1;
		mZCL_CFGREPORT.AttriReportCfgRecord[0].AttriID=0x0000;
		mZCL_CFGREPORT.AttriReportCfgRecord[0].Direction=0;
		mZCL_CFGREPORT.AttriReportCfgRecord[0].DataType=ZCL_BOOL;
		mZCL_CFGREPORT.AttriReportCfgRecord[0].MaxInterval=0x0001;
		mZCL_CFGREPORT.AttriReportCfgRecord[0].MinInterval=0x0000;
		rt=ZclCfgAttriReport(pZclInst, NULL, TestDevAddr, 30, CLUSTER_ONOFF, &mZCL_CFGREPORT, &mZCL_CFGREPORT_R);
		if(rt!=0){
			printf("Error, ZclCfgAttriReport failed %d.\n",rt);
		}
	}

	while(1){
		{
			unsigned short AttriIDList[1]={0x0000};
			stZCL_ATTRIREAD_R mZCL_ATTRIREAD_R;

			//读取
			rt=ZclAttriRead(pZclInst,NULL,TestDevAddr,30,CLUSTER_ONOFF,1,AttriIDList,&mZCL_ATTRIREAD_R);
			if(rt!=0){
				printf("Error, ZclAttriRead failed %d.\n",rt);
			}else{
				printf("ZclAttriRead ok %d,%d.\n",mZCL_ATTRIREAD_R.Records[0].Status,mZCL_ATTRIREAD_R.Records[0].AttriData.Data[0]);
			}

			rt=ZclClusterCtrl(pZclInst,NULL,TestDevAddr,30,CLUSTER_ONOFF,CMD_ONOFF_TOGGLE,NULL);
			if(rt!=0){
				printf("Error, ZclClusterCtrl failed %d.\n",rt);
			}

			sleep(1);
		}
	}

	return 0;
}
