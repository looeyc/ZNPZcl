/*
 * zclmsg.h
 *
 *  Created on: 2014-2-21
 *      Author: chengm
 */

#ifndef ZCLMSG_H_
#define ZCLMSG_H_

#include <pthread.h>
#include "zclframe.h"
#include "zcldatatype.h"
#include "clusters.h"

#define ZCL_MAXPAYLOADLEN				256

#define MAXATTRINUM						16
#define MAXATTRIDISVNUM					16
#define MAXATTRIRPORTNUM				32

#define ZCL_FRAMETYPE_PROFILE			(unsigned char)0x00
#define ZCL_FRAMETYPE_CLUSTER			(unsigned char)0x01

#define ZCL_MANUFACT_EXIST				(unsigned char)0x01
#define ZCL_MANUFACT_NOTEXIST			(unsigned char)0x00

#define	ZCL_DIR_SERVER_TO_CLIENT		(unsigned char)0x01
#define	ZCL_DIR_CLIENT_TO_SERVER		(unsigned char)0x00

#define ZCL_DEFAULTRSP_DISABLE			(unsigned char)0x01
#define ZCL_DEFAULTRSP_ENABLE			(unsigned char)0x00

#define ZCL_GENERAL_ATTRIREAD 			(unsigned char)0x00 //Read attributes
#define ZCL_GENERAL_ATTRIREAD_R	 		(unsigned char)0x01 //Read attributes response
#define ZCL_GENERAL_ATTRIWRITE		 	(unsigned char)0x02 //Write attributes
#define ZCL_GENERAL_ATTRIWRITEUNDIV 	(unsigned char)0x03 //Write attributes undivided
#define ZCL_GENERAL_ATTRIWRITE_R	 	(unsigned char)0x04 //Write attributes response
#define ZCL_GENERAL_ATTRIWRITENORSP		(unsigned char)0x05 //Write attributes no response
#define ZCL_GENERAL_CFGREPORT			(unsigned char)0x06 //Configure reporting
#define ZCL_GENERAL_CFGREPORT_R			(unsigned char)0x07 //Configure reporting response
#define ZCL_GENERAL_REPORTCFGREAD		(unsigned char)0x08 //Read reporting configuration
#define ZCL_GENERAL_REPORTCFGREAD_R		(unsigned char)0x09 //Read reporting configuration response
#define ZCL_GENERAL_ATTRIREPORT			(unsigned char)0x0A //Report attributes
#define ZCL_GENERAL_DEFAULTRSP			(unsigned char)0x0B //Default response
#define ZCL_GENERAL_DISCVATTRI			(unsigned char)0x0C //Discover attributes
#define ZCL_GENERAL_DISCVATTRI_R		(unsigned char)0x0D //Discover attributes response
#define ZCL_GENERAL_ATTRIREADSTR		(unsigned char)0x0E //Read attributes structured
#define ZCL_GENERAL_ATTRIWRITESTR		(unsigned char)0x0F //Write attributes structured
#define ZCL_GENERAL_ATTRIWRITESTR_R		(unsigned char)0x10 //Write attributes structured response

//以下类型适用于所有Cluster---------------------------------------------------------------------------
//ZCL_ATTRIREAD----------------------------------------------------------
typedef struct{
	unsigned char AttriNum;							//需要读取的Attri数量
	unsigned short AttriIDList[MAXATTRINUM];		//需要读取的AttriID列表
}stZCL_ATTRIREAD;

typedef struct{
	unsigned short AttriID;							//Attribute identifier
	unsigned char Status;							//Status
	stZclDataUnit AttriData;						//ZCL数据类型
}stAttriReadRecord;
typedef struct{
	unsigned char RecordNum;
	stAttriReadRecord Records[MAXATTRINUM];
}stZCL_ATTRIREAD_R;

//ZCL_ATTRIWRITE---------------------------------------------------------
typedef struct{
	unsigned short AttriID;							//Attribute identifier
	stZclDataUnit AttriData;						//ZCL数据类型
}stAttriWriteRecord;
typedef struct{
	unsigned char RecordNum;						//需要写的Attri数量
	stAttriWriteRecord Records[MAXATTRINUM];		//需要写的AttriID列表
}stZCL_ATTRIWRITE;

