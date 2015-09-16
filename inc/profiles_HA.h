/*
 * profiles_HA.h
 *
 *  Created on: 2014-2-22
 *      Author: chengm
 */

#ifndef PROFILES_HA_H_
#define PROFILES_HA_H_

//DeviceID of HA----------------------------------------------------------------------------------------------
//Generic
#define DEV_HA_ON_OFF_SWITCH						((unsigned short)0x0000)
#define DEV_HA_LEVEL_CONTROL_SWITCH 				((unsigned short)0x0001)
#define DEV_HA_ON_OFF_OUTPUT						((unsigned short)0x0002)
#define DEV_HA_LEVEL_CONTROLLABLE_OUTPUT			((unsigned short)0x0003)
#define DEV_HA_SCENE_SELECTOR						((unsigned short)0x0004)
#define DEV_HA_CONFIGURATION_TOOL					((unsigned short)0x0005)
#define DEV_HA_REMOTE_CONTROL						((unsigned short)0x0006)
#define DEV_HA_COMBINED_INTERFACE					((unsigned short)0x0007)
#define DEV_HA_RANGE_EXTENDER						((unsigned short)0x0008)
#define DEV_HA_MAINS_POWER_OUTLET					((unsigned short)0x0009)
#define DEV_HA_DOOR_LOCK							((unsigned short)0x000A)
#define DEV_HA_DOOR_LOCK_CONTROLLER					((unsigned short)0x000B)
#define DEV_HA_SIMPLE_SENSOR						((unsigned short)0x000C)
//Lighting
#define DEV_HA_ON_OFF_LIGHT							((unsigned short)0x0100)
#define DEV_HA_DIMMABLE_LIGHT						((unsigned short)0x0101)
#define DEV_HA_COLOR_DIMMABLE_LIGHT					((unsigned short)0x0102)
#define DEV_HA_ON_OFF_LIGHT_SWITCH					((unsigned short)0x0103)
#define DEV_HA_DIMMER_SWITCH						((unsigned short)0x0104)
#define DEV_HA_COLOR_DIMMER_SWITCH					((unsigned short)0x0105)
#define DEV_HA_LIGHT_SENSOR							((unsigned short)0x0106)
#define DEV_HA_OCCUPANCY_SENSOR						((unsigned short)0x0107)
///Closures
#define DEV_HA_SHADE								((unsigned short)0x0200)
#define DEV_HA_SHADE_CONTROLLER						((unsigned short)0x0201)
#define DEV_HA_WINDOW_COVERING_DEVICE				((unsigned short)0x0202)
#define DEV_HA_WINDOW_COVERING_CONTROLLER			((unsigned short)0x0203)
//HVAC
#define DEV_HA_HEATING_COOLING_UNIT					((unsigned short)0x0300)
#define DEV_HA_THERMOSTATE							((unsigned short)0x0301)
#define DEV_HA_TEMPERATURE_SENSOR					((unsigned short)0x0302)
#define DEV_HA_PUMP									((unsigned short)0x0303)
#define DEV_HA_PUMP_CONTROLLER						((unsigned short)0x0304)
#define DEV_HA_PRESSURE_SENSOR						((unsigned short)0x0305)
#define DEV_HA_FLOW_SENSOR							((unsigned short)0x0306)
//Intruder Alarm Systems
#define DEV_HA_IAS_CONTROL_AND_INDICATING_EQUIPMENT	((unsigned short)0x0400)
#define DEV_HA_IAS_ANCILLARY_CONTROL_EQUIPMENT		((unsigned short)0x0401)
#define DEV_HA_IAS_ZONE		 						((unsigned short)0x0402)
#define DEV_HA_IAS_WARNING_DEVICE					((unsigned short)0x0403)

#endif /* PROFILES_HA_H_ */
