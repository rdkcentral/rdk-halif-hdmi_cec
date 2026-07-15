/*
 * If not stated otherwise in this file or this component's LICENSE file the
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
 * @addtogroup HPK Hardware Porting Kit
 * @{
 * @par The Hardware Porting Kit
 * HPK is the next evolution of the well-defined Hardware Abstraction Layer
 * (HAL), but augmented with more comprehensive documentation and test suites
 * that OEM or SOC vendors can use to self-certify their ports before taking
 * them to RDKM for validation or to an operator for final integration and
 * deployment. The Hardware Porting Kit effectively enables an OEM and/or SOC
 * vendor to self-certify their own Video Accelerator devices, with minimal RDKM
 * assistance.
 *
 */


/**
 * @defgroup HDMI_CEC HDMI CEC Module
 * @{
 *
 */

/**
 * @defgroup HDMI_CEC_HAL HDMI CEC HAL
 * @{
 * @par Application API Specification
 * HDMI CEC HAL provides an API to send and receive CEC messages with other
 * CEC devices. The HDMI CEC HAL also provides API functions to retrieve the
 * logical address and physical address of the host device.
 * CEC messages are transmitted on the bus with a synchronous function call
 * that returns the acknowledgment of the message when a receiver on the bus
 * ACKs it.
 *
 */

/**
 * @defgroup HDMI_CEC_DRIVER_H HDMI CEC Header
 * @{
 */

/**
 * @file hdmi_cec_driver.h
 */

#ifndef __HDMI_CEC_DRIVER_H__
#define __HDMI_CEC_DRIVER_H__
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Error Codes
 * 
 * 
 */
typedef enum HDMI_CEC_IO_ERROR
{
    HDMI_CEC_IO_SUCCESS = 0,                ///< Input output operation is successful
    HDMI_CEC_IO_SENT_AND_ACKD = 1,          ///< Send and acknowledgement received
    HDMI_CEC_IO_SENT_BUT_NOT_ACKD,          ///< Sent but acknowledgement not received
    HDMI_CEC_IO_SENT_FAILED,                ///< Operation failed
    HDMI_CEC_IO_NOT_OPENED,                 ///< Module is not initialised
    HDMI_CEC_IO_INVALID_ARGUMENT,           ///< Invalid argument is passed to the module
    HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE, ///< Logical address is not available
    HDMI_CEC_IO_GENERAL_ERROR,              ///< Operation general error.
    HDMI_CEC_IO_ALREADY_OPEN,               ///< Module is already initialised
    HDMI_CEC_IO_ALREADY_REMOVED,            ///< Removal operation is already executed
    HDMI_CEC_IO_INVALID_OUTPUT,             ///< Output arguments fall outside the valid range
    HDMI_CEC_IO_INVALID_HANDLE,             ///< An invalid handle argument has been passed
    HDMI_CEC_IO_OPERATION_NOT_SUPPORTED,    ///< Operation not supported
    HDMI_CEC_IO_NOT_ADDED,                  ///< Not added
    HDMI_CEC_IO_MAX                         ///< Out of range - required to be the 
                                            ///< last item of the enum
} HDMI_CEC_STATUS;

/**
 * @brief Callback function triggered when a complete CEC message is received
 *
 * Upon each callback, only one complete CEC message shall be contained in the buffer.
 * The message data shall be copied by the receiver during the callback
 * execution, as the buffer pointer shall no longer be valid after the callback
 * returns.
 *
 * @param[in] handle       - The handle used by application to uniquely identify the HAL instance. Non zero value
 * @param[in] callbackData - Callback data for the receive callback
 * @param[in] buf          - Buffer passed in receive callback
 * @param[in] len          - Length of buffer passed in receive callback
 */
typedef void (*HdmiCecRxCallback_t)(int handle, void *callbackData, unsigned char *buf, int len);

/**
 * @note This API is deprecated.
 *
 * @brief Callback function triggered to report the status of the latest transmit message
 *
 * @param[in] handle       - The handle used by application to uniquely identify
 *                           the HAL instance. The handle shall be valid (non zero).
 * @param[in] callbackData - Callback data provided during callback registration
 * @param[in] result       - Async transmit result from the platform implementation
 */
typedef void (*HdmiCecTxCallback_t)(int handle, void *callbackData, int result);

