/*
 * alige.h
 *
 *  Created on: 2014-2-19
 *      Author: chengm
 */

#ifndef ALIGE_H_
#define ALIGE_H_

//以指针p所指地址为起始，读取两个字节拼成一个Uint16型的数值，不需要p字节对齐
#define UNALIGEREAD_16BIT(p)		(((unsigned short)(*((unsigned char *)(p))))|(((unsigned short)(*((unsigned char *)(p)+1)))<<8))
//以指针p所指地址为起始，写入一个Uint16型的数据，不需要p字节对齐
#define UNALIGEWRITE_16BIT(p,a)		{*(unsigned char *)(p)=(unsigned char)(a);*((unsigned char *)(p)+1)=(unsigned char)((a)>>8);}


#endif /* ALIGE_H_ */
