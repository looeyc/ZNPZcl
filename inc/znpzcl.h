/*
 * znpzcl.h
 *
 *  Created on: 2014-2-20
 *      Author: chengm
 */

#ifndef ZNPZCL_H_
#define ZNPZCL_H_

#include "zclmsg.h"
#include "clusters.h"
#include "zclglobal.h"

#define MAXROUTERELAYCNT	16

//Zcl初始化参数--------------------------------------------------------------------------------------------------------------
typedef struct{
	//空
}stZclCfg;

//初始化zcl
//pZclCfg - 初始化配置参数
//return 0-成功，else-失败
//要求调用者先初始化ZNP，否则结果不可预料
int ZclInit(stZclCfg *pZclCfg);

//注册一个Zcl实例，每一个实例对应一个Profile
//注册参数
typedef struct{
	unsigned char EndPoint;//使用的EP
	unsigned short Profile;//注册的Profile
	void (*pCBFxn)(stZclMsg *pZclMsg);//注册ZclMsg的回调处理函数，符合Profile的ZclMsg将调用该回调进行处理，该回调执行结束后，pZclMsg将被收回销毁，如果需要留着用的话应当另作拷贝
}stZclInstRegistParam;

//pRegParam - 注册参数
//return 注册成功时返回Zcl实例的句柄，NULL表示注册失败
stZclInst *ZclRegistInst(stZclInstRegistParam *pRegParam);

//读取属性数据---------------------------------------------------------------------------------------------------------------
//路由
typedef struct{
	unsigned char RelayCount;
	unsigned short RelayList[MAXROUTERELAYCNT];
}stRoute;

//Read Attributes Command
//pZclInst - 请求的ZclInst句柄
//pRoute - 指定路由，如果为NULL则不指定路由
//DevNwkAddr - 设备短地址
//EP - Endpoint
//Cluster - attributes所属的Cluster
//AttriNum - 需要读取的AttriID数量
//pAttriIDList - 需要读取的AttriID列表
//pResault - 调用者提供空间，用于返回读取结果
//return 0-读取操作成功,pResault可用；else-出错，pResault不可用
int ZclAttriRead(stZclInst *pZclInst, stRoute *pRoute, unsigned short DevNwkAddr, unsigned char EP, unsigned short ClusterID, unsigned char AttriNum, unsigned short *pAttriIDList, stZCL_ATTRIREAD_R *pResault);

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
//如果pResault中只有一项记录，并且是成功的，则表示所有的写操作都成功了
int ZclAttriWrite(stZclInst *pZclInst, stRoute *pRoute, unsigned short DevNwkAddr, unsigned char EP, unsigned short ClusterID, unsigned char AttriNum, stAttriWriteRecord *pAttriWriteRecordList, stZCL_ATTRIWRITE_R *pResault);

//Configure Reporting Command
//pZclInst - 请求的ZclInst句柄
//pRoute - 指定路由，如果为NULL则不指定路由
//DevNwkAddr - 设备短地址
//EP - Endpoint
//Cluster - 设置的Cluster
//pCfg - 配置参数
//pRet - 配置结果
int ZclCfgAttriReport(stZclInst *pZclInst, stRoute *pRoute, unsigned short DevNwkAddr, unsigned char EP, unsigned short ClusterID, stZCL_CFGREPORT *pCfg, stZCL_CFGREPORT_R *pRet);

//Discover attributes

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
int ZclClusterCtrl(stZclInst *pZclInst, stRoute *pRoute, unsigned short DevNwkAddr, unsigned char EP, unsigned short ClusterID, unsigned char Cmd, unClusterCmd *pClusterCmd, char NeedReply);

#endif /* ZNPZCL_H_ */







