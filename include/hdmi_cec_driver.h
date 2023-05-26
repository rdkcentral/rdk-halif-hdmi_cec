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
 * HDMI CEC HAL is to retrieve discovered logical and physical address of host device 
 * and to transmit and receive messages to and from the
 * connected with HDMI cable. The purpose of the HAL is to retrieve discovered logical
 * and physical address of the host device and to transmit and receive messages with the
 * remote device synchronously / asynchronously.
 */

/**
 * @defgroup hdmicec hdmicec
 * @{
 * @defgroup ccec ccec
 * @{
 */


#ifndef HDMI_CEC_DRIVER_H_
#define HDMI_CEC_DRIVER_H_
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Error Codes
 */
enum HDMI_CEC_IO_ERROR
{
    HDMI_CEC_IO_SUCCESS = 0,                ///< Input output operation is successful
    HDMI_CEC_IO_SENT_AND_ACKD = 1,          ///< Send and acknowledgement received
    HDMI_CEC_IO_SENT_BUT_NOT_ACKD,          ///< Sent but acknowledgement not received
    HDMI_CEC_IO_SENT_FAILED,                ///< Operation failed
    HDMI_CEC_IO_INVALID_STATE,              ///< Module is not intialised
    HDMI_CEC_IO_INVALID_ARGUMENT,           ///< Invalid argument is passed to the module
    HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE, ///< Logical address is not available
    HDMI_CEC_IO_GENERAL_ERROR,              ///< Operation general error
    HDMI_CEC_IO_MAX                         ///< Out of range - required to be the 
                                            ///< last item of the enum
};

/**
 * @addtogroup HDMI_CEC
 * @{
 */

/**
 * @brief This function will be triggered when a complete CEC packet is received.
 *
 * Upon each callback, only 1 complete packet should be contained in the buffer.
 *
 * @param[in] handle       - The handle used by application to uniquely identify the HAL instance
 * @param[in] callbackData - Callback data for the receive callback
 * @param[in] buf          - Buffer passed in receive callback
 * @param[in] len          - Length of buffer passed in receive callback
 */
typedef void (*HdmiCecRxCallback_t)(int handle, void *callbackData, unsigned char *buf, int len);

/**
 * @brief This function will be triggered to report the status of the latest transmit message
 *
 * @param[in] handle       - The handle used by application to uniquely identify the HAL instance
 * @param[in] callbackData - Callback data for the transmit callback
 * @param[in] result       - Async transmit result from the platform implementation
 */
typedef void (*HdmiCecTxCallback_t)(int handle, void *callbackData, int result);

/**
 * @brief Initializes the CEC HAL.
 *
 * This function is required to be called before the other APIs in this module.@n
 * Subsequent calls to this API will return HDMI_CEC_IO_SUCCESS.
 *
 * @param [out] handle                  - The handle used by application to uniquely 
 *                                        identify the HAL instance
 *
 * @return HDMI_CEC_IO_ERROR            - Status
 * @retval HDMI_CEC_IO_SUCCESS          - Success
 * @retval HDMI_CEC_IO_INVALID_STATE    - Function is already open. 
 *                                        This error code will deprecated in the next phase.
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT - Parameter passed to this function is invalid
 * @retval HDMI_CEC_IO_GENERAL_ERROR    - Underlying undefined platform error
 *
 * @warning This API is NOT thread safe.
 *
 * @see HdmiCecClose()
 *
 * TODO: Change the return type to HDMI_CEC_IO_ERROR instead of int
 */
int HdmiCecOpen(int *handle);

/**
 * @brief Closes an instance of CEC HAL.
 *
 * This function will uninitialize the module.@n
 * Close will clear up registered logical addresses.@n
 * Subsequent calls to this API will return HDMI_CEC_IO_SUCCESS.
 *
 * @param[in] handle - The handle returned from the HdmiCecOpen() function
 *
 * @return HDMI_CEC_IO_ERROR            - Status
 * @retval HDMI_CEC_IO_SUCCESS          - Success
 * @retval HDMI_CEC_IO_INVALID_STATE    - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT - Parameter passed to this function is invalid
 * @retval HDMI_CEC_IO_GENERAL_ERROR    - Underlying undefined platform error
 *
 * @pre HdmiCecOpen() must be called before calling this API.
 * @warning This API is NOT thread safe.
 *
 * @see HdmiCecOpen()
 *
 */
int HdmiCecClose(int handle);

