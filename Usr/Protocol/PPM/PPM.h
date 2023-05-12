/****************************************************************************
 * Title                 :   PPM Protocol
 * Filename              :   PPM.h
 * Author                :   Ehsan Khiabani
 * Origin Date           :   05/09/2023
 * Version               :   1.0.0
 * Compiler              :
 * Target                :   STM32F746
 * Notes                 :   None
 *
 * This module demodulates the PPM signal and extracts channel data.
 * This module is Tested by developers. The developers tried to implement
 * guards and make this module safer for users but it's essential to be
 * careful when using it.
 *****************************************************************************/

/*************** INTERFACE CHANGE LIST **************************************
 *
 *    Date    Version   Author         Description
 *  05/09/23   1.0.0   Ehsan Khiabani   Interface Created.
 *
 *****************************************************************************/
/** @file PPM.h
 *  @brief This module is used to extract channel data
 */

#ifndef __PPM_H
#define __PPM_H

/******************************************************************************
 * Includes
 *******************************************************************************/
#include <stdint.h>
/******************************************************************************
 * Preprocessor Constants
 *******************************************************************************/

/******************************************************************************
 * Configuration Constants
 *******************************************************************************/

/******************************************************************************
 * Macros
 *******************************************************************************/
#define kChannelSpliteWidth		400    /*Set width of channel splitter signal*/
#define kPacketSpliteWidthMin	3000   /*Set Minimum value that is lower than
 	 	 	 	 	 	 	 	 	 	 packet splitter and bigger than channel
 	 	 	 	 	 	 	 	 	 	 maximum of channel pulse width, set
 	 	 	 	 	 	 	 	 	 	 this number carefully*/
#define kNumberOfChannel		8		/*Set Number of pulse*/
#define kChannelWidthMin		1200
#define kChannelWidthMax		1500
/******************************************************************************
 * Typedefs
 *******************************************************************************/
/**
 * @enum
 * @brief Module Status enumeration definition
 *
 */
typedef enum {
	kPpmPacketLost	= 0,/**< Packet loss status ID */
	kPpmOK			= 1 /**< Successful operation status ID */
}tyPpmStatus;
/**
 * @enum
 * @brief channel enumeration definition
 *
 */
typedef enum {
	kPpmChannel1 = 0,/**< Channel 1 ID */
	kPpmChannel2 = 1,/**< Channel 2 ID */
	kPpmChannel3 = 2,/**< Channel 3 ID */
	kPpmChannel4 = 3,/**< Channel 4 ID */
	kPpmChannel5 = 4,/**< Channel 5 ID */
	kPpmChannel6 = 5,/**< Channel 6 ID */
	kPpmChannel7 = 6,/**< Channel 7 ID */
	kPpmChannel8 = 7 /**< Channel 8 ID */
}tyPpmChannel;
/******************************************************************************
 * Variables
 *******************************************************************************/

/******************************************************************************
 * Function Prototypes
 *******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif
void PpmEngine(const uint16_t pulse_width);
void PpmTimeOut(void);
tyPpmStatus PpmStatusCheck(void);
uint8_t PpmGetChannelPercent(const tyPpmChannel channel_number);
uint16_t PpmGetChannelRaw(const tyPpmChannel channel_number);
uint32_t PpmGetPacketNumber(void);
__attribute__((weak)) void PpmHeartBeat(void);
__attribute__((weak)) void PpmfualtDetect(void);
#ifdef __cplusplus
} // extern "C"
#endif

#endif /*__PPM_H_*/

/*** End of File **************************************************************/
