/*
 * cluster_onoff.h
 *
 *  Created on: 2014-2-20
 *      Author: chengm
 */

#ifndef CLUSTER_ONOFF_H_
#define CLUSTER_ONOFF_H_

//Cluster Command结构-----------------------------------------------------------------------------------------------------------------
//CMD定义
#define CMD_ONOFF_OFF			(unsigned char)0x00
#define CMD_ONOFF_ON			(unsigned char)0x01
#define CMD_ONOFF_TOGGLE		(unsigned char)0x02

typedef struct{
	//空
}stCMD_ONOFF_OFF;

typedef struct{
	//空
}stCMD_ONOFF_ON;

typedef struct{
	//空
}stCMD_ONOFF_TOGGLE;

typedef union{
	stCMD_ONOFF_OFF mCMD_ONOFF_OFF;
	stCMD_ONOFF_ON mCMD_ONOFF_ON;
	stCMD_ONOFF_TOGGLE mCMD_ONOFF_TOGGLE;
}unClusterCmd_OnOff;

//Cluster Attribute结构--------------------------------------------------------------------------------------------------------------
//属性定义
#define ATTRIID_ONOFF_ONOFF				(unsigned short)0x0000
#define ATTRIDATATYPE_ONOFF_ONOFF		ZCL_BOOL

typedef struct{
	unsigned short AttriID;
	union{
		unsigned char OnOff;//0 = Off, 1 = On
	}un;
}stClusterAttri_OnOff;

#endif /* CLUSTER_ONOFF_H_ */