/**
 * @brief Adds one Logical Addresses to be used by host device.
 *
 * This function will block until the intended logical address is secured by the module.@n
 * HAL will forward all received messages with destination being the acquired logical address.@n
 * HAL should ACK all POLL messages destined to this logical address.@n
 * This API is only applicable for sink devices. Supported logical address value must be 0x0.@n
 * Invoking this API in source device must return HDMI_CEC_IO_INVALID_ARGUMENT@n@n
 *
 * In HAL implementation, this API would trigger HAL sending a POLL CEC packet to the CEC Bus:@n
 * Packet::HeaderBlock::Initiator   =  Requested LogicalAddress@n
 * Packet::HeaderBlock::Destination =  Requested LogicalAddress@n
 * Packet::DataBlock   			   =  Empty
 *
 * @param[in] handle                              - The handle returned from the HdmiCecOpen() 
 *                                                    function
 * @param[in] logicalAddresses                    - The logical address to be acquired
 *
 * @return HDMI_CEC_IO_ERROR                      - Status
 * @retval HDMI_CEC_IO_SUCCESS                    - POLL message is sent successfully and not 
 *                                                    ACK'd by any device on the bus
 * @retval HDMI_CEC_IO_INVALID_STATE              - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT           - Parameter passed to this function is invalid
 * @retval HDMI_CEC_IO_GENERAL_ERROR              - Underlying undefined platform error
 * @retval HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE - POLL message is sent and 
 *                                                     ACK'd by a device on the bus
 * @retval HDMI_CEC_IO_SENT_FAILED                - POLL message send failed.
 *
 * @pre HdmiCecOpen() must be called before calling this API.
 * @warning This API is NOT thread safe.
 * @note This API is not required if the SOC is performing the logical address discovery.
 *
 * @see HdmiCecRemoveLogicalAddress(), HdmiCecGetLogicalAddress()
 * 
 */
int HdmiCecAddLogicalAddress(int handle, int logicalAddresses);

/**
 * @brief Clears the Logical Addresses claimed by the host device.
 *
 * This function releases the previously acquired logical address.@n
 * Once released, the module must not ACK any POLL message destined to the
 * released address.@n
 * Subsequent calls to this API will return HDMI_CEC_IO_SUCCESS.
 * This API is only applicable for sink devices. Invoking this API in source 
 *  device must return HDMI_CEC_IO_INVALID_ARGUMENT@n@n
 * 
 * #TODO if we try to remove it again. Should throw an error. It should return invalid argument. 
 *       It will be done in next phase.
 *
 * @param[in] handle                   - The handle returned from the HdmiCecOpen() function
 * @param[in] logicalAddresses         - The logicalAddresses to be released
 *
 * @return HDMI_CEC_IO_ERROR            - Status
 * @retval HDMI_CEC_IO_SUCCESS          - Success
 * @retval HDMI_CEC_IO_INVALID_STATE    - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT - Parameter passed to this function is invalid
 * @retval HDMI_CEC_IO_GENERAL_ERROR    - Underlying undefined platform error
 *
 * @pre HdmiCecOpen() must be called before calling this API.
 * @warning This API is NOT thread safe.
 * @note This API is not required if the SOC is performing the logical address discovery.
 * @see HdmiCecAddLogicalAddress(), HdmiCecGetLogicalAddress()
 */
int HdmiCecRemoveLogicalAddress(int handle, int logicalAddresses);

/**
 * @brief Gets the Logical Address obtained by the module.
 *
 * This function gets the logical address for the specified device type. @n
 * For sink devices, if logical address is not added or removed, 
 *    the logical address returned will be 0x0F.
 * For source devices, logical address returned must be in between 0x00 and 0x0F, 
 *     excluding both the values.
 *
 * @param[in] handle                    - The handle returned from the HdmiCecOpen() function
 * @param[in] devType                   - The device type (tuner, record, playback etc.)
 * @param[out] logicalAddress           - The logical address acquired
 *
 * @return HDMI_CEC_IO_ERROR            - Status
 * @retval HDMI_CEC_IO_SUCCESS          - Success
 * @retval HDMI_CEC_IO_INVALID_STATE    - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT - Parameter passed to this function is invalid
 * @retval HDMI_CEC_IO_GENERAL_ERROR    - Underlying undefined platform error
 *
 * @pre HdmiCecOpen() must be called before calling this API.
 * @warning This API is NOT thread safe.
 * @note This API is not required if the SOC is performing the logical address discovery.
 * @see HdmiCecAddLogicalAddress(), HdmiCecRemoveLogicalAddress()
 * TODO: Unused variable devType. Need to remove.
 */
int HdmiCecGetLogicalAddress(int handle, int devType,  int *logicalAddress);

/**
 * @brief Gets the Physical Address obtained by the module.
 *
 * This function gets the Physical address for the specified device type.
 *
 * @param[in] handle            - Returned from the HdmiCecOpen() function
 * @param[out] physicalAddress  - Physical address acquired
 *
 * @pre HdmiCecOpen() must be called before calling this API.
 * @warning This API is NOT thread safe.
 * @see HdmiCecGetLogicalAddress()
 *
 * TODO: Need to return HDMI_CEC_IO_ERROR for HdmiCecOpen() failure
 */
void HdmiCecGetPhysicalAddress(int handle, unsigned int *physicalAddress);


