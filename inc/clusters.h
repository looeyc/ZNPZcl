/*
 * clusters.h
 *
 *  Created on: 2014-2-20
 *      Author: chengm
 */

#ifndef CLUSTERS_H_
#define CLUSTERS_H_

#include "cluster_onoff.h"
#include "cluster_levelctrl.h"
#include "cluster_iaszone.h"
#include "cluster_powercfg.h"
#include "cluster_alarm.h"
#include "cluster_identify.h"

#include "cluster_selfintroduction.h"
#include "cluster_its_vehicle_presence_sensing_geomag.h"

//GENERAL SPECIFICATION
#define CLUSTER_BASIC				((unsigned short)0x0000)//Basic Cluster
#define CLUSTER_POWERCFG			((unsigned short)0x0001)//Power Configuration Cluster
#define CLUSTER_DEVTEMPCFG			((unsigned short)0x0002)//Device Temperature Configuration Cluster
#define CLUSTER_IDENTIFY			((unsigned short)0x0003)//Identify Cluster
#define CLUSTER_GROUPS				((unsigned short)0x0004)//Groups Cluster
#define CLUSTER_SCENES				((unsigned short)0x0005)//Scenes Cluster
#define CLUSTER_ONOFF				((unsigned short)0x0006)//On/Off Cluster
#define CLUSTER_ONOFFSWITCHCFG		((unsigned short)0x0007)//On/Off Switch Configuration Cluster
#define CLUSTER_LEVELCTRL			((unsigned short)0x0008)//Level Control Cluster
#define CLUSTER_ALARMS				((unsigned short)0x0009)//Alarms Cluster
#define CLUSTER_TIME				((unsigned short)0x000A)//Time Cluster
#define CLUSTER_RSSILOCATION		((unsigned short)0x000B)//RSSI Location Cluster
#define CLUSTER_ANALOGINPUT			((unsigned short)0x000C)
#define CLUSTER_ANALOGOUTPUT		((unsigned short)0x000D)
#define CLUSTER_ANALOGVALUE			((unsigned short)0x000E)
#define CLUSTER_BINARYINPUT			((unsigned short)0x000F)
#define CLUSTER_BINARYOUTPUT		((unsigned short)0x0010)
#define CLUSTER_BINARYVALUE			((unsigned short)0x0011)
#define CLUSTER_MULTISTATEINPUT		((unsigned short)0x0012)
#define CLUSTER_MULTISTATEOUTPUT	((unsigned short)0x0013)
#define CLUSTER_MULTISTATEVALUE		((unsigned short)0x0014)
#define	CLUSTER_COMMISSION			((unsigned short)0x0015)//Commissioning Cluster
//MEASUREMENT AND SENSING SPECIFICATION
#define	CLUSTER_ILLUMMEASURE		((unsigned short)0x0400)//Illuminance Measurement Cluster
#define	CLUSTER_ILLUMLEVELSENSE		((unsigned short)0x0401)//Illuminance Level Sensing Cluster
#define	CLUSTER_TEMPMEASURE			((unsigned short)0x0402)//Temperature Measurement Cluster
#define	CLUSTER_PRESSUREMEASURE		((unsigned short)0x0403)//Pressure Measurement Cluster
#define	CLUSTER_FLOWMEASURE			((unsigned short)0x0404)//Flow Measurement Cluster
#define	CLUSTER_RELHUMDITYMEASURE	((unsigned short)0x0405)//Relative Humidity Measurement Cluster
#define	CLUSTER_OCCUPANCYSENS		((unsigned short)0x0406)//Occupancy Sensing Cluster
//LIGHTING SPECIFICATION
#define	CLUSTER_COLORCTRL			((unsigned short)0x0300)//Color Control Cluster
#define	CLUSTER_BALLASTCFG			((unsigned short)0x0301)//Ballast Configuration Cluster
//HVAC SPECIFICATION
#define	CLUSTER_PUMPCFGCTRL			((unsigned short)0x0200)//Pump Configuration and Control Cluster
#define	CLUSTER_THERMOSTAT			((unsigned short)0x0201)//Thermostat Cluster
#define	CLUSTER_FANCTRL				((unsigned short)0x0202)//Fan Control
#define	CLUSTER_DEHUMIDIFICATION	((unsigned short)0x0203)//Dehumidification Control
#define	CLUSTER_THERMOSTATUICFG		((unsigned short)0x0204)//Thermostat User Interface Configuration Cluster
//CLOSURES SPECIFICATION
#define	CLUSTER_SHADECFG			((unsigned short)0x0100)//Shade Configuration Cluster
#define	CLUSTER_DOORLOCK			((unsigned short)0x0101)//Door Lock Cluster
//SECURITY AND SAFETY SPECIFICATION
#define	CLUSTER_IASZONE				((unsigned short)0x0500)//IAS Zone Cluster
#define	CLUSTER_IASACE				((unsigned short)0x0501)//IAS ACE Cluster
#define	CLUSTER_IASWD				((unsigned short)0x0502)//IAS WD Cluster

//自定义Cluster
//ITS
#define	CLUSTER_ITS_VEHICLE_PRESENCE_SENSING_GEOMAG		((unsigned short)0xC005) //地磁车辆探测
//Self introduction, 自我介绍
#define	CLUSTER_SELFINTRODUCTION						((unsigned short)0xC000) //自我介绍

//Cluster command联合体类型
typedef union{
	unClusterCmd_OnOff unClusterCmd_OnOff;
	unClusterCmd_LevelCtrl unClusterCmd_LevelCtrl;
	unClusterCmd_IASZone unClusterCmd_IASZone;
	unClusterCmd_PowerCfg unClusterCmd_PowerCfg;
	unClusterCmd_Alarm unClusterCmd_Alarm;
	unClusterCmd_Identify unClusterCmd_Identify;
	//自定义
	unClusterCmd_SelfIntroduction unClusterCmd_SelfIntroduction;
	unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag unClusterCmd_ITS_Vehicle_Presence_Sensing_Geomag;
}unClusterCmd;

//Cluster attribute联合体类型
typedef union{
	stClusterAttri_OnOff mClusterAttri_OnOff;
	stClusterAttri_LevelCtrl mClusterAttri_LevelCtrl;
	stClusterAttri_IASZone mClusterAttri_IASZone;
	stClusterAttri_PowerCfg mClusterAttri_PowerCfg;
	stClusterAttri_Alarm mClusterAttri_Alarm;
	stClusterAttri_Identify mClusterAttri_Identify;
	//自定义
	stClusterAttri_SelfIntroduction mClusterAttri_SelfIntroduction;
	stClusterAttri_ITS_Vehicle_Presence_Sensing_Geomag mClusterAttri_ITS_Vehicle_Presence_Sensing_Geomag;
}unClusterAttri;

#endif /* CLUSTERS_H_ */
