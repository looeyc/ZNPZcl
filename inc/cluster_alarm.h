/*
 * cluster_alarm.h
 *
 *  Created on: 2014-3-21
 *      Author: chengm
 */

#ifndef CLUSTER_ALARM_H_
#define CLUSTER_ALARM_H_

//Cluster Command结构-----------------------------------------------------------------------------------------------------------------
//CMD定义
//Commands Received
#define CMD_ALARM_RESETALARM				(unsigned char)0x00 //Reset Alarm
#define CMD_ALARM_RESETALLALARMS			(unsigned char)0x01 //Reset all alarms
#define CMD_ALARM_GETALARM					(unsigned char)0x02 //Get Alarm
#define CMD_ALARM_RESETALLALARMLOG			(unsigned char)0x03 //Reset alarm log
//Commands Generated
#define CMD_ALARM_ALARM						(unsigned char)0x00 //Alarm
#define CMD_ALARM_GETALARMRESPONSE			(unsigned char)0x01 //Get alarm response

typedef struct{
	unsigned char AlarmCode;//Alarm code
	unsigned short ClusterID;//Cluster identifier
}stCMD_ALARM_RESETALARM;

typedef struct{	//空
}stCMD_ALARM_RESETALLALARMS;

typedef struct{	//空
}stCMD_ALARM_GETALARM;

typedef struct{	//空
}stCMD_ALARM_RESETALLALARMLOG;

typedef struct{
	unsigned char AlarmCode;//Alarm code
	unsigned short ClusterID;//Cluster identifier
}stCMD_ALARM_ALARM;

typedef struct{
	unsigned char Status;//Status
	unsigned char AlarmCode;//Alarm code
	unsigned short ClusterID;//Cluster identifier
	unsigned int TimeStamp;//Time stamp
}stCMD_ALARM_GETALARMRESPONSE;

typedef union{
	stCMD_ALARM_RESETALARM mCMD_ALARM_RESETALARM;
	stCMD_ALARM_RESETALLALARMS mCMD_ALARM_RESETALLALARMS;
	stCMD_ALARM_GETALARM mCMD_ALARM_GETALARM;
	stCMD_ALARM_RESETALLALARMLOG mCMD_ALARM_RESETALLALARMLOG;
	stCMD_ALARM_ALARM mCMD_ALARM_ALARM;
	stCMD_ALARM_GETALARMRESPONSE mCMD_ALARM_GETALARMRESPONSE;
}unClusterCmd_Alarm;

//Cluster Attribute结构--------------------------------------------------------------------------------------------------------------
//属性定义
#define ATTRIID_ALARM_ALARMCOUNT			(unsigned short)0x0000
#define ATTRIDATATYPE_ALARM_ALARMCOUNT		ZCL_UINT16

typedef struct{
	unsigned short AttriID;
	union{
		unsigned short AlarmCount;	//AlarmCount, specifies the number of entries currently in the alarm table.
	}un;
}stClusterAttri_Alarm;

#endif /* CLUSTER_ALARM_H_ */
