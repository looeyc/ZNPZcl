/*
 * zcldatatype.c
 *
 *  Created on: 2014-2-21
 *      Author: chengm
 */
#include <stdio.h>
#include <memory.h>
#include "debuglog.h"
#include "zcldatatype.h"

//从一个字节流buffer中提取一个指定ZclDataType类型的ZclData
//ZclDataType - 指定需要读取的zcl数据类型
//SrcDataLen - 待读取的数据长度，该长度如果不够，读取会返回失败，如果过长，超过部分将被忽略
//pSrcDataBuffer - 待读取的数据区指针
//pZclData - 调用者提供空间，用于返回读取到的数据
//return <0读取失败，0没有数据可读了，else-读取的字节数，即pSrcDataBuffer中被读走的字节数
int ZclDataTypeRead(unsigned char ZclDataType, unsigned char SrcDataLen, unsigned char *pSrcDataBuffer, stZclDataUnit *pZclData)
{
	unsigned int DataSize=0;
	unsigned int DataOffset=0;//有的数据类型前面个别字节不是数据

	if((pSrcDataBuffer==NULL)||(pZclData==NULL)){
		return -1;
	}
	if(SrcDataLen==0){
		return 0;
	}

	//确定数据占用空间大小
	switch(ZclDataType){
	case ZCL_NODATA:
	case ZCL_UNKNOWN:
		DataSize=0;
		break;
	case ZCL_GEN8:
	case ZCL_BOOL:
	case ZCL_BITMAP8:
	case ZCL_UINT8:
	case ZCL_INT8:
	case ZCL_ENUM8:
		DataSize=1;
		break;
	case ZCL_GEN16:
	case ZCL_BITMAP16:
	case ZCL_UINT16:
	case ZCL_INT16:
	case ZCL_ENUM16:
	case ZCL_SEMIFLOAT:
	case ZCL_CLUSTERID:
	case ZCL_ATTRIID:
		DataSize=2;
		break;
	case ZCL_GEN24:
	case ZCL_BITMAP24:
	case ZCL_UINT24:
	case ZCL_INT24:
		DataSize=3;
		break;
	case ZCL_GEN32:
	case ZCL_BITMAP32:
	case ZCL_UINT32:
	case ZCL_INT32:
	case ZCL_SINGLEFLOAT:
	case ZCL_TIMEOFDAY:
	case ZCL_DATE:
	case ZCL_UTCTIME:
	case ZCL_BACNETID:
		DataSize=4;
		break;
	case ZCL_GEN40:
	case ZCL_BITMAP40:
	case ZCL_UINT40:
	case ZCL_INT40:
		DataSize=5;
		break;
	case ZCL_GEN48:
	case ZCL_BITMAP48:
	case ZCL_UINT48:
	case ZCL_INT48:
		DataSize=6;
		break;
	case ZCL_GEN56:
	case ZCL_BITMAP56:
	case ZCL_UINT56:
	case ZCL_INT56:
		DataSize=7;
		break;
	case ZCL_GEN64:
	case ZCL_BITMAP64:
	case ZCL_UINT64:
	case ZCL_INT64:
	case ZCL_DOUBLEFLOAT:
	case ZCL_IEEEADDR:
		DataSize=8;
		break;
	case ZCL_SECKEY128:
		DataSize=16;
		break;
	case ZCL_OCTETSTRING:
	case ZCL_CHARSTRING:
	case ZCL_LONGOCTETSTRING:
	case ZCL_LONGCHARSTRING:
		DataSize=pSrcDataBuffer[0];
		DataOffset=1;
		break;
	//case ZCL_ARRAY:
	//	break;
	//case ZCL_STRUCT:
	//	break;
	//case ZCL_SET:
	//	break;
	//case ZCL_BAG:
	//	break;
	default:
		DEBUGLOG1(3,"Error, unknown ZclDataType 0x%02X.\n",ZclDataType);
		return -2;
		break;
	}

	if(SrcDataLen<DataSize+DataOffset){//实际的数据长度不够
		return -3;
	}
	if(DataSize>MAXZCLDATALEN){//实际的数据长度超过了该有的长度
		return -4;
	}

	pZclData->ZclDataType=ZclDataType;
	pZclData->DataSize=DataSize;
	pZclData->DataHdrLen=DataOffset;
	memcpy(pZclData->Data,pSrcDataBuffer+DataOffset,DataSize);

	return DataSize;
}



























