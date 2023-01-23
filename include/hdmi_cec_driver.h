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
 * @defgroup HDMI_CEC RDK HDMI-CEC
 * @par Application API Specification
 * In general, the RDK CEC Library offers three categories of Application APIs:
 * @par HDMI-CEC Connection
 * The Connection APIs allows application to transmit and receive raw data bytes
 * (a.k.a. CEC Frame) that conforms to the HDMI-CEC specification.
 * This is the only interface that the application can use to access CEC Bus.
 * @par HDMI-CEC Message and Frame Constructs
 * The Messages APIs allows application to encode high-level message constructs into @b CECFrame raw bytes,
 * or decode @b CECFrame raw bytes into high-level message constructs.
 *
 * @image html hdmi_cec_architecture.png
 *
 * @par Host Interface
 * The @b Host interface allows libCEC implementation to interact with the host environment
 * Such interaction includes monitoring of the Power State change, the HDMI HotPlug events, or APIs to change the Host States.
 * This allows the CEC stack to run in any devices that implements the Host Interface.
 * @par Driver Component
 * The @b Driver component access the HDMI-CEC SoC Driver via the CEC HAL API.
 * The Vendors are responsible in delivering a SoC Driver that conforms to the HAL API (see hdmi_cec_driver.h"
 *
 * @defgroup HDMI_CEC_APPLICATION HDMI-CEC Applications
 * The relation between Application, Connection and CEC-Bus is described in the figure.
 *
 * @image html hdmi_cec_architecture.png
 *
 * @par CEC Daemon
 * This daemon receives Raw Bytes from the SoC Driver and dispatches them into IARM Bus via CecIARMMgr,
 * it also receives Raw Bytes from IARM Bus and send to the SoC Driver. It controls
 * the only access point to the physical HDMI CEC Bus.
 *
 * @par Receiver
 * This application receives Raw Bytes from IARM Bus and dispatch to Service Manager (which in turn may dispatch
 * the bytes to Guide Application). It also receives Raw bytes from Service Manager and send to the IARM-Bus.
 *
 * @par CECDevMgr
 * This application is a "sniffer" on the CEC Bus. It monitors all messages on the bus and construct a Device Map,
 * which depicts the topology of all connected devices on the CEC Bus. Other applications can be developed similar
 * to Receiver or CECDevMgr where the CEC Raw bytes (in form of CECFrame) are send/receive to/from IARM Bus.
 * This pseudo CEC Driver implemented on IARM Bus is called CEC IARM Adapter in the diagram.
 * @note CECDevMgr binary is not currently being copied to the box as it is not being used in box.
 * @ingroup HDMI_CEC
 *
 * @defgroup HDMI_CEC_CONNECTION HDMI-CEC Connection
 * The connection API allows the application to access CEC Bus. A connection is a tap into the CEC bus.
 * The application can use a connection to send raw bytes (in form of CECFrame) onto CEC bus or receive raw bytes from it.
 * @n @n
 * Each connection must have one and only one logical address. When application sends a CEC frame onto the connection,
 * the connection's logical address will be used as the source address. On such connection, the application can only
 * receive broadcast messages or unicast messages destined to the logical address of the connection.
 * @n @n
 * The application can open multiple connections, with one connection dedicated to each Role of the application.
 * For example, a Set-top Box of three roles (Record, Playback, and Tuner) can open three connections, with each connection
 * used to send/receive messages of the relevant role. Such Role-Connection mapping is transparent to the devices on the CEC bus.
 * The external devices will not be able to tell if the CEC messages come from connections of a same or different physical device.
 * @n @n
 * All incoming CEEFrame arrived at the Physical CEC bus will be dispatched to all opened connections, based on the filtering
 * criteria of the connection. By default, a connection with a given logical address filters out messages destined
 * to other logical addresses.
 * @n @n
 * When a connection is opened without a given logical address, this connection will pick up all messages destined
 * to the host device, regardless what roles the device has. This is useful if the application wants to sniff all
 * available CEC packets from the bus.
 * @ingroup HDMI_CEC
 *
 * @defgroup HDMI_CEC_DRIVER HDMI-CEC Driver APIs
 * The Driver component access the HDMI-CEC SoC Driver via the CEC HAL API. Vendors are responsible in delivering
 * a SoC Driver that conforms to the HAL API (see hdmi_cec_driver.h)
 * @ingroup HDMI_CEC
 *
 * @defgroup HDMI_CEC_MULTI_APP Multi-App Support
 *
 * Often , the application functionality (Record, Tune and Playback) is distributed across
 * multiple components. In order for any component to have equivalent access to the HDMI-CEC bus,
 * the library offers Multi-App support via IARM-Bus. This support is enabled by default, and can be disabled if desired.
 * @n @n
 * In essence, there is only one physical CEC bus on asystem. However, with the help of Connection, Logical CEC-Bus,
 * and IARM-Bus, the CEC library can converge the CEC traffic from different Connections and Logical Buses before
 * forwarding them to the single physical bus. This is illustrated by the diagram below.
 *
 * @image html hdmi_cec_architecture.png
 *
 * In this diagram there are two applications (Receiver and CECDevMgr). Since both applications can only access the
 * underlying physical CEC Bus via Connection API, they have no knowledge how the message are eventually delivered
 * to the Physical Bus.
 * @n @n
 * For both applications, its CEC messages flows from,
 *
 * @code
 * Connection --> Logical Bus--> CEC IARM Apaper--> IARM Bus---> (CECDaemonMain)
 * @endcode
 *
 * For CECDaemonMain, its CEC messages flows from,
 *
 * @code
 * IARM Bus --> CecIARMMgr --> Connection --> Logical Bus --> CEC Driver --> (Physical Bus)
 * @endcode
 *
 * The message flow on Connections and Logical Buses are full duplex.
 * @ingroup HDMI_CEC
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
/*
 * Error Codes.
 */
enum HDMI_CEC_IO_ERROR{
	/// Hdmi cec input output operation is successful
	HDMI_CEC_IO_SUCCESS = 0,
	/// Hdmi cec io operation send and acknowledgement received
    HDMI_CEC_IO_SENT_AND_ACKD = 1, 
	/// Hdmi cec io operation send but acknowledgement not received
    HDMI_CEC_IO_SENT_BUT_NOT_ACKD,
	/// Hdmi cec io operation failed
    HDMI_CEC_IO_SENT_FAILED,
	/// Hdmi cec module is in invalid state and hence not able to do io operation
    HDMI_CEC_IO_INVALID_STATE,
	/// Invalid argument is passed to hdmi cec module
    HDMI_CEC_IO_INVALID_ARGUMENT,
	/// Logical address is not available for doing hdmi cec io operation
    HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE,
	/// Hdmi cec io operation general error
    HDMI_CEC_IO_GENERAL_ERROR,
};

/**
 * @addtogroup HDMI_CEC_DRIVER
 * @{
 */
/**
 * CEC Driver should call this function whenever there is a complete CEC packet received.
 * Upon each callback, only 1 complete CEC packet should be contained in the buffer.
 * @param [in] handle - The handle used by application to uniquely identify the driver instance.
 * @param [in] callbackData - callback data for the receive callback
 * @param [in] buf - buffer passed in hdmi receive callback
 * @param [in] len - length of buffer passed in receive hdmi callback
 */

typedef void (*HdmiCecRxCallback_t)(int handle, void *callbackData, unsigned char *buf, int len);

/**
 * CEC Driver should call this function to report the status of the latest transmit.
 * @param [in] handle - The handle used by application to uniquely identify the driver instance.
 * @param [in] callbackData - callback data for the transmit callback
 * @param [in] result - async transmit result from the platform implementation.
 */

typedef void (*HdmiCecTxCallback_t)(int handle, void *callbackData, int result);
/**
 * @brief opens an instance of CEC driver.  
 * This function should be call once before the functions in this API can be used.
 *
 * @param [out] handle - The handle used by application to uniquely identify the driver instance. 
 *
 * @return Error Code: If error code is returned, the open is failed.
 * TODO: Change the return type to HDMI_CEC_IO_ERROR instead of int
 * Possible error codes are:
 * @retval HDMI_CEC_IO_SUCCESS HdmiCecOpen was successful.
 * @retval HDMI_CEC_IO_INVALID_STATE: Calling HdmiCecOpen again after a successful open in a process will result in this error.
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_IO_GENERAL_ERROR: This error code indicates the underlying undefined platform error. In the HAL side implementation of this function return value will
 * be initialized with this error code. So that any of the undefined error case scenario in this function, will be report as this error code.
 * @warning  This API is Not thread safe.
 * @see HdmiCecClose()
 */
int HdmiCecOpen(int *handle);

/**
 * @brief close an instance of CEC driver.  
 * This function should close the currently opened driver instance.
 *
 * @param [in] handle - The handle returned from the HdmiCecOpen() function.
 *
 * @return Error Code:  If error code is returned, the open is failed.
 * TODO: Change the return type to HDMI_CEC_IO_ERROR instead of int
 * Possible error codes are:
 * @retval HDMI_CEC_IO_SUCCESS HdmiCecClose was successful.
 * @retval HDMI_CEC_IO_INVALID_STATE: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_IO_GENERAL_ERROR: This error code indicates the underlying undefined platform error. In the HAL side implementation of this function return value will
 * be initialized with this error code. So that any of the undefined error case scenario in this function, will be report as this error code.
 * This usually indicates the underlying unknown platform error.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecOpen()
 *
 */
int HdmiCecClose(int handle);

/**
 *
 * Set the Logical Addresses claimed by host device.
 * This function sets multiple logical addresses used by the host.  The host
 * has claimed these logical address through the Discovery process.  Once 
 * set, the host shall receive all CEC packets destined to these addresses.
 *
 * Once the driver is opened, the host shall receive all broadcast messages 
 * regardless the Logical addresses.
 * 
 * when there is no logical address set, the host shall only receive broadcast
 * messages.
 *
 * @param [in] handle - The handle returned from the HdmiCecOpen() function.
 * @param [in] logicalAddresses - The logicalAddresses set or replace addresses claimed by host.
 *                 A null value clears the current list. 
 * @param [in] num - The number of logical addresses.  If any logical address in the list
 *                 cannot be set, none of them should be set upon return. Success
 *                 return indicates that all addresses in the list are set.
 *
 * @return Error Code:  If error code is returned, the set is failed.
 * TODO: Change the return type to HDMI_CEC_IO_ERROR instead of int
 * Possible error codes are:
 * @retval HDMI_CEC_IO_SUCCESS HdmiCecSetLogicalAddress was successful.
 * @retval HDMI_CEC_IO_INVALID_STATE: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_IO_GENERAL_ERROR: This error code indicates the underlying undefined platform error. In the HAL side implementation of this function return value will
 * be initialized with this error code. So that any of the undefined error case scenario in this function, will be report as this error code.
 * This usually indicates the underlying unknown platform error.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @note This API is not used as SOC is responsible to perform logical address discovery.
 * @see HdmiCecAddLogicalAddress(), HdmiCecRemoveLogicalAddress(), HdmiCecGetLogicalAddress()
 */
int HdmiCecSetLogicalAddress(int handle, int *logicalAddresses, int num);

/**
 * @brief Add one Logical Addresses to be used by host device.
 *
 * This function can block until the intended logical address is secured by
 * the driver.
 *
 * In driver implementation, this API would trigger driver sending a POLL
 * CEC packet to the CEC Bus,
 *
 * Packet::HeaderBlock::Initiator   =  Requested LogicalAddress.
 * Packet::HeaderBlock::Destination =  Requested LogicalAddress.
 * Packet::DataBlock   				=  Empty.
 *
 * The function shall return HDMI_CEC_IO_SUCCESS if the POLL message is sent
 * successfully and not ACK'd by any device on the bus. From this point on the
 * driver shall forward all received messages with destination being the acquired
 * logical address. Driver should ACK all POLL messages destined to this logical
 * address.
 *
 * The function shall return HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE if the POLL
 * message is sent and ACK'd by a device on the bus.
 *
 * The function shall return relevant error code if the POLL message is not sent
 * successfully.
 *
 *
 * @param [in] handle - The handle returned from the HdmiCecOpen() function.
 * @param[in] logicalAddresses - The logical address to be acquired.
 *
 * TODO: Change the return type to HDMI_CEC_IO_ERROR instead of int
 * @return Error Code: Possible error codes are:
 * @retval HDMI_CEC_IO_SUCCESS HdmiCecAddLogicalAddress was successful.
 * @retval HDMI_CEC_IO_INVALID_STATE: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_IO_GENERAL_ERROR: This error code indicates the underlying undefined platform error. In the HAL side implementation of this function return value will
 * be initialized with this error code. So that any of the undefined error case scenario in this function, will be report as this error code.
 * This usually indicates the underlying unknown platform error.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @note This API is not used as SOC is responsible to perform logical address discovery.
 * @see HdmiCecSetLogicalAddress(), HdmiCecRemoveLogicalAddress(), HdmiCecGetLogicalAddress()
 */
int HdmiCecAddLogicalAddress(int handle, int logicalAddresses);

/**
 * @brief Clear the Logical Addresses claimed by host device.
 *
 * This function releases the previously acquired logical address.  Once
 * released, driver should not ACK any POLL message destined to the
 * released address.
 *
 * @param [in] handle - The handle returned from the HdmiCecOpen() function.
 * @param [in] logicalAddresses - The logicalAddresses to be released.
 *
 * @return Error Code: If error code is returned, the remove is failed.
 * TODO: Change the return type to HDMI_CEC_IO_ERROR instead of int
 * Possible error codes are:
 * @retval HDMI_CEC_IO_SUCCESS HdmiCecRemoveLogicalAddress was successful.
 * @retval HDMI_CEC_IO_INVALID_STATE: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_IO_GENERAL_ERROR: This error code indicates the underlying undefined platform error. In the HAL side implementation of this function return value will
 * be initialized with this error code. So that any of the undefined error case scenario in this function, will be report as this error code.
 * This usually indicates the underlying unknown platform error.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @note This API is not used as SOC is responsible to perform logical address discovery.
 * @see HdmiCecAddLogicalAddress(), HdmiCecSetLogicalAddress(), HdmiCecGetLogicalAddress()
 */
int HdmiCecRemoveLogicalAddress(int handle, int logicalAddresses);

/**
 * @brief Get the Logical Address obtained by the driver.
 *
 * This function get the logical address for the specified device type.
 *
 * @param [in] handle - The handle returned from the HdmiCecOpen() function.
 * @param [in] devType - The device type (tuner, record, playback etc.).
 * @param [out] logicalAddress - The logical address acquired
 *
 * @return Error Code:  If error code is returned, the get is failed.
 * TODO: Change the return type to HDMI_CEC_IO_ERROR instead of int
 * Possible error codes are:
 * @retval HDMI_CEC_IO_SUCCESS HdmiCecGetLogicalAddress was successful.
 * @retval HDMI_CEC_IO_INVALID_STATE: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_IO_GENERAL_ERROR: This error code indicates the underlying undefined platform error. In the HAL side implementation of this function return value will
 * be initialized with this error code. So that any of the undefined error case scenario in this function, will be report as this error code.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecAddLogicalAddress(), HdmiCecRemoveLogicalAddress(), HdmiCecSetLogicalAddress()
 */
int HdmiCecGetLogicalAddress(int handle, int devType,  int *logicalAddress);

/**
 * @brief Get the Physical Address obtained by the driver.
 *
 * This function get the Physical address for the specified device type.
 *
 * @param [in] handle - returned from the HdmiCecOpen() function.
 * @param [out] physicalAddress - physical address acquired
 *
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecGetLogicalAddress(), HdmiCecSetLogicalAddress()
 */
void HdmiCecGetPhysicalAddress(int handle, unsigned int *physicalAddress);


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
 * TODO: Change the return type to HDMI_CEC_IO_ERROR instead of int
 * Possible error codes are:
 * @retval HDMI_CEC_IO_SUCCESS HdmiCecGetLogicalAddress was successful.
 * @retval HDMI_CEC_IO_INVALID_STATE: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_IO_GENERAL_ERROR: This error code indicates the underlying undefined platform error. In the HAL side implementation of this function return value will
 * be initialized with this error code. So that any of the undefined error case scenario in this function, will be report as this error code.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecTx(), HdmiCecTxAsync(), HdmiCecSetTxCallback()
 */
int HdmiCecSetRxCallback(int handle, HdmiCecRxCallback_t cbfunc, void *data);

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
 * TODO: Change the return type to HDMI_CEC_IO_ERROR instead of int
 * Possible error codes are:
 * @retval HDMI_CEC_IO_SUCCESS HdmiCecGetLogicalAddress was successful.
 * @retval HDMI_CEC_IO_INVALID_STATE: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_IO_GENERAL_ERROR: This error code indicates the underlying undefined platform error. In the HAL side implementation of this function return value will
 * be initialized with this error code. So that any of the undefined error case scenario in this function, will be report as this error code.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecTx(), HdmiCecTxAsync()
 */
int HdmiCecSetTxCallback(int handle, HdmiCecTxCallback_t cbfunc, void *data);

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
 * TODO: Change the return type to HDMI_CEC_IO_ERROR instead of int
 * Possible error codes are:
 * @retval HDMI_CEC_IO_SUCCESS HdmiCecGetLogicalAddress was successful.
 * @retval HDMI_CEC_IO_INVALID_STATE: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_IO_SENT_AND_ACKD : Cec message is send and acknowledged.
 * @retval HDMI_CEC_IO_SENT_BUT_NOT_ACKD : Hdmicec message send but not acknowledged by the receiver.
 * @retval HDMI_CEC_IO_SENT_FAILED: Hdmicec io send message failed.
 * @retval HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE Hdmi Cec receiver logical address is not available.
 * @retval HDMI_CEC_IO_GENERAL_ERROR: This error code indicates the underlying undefined platform error. In the HAL side implementation of this function return value will
 * be initialized with this error code. So that any of the undefined error case scenario in this function, will be report as this error code.
 * @pre  HdmiCecOpen() should be called before calling this API.
 * @warning  This API is Not thread safe.
 * @see HdmiCecTxAsync(), HdmiCecSetRxCallback()
 */
int HdmiCecTx(int handle, const unsigned char *buf, int len, int *result);

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
 * TODO: Change the return type to HDMI_CEC_IO_ERROR instead of int
 * Possible error codes are:
 * @retval HDMI_CEC_IO_SUCCESS HdmiCecGetLogicalAddress was successful.
 * @retval HDMI_CEC_IO_INVALID_STATE: Indicates the respective api is called with out calling  HdmiCecOpen () or  preceding HdmiCecOpen () has failed.
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT: This error code will be returned, if parameter passed to this function is invalid.
 * @retval HDMI_CEC_IO_SENT_AND_ACKD : Cec message is send and acknowledged.
 * @retval HDMI_CEC_IO_SENT_BUT_NOT_ACKD : Hdmicec message send but not acknowledged by the receiver.
 * @retval HDMI_CEC_IO_SENT_FAILED: Hdmicec io send message failed.
 * @retval HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE Hdmi Cec receiver logical address is not available.
 * @retval HDMI_CEC_IO_GENERAL_ERROR: This error code indicates the underlying undefined platform error. In the HAL side implementation of this function return value will
 * be initialized with this error code. So that any of the undefined error case scenario in this function, will be report as this error code.
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

