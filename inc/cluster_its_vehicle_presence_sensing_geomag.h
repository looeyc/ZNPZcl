/*
 * cluster_its_vehicle_presence_sensing_geomag.h
 *
 *  Created on: 2014-12-12
 *      Author: chengm
 */

#ifndef CLUSTER_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_H_
#define CLUSTER_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_H_

//Cluster Command结构-----------------------------------------------------------------------------------------------------------------
//CMD定义
//Commands Received
#define CMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_ADJUST      (unsigned char)0x00//校正，以当前测量值为背景磁场
#define CMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG      (unsigned char)0x01//设置参数

//Commands Generated
//无

typedef struct{//空
}stCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_ADJUST;

typedef struct{
	unsigned short TS;   //0xFFFF时无效  TS参数
	unsigned short TDMA; //0xFFFF时无效  TDMA参数
	unsigned short TDME; //0xFFFF时无效  TDME参数
	unsigned short TBME; //0xFFFF时无效  TBME参数
	unsigned short TPZ;  //0xFFFF时无效  TPZ参数
	unsigned short TP3;  //0xFFFF时无效  TP3参数
	unsigned short TPVS; //0xFFFF时无效  TPVS参数
	unsigned short TPVL; //0xFFFF时无效  TPVL参数
	unsigned char WMODE; //0xFF时无效  WMODE参数
	unsigned char RHMN;  //0xFF时无效  RHMN参数
	unsigned char PMF;   //0xFF时无效  PMF参数
	unsigned char PMN;   //0xFF时无效  PMN参数
	unsigned char PRPT;  //0xFF时无效  PRPT参数
}stCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG;

typedef union{
	stCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_ADJUST mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_ADJUST;
	stCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG mCMD_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_CONFIG;
}unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag;

//Cluster Attribute结构--------------------------------------------------------------------------------------------------------------
//属性定义
//无
typedef struct{
}stClusterAttri_ITS_Vehicle_Presence_Sensing_Geomag;

#endif /* CLUSTER_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG_H_ */
