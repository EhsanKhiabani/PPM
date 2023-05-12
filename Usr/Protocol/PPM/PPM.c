/*******************************************************************************
* Title                 :   PPM Protocol
* Filename              :   PPM.c
* Author                :   Ehsan Khiabani
* Origin Date           :   05/09/2023
* Version               :   1.0.0
* Compiler              :   
* Target                :   STM32F746
* Notes                 :   None
*
* THIS SOFTWARE IS PROVIDED BY BENINGO ENGINEERING "AS IS" AND ANY EXPRESSED
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL BENINGO ENGINEERING OR ITS CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/

/*************** SOURCE REVISION LOG *****************************************
*
*    Date    Version   Author         Description 
*  05/09/23   1.0.0   Ehsan Khiabani   Initial Release.
*
*******************************************************************************
*******************************************************************************
@verbatim
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================
  [..]
 	 The PPM module can be used as follows:

 	 (#) Check the macro parameters in header file and replace it if it's
 	 	 essential.
 	 (#) Call PpmEngine() in the timer callback function
@endverbatim
******************************************************************************


 */
/** @file  PPM.c
 *  @brief This module is used to extract ppm signal
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "PPM.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
/**
 * Doxygen tag for documenting variables and constants
 */



/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
/**
 * @enum
 * @brief state machine enumeration
 *
 */
typedef enum {
	kPpmIdeal	= 0, /**< Ideal state ID */
	kPpmChannel	= 1/**< Channel read state ID */
}tyPpmStateMachine;
/******************************************************************************
* Module Variable Definitions
*******************************************************************************/


static tyPpmStateMachine ppm_state_machine = kPpmIdeal;
static tyPpmStatus ppm_status = kPpmPacketLost;
static uint16_t	ppm_channel_raw[24];
static uint8_t	ppm_channel_scaled[24];
static uint8_t	ppm_channel_counter = 0;
static uint32_t ppm_number_of_packet = 0;


/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
/**
 * @fn void PpmEngine(const uint16_t)
 * @brief This function must call when the pulse width is calculated
 * 		  This function get pulse width and run a state machine. State
 * 		  machine have two state. In Ideal state pulse width check and
 * 		  if it's greater than a specified threshold, state change. In
 * 		  Channel state, pulse width modulate from ppm signal.
 * @pre   Must initialize timer, ppm module, and set the signal parameter
 * @post  Nothing to do
 * @param pulse_width
 */
void PpmEngine(const uint16_t pulse_width)
{
	switch(ppm_state_machine) {
	case kPpmIdeal:
		if( pulse_width > kPacketSpliteWidthMin )
		{
			ppm_state_machine = kPpmChannel;
			ppm_channel_counter = 0;
		}
		break;
	case kPpmChannel:
		if( pulse_width < kPacketSpliteWidthMin )
		{
			ppm_channel_raw[ppm_channel_counter] = pulse_width - kChannelSpliteWidth;
			ppm_channel_scaled[ppm_channel_counter] = (pulse_width - kChannelSpliteWidth - kChannelWidthMin)
													/ (kChannelWidthMax - kChannelWidthMin);
			ppm_channel_counter++;
		}
		else
		{
			ppm_status = kPpmPacketLost;
			ppm_state_machine = kPpmIdeal;
			PpmfualtDetect();
		}

		if (ppm_channel_counter > kNumberOfChannel-2)
		{
			ppm_status = kPpmOK;
			ppm_state_machine = kPpmIdeal;
			ppm_number_of_packet ++ ;
			PpmHeartBeat();
		}
		break;
	default:
		break;
	}
	PpmHeartBeat();
}
/**
 * @fn void PpmTimeOut(void)
 * @brief This function should be called when the PPM signal was lost
 * 			So, the State machine will change to the Ideal state and
 * 			status flag will be set to the lost
 * @pre Nothing
 * @post Nothing
 */
void PpmTimeOut(void)
{
	ppm_state_machine = kPpmIdeal;
	ppm_status = kPpmPacketLost;
	PpmfualtDetect();
}
/**
 * @fn tyPpmStatus PpmStatusCheck(void)
 * @brief Return status of PPM signal
 *
 * @pre
 * @post
 * @return PPM Signal status
 */
tyPpmStatus PpmStatusCheck(void)
{
	return ppm_status;
}
/**
 * @fn uint32_t PpmGetPacketNumber(void)
 * @brief return number of received packet
 *
 * @pre
 * @post
 * @return number of received packet
 */
uint32_t PpmGetPacketNumber(void)
{
	return ppm_number_of_packet;
}
/**
 * @fn uint8_t PpmGetChannelPercent(const tyPpmChannel)
 * @brief Return value of channel
 * 			Warning: If channel_number is out of range just
 * 						is ignored because isn't wrote any code
 * 						for this state.
 * @pre
 * @post
 * @param channel_number
 * @return percentage of channel value
 */
uint8_t PpmGetChannelPercent(const tyPpmChannel channel_number)
{
	if(channel_number < kNumberOfChannel)
	{
		return ppm_channel_scaled[channel_number];
	}
	else
	{
		/* This code run when channel_number is out of range */
		return 0;
	}
}
/**
 * @fn uint8_t PpmGetChannelPercent(const tyPpmChannel)
 * @brief Return value of channel
 * 			Warning: If channel_number is out of range just
 * 						is ignored because isn't wrote any code
 * 						for this state.
 * @pre
 * @post
 * @param channel_number
 * @return channel raw value
 */
uint16_t PpmGetChannelRaw(const tyPpmChannel channel_number)
{
	if(channel_number < kNumberOfChannel)
	{
		return ppm_channel_raw[channel_number];
	}
	else
	{
		/* This code run when channel_number is out of range */
		return 0;
	}
}

/*************** END OF FUNCTIONS ***************************************************************************/
