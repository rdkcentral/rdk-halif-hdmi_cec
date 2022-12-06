@mainpage

# HDMI CEC HAL Documentation




# Version and Version History


1.0.0 Initial Revision covers existing HDMI CEC HAL implementation.


# Description
The diagram below describes a high-level software architecture of the HDMI CEC module stack. HDMICecDaemon is the main process responsible to initialized CEC HAL component. For any platform/product one of the CEC thunder plugin is running which is responsible to initialize CEC driver and perform respective CEC operations using CEC client side library.

![HDMI CEC Architecture Diag](hdmi_cec_architecture.png)

HdmiCec HAL provides a set of API's to initialize HDMI CEC driver and communicate CEC messages with other CEC devices connected with HDMI cable. Mainly the HAL API corresponds to CEC open and close driver, retrieve discovered Logical and Physical address of host device, synchronous and asynchronous API's to transmit and receive messages to and from the remote device.
 
# Component Runtime Execution Requirements

 The 3rd party delivery is responsible to perform CEC logical and Physical address discovery.
 CEC message transmit operation should complete within few hundred ms.
 RDK middleware is responsible to perform retry operation as per the CEC specification requirements. 
 Failure to meet these requirements will likely result in undefined and unexpected behavior.

## Theory of operation

HDMI CEC HAL API is initialized by HdmiCecDaemon module. RDK-V middleware expected to have complete control over the life cycle over the Hdmi CEC HAL module.

  1. Initialize the driver using function: HdmiCecOpen(int *) before making any other CEC HAL API calls. This call does the CEC driver open functionality and discovers the CEC logical address based on the device type and discover the physical address based on the connection topology. If HdmiCecOpen(int *) calls fails, CEC HAL should return the respective error code, so that the client can retry the operation.

  2. Once logical address and physical address is assigned. CEC module should able to send and receive the respective CEC commands

        2.1. For asynchronous receive operations use the call back function: HdmiCecSetRxCallback(int, HdmiCecRxCallback_t, void*)
             Please ensure receive call back is registered before doing transmit, if we are expecting any reply from the 
             destination device

        2.2. For synchronous transmit use the function: HdmiCecTx(int, const unsigned char *, int, int*)

        2.3. For asynchronous transmit ack use the call back function: HdmiCecSetTxCallback(int, HdmiCecTxCallback_t, void*)
             Please ensure this call back is registered before doing asynchronous transmit operation. This call back
             is used to get the ack for the HdmiCecTxAsync

		2.4. For async transmit use the function: HdmiCecTxAsync(int, const unsigned char *, int)

  4. Once HDMI CEC operations are finished part of cleanup process, remove the assigned logical address using the function: HdmiCecRemoveLogicalAddress(int, int)

  5. Finally close the CEC driver using the HAL api: HdmiCecClose(int)

Need to call the function sequence in above order so that the system behaves
in a deterministic manner, every time.

## Threading Model

HDMI CEC HAL is not thread safe, any module which is invoking the HDMI CEC HAL api 
should ensure calls are made in a thread safe manner.

All thread should be correctly terminated when  HdmiCecClose() is called.

Different 3rd party vendors allowed to create internal threads to meet the requirements of underlaying CEC driver module and meet the operational requirements. In this case 3rd party implementations should be responsible to synchronize between the calls, events and cleanup the thread.

## Process Model

HdmiCecDaemon process takes care of Initializing CEC HAL component. At any point of time a single instance of HdmiCecDaemon process exists to respond to any CEC related functionality to the application.
The interface implementation should not manipulate any process-wide state, such as the current directory, locale, etc. Doing so could have unintended consequences for other threads within the process group.

## Memory Model

CEC HAL client module is responsible to pass message buffer and free it for transmit request. For Receive message passes message buffer in the callback and client is responsible to copy to its internal buffer before callback is returned.

## Power Management Requirements

The CEC HAL is not involved in any of the power management operation.
Any power management state transitions MUST not affect the operation of the CEC HAL. e.g. on resumption from a low power state the operation of the CEC should continue as if no transition has occurred.

For further information on handling wake from DEEPSLEEP see the RDK PowerManager HAL documentation.

## Asynchronous Notification Model

Asynchronous CEC transmit and receive operations following API's  and callback registrations are used.
  1. For async transmit use the function: HdmiCecTxAsync(int, const unsigned char *, int)

  2. For async receive call back use the function: HdmiCecSetRxCallback(int, HdmiCecRxCallback_t, void*)

  3. For async transmit ack use the function: HdmiCecSetTxCallback(int, HdmiCecTxCallback_t, void*)



## Blocking calls

Please see the blocking calls used in HDMI CEC HAL:
  1. HdmiCecOpen(int *)
  2. HdmiCecClose(int)
  3. HdmiCecSetLogicalAddress(int, int *, int)
  4. HdmiCecAddLogicalAddress(int, int)
  5. HdmiCecRemoveLogicalAddress(int, int)
  6. HdmiCecGetLogicalAddress(int, int,  int *)
  7. HdmiCecGetPhysicalAddress(int,unsigned int *)
  8. HdmiCecTx(int, const unsigned char *, int, int *)
Blocking calls should complete within a time period commensurate with the complexity of the operation and in accordance with any relevant CEC specification. Any calls that can fail due to the lack of a response from connected device should have a timeout period in accordance with any relevant documentation. If any of the blocking call is prematurely terminated after successful HdmiCecOpen(int *) call, client is responsible to call HdmiCecClose(int) and retry the steps in Theory of operation.

## Internal Error Handling

All the CEC HAL API's should return error synchronously as a return argument. HAL is responsible to handle system errors(e.g. out of memory) internally.

## Persistence Model

There is no requirement for HAL to persist any setting information. Application/Client is responsible to persist any settings related to HDMI CEC feature.


# Nonfunctional requirements

Following non functional requirement should be supported by the CEC HAL component.

## Logging and debugging requirements

HDMI CEC HAL component should log all the error and critical informative messages which helps to debug/triage the issues and understand the functional flow of the system.

## Memory and performance requirements

Make sure CEC HAL is not contributing more to memory and CPU utilization while performing normal CEC operations and in idle mode. Commensurate with the operation required.


## Quality Control

CEC HAL implementation should pass Coverity, Black duck scan, valgrind checks without any issue.

There should not be any memory leaks/corruption introduced by HAL and underneath 3rd party software implementation.

HAL implementation should pass RDK CEC HAL available at: https://gerrit.teamccp.com/plugins/gitiles/rdk/components/generic/tdk/generic/+/stable2/framework/web-app/fileStore/testscriptsRDKV/component/HdmicecHal/

## Licensing

HDMI CEC HAL implementation is expected to released under the Apache License. 

## Build Requirements

CEC HAL source code should be build under Linux Yocto environment and should be delivered as a shared library.
  

## Variability Management

Any new API introduced should be implemented by all the 3rd party module and RDK generic code should be compatible with specific version of CEC HAL software

## Platform or Product Customization

None

# Interface API Documentation

Covered as per Doxygen documentations.

## Theory of operation and key concepts

Covered as per "Description" sections.

### UML Diagrams

#### Sequence Diagram

![HDMI CEC Sequence Diagram](hdmi_cec_sequence.png)

#### State Diagram

![HDMI CEC State Diagram](hdmi_cec_state.png)