/**
 * @brief Initializes the HDMI CEC HAL
 *
 * This function is required to be called before the other APIs in this module.@n
 * Subsequent calls to this API will return HDMI_CEC_IO_SUCCESS if the HAL is
 * already initialized. Support for HDMI_CEC_IO_ALREADY_OPEN is deprecated and
 * may be removed in a future release.
 *
 * HDMI Source devices:
 * - Logical address discovery shall take place during HdmiCecOpen().
 * - The allocated logical address can be obtained using HdmiCecGetLogicalAddress().
 * - If logical address discovery fails,
 *   HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE shall be returned and the HDMI-CEC
 *   HAL instance shall release all allocated resources before returning.
 *
 * HDMI Sink devices:
 * - HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE shall not be returned.
 * - Logical address discovery shall not be performed during HdmiCecOpen().
 * - Logical address allocation and management shall be performed by the caller.
 *
 * @param [out] handle                            - The handle used by application to uniquely
 *                                                  identify the HAL instance and valid only
 *                                                  when HDMI_CEC_IO_SUCCESS is returned.
 * @return HDMI_CEC_STATUS                        - Status
 * @retval HDMI_CEC_IO_SUCCESS                    - Success
 * @retval HDMI_CEC_IO_ALREADY_OPEN               - This error code will be Deprecated in next phase.
                                                    Module is already open.
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT           - Invalid argument passed to this function.
 *                                                  This includes a NULL handle parameter.
 * @retval HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE - Logical address is not available for source devices.
 * @retval HDMI_CEC_IO_GENERAL_ERROR              - Unexpected hardware or platform failure
 * 
 * 
 * @post HdmiCecClose() must be called to release resources.
 * @warning This API is NOT thread safe.
 *
 * @see HdmiCecClose()
 *
 */
HDMI_CEC_STATUS HdmiCecOpen(int *handle);

/**
 * @brief Closes an instance of HDMI CEC HAL
 *
 * This function will uninitialise the module.@n
 * Close will clear up registered logical addresses.@n
 * Subsequent calls to this API with the same handle will return
 * HDMI_CEC_IO_NOT_OPENED.
 *
 * @param[in] handle                    - A Valid handle returned from the HdmiCecOpen().
 *                                        Should be non zero value
 *
 * @return HDMI_CEC_STATUS              - Status
 * @retval HDMI_CEC_IO_SUCCESS          - Success
 * @retval HDMI_CEC_IO_NOT_OPENED       - Module is not initialised or the handle
 *                                        has already been closed
 * @retval HDMI_CEC_IO_INVALID_HANDLE   - An invalid handle argument has been passed
 * @retval HDMI_CEC_IO_GENERAL_ERROR    - Unexpected platform-level failure
 *
 * @pre HdmiCecOpen() must be called before calling this API.
 * @warning This API is NOT thread safe.
 *
 * @see HdmiCecOpen()
 *
 */
HDMI_CEC_STATUS HdmiCecClose(int handle);

/**
 * @brief Sets the logical address assignment for a HDMI sink device.
 *
 * HDMI Sink devices:
 * - This API is applicable.
 * - Logical address discovery shall be performed by the caller.
 * - The logical address provided through this API shall be in the range
 *   0x0 to 0xF.
 *
 * HDMI Source devices:
 * - This API is not applicable.
 * - Invoking this API shall return HDMI_CEC_IO_OPERATION_NOT_SUPPORTED.
 *
 * @param[in] handle                              - A Valid handle returned from the HdmiCecOpen().
 *                                                  must be a non zero value
 * @param[in] logicalAddresses                    - Logical address to be acquired.
 *                                                  Valid range is 0x0 to 0xF.
 * @return HDMI_CEC_STATUS                        - Status
 * @retval HDMI_CEC_IO_SUCCESS                    - POLL message is sent successfully and
 *                                                  no device acknowledges the logical address
 *                                                  on the CEC bus. The logical address is
 *                                                  assigned successfully.
 * @retval HDMI_CEC_IO_NOT_OPENED                 - Module is not initialized
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT           - The logicalAddress argument is invalid.
 *                                                  Valid range is 0x0 to 0xF.
 * @retval HDMI_CEC_IO_INVALID_HANDLE             - An invalid handle has been provided.
 * @retval HDMI_CEC_IO_GENERAL                    - Unable to verify logical address
 *                                                  availability due to a bus or connection error.
 * @retval HDMI_CEC_IO_OPERATION_NOT_SUPPORTED    - The requested operation is not supported
 *                                                  for the current device type (HDMI Source).
 *
 * @pre HdmiCecOpen() must be called successfully before invoking this API.
 *
 * @post On successful return, the logical address is assigned and can be retrieved using
 *       HdmiCecGetLogicalAddress() and removed using HdmiCecRemoveLogicalAddress().
 *
 * @warning This API is not thread safe. The caller shall ensure proper synchronization
 *          when invoking this API concurrently with other HDMI CEC APIs.
 *
 * @see HdmiCecRemoveLogicalAddress(), HdmiCecGetLogicalAddress()
 */ 
