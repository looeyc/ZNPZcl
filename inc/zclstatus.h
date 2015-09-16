/*
 * zclstatus.h
 *
 *  Created on: 2014-2-21
 *      Author: chengm
 */

#ifndef ZCLSTATUS_H_
#define ZCLSTATUS_H_

#define ZCLS_SUCCESS 						(unsigned char)0x00 //Operation was successful.
#define ZCLS_FAILURE 						(unsigned char)0x01 //Operation was not successful.
#define ZCLS_NOT_AUTHORIZED 				(unsigned char)0x7E //The sender of the command does not have authorization to carry out this command.
#define ZCLS_RESERVED_FIELD_NOT_ZERO 		(unsigned char)0x7F //A reserved field/subfield/bit contains a non-zero value.
#define ZCLS_MALFORMED_COMMAND 				(unsigned char)0x80 //The command appears to contain the wrong fields, as detected either by the presence of one or more invalid field entries or by there being missing fields. Command not carried out. Implementer has discretion as to whether to return this error or INVALID_FIELD.
#define ZCLS_UNSUP_CLUSTER_COMMAND 			(unsigned char)0x81 //The specified cluster command is not supported on the device. Command not carried out.
#define ZCLS_UNSUP_GENERAL_COMMAND 			(unsigned char)0x82 //The specified general ZCL command is not supported on the device.
#define ZCLS_UNSUP_MANUF_CLUSTER_COMMAND 	(unsigned char)0x83 //A manufacturer specific unicast, cluster specific command was received with an unknown manufacturer code, or the manufacturer code was recognized but the command is not supported.
#define ZCLS_UNSUP_MANUF_GENERAL_COMMAND 	(unsigned char)0x84 //A manufacturer specific unicast, ZCL specific command was received with an unknown manufacturer code, or the manufacturer code was recognized but the command is not supported.
#define ZCLS_INVALID_FIELD 					(unsigned char)0x85 //At least one field of the command contains an incorrect value, according to the specification the device is implemented to.
#define ZCLS_UNSUPPORTED_ATTRIBUTE 			(unsigned char)0x86 //The specified attribute does not exist on the device.
#define ZCLS_INVALID_VALUE 					(unsigned char)0x87 //Out of range error, or set to a reserved value. Attribute keeps its old value. Note that an attribute value may be out of range if an attribute is related to another, e.g. with minimum and maximum attributes. See the individual attribute descriptions for specific details.
#define ZCLS_READ_ONLY 						(unsigned char)0x88 //Attempt to write a read only attribute.
#define ZCLS_INSUFFICIENT_SPACE 			(unsigned char)0x89 //An operation (e.g. an attempt to create an entry in a table) failed due to an insufficient amount of free space available.
#define ZCLS_DUPLICATE_EXISTS 				(unsigned char)0x8A //An attempt to create an entry in a table failed due to a duplicate entry already being present in the table.
#define ZCLS_NOT_FOUND 						(unsigned char)0x8B //The requested information (e.g. table entry) could not be found.
#define ZCLS_UNREPORTABLE_ATTRIBUTE 		(unsigned char)0x8C //Periodic reports cannot be issued for this attribute.
#define ZCLS_INVALID_DATA_TYPE 				(unsigned char)0x8D //The data type given for an attribute is incorrect. Command not carried out.
#define ZCLS_INVALID_SELECTOR 				(unsigned char)0x8E //The selector for an attribute is incorrect.
#define ZCLS_WRITE_ONLY 					(unsigned char)0x8F //A request has been made to read an attribute that the requestor is not authorized to read. No action taken.
#define ZCLS_INCONSISTENT_STARTUP_STATE 	(unsigned char)0x90 //Setting the requested values would put the device in an inconsistent state on startup. No action taken.
#define ZCLS_DEFINED_OUT_OF_BAND 			(unsigned char)0x91 //An attempt has been made to write an attribute that is present but is defined using an out-of-band method and not over the air.
#define ZCLS_INCONSISTENT 					(unsigned char)0x92 //The supplied values (e.g. contents of table cells) are inconsistent.
#define ZCLS_ACTION_DENIED 					(unsigned char)0x93 //The credentials presented by the device sending the command are not sufficient to perform this action.
#define ZCLS_TIMEOUT 						(unsigned char)0x94 //The exchange was aborted due to excessive response time.
#define ZCLS_ABORT 							(unsigned char)0x95 //Failed case when a client or a server decides to abort the upgrade process.
#define ZCLS_INVALID_IMAGE 					(unsigned char)0x96 //Invalid OTA upgrade image (ex. failed signature validation or signer information check or CRC check).
#define ZCLS_WAIT_FOR_DATA 					(unsigned char)0x97 //Server does not have data block available yet.
#define ZCLS_NO_IMAGE_AVAILABLE 			(unsigned char)0x98 //No OTA upgrade image available for a particular client.
#define ZCLS_REQUIRE_MORE_IMAGE 			(unsigned char)0x99 //The client still requires more OTA upgrade image files in order to successfully upgrade.
#define ZCLS_HARDWARE_FAILURE 				(unsigned char)0xC0 //An operation was unsuccessful due to a hardware failure.
#define ZCLS_SOFTWARE_FAILURE 				(unsigned char)0xC1 //An operation was unsuccessful due to a software failure.
#define ZCLS_CALIBRATION_ERROR 				(unsigned char)0xC2 //An error occurred during calibration.

#endif /* ZCLSTATUS_H_ */