/**
 * @brief Sets CEC packet receive callback.
 *
 * This function sets the callback function to be invoked for each packet arrival@n
 * The packet contained in the buffer will follow this format 
 *     (ref <HDMI Specification 1-4> Section <CEC 6.1>) :
 * 
 * complete packet  = header block + data block@n
 * header block     = destination logical address (4-bit) + source address (4-bit)@n
 * data block       = opcode block (8-bit) + operand block (N-bytes)
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
 * When receiving, the returned buffer should not contain EOM and ACK bits.@n
 * 
 * When transmitting, it is HAL's responsibility to insert EOM bit and ACK bit 
 * for each header or data block.
 *
 * When HdmiCecSetRxCallback() is called, it replaces the previous set cbfunc and data
 * values. Setting a value of (cbfunc=null) disables the callback.
 *
 * This function will block if callback invocation is in progress.
 *
 * @param[in] handle                    - The handle returned from the HdmiCecOpen(() function
 * @param[in] cbfunc                    - Function pointer to be invoked 
 *                                          when a complete packet is received
 * @param[in] data                      - Callback data
 *
 * @return HDMI_CEC_IO_ERROR            - Status
 * @retval HDMI_CEC_IO_SUCCESS          - Success
 * @retval HDMI_CEC_IO_INVALID_STATE    - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT - Parameter passed to this function is invalid
 *
 * @pre HdmiCecOpen() must be called before calling this API.
 * @warning This API is NOT thread safe.
 * @see HdmiCecTx(), HdmiCecTxAsync(), HdmiCecSetTxCallback()
 */
int HdmiCecSetRxCallback(int handle, HdmiCecRxCallback_t cbfunc, void *data);

/**
 * @brief Sets CEC packet transmit callback.
 *
 * This function sets a callback which will be invoked once the async transmit
 * result is available. This is only necessary if the caller chooses to transmit
 * the packet asynchronously.
 *
 * This function will block if callback invocation is in progress.
 *
 * @param[in] handle                    - The handle returned from the HdmiCecOpen() function.
 * @param[in] cbfunc                    - Function pointer to be invoked 
 *                                          when a complete packet is transmitted
 * @param[in] data                      - Callback data
 *
 * @return HDMI_CEC_IO_ERROR            - Status
 * @retval HDMI_CEC_IO_SUCCESS          - Success
 * @retval HDMI_CEC_IO_INVALID_STATE    - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT - Parameter passed to this function is invalid
 *
 * @pre HdmiCecOpen() must be called before calling this API.
 * @warning This API is NOT thread safe.
 * @see HdmiCecTx(), HdmiCecTxAsync()
 */
int HdmiCecSetTxCallback(int handle, HdmiCecTxCallback_t cbfunc, void *data);

/**
 * @brief Synchronous transmit call.
 *
 * This function writes a complete CEC packet onto the bus and waits for ACK.
 *
 * The packet contained in the buffer will follow the format detailed in HdmiCecSetRxCallback_t().
 * (ref <HDMI Specification 1-4> Section <CEC 6.1>)
 *
 *
 * @param[in] handle                              - The handle returned from the 
 *                                                    HdmiCecOpen() function.
 * @param[in] buf                                 - The buffer contains a complete 
 *                                                    CEC packet to send.
 * @param[in] len                                 - Number of bytes in the packet.
 * @param[out] result                             - send status buffer. Possible results are 
 *                    SENT_AND_ACKD,
 *                    SENT_BUT_NOT_ACKD (e.g. no follower at the destination),
 *                    SENT_FAILED (e.g. collision).
 *
 * @return HDMI_CEC_IO_ERROR                      - Status
 * @retval HDMI_CEC_IO_SUCCESS                    - Success
 * @retval HDMI_CEC_IO_INVALID_STATE              - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT           - Parameter passed to this function is invalid
 * @retval HDMI_CEC_IO_GENERAL_ERROR              - Underlying undefined platform error
 * @retval HDMI_CEC_IO_SENT_AND_ACKD              - Cec message is send and acknowledged.
 * @retval HDMI_CEC_IO_SENT_BUT_NOT_ACKD          - Message send but not acknowledged 
 *                                                    by the receiver.
 * @retval HDMI_CEC_IO_SENT_FAILED                - send message failed.
 *
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecTxAsync(), HdmiCecSetRxCallback()
 * TODO: Need to check the why result argument is required.
 */
int HdmiCecTx(int handle, const unsigned char *buf, int len, int *result);

/**
 * @brief Writes CEC packet onto bus asynchronously.
 *
 * This function writes a complete CEC packet onto the bus but does not wait
 * for ACK. The result will be reported via HdmiCecRxCallback_t()
 *
 *
 * @param[in] handle                              - The handle returned from the 
 *                                                    HdmiCecOpen() function.
 * @param[in] buf                                 - The buffer contains a complete 
 *                                                    CEC packet to send.
 * @param[in] len                                 - Number of bytes in the packet.
 *
 * @return HDMI_CEC_IO_ERROR                      - Status
 * @retval HDMI_CEC_IO_SUCCESS                    - Success
 * @retval HDMI_CEC_IO_INVALID_STATE              - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT           - Parameter passed to this function is invalid
 * @retval HDMI_CEC_IO_GENERAL_ERROR              - Underlying undefined platform error
 *
 * @warning  This API is Not thread safe.
 * @see HdmiCecTx(), HdmiCecSetRxCallback()
 */
int HdmiCecTxAsync(int handle, const unsigned char *buf, int len);
/** @} */
#ifdef __cplusplus
}
#endif
#endif


/** @} */
/** @} */