HDMI_CEC_STATUS HdmiCecAddLogicalAddress(int handle, int logicalAddresses);

/**
 * @brief Clears the Logical Addresses claimed by the host device
 *
 * HDMI Sink devices:
 * - This API is applicable and releases the previously assigned logical address.@n
 * Once released:@n
 * 1. This API shall set the logical address to the default value (0xF).@n
 * 2. The module shall not ACK any POLL message destined for the released address.
 *
 * HDMI Source devices:
 * - This API is not applicable.
 * - Invoking this API shall return HDMI_CEC_IO_OPERATION_NOT_SUPPORTED.
 * 
 *
 * @param[in] handle                              - A Valid handle returned from the HdmiCecOpen().
 *                                                  must be a non zero value
 * @param[in] logicalAddresses                    - Logical address to be acquired.
 *                                                  Valid range is 0x0 to 0xF.
 *
 * @return HDMI_CEC_STATUS                        - Status
 * @retval HDMI_CEC_IO_SUCCESS                    - Success
 * @retval HDMI_CEC_IO_NOT_OPENED                 - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT           - The logicalAddress argument is invalid.
 *                                                  Valid range is 0x0 to 0xF.
 * @retval HDMI_CEC_IO_NOT_ADDED                  - Logical address was never added before [or] was previously removed successfully
 * @retval HDMI_CEC_IO_INVALID_HANDLE             - An invalid handle has been provided.
 * @retval HDMI_CEC_IO_OPERATION_NOT_SUPPORTED    - The requested operation is not supported
 *                                                  for the current device type (HDMI Source).
 *
 * @pre HdmiCecOpen() must be called successfully before calling this API.
 * @pre HdmiCecAddLogicalAddress() must have been called successfully for the
 *      logical address before invoking this API.
 * @post On successful return, the logical address is released, the module is
      reset to the default logical address (0xF), and the module shall not
      acknowledge POLL messages for the released logical address.
 * @warning This API is NOT thread safe.
 * @see HdmiCecAddLogicalAddress(), HdmiCecGetLogicalAddress()
 * 
 */
HDMI_CEC_STATUS HdmiCecRemoveLogicalAddress(int handle, int logicalAddresses);

/**
 * @brief Gets the Logical Address obtained by the module
*
 * This function retrieves the logical address for the specified device type.@n
 *
 * For sink devices:
 * - If no logical address has been assigned or the assigned logical address
 *   has been removed, this API shall return the default value 0x0F.
 *
 * For source devices:
 * - The returned logical address shall correspond to the device type as defined
 *   in the HDMI Specification.
 *
 * @note This note applies to HDMI source devices only.
 *       The HAL internally re-discovers and updates the logical address when an
 *       HDMI hotplug-in event is detected (via an internal platform callback).
 *       The caller does not need to re-invoke HdmiCecOpen() to trigger this;
 *       a subsequent call to HdmiCecGetLogicalAddress() after the hotplug-in
 *       event has been processed shall return the newly assigned address.
 *
 * @note After an HDMI hotplug-out event, the HAL shall reset the logical address
 *       to the default value 0x0F. Calling this API while the HDMI cable is
 *       disconnected shall return 0x0F.
 *
 * @param[in] handle                    - A Valid handle returned from the HdmiCecOpen().
 *                                        must be a non zero value
 * @param[out] logicalAddress           - The logical address acquired
 *
 * @return HDMI_CEC_STATUS              - Status
 * @retval HDMI_CEC_IO_SUCCESS          - Success
 * @retval HDMI_CEC_IO_NOT_OPENED       - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT - The logicalAddress argument is invalid
 *                                        or is a NULL pointer.
 * @retval HDMI_CEC_IO_INVALID_HANDLE   - An invalid handle argument has been passed
 *
 * @pre HdmiCecOpen() must be called successfully before calling this API.
 *
 * @post On successful return, the logicalAddress output parameter contains the
 *       logical address assigned to the module.
 * @pre HdmiCecOpen() must be called before calling this API.
 * @pre HdmiCecAddLogicalAddress() must have been called successfully and the
 *      specified logical address must be assigned before invoking this API.
 * @warning This API is NOT thread safe.
 * @note This API is not required if the SOC is performing the logical address discovery.
 * @see HdmiCecAddLogicalAddress(), HdmiCecRemoveLogicalAddress()
 *
 * 
 */