typedef struct{
	unsigned short AttriID;							//Attribute identifier
	unsigned char Status;							//Status
}stAttriWriteStatusRecord_R;
typedef struct{
	unsigned char RecordNum;						//写Attri数量
	stAttriWriteStatusRecord_R Records[MAXATTRINUM];//写AttriID列表
}stZCL_ATTRIWRITE_R;

//ZCL_CFGREPORT-----------------------------------------------------------
typedef struct{
	unsigned char Direction;//Direction, 0-设备对外报告的属性参数、1-设备需要等待别的设备向其报告的属性参数
	unsigned short AttriID;//Attribute identifier
	unsigned char DataType;//Attribute data type
	unsigned short MinInterval;//Minimum reporting interval，0x0000表示没有最小间隔限制
	unsigned short MaxInterval;//Maximum reporting interval，0xffff表示不要报告该属性
	stZclDataUnit ReportableChange;//Reportable change
	unsigned short TimeoutPeriod;//Timeout period
}stAttriReportCfgRecord;
typedef struct{
	unsigned char RecordNum;
	stAttriReportCfgRecord AttriReportCfgRecord[MAXATTRINUM];
}stZCL_CFGREPORT;

//Attribute status record
typedef struct{
	unsigned char Status;//Status
	unsigned char Direction;//Direction，Status==0时该字段无意义
	unsigned short AttriID;//Attribute identifier，Status==0时该字段无意义
}stAttriStatusRecord;
typedef struct{
	unsigned char RecordNum;//如果所有的属性报告都设置成功了，则RecordNum=1
	stAttriStatusRecord AttriStatusRecord[MAXATTRINUM];
}stZCL_CFGREPORT_R;

typedef struct{
	unsigned short AttriID;
	stZclDataUnit AttriData;
}stAttriReportRecord;
typedef struct{
	unsigned char AttriNum;//AttriReportRecord中的实际Attri数量
	stAttriReportRecord AttriReportRecord[MAXATTRIRPORTNUM];
}stZCL_ATTRIREPORT;

typedef struct{
	unsigned char CmdID;//Command identifier
	unsigned char Status;//Status code
}stZCL_DEFAULTRSP;

typedef struct{
	unsigned short StartAttriID;//Start attribute identifier
	unsigned char MaxAttriIDNum;//要求<=MAXATTRIDISVNUM, The maximum number of attribute identifiers that are to be returned in the resulting
}stZCL_ATTRIDISCV;

typedef struct{
	unsigned short AttriID;//Attribute identifier
	unsigned char AttriDataType;//Attribute data type
}stAttriInfo;
typedef struct{
	unsigned char isComplete;//0 - that there are more attributes to be discovered. 1 - that there are no more	attributes to be discovered.
	stAttriInfo AttriInfo[MAXATTRIDISVNUM];//Attribute information
}stZCL_ATTRIDISCV_R;

typedef union{
	stZCL_ATTRIREAD 	mZCL_ATTRIREAD;
	stZCL_ATTRIREAD_R 	mZCL_ATTRIREAD_R;
	stZCL_ATTRIWRITE 	mZCL_ATTRIWRITE;
	stZCL_ATTRIWRITE_R 	mZCL_ATTRIWRITE_R;
	stZCL_CFGREPORT 	mZCL_CFGREPORT;
	stZCL_CFGREPORT_R 	mZCL_CFGREPORT_R;
	stZCL_ATTRIREPORT 	mZCL_ATTRIREPORT;
	stZCL_DEFAULTRSP 	mZCL_DEFAULTRSP;
	stZCL_ATTRIDISCV 	mZCL_ATTRIDISCV;
	stZCL_ATTRIDISCV_R 	mZCL_ATTRIDISCV_R;
}unGeneralCmd;

