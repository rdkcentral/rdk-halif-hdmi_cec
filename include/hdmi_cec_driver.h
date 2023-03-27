/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/



/**
 * HDMI CEC HAL provides a set of APIs to communicate CEC messages with other CEC devices 
 * connected with HDMI cable. The purpose of the HAL is to retrieve discovered logical
 * and physical address of the host device and to transmit and receive messages with the
 * remote device synchronously / asynchronously.
*/

/**
* @defgroup hdmicec hdmicec
* @{
* @defgroup ccec ccec
* @{
**/


#ifndef HDMI_CEC_DRIVER_H_
#define HDMI_CEC_DRIVER_H_
#ifdef __cplusplus
extern "C" {
#endif
	
#include <inttypes.h>

/**
 * Error Codes
*/
typedef enum
{
    HDMI_CEC_STATUS_SUCCESS = 0,		/// Input output operation is successful
    HDMI_CEC_STATUS_ERROR,			/// Operation general error
    HDMI_CEC_STATUS_TIMEOUT,			/// Timeout
    HDMI_CEC_STATUS_NOT_INITIALISED,		/// Module is not initialised
    HDMI_CEC_STATUS_INVALID_ARGUMENT,		/// Invalid argument is passed to module
    HDMI_CEC_STATUS_SENT_AND_ACKD,		/// Operation send and acknowledgement received	
    HDMI_CEC_STATUS_SENT_BUT_NOT_ACKD,		/// Operation send but acknowledgement not received
    HDMI_CEC_STATUS_SEND_FAILED,		/// Operation failed
    HDMI_CEC_STATUS_LOGICALADDRESS_UNAVAILABLE,	/// Logical address is not available
    HDMI_CEC_STATUS_MAX				/// Out of range - required to be the last item of the enum
}HdmiCec_Status_t;

typedef int32_t HdmiCec_Handle_t;		/// Module handle

/**
 * @addtogroup HDMI_CEC
 * @{
 */
/**
 * This function will be triggered when a complete CEC packet is received.
 * Only 1 complete CEC packet should be contained in the buffer.
 * @param [in] handle - The handle used by application to uniquely identify the HAL instance.
 * @param [in] callbackData - callback data for the receive callback
 * @param [in] buf - buffer passed in CEC receive callback
 * @param [in] len - length of buffer passed in receive CEC callback
 */

typedef void (*HdmiCecRxCallback_t)(HdmiCec_Handle_t handle, void *callbackData, unsigned char *buf, int len);

/**
 * CEC HAL should call this function to report the status of the latest transmit.
 * @param [in] handle - The handle used by application to uniquely identify the HAL instance.
 * @param [in] callbackData - callback data for the transmit callback
 * @param [in] result - async transmit result from the platform implementation.
 */

typedef void (*HdmiCecTxCallback_t)(HdmiCec_Handle_t handle, void *callbackData, int result);
/**
 * @brief initialize the CEC HAL.  
 * This function should be call once before the functions in this API can be used.
 *
 * @param [out] handle - The handle used by application to uniquely identify the HAL instance. 
 *
 * @return Error Code: If error code is returned, the open is failed.
 * Possible error codes are:
 * @retval HDMI_CEC_STATUS_SUCCESS HdmiCecOpen was successful.
 * @retval HDMI_CEC_STATUS_NOT_INITIALISED: Calling HdmiCecOpen again after a successful open in a process will result in this error.
 * @retval HDMI_CEC_STATUS_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_STATUS_ERROR: This error code indicates the underlying undefined platform error.
 * @warning  This API is Not thread safe.
 * @see HdmiCecClose()
 */
HdmiCec_Status_t HdmiCecOpen(HdmiCec_Handle_t *handle);

/**
 * @brief close an instance of CEC HAL.  
 * This function should uninitialize the CEC HAL.
 *
 * @param [in] handle - The handle returned from the HdmiCecOpen() function.
 *
 * @return Error Code:  If error code is returned, the open is failed.
 * Possible error codes are:
 * @retval HDMI_CEC_STATUS_SUCCESS HdmiCecClose was successful.
 * @retval HDMI_CEC_STATUS_NOT_INITIALISED: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_STATUS_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_STATUS_ERROR: This error code indicates the underlying undefined platform error.
 * This usually indicates the underlying unknown platform error.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecOpen()
 *
 */
HdmiCec_Status_t HdmiCec_Close(HdmiCec_Handle_t handle);

/**
 * @brief Add one Logical Addresses to be used by host device.
 *
 * This function can block until the intended logical address is secured by
 * the HAL module.
 *
 * In driver implementation, this API would trigger HAL sending a POLL
 * CEC packet to the CEC Bus,
 *
 * Packet::HeaderBlock::Initiator   =  Requested LogicalAddress.
 * Packet::HeaderBlock::Destination =  Requested LogicalAddress.
 * Packet::DataBlock   				=  Empty.
 *
 * The function shall return HDMI_CEC_STATUS_SUCCESS if the POLL message is sent
 * successfully and not ACK'd by any device on the bus. From this point on the
 * driver shall forward all received messages with destination being the acquired
 * logical address. Driver should ACK all POLL messages destined to this logical
 * address.
 *
 * The function shall return HDMI_CEC_STATUS_LOGICALADDRESS_UNAVAILABLE if the POLL
 * message is sent and ACK'd by a device on the bus.
 *
 * The function shall return relevant error code if the POLL message is not sent
 * successfully.
 *
 *
 * @param [in] handle - The handle returned from the HdmiCecOpen() function.
 * @param[in] logicalAddresses - The logical address to be acquired.
 *
 * @return Error Code: Possible error codes are:
 * @retval HDMI_CEC_STATUS_SUCCESS HdmiCecAddLogicalAddress was successful.
 * @retval HDMI_CEC_STATUS_NOT_INITIALISED: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_STATUS_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_STATUS_ERROR: This error code indicates the underlying undefined platform error.
 * This usually indicates the underlying unknown platform error.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @note This API is not used as SOC is responsible to perform logical address discovery.
 * @see HdmiCecRemoveLogicalAddress(), HdmiCecGetLogicalAddress()
 */
HdmiCec_Status_t HdmiCecAddLogicalAddress(HdmiCec_Handle_t handle, int logicalAddresses);

/**
 * @brief Clear the Logical Addresses claimed by host device.
 *
 * This function releases the previously acquired logical address.  Once
 * released, HAL should not ACK any POLL message destined to the
 * released address.
 *
 * @param [in] handle - The handle returned from the HdmiCecOpen() function.
 * @param [in] logicalAddresses - The logicalAddresses to be released.
 *
 * @return Error Code: If error code is returned, the remove is failed.
 * Possible error codes are:
 * @retval HDMI_CEC_STATUS_SUCCESS HdmiCecRemoveLogicalAddress was successful.
 * @retval HDMI_CEC_STATUS_NOT_INITIALISED: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_STATUS_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_STATUS_ERROR: This error code indicates the underlying undefined platform error.
 * This usually indicates the underlying unknown platform error.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @note This API is not used as SOC is responsible to perform logical address discovery.
 * @see HdmiCecAddLogicalAddress(), HdmiCecGetLogicalAddress()
 */
HdmiCec_Status_t HdmiCecRemoveLogicalAddress(HdmiCec_Handle_t handle, int logicalAddresses);

/**
 * @brief Get the Logical Address obtained by the CEC HAL.
 *
 * This function get the logical address for the specified device type.
 *
 * @param [in] handle - The handle returned from the HdmiCecOpen() function.
 * @param [in] devType - The device type (tuner, record, playback etc.).
 * @param [out] logicalAddress - The logical address acquired
 *
 * @return Error Code:  If error code is returned, the get is failed.
 * Possible error codes are:
 * @retval HDMI_CEC_STATUS_SUCCESS HdmiCecGetLogicalAddress was successful.
 * @retval HDMI_CEC_STATUS_NOT_INITIALISED: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_STATUS_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_STATUS_ERROR: This error code indicates the underlying undefined platform error.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecAddLogicalAddress(), HdmiCecRemoveLogicalAddress()
 */
HdmiCec_Status_t HdmiCecGetLogicalAddress(HdmiCec_Handle_t handle, int devType,  int *logicalAddress);

/**
 * @brief Get the Physical Address obtained by the HAL.
 *
 * This function get the Physical address for the specified device type.
 *
 * @param [in] handle - returned from the HdmiCecOpen() function.
 * @param [out] physicalAddress - physical address acquired
 *
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecGetLogicalAddress()
 */
void HdmiCecGetPhysicalAddress(HdmiCec_Handle_t handle, unsigned int *physicalAddress);


/**
 * @brief Sets CEC packet Receive callback.  
 *
 * This function sets a callback function to be invoked for each packet arrival.   
 * The packet contained in the buffer is expected to follow this format:
 *
 * (ref <HDMI Specification 1-4> Section <CEC 6.1>)
 * 
 * complete packet = header block + data block;
 * header block = destination logical address (4-bit) + source address (4-bit)
 * data   block = opcode block (8-bit) + operand block (N-bytes)
 *
 * @code
 * |------------------------------------------------
 * | header block  |          data blocks          |
 * |------------------------------------------------
 * |3|2|1|0|3|2|1|0|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|  
 * |------------------------------------------------
 * | Dest  |  src  |  opcode block | operand block |
 * |------------------------------------------------
 * @endcode
 *
 * when receiving, the returned buffer should not contain EOM and ACK bits.
 * 
 * When transmitting, it is driver's responsibility to insert EOM bit and ACK bit 
 * for each header or data block 
 *
 * When HdmiCecSetRxCallback is called, it replaces the previous set cbfunc and data
 * values.  Setting a value of (cbfunc=null) turns off the callback.
 *
 * This function should block if callback invocation is in progress.
 *
 * @param [in] handle - The handle returned from the HdmiCecOpen(() function.
 * @param[in] cbfunc - A callback function to be invoked when a complete packet is received. This call back function is re-entrant
 * @param [in] data - the data used when invoking callback function. 
 *
 * @return Error Code:  If error code is returned, the set is failed.
 * Possible error codes are:
 * @retval HDMI_CEC_STATUS_SUCCESS HdmiCecGetLogicalAddress was successful.
 * @retval HDMI_CEC_STATUS_NOT_INITIALISED: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_STATUS_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_STATUS_ERROR: This error code indicates the underlying undefined platform error.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecTx(), HdmiCecTxAsync(), HdmiCecSetTxCallback()
 */
HdmiCec_Status_t HdmiCecSetRxCallback(HdmiCec_Handle_t handle, HdmiCecRxCallback_t cbfunc, void *data);

/**
 * @brief Sets CEC packet Transmit callback.  
 *
 * This function sets a callback function to be invoked once the async transmit
 * result is available. This is only necessary if application choose to transmit
 * the packet asynchronously.
 *
 * This function should block if callback invocation is in progress.
 *
 * @param [in] handle  - The handle returned from the HdmiCecOpen(() function.
 * @param [in] cbfunc - Function pointer to be invoked when a complete packet is received.
 * @param[in] data - It is used when invoking callback function.
 *
 * @return Error Code:  If error code is returned, the set is failed.
 * Possible error codes are:
 * @retval HDMI_CEC_STATUS_SUCCESS HdmiCecGetLogicalAddress was successful.
 * @retval HDMI_CEC_STATUS_NOT_INITIALISED: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_STATUS_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_STATUS_ERROR: This error code indicates the underlying undefined platform error.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecTx(), HdmiCecTxAsync()
 */
HdmiCec_Status_t HdmiCecSetTxCallback(HdmiCec_Handle_t handle, HdmiCecTxCallback_t cbfunc, void *data);

/**
 * @brief Writes CEC packet onto bus.  
 *
 * This function writes a complete CEC packet onto the CEC bus and wait for ACK.
 * application should check for result when return value of the function is 0;
 *
 * The bytes in buf that is to be transmitted should follow the buffer
 * byte format required for receiving buffer. (See detailed description from 
 * HdmiCecSetRxCallback)
 *
 * @param [in] handle - The handle returned from the HdmiCecOpen(() function.
 * @param [in] buf - The buffer contains a complete CEC packet.
 * @param [in] len - Number of bytes in the packet.
 * @param [out] result - Output of the send. Possible results are SENT_AND_ACKD,
 *                    SENT_BUT_NOT_ACKD (e.g. no follower at the destionation),
 *                    SENT_FAILED (e.g. collision).
 *
 * @return Error Code:  If error code is returned, the transmit did not happen.
 * Possible error codes are:
 * @retval HDMI_CEC_STATUS_SUCCESS HdmiCecGetLogicalAddress was successful.
 * @retval HDMI_CEC_STATUS_NOT_INITIALISED: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_STATUS_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_STATUS_SENT_AND_ACKD : Cec message is send and acknowledged.
 * @retval HDMI_CEC_STATUS_SENT_BUT_NOT_ACKD : Hdmicec message send but not acknowledged by the receiver.
 * @retval HDMI_CEC_STATUS_SEND_FAILED: Hdmicec io send message failed.
 * @retval HDMI_CEC_STATUS_LOGICALADDRESS_UNAVAILABLE Hdmi Cec receiver logical address is not available.
 * @retval HDMI_CEC_STATUS_ERROR: This error code indicates the underlying undefined platform error.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecTxAsync(), HdmiCecSetRxCallback()
 */
HdmiCec_Status_t HdmiCecTx(HdmiCec_Handle_t handle, const unsigned char *buf, int len, int *result);

/**
 * @brief Writes CEC packet onto bus asynchronously.
 *
 * This function writes a complete CEC packet onto the CEC bus but does not wait 
 * for ACK. The result will be reported via HdmiCecRxCallback_t if return value
 * of this function is 0.
 *
 * @param [in] handle - The handle returned from the HdmiCecOpen(() function.
 * @param [in] buf - Buffer contains a complete CEC packet.
 * @param [in] len - Number of bytes in the packet.
 *
 * @return Error Code:  If error code is returned, the transmit did not happen.
 * Possible error codes are:
 * @retval HDMI_CEC_STATUS_SUCCESS HdmiCecGetLogicalAddress was successful.
 * @retval HDMI_CEC_STATUS_NOT_INITIALISED: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_STATUS_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_STATUS_SENT_AND_ACKD : Cec message is send and acknowledged.
 * @retval HDMI_CEC_STATUS_SENT_BUT_NOT_ACKD : Hdmicec message send but not acknowledged by the receiver.
 * @retval HDMI_CEC_STATUS_SEND_FAILED: Hdmicec io send message failed.
 * @retval HDMI_CEC_STATUS_LOGICALADDRESS_UNAVAILABLE Hdmi Cec receiver logical address is not available.
 * @retval HDMI_CEC_STATUS_ERROR: This error code indicates the underlying undefined platform error.
 * @warning  This API is Not thread safe.
 * @see HdmiCecTx(), HdmiCecSetRxCallback()
 */
HdmiCec_Status_t HdmiCecTxAsync(HdmiCec_Handle_t handle, const unsigned char *buf, int len);
/** @} */
#ifdef __cplusplus
}
#endif
#endif


/** @} */
/** @} */