HDMI_CEC_STATUS HdmiCecGetLogicalAddress(int handle, int *logicalAddress);


/**
 * @brief Gets the Physical Address obtained by the module
 *
 * This function gets the Physical address for the specified device type.
 *
 * HDMI Sink devices:
 * - A sink device directly connected at the root shall have a fixed physical
 *   address of 0.0.0.0.
 * - The physical address shall not change due to HDMI hotplug events.
 *
 * HDMI Source devices:
 * - The physical address shall be obtained from the connected HDMI topology.
 * - After an HDMI hotplug-in event, the HAL shall internally re-discover the
 *   physical address.
 * - Until physical address discovery completes successfully, this API shall
 *   return HDMI_CEC_IO_INVALID_OUTPUT.
 * - After an HDMI hotplug-out event, the previously obtained physical address
 *   shall be considered invalid.
 *   This API shall return HDMI_CEC_IO_INVALID_OUTPUT until a new HDMI connection
 *   is established and physical address discovery completes successfully.
 *
 * @param[in] handle                    - A Valid handle returned from the HdmiCecOpen().
 *                                         must be a non zero value
 * @param[out] physicalAddress          - Pointer to store the physical address
 *                                       obtained by the module.
 *                                       The valid physical address range is
 *                                       0.0.0.0 to F.F.F.E.
 *                                       A sink device connected directly at the
 *                                       root shall have a physical address
 *                                       of 0.0.0.0.
 *
 * @return HDMI_CEC_STATUS              - Status
 * @retval HDMI_CEC_IO_SUCCESS          - Success
 * @retval HDMI_CEC_IO_NOT_OPENED       - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT - The physicalAddress argument is invalid
 *                                        or is a NULL pointer.
 * @retval HDMI_CEC_IO_INVALID_HANDLE   - An invalid handle argument has been passed
 * @retval HDMI_CEC_IO_INVALID_OUTPUT   - Physical address cannot be retrieved. This includes:
 *                                        the HDMI cable is not connected (source devices),
 *                                        physical address discovery is still in progress
 *                                        after a hotplug-in event, or the retrieved address
 *                                        is outside the valid range (0.0.0.0 to F.F.F.E).
 *
 * @pre HdmiCecOpen() must be called successfully before calling this API.
 *
 * @post On successful return, the physicalAddress output parameter contains the
 *       physical address obtained by the module.
 *
 * @warning This API is NOT thread safe.
 *
 * @see HdmiCecGetLogicalAddress()
 * 
 */
HDMI_CEC_STATUS HdmiCecGetPhysicalAddress(int handle, unsigned int *physicalAddress);


/**
 * @brief Sets CEC message receive callback
 *
 * This function sets the callback function to be invoked for each message arrival@n
 * The message contained in the buffer will follow this format
 *     (ref <HDMI Specification 1-4> Section <CEC 6.1>) :
 * 
 * complete message  = header block + data block@n
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
 * When receiving, the returned buffer should not contain EOM and ACK bits. HAL internal logic.@n
 * HAL implementation should remove the EOM and ACK bits in the returned buffer
 * 
 * When transmitting, it is HAL's responsibility to insert EOM bit and ACK bit 
 * for each header or data block.
 *
 * When HdmiCecSetRxCallback() is called, it replaces the previous set cbfunc and data
 * values. Setting a value of (cbfunc=null) disables the callback.
 *
 * This function will block if callback invocation is in progress.
 *
 * @param[in] handle                    - The handle returned from the HdmiCecOpen(() function. Non zero value
 * @param[in] cbfunc                    - Function pointer to be invoked 
 *                                          when a complete message is received
 * @param[in] data                      - Callback data
 *
 * @return HDMI_CEC_STATUS              - Status
 * @retval HDMI_CEC_IO_SUCCESS          - Success
 * @retval HDMI_CEC_IO_NOT_OPENED       - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_HANDLE   - An invalid handle argument has been passed
 *
 * @pre HdmiCecOpen() must be called before calling this API.
 * @warning This API is NOT thread safe.
 * @see HdmiCecTx(), HdmiCecTxAsync(), HdmiCecSetTxCallback()
 * 
 */