//统一的ZclMsg结构----------------------------------------------------------------------------------
//ZclMsg的附带属性
typedef struct{
	unsigned short ProfileID;
	unsigned short ClusterID;
	//消息的来源，有系统填写，在填写请求消息时，以下这两个字段不用填
	unsigned short SrcAddr;//源地址
	unsigned char SrcEP;//源EP
}stZclMsgAccessory;
typedef struct{
	unsigned char FrameType;					//最低2bit, Frame type
	unsigned char Manufact;						//最低1bit, Manufacturer specific
	unsigned char Direction;					//最低1bit, Direction
	unsigned char DisableDefaultRsp;			//最低1bit, Disable default	response
}stZclMsgHdr_FC;
typedef struct{
	stZclMsgHdr_FC FrameCtrl;					//Frame control
	unsigned short ManufactCode;				//Manufacturer code
	unsigned char TranSeqNum;					//Transaction sequence number
	unsigned char CmdID;						//Command identifier
}stZclMsgHdr;
typedef struct{
	//附带属性
	stZclMsgAccessory ZclMsgAccessory;
	//其他
	stZclMsgHdr ZclMsgHdr;
	//由FrameType决定un的具体类型
	union{
		//FrameType==ZCL_FRAMETYPE_PROFILE，所有Cluster通用，具体类型由CmdID决定
		unGeneralCmd GeneralCmd;
		//FrameType==ZCL_FRAMETYPE_CLUSTER，针对具体Cluster,具体类型有ProfileID、ClusterID、CmdID共同决定
		unClusterCmd ClusterCmd;//具体类型由Cluster和CmdID共同决定
	}un;
}stZclMsg;

//创建一个stZclMsg
//return NULL表示创建失败
//创建的stZclMsg在使用完毕后一定要调用DelocateZclMsg销毁
stZclMsg *AllocateZclMsg();

//销毁一个ZclFrame
void DelocateZclMsg(stZclMsg *pZclMsg);

//转换ZclMsg -> ZclFrame
//pZclMsg - 待转换的pZclMsg
//pZclFrame - 调用者提供空间，用于返回转换结果
//return 0-成功，else-失败
int TransZclMsgToZclFrame(stZclMsg *pZclMsg, stZclFrame *pZclFrame);

//转换ZclFrame -> ZclMsg
//pZclFrame - 待转换的ZclFrame
//pZclMsg - 调用者提供空间，用于返回转换结果
//return 0-成功，else-失败
int TransZclFrameToZclMsg(stZclFrame *pZclFrame, stZclMsg *pZclMsg);

//订阅消息-------------------------------------------------------------------------------------------------------------------
//用于描述订阅的ZclMsg
typedef struct{
	unsigned short ProfileID;
	unsigned short ClusterID;
	unsigned char TranSeqNum;
	unsigned char CmdID;
}stZclMsgInfo;

typedef struct stZclMsgSubsHdl_ stZclMsgSubsHdl;
struct stZclMsgSubsHdl_{
	//用于链表
	stZclMsgSubsHdl *pPrev;
	stZclMsgSubsHdl *pNext;

	//所订阅的Zcl消息的特征
	stZclMsgInfo ZclMsgInfo;
	int isMsgValid;
	pthread_mutex_t m;
	pthread_cond_t c;//用于消息等待的信号量

	//用于被订阅方向订阅方返回订阅的消息
	stZclMsg *pZclMsg;
};

//pZclMsgInfo 需要订阅的Zcl消息特征
stZclMsgSubsHdl *ZclMsgSubscribe(stZclMsgInfo *pZclMsgInfo);

//等待订阅的消息
//pZnpMsgSubsHdl 订阅句柄
//Timeout 最大等待时间（毫秒 ms）
//return 成功时，返回得到的消息，失败时为NULL
stZclMsg *ZclMsgPend(stZclMsgSubsHdl *pZclMsgSubsHdl, int Timeout);

//寄出消息
//在消息订阅列表中查找订阅者，并向其寄出消息
//pZclMsg - 需要寄出的消息
//return 0-表示该消息当前无订阅者, >0-表示该消息当前的订阅者数量, <0-出错
//当前只允许一条消息有一个订阅者
int ZclMsgPost(stZclMsg *pZclMsg);

//退订消息
void ZclMsgUnsubscribe(stZclMsgSubsHdl *pZclMsgSubsHdl);

#endif /* ZCLMSG_H_ */






















