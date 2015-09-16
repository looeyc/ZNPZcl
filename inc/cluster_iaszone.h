/*
 * cluster_iaszone.h
 *
 *  Created on: 2014-3-19
 *      Author: chengm
 */

#ifndef CLUSTER_IASZONE_H_
#define CLUSTER_IASZONE_H_

//Cluster Command结构-----------------------------------------------------------------------------------------------------------------
//CMD定义
//Commands Received
#define CMD_IASZONE_ZONEENROLLRESPONSE		(unsigned char)0x00	//Zone Enroll Response
//Commands Generated
#define CMD_IASZONE_ZONESTATUSCHANGENOTIF	(unsigned char)0x00	//Zone Status Change Notification
#define CMD_IASZONE_ZONEENROLLREQUEST		(unsigned char)0x01	//Zone Enroll Request

typedef struct{
	unsigned char EnrollRespCode;//Enroll response code
	unsigned char ZoneID;//Zone ID
}stCMD_IASZONE_ZONEENROLLRESPONSE;

typedef struct{
	unsigned short ZoneStatus;//Zone Status, shall be the current value of the ZoneStatus attribute.
	unsigned char ExtStatus;//Extended Status, reserved for additional status information and shall	be set to zero.
}stCMD_IASZONE_ZONESTATUSCHANGENOTIF;

typedef struct{
	unsigned short ZoneType;//Zone Type
	unsigned short ManufactCode;//Manufacturer Code
}stCMD_IASZONE_ZONEENROLLREQUEST;

typedef union{
	stCMD_IASZONE_ZONEENROLLRESPONSE mCMD_IASZONE_ZONEENROLLRESPONSE;
	stCMD_IASZONE_ZONESTATUSCHANGENOTIF mCMD_IASZONE_ZONESTATUSCHANGENOTIF;
	stCMD_IASZONE_ZONEENROLLREQUEST mCMD_IASZONE_ZONEENROLLREQUEST;
}unClusterCmd_IASZone;

//Cluster Attribute结构--------------------------------------------------------------------------------------------------------------
//属性定义
#define ATTRIID_IASZONE_ZONESTATE 			(unsigned short)0x0000
#define ATTRIDATATYPE_IASZONE_ZONESTATE 	ZCL_ENUM8

#define ATTRIID_IASZONE_ZONETYPE			(unsigned short)0x0001
#define ATTRIDATATYPE_IASZONE_ZONETYPE	 	ZCL_ENUM16

#define ATTRIID_IASZONE_ZONESTATUS			(unsigned short)0x0002
#define ATTRIDATATYPE_IASZONE_ZONESTATUS 	ZCL_BITMAP16

#define ATTRIID_IASZONE_IASCIEADDR			(unsigned short)0x0010
#define ATTRIDATATYPE_IASZONE_IASCIEADDR 	ZCL_IEEEADDR

typedef struct{
	unsigned short AttriID;
	union{
		unsigned char ZoneState;			//0x0000 ZoneState 8-bit enumeration
		unsigned short ZoneType;			//0x0001 ZoneType 16-bit enumeration
		unsigned short ZoneStatus;			//0x0002 ZoneStatus 16-bit bitmap
		unsigned char IAS_CIE_Address[8];	//0x0010 IAS_CIE_Address IEEE address
	}un;
}stClusterAttri_IASZone;

#endif /* CLUSTER_IASZONE_H_ */







