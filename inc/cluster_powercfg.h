/*
 * cluster_powercfg.h
 *
 *  Created on: 2014-3-21
 *      Author: chengm
 */

#ifndef CLUSTER_POWERCFG_H_
#define CLUSTER_POWERCFG_H_

//Cluster Command结构-----------------------------------------------------------------------------------------------------------------
//CMD定义
//Commands Received
//无
//Commands Generated
//无
typedef union{
	//无
}unClusterCmd_PowerCfg;

//Cluster Attribute结构--------------------------------------------------------------------------------------------------------------
//属性定义
#define ATTRIID_POWERCFG_MAINSVLOTAGE 						(unsigned short)0x0000//MainsVoltage
#define ATTRIDATATYPE_POWERCFG_MAINSVLOTAGE 				ZCL_UINT16

#define ATTRIID_POWERCFG_MAINSFREQUENCY						(unsigned char)0x0001//MainsFrequency
#define ATTRIDATATYPE_POWERCFG_MAINSFREQUENCY 				ZCL_UINT8

#define ATTRIID_POWERCFG_MAINSALARMSMASK					(unsigned short)0x0010 //MainsAlarmMask
#define ATTRIDATATYPE_POWERCFG_MAINSALARMSMASK				ZCL_BITMAP8

#define ATTRIID_POWERCFG_MAINSVLOTAGEMINTHRESHOLD			(unsigned short)0x0011 //MainsVoltageMinThreshold
#define ATTRIDATATYPE_POWERCFG_MAINSVLOTAGEMINTHRESHOLD		ZCL_UINT16

#define ATTRIID_POWERCFG_MAINSVLOTAGEMAXTHRESHOLD			(unsigned short)0x0012 //MainsVoltageMaxThreshold
#define ATTRIDATATYPE_POWERCFG_MAINSVLOTAGEMAXTHRESHOLD		ZCL_UINT16

#define ATTRIID_POWERCFG_MAINSVLOTAGEDWELLTRIPPOINT			(unsigned short)0x0013 //MainsVoltageDwellTripPoint
#define ATTRIDATATYPE_POWERCFG_MAINSVLOTAGEDWELLTRIPPOINT	ZCL_UINT16

#define ATTRIID_POWERCFG_BATTERYVOLTAGE						(unsigned short)0x0020 //BatteryVoltage
#define ATTRIDATATYPE_POWERCFG_BATTERYVOLTAGE				ZCL_UINT8

//其他定义
#define POWERCFG_MAINS_ALARM_VOLT_TOO_LOW					(unsigned char)0x01
#define POWERCFG_MAINS_ALARM_VOLT_TOO_HIGH					(unsigned char)0x02

typedef struct{
	unsigned short AttriID;
	union{
		unsigned short MainsVoltage;//in units of 100mV.
		unsigned char MainsFrequency;//in Hz
		unsigned char MainsAlarmMask;//BIT0-Mains Voltage too low、BIT1-Mains Voltage too high
		unsigned short MainsVoltageMinThreshold;//in units of 100mV.
		unsigned short MainsVoltageMaxThreshold;//in units of 100mV.
		unsigned short MainsVoltageDwellTripPoint;//in seconds
		unsigned char BatteryVoltage;//in units of 100mV.
	}un;
}stClusterAttri_PowerCfg;

#endif /* CLUSTER_POWERCFG_H_ */