HDMI_CEC_STATUS HdmiCecSetRxCallback(int handle, HdmiCecRxCallback_t cbfunc, void *data);

/**
 * @note This API is deprecated.
 *
 * @brief Sets CEC message transmit callback
 *
 * This function sets a callback which will be invoked once the async transmit
 * result is available. This is only necessary if the caller chooses to transmit
 * the message asynchronously.
 *
 * This function will block if callback invocation is in progress.
 *
 * @param[in] handle                    - The handle returned from the HdmiCecOpen(). Non zero value.
 * @param[in] cbfunc                    - Function pointer to be invoked 
 *                                          when a complete message is transmitted
 * @param[in] data                      - Callback data
 * 
 * @return HDMI_CEC_STATUS              - Status
 * @retval HDMI_CEC_IO_SUCCESS          - Success
 * @retval HDMI_CEC_IO_NOT_OPENED       - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_HANDLE   - An invalid handle argument has been passed
 *
 * @pre HdmiCecOpen() must be called before calling this API.
 * @warning This API is NOT thread safe.
 * @see HdmiCecTx(), HdmiCecTxAsync()
 * 
 */
HDMI_CEC_STATUS HdmiCecSetTxCallback(int handle, HdmiCecTxCallback_t cbfunc, void *data);

/**
 * @brief Transmits a CEC message synchronously.
 *
 * This function writes a complete CEC message onto the bus and waits for the
 * transmission result.
 *
 * The message contained in the buffer shall follow the format described in
 * HdmiCecSetRxCallback().
 * (Reference: HDMI Specification 1.4, Section 6.1)
 *
 * @param[in] handle                              - Valid handle returned by HdmiCecOpen().
 *                                                  Must be a non-zero value.
 * @param[in] buf                                 - Buffer containing the complete
 *                                                  CEC message to transmit.
 * @param[in] len                                 - Number of bytes in the message.
 * @param[out] result                             - Pointer to store the transmission result.
 *                                                  Valid results are:
 *                                                  HDMI_CEC_IO_SENT_AND_ACKD,
 *                                                  HDMI_CEC_IO_SENT_BUT_NOT_ACKD,
 *                                                  HDMI_CEC_IO_SENT_FAILED.
 *                                                  Valid only for directly addressed
 *                                                  messages.
 *
 * @return HDMI_CEC_STATUS                        - Status
 * @retval HDMI_CEC_IO_SUCCESS                    - Success
 * @retval HDMI_CEC_IO_NOT_OPENED                 - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT           - Invalid buffer, length, or result
 *                                                  parameter.
 * @retval HDMI_CEC_IO_INVALID_HANDLE             - An invalid handle argument has been passed
 *
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecTxAsync(), HdmiCecSetRxCallback()
 *
 */
HDMI_CEC_STATUS HdmiCecTx(int handle, const unsigned char *buf, int len, int *result);

/**
 * @note This API is deprecated.
 *
 * @brief Writes CEC message onto bus asynchronously.
 *
 * This function writes a complete CEC message onto the bus but does not wait
 * for ACK. The result will be reported via HdmiCecRxCallback_t()
 *
 *
 * @param[in] handle                              - The handle returned from the
 *                                                    HdmiCecOpen() function. Non zero value
 * @param[in] buf                                 - The buffer contains a complete
 *                                                    CEC message to send
 * @param[in] len                                 - Number of bytes in the message
 *
 * @return HDMI_CEC_STATUS                        - Status
 * @retval HDMI_CEC_IO_SUCCESS                    - Success
 * @retval HDMI_CEC_IO_NOT_OPENED                 - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT           - Parameter passed to this function is invalid
 * @retval HDMI_CEC_IO_INVALID_HANDLE             - An invalid handle argument has been passed
 *
 * @pre  HdmiCecOpen(), HdmiCecSetRxCallback(), HdmiCecSetTxCallback()  should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecTx(), HdmiCecSetRxCallback()
 *
 */
HDMI_CEC_STATUS HdmiCecTxAsync(int handle, const unsigned char *buf, int len);
#ifdef __cplusplus
}
#endif
#endif // End of __HDMI_CEC_DRIVER_H__

/** @} */ // End of HDMI_CEC_DRIVER_H
/** @} */ // End of HDMI_CEC_HAL
/** @} */ // End of HDMI_CEC
/** @} */ // End of HPK
