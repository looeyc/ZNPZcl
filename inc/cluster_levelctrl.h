/*
 * cluster_levelctrl.h
 *
 *  Created on: 2014-2-21
 *      Author: chengm
 */

#ifndef CLUSTER_LEVELCTRL_H_
#define CLUSTER_LEVELCTRL_H_

//Cluster Command结构-----------------------------------------------------------------------------------------------------------------
//CMD定义
#define CMD_LEVELCTRL_MOVETOLEVEL			(unsigned char)0x01
#define CMD_LEVELCTRL_MOVE					(unsigned char)0x02

typedef struct{
	unsigned char Level;//目标Level
	unsigned short TransTime;//切换时间（秒），取0xFFFF时，由OnOffTransitionTime属性(这是协议规定的可选属性)绝对切换时间，如果OnOffTransitionTime属性不存在，则立即切换
}stCMD_LEVELCTRL_MOVETOLEVEL;

typedef struct{
	unsigned char MoveMode;//0x00-Up, 0x01-Down
	unsigned char Rate;
}stCMD_LEVELCTRL_MOVE;

typedef union{
	stCMD_LEVELCTRL_MOVETOLEVEL mCMD_LEVELCTRL_MOVETOLEVEL;
	stCMD_LEVELCTRL_MOVE mCMD_LEVELCTRL_MOVE;
}unClusterCmd_LevelCtrl;

//Cluster Attribute结构--------------------------------------------------------------------------------------------------------------
//属性定义
#define ATTRI_LEVELCTRL_CURRENTLEVEL		(unsigned short)0x0000
#define ATTRI_LEVELCTRL_REMAININGTIME		(unsigned short)0x0001
#define ATTRI_LEVELCTRL_ONOFFTRANSTIME		(unsigned short)0x0002
#define ATTRI_LEVELCTRL_ONLEVEL				(unsigned short)0x0003

typedef struct{
	unsigned short AttriID;
	union{
		unsigned char CurrentLevel;
		unsigned short RemainingTime;
		unsigned short OnOffTransitionTime;
		unsigned char OnLevel;
	}un;
}stClusterAttri_LevelCtrl;

#endif /* CLUSTER_LEVELCTRL_H_ */
