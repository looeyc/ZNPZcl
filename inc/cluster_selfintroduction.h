/*
 * cluster_selfintroduction.h
 *
 *  Created on: 2014-7-1
 *      Author: chengm
 */

#ifndef CLUSTER_SELFINTRODUCTION_H_
#define CLUSTER_SELFINTRODUCTION_H_

#define MAXUSERDESCRIPTORLEN		16//用户描述符最大长度(znp设备只支持16字节)
#define MAXCLUSTERNUM				8

//Cluster Command结构-----------------------------------------------------------------------------------------------------------------
//CMD定义
//Commands Received
#define CMD_SELFINTRODUCTION_INTRODUCE_REQ	((unsigned char)0x00) //请进行一次自我介绍，服务端收到该命令后，将向请求发起者返回CMD_SELFINTRODUCTION_INTRODUCE_RSP进行自我介绍
//Commands Generated
#define CMD_SELFINTRODUCTION_INTRODUCE_RSP	((unsigned char)0x00) //自我介绍

typedef struct{
	//空
}stCMD_SELFINTRODUCTION_INTRODUCE_REQ;

typedef struct{
	unsigned char IEEEAddr[8];	//64bit IEEE address
	char UserDescriptor[MAXUSERDESCRIPTORLEN];
	unsigned short NwkAddr;		//16bit address
	unsigned short ProfileID;
	unsigned short DeviceID;
	unsigned char EP;			//End point
	unsigned char NumInClusters;//输入Cluster数量
	unsigned char NumOutClusters;//输出Cluster数量
	unsigned short InClusterIDList[MAXCLUSTERNUM];//输入ClusterID列表
	unsigned short OutClusterIDList[MAXCLUSTERNUM];//输出ClusterID列表
}stCMD_SELFINTRODUCTION_INTRODUCE_RSP;

typedef union{
	//Commands Received
	stCMD_SELFINTRODUCTION_INTRODUCE_REQ mCMD_SELFINTRODUCTION_INTRODUCE_REQ;
	//Commands Generated
	stCMD_SELFINTRODUCTION_INTRODUCE_RSP mCMD_SELFINTRODUCTION_INTRODUCE_RSP;
}unClusterCmd_SelfIntroduction;

//Cluster Attribute结构--------------------------------------------------------------------------------------------------------------
//属性定义
//无
typedef struct{
	//无
}stClusterAttri_SelfIntroduction;

#endif /* CLUSTER_SELFINTRODUCTION_H_ */
