/*
 * zcldatatype.h
 *
 *  Created on: 2014-2-21
 *      Author: chengm
 */

#ifndef ZCLDATATYPE_H_
#define ZCLDATATYPE_H_

#define MAXZCLDATALEN			16

#define ZCL_NODATA				(unsigned char)0x00
#define ZCL_GEN8				(unsigned char)0x08
#define ZCL_GEN16				(unsigned char)0x09
#define ZCL_GEN24				(unsigned char)0x0A
#define ZCL_GEN32				(unsigned char)0x0B
#define ZCL_GEN40				(unsigned char)0x0C
#define ZCL_GEN48				(unsigned char)0x0D
#define ZCL_GEN56				(unsigned char)0x0E
#define ZCL_GEN64				(unsigned char)0x0F
#define ZCL_BOOL				(unsigned char)0x10
#define ZCL_BITMAP8				(unsigned char)0x18
#define ZCL_BITMAP16			(unsigned char)0x19
#define ZCL_BITMAP24			(unsigned char)0x1A
#define ZCL_BITMAP32			(unsigned char)0x1B
#define ZCL_BITMAP40			(unsigned char)0x1C
#define ZCL_BITMAP48			(unsigned char)0x1D
#define ZCL_BITMAP56			(unsigned char)0x1E
#define ZCL_BITMAP64			(unsigned char)0x1F
#define ZCL_UINT8				(unsigned char)0x20
#define ZCL_UINT16				(unsigned char)0x21
#define ZCL_UINT24				(unsigned char)0x22
#define ZCL_UINT32				(unsigned char)0x23
#define ZCL_UINT40				(unsigned char)0x24
#define ZCL_UINT48				(unsigned char)0x25
#define ZCL_UINT56				(unsigned char)0x26
#define ZCL_UINT64				(unsigned char)0x27
#define ZCL_INT8				(unsigned char)0x28
#define ZCL_INT16				(unsigned char)0x29
#define ZCL_INT24				(unsigned char)0x2A
#define ZCL_INT32				(unsigned char)0x2B
#define ZCL_INT40				(unsigned char)0x2C
#define ZCL_INT48				(unsigned char)0x2D
#define ZCL_INT56				(unsigned char)0x2E
#define ZCL_INT64				(unsigned char)0x2F
#define ZCL_ENUM8				(unsigned char)0x30
#define ZCL_ENUM16				(unsigned char)0x31
#define ZCL_SEMIFLOAT			(unsigned char)0x38
#define ZCL_SINGLEFLOAT			(unsigned char)0x39
#define ZCL_DOUBLEFLOAT			(unsigned char)0x3A
#define ZCL_OCTETSTRING			(unsigned char)0x41
#define ZCL_CHARSTRING			(unsigned char)0x42
#define ZCL_LONGOCTETSTRING		(unsigned char)0x43
#define ZCL_LONGCHARSTRING		(unsigned char)0x44
#define ZCL_ARRAY				(unsigned char)0x48
#define ZCL_STRUCT				(unsigned char)0x4C
#define ZCL_SET					(unsigned char)0x50
#define ZCL_BAG					(unsigned char)0x51
#define ZCL_TIMEOFDAY			(unsigned char)0xE0
#define ZCL_DATE				(unsigned char)0xE1
#define ZCL_UTCTIME				(unsigned char)0xE2
#define ZCL_CLUSTERID			(unsigned char)0xE8
#define ZCL_ATTRIID				(unsigned char)0xE9
#define ZCL_BACNETID			(unsigned char)0xEA
#define ZCL_IEEEADDR			(unsigned char)0xF0
#define ZCL_SECKEY128			(unsigned char)0xF1
#define ZCL_UNKNOWN				(unsigned char)0xFF

//一个Zcl数据单元
typedef struct{
	unsigned char ZclDataType;
	unsigned char DataSize;//Data部分占用的字节数，包含DataHdr部分
	unsigned char DataHdrLen;//Data中DataHdr部分的长度
	unsigned char Data[MAXZCLDATALEN];//对于有些数据类型，例如string型，前面的部分字节DataHdr有其他含义，这里面的内容包含两部分
}stZclDataUnit;

//从一个字节流buffer中提取一个指定ZclDataType类型的ZclData
//ZclDataType - 指定需要读取的zcl数据类型
//SrcDataLen - 待读取的数据长度，该长度如果不够，读取会返回失败，如果过长，超过部分将被忽略
//pSrcDataBuffer - 待读取的数据区指针
//pZclData - 调用者提供空间，用于返回读取到的数据
//return <=0读取失败，else-读取的字节数，即pSrcDataBuffer中被读走的字节数
int ZclDataTypeRead(unsigned char ZclDataType, unsigned char SrcDataLen, unsigned char *pSrcDataBuffer, stZclDataUnit *pZclData);

#endif /* ZCLDATATYPE_H_ */
