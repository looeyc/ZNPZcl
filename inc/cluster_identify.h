/*
 * cluster_identify.h
 *
 *  Created on: 2014-3-22
 *      Author: chengm
 */

#ifndef CLUSTER_IDENTIFY_H_
#define CLUSTER_IDENTIFY_H_

//Cluster Command结构-----------------------------------------------------------------------------------------------------------------
//CMD定义
//Commands Received
#define CMD_IDENTIFY_IDENTIFY				(unsigned char)0x00
#define CMD_IDENTIFY_IDENTIFYQUERY			(unsigned char)0x01
//Commands Generated
#define CMD_IDENTIFY_IDENTIFYQUERYRSP		(unsigned char)0x00

typedef struct{
	unsigned short IdentifyTime;//Identify Time（单位：秒）
}stCMD_IDENTIFY_IDENTIFY;

typedef struct{//空
}stCMD_IDENTIFY_IDENTIFYQUERY;

typedef struct{
	unsigned short Timeout;//The Timeout field contains the current value of the IdentifyTime attribute, and specifies the length of time, in seconds, that the device will continue to identify itself.
}stCMD_IDENTIFY_IDENTIFYQUERYRSP;

typedef union{
	stCMD_IDENTIFY_IDENTIFY mCMD_IDENTIFY_IDENTIFY;
	stCMD_IDENTIFY_IDENTIFYQUERY mCMD_IDENTIFY_IDENTIFYQUERY;
	stCMD_IDENTIFY_IDENTIFYQUERYRSP mCMD_IDENTIFY_IDENTIFYQUERYRSP;
}unClusterCmd_Identify;

//Cluster Attribute结构--------------------------------------------------------------------------------------------------------------
//属性定义
#define ATTRIID_IDENTIFY_IDENTIFYTIME			(unsigned short)0x0000
#define ATTRIDATATYPE_IDENTIFY_IDENTIFYTIME		ZCL_UINT16

typedef struct{
	unsigned short AttriID;
	union{
		unsigned short IdentifyTime; //AlarmCount, specifies the number of entries currently in the alarm table.
	}un;
}stClusterAttri_Identify;

#endif /* CLUSTER_IDENTIFY_H_ */
