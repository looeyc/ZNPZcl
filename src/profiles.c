/*
 * profiles.c
 *
 *  Created on: 2014-2-21
 *      Author: root
 */
#include "debuglog.h"
#include "clusters.h"
#include "profiles_HA.h"
#include "profiles.h"

////根据Profile的Cluster列表，将数值型的ClusterID转换为对应的枚举型
////ProfileID - Profile
////ClusterID - 待转换的ClusterID
////pResault - 调用者提供空间，用于返回转换结果
////return 0-成功，else-失败
//int fClusterIDToClusterType(unsigned short ProfileID, unsigned short ClusterID, enClusterType *pResault)
//{
//	enClusterType ClusterType;
//
//	if(pResault==NULL){
//		return -1;
//	}
//
//	if(ProfileID==PROFILE_HA){
//		switch(ClusterID){
//		case HA_GEN_BASIC:				ClusterType=CLUSTER_BASIC;				break;
//		case HA_GEN_POWERCFG:			ClusterType=CLUSTER_POWERCFG;			break;
//		case HA_GEN_DEVTEMPCFG:			ClusterType=CLUSTER_DEVTEMPCFG;         break;
//		case HA_GEN_IDENTIFY:			ClusterType=CLUSTER_IDENTIFY;           break;
//		case HA_GEN_GROUPS:				ClusterType=CLUSTER_GROUPS;             break;
//		case HA_GEN_SCENES:				ClusterType=CLUSTER_SCENES ;            break;
//		case HA_GEN_ONOFF:				ClusterType=CLUSTER_ONOFF;              break;
//		case HA_GEN_ONOFFSWCFG:			ClusterType=CLUSTER_ONOFFSWITCHCFG;     break;
//		case HA_GEN_LEVELCTRL:			ClusterType=CLUSTER_LEVELCTRL;			break;
//		case HA_GEN_ALARM:				ClusterType=CLUSTER_ALARMS;				break;
//		case HA_ILUMIN_MEASURE:			ClusterType=CLUSTER_ILLUMMEASURE;		break;
//		case HA_ILUMIN_LEVELSENSE:		ClusterType=CLUSTER_ILLUMLEVELSENSE;	break;
//		case HA_TEMP_MEASURE:			ClusterType=CLUSTER_TEMPMEASURE;		break;
//		case HA_PRESSURE_MEASURE:		ClusterType=CLUSTER_PRESSUREMEASURE;	break;
//		case HA_FLOW_MEASURE:			ClusterType=CLUSTER_FLOWMEASURE;		break;
//		case HA_RELATIVEHUM_MEASURE:	ClusterType=CLUSTER_RELHUMDITYMEASURE;	break;
//		case HA_OCCUPANCY_SENSE:		ClusterType=CLUSTER_OCCUPANCYSENS;		break;
//		case HA_COLOR_CTRL:				ClusterType=CLUSTER_COLORCTRL;			break;
//		case HA_PUMP_CFG_CTRL:			ClusterType=CLUSTER_PUMPCFGCTRL;		break;
//		case HA_THERMOSTAT:				ClusterType=CLUSTER_THERMOSTAT;			break;
//		case HA_FAN_CTRL:				ClusterType=CLUSTER_FANCTRL;			break;
//		case HA_THERMOSTAT_UI_CFG:		ClusterType=CLUSTER_DEHUMIDIFICATION;	break;
//		case HA_SHADE_CFG:				ClusterType=CLUSTER_SHADECFG;			break;
//		case HA_DOOR_LOCK:				ClusterType=CLUSTER_DOORLOCK;			break;
//		case HA_IAS_ACE:				ClusterType=CLUSTER_IASACE;				break;
//		case HA_IAS_ZONE:				ClusterType=CLUSTER_IASZONE;			break;
//		case HA_IAS_WD:					ClusterType=CLUSTER_IASWD;				break;
//		default:
//			DEBUGLOG2(3,"Error, unknown ClusterID 0x%04X for profile 0x%04X.\n",ClusterID,ProfileID);
//			break;
//		}
//	}else{
//		DEBUGLOG1(3,"Error, unknown profile 0x%04X.\n",ProfileID);
//	}
//	*pResault=ClusterType;
//	return 0;
//}
//
//
////根据Profile的Cluster列表，将枚举型的ClusterID转换为对应的数值型
////ProfileID - Profile
////ClusterType - 待转换的ClusterType
////pResault - 调用者提供空间，用于返回转换结果
////return 0-成功，else-失败
//int fClusterTypeToClusterID(unsigned short ProfileID, enClusterType ClusterType, unsigned short *pResault)
//{
//	unsigned short ClusterID=0;
//
//	if(pResault==NULL){
//		return -1;
//	}
//
//	if(ProfileID==PROFILE_HA){
//		switch(ClusterType){
//		case CLUSTER_BASIC:				ClusterID=HA_GEN_BASIC;				break;
//		case CLUSTER_POWERCFG:			ClusterID=HA_GEN_POWERCFG;			break;
//		case CLUSTER_DEVTEMPCFG:        ClusterID=HA_GEN_DEVTEMPCFG;		break;
//		case CLUSTER_IDENTIFY:          ClusterID=HA_GEN_IDENTIFY;			break;
//		case CLUSTER_GROUPS:           	ClusterID=HA_GEN_GROUPS;			break;
//		case CLUSTER_SCENES:           	ClusterID=HA_GEN_SCENES;			break;
//		case CLUSTER_ONOFF:             ClusterID=HA_GEN_ONOFF;				break;
//		case CLUSTER_ONOFFSWITCHCFG:    ClusterID=HA_GEN_ONOFFSWCFG;		break;
//		case CLUSTER_LEVELCTRL:			ClusterID=HA_GEN_LEVELCTRL;			break;
//		case CLUSTER_ALARMS:			ClusterID=HA_GEN_ALARM;				break;
//		case CLUSTER_ILLUMMEASURE:		ClusterID=HA_ILUMIN_MEASURE;		break;
//		case CLUSTER_ILLUMLEVELSENSE:	ClusterID=HA_ILUMIN_LEVELSENSE;		break;
//		case CLUSTER_TEMPMEASURE:		ClusterID=HA_TEMP_MEASURE;			break;
//		case CLUSTER_PRESSUREMEASURE:	ClusterID=HA_PRESSURE_MEASURE;		break;
//		case CLUSTER_FLOWMEASURE:		ClusterID=HA_FLOW_MEASURE;			break;
//		case CLUSTER_RELHUMDITYMEASURE:	ClusterID=HA_RELATIVEHUM_MEASURE;	break;
//		case CLUSTER_OCCUPANCYSENS:		ClusterID=HA_OCCUPANCY_SENSE;		break;
//		case CLUSTER_COLORCTRL:			ClusterID=HA_COLOR_CTRL;			break;
//		case CLUSTER_PUMPCFGCTRL:		ClusterID=HA_PUMP_CFG_CTRL;			break;
//		case CLUSTER_THERMOSTAT:		ClusterID=HA_THERMOSTAT;			break;
//		case CLUSTER_FANCTRL:			ClusterID=HA_FAN_CTRL;				break;
//		case CLUSTER_DEHUMIDIFICATION:	ClusterID=HA_THERMOSTAT_UI_CFG;		break;
//		case CLUSTER_SHADECFG:			ClusterID=HA_SHADE_CFG;				break;
//		case CLUSTER_DOORLOCK:			ClusterID=HA_DOOR_LOCK;				break;
//		case CLUSTER_IASACE:			ClusterID=HA_IAS_ACE;				break;
//		case CLUSTER_IASZONE:			ClusterID=HA_IAS_ZONE;				break;
//		case CLUSTER_IASWD:				ClusterID=HA_IAS_WD;				break;
//		default:
//			DEBUGLOG2(3,"Error, unknown ClusterType 0x%04X for profile 0x%04X.\n",ClusterType,ProfileID);
//			break;
//		}
//	}else{
//		DEBUGLOG1(3,"Error, unknown profile 0x%04X.\n",ProfileID);
//	}
//	*pResault=ClusterID;
//	return 0;
//}



