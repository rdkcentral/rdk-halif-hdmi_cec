@mainpage

# HDMI CEC HAL Documentation


# Version and Version History

## Version History

| Date | Author | Comment | Version |
| --- | --------- | --- | --- |
| 13/03/23 | Review Team | Edit  | 1.0.1 |
| 06/12/22| Amit Patel | First Release | 1.0.0 |


# Description
The diagram below describes a high-level software architecture of the HDMI CEC module stack. 
```mermaid
%%{ init : { "theme" : "forest", "flowchart" : { "curve" : "linear" }}}%%
flowchart TD
y[HDMI CEC HAL Client process]<-->x[HDMI CEC HAL lib];
x[HDMI CEC HAL lib]<-->z[HDMI CEC SOC Driver];
style y fill:#99CCFF,stroke:#333,stroke-width:0.3px,align:left
style z fill:#fcc,stroke:#333,stroke-width:0.3px,align:left
style x fill:#9f9,stroke:#333,stroke-width:0.3px,align:left

 ```

HdmiCec HAL provides a set of APIs to initialize HDMI CEC driver and communicate CEC messages with other CEC devices connected with HDMI cable. Mainly the HAL API corresponds to CEC open and close driver, retrieve discovered Logical and Physical address of host device, synchronous and asynchronous APIs to transmit and receive messages to and from the remote device.
 
# Component Runtime Execution Requirements

CEC HAL client module is responsible to perform CEC logical and Physical address discovery. CEC message transmit operation should complete within a reasonable time (a few hundred ms). Client is responsible to perform retry operation as per the CEC specification requirements. 
 
 #TODO Sreenivasa to check on the specs
 

## Initialization and Startup

Hdmi CEC HAL Client process should initialize CEC HAL by calling HdmiCecOpen() API before calling any other HAL API.


## Threading Model

HDMI CEC HAL is not thread safe, any module which is invoking the HDMI CEC HAL APIs should ensure calls are made in a thread safe manner.

All threads should be correctly terminated when HdmiCecClose() is called.

HDMI CEC HAL implementation is allowed to create internal threads to meet the requirements of underlaying CEC driver module and meet the operational requirements. In this case HDMI CEC HAL implementations should be responsible to synchronize between the calls, events and cleanup the thread.

## Process Model

At any point of time a single instance of the CEC HAL client moduleprocess can exist to respond to any CEC related functionality.

## Memory Model

For transmit messages, it is upto the HDMI CEC HAL client module to allocate and free the memory for the message buffer. For callbacks/received messages, the HDMI CEC HAL is responsible for the memory allocation and de-allocation.

## Power Management Requirements

The HDMI CEC HAL is not involved in any of the power management operations. Any power management state transitions MUST not affect the operation of the HDMI CEC HAL. e.g. on resumption from a low power state, the operation of the HDMI CEC should continue as if no transition has occurred.

## Asynchronous Notification Model

For asynchronous HDMI CEC transmit and receive operations, the following APIs and callback registrations are used:

  1. For async transmit use the function: HdmiCecTxAsync( int, const unsigned char * , int )
  2. For async receive call back use the function: HdmiCecSetRxCallback(int, HdmiCecRxCallback_t, void* )
  3. For async transmit ack use the function: HdmiCecSetTxCallback( int, HdmiCecTxCallback_t, void* )

Please refer [Memory Model](#memory_model) for memory allocation for asynchronous calls.

## Blocking calls

Please see the blocking calls used in HDMI CEC HAL:
  1. HdmiCecOpen( int * )
  2. HdmiCecClose( int )
  3. HdmiCecAddLogicalAddress( int, int )
  4. HdmiCecRemoveLogicalAddress( int, int )
  5. HdmiCecGetLogicalAddress( int, int, int * )
  6. HdmiCecGetPhysicalAddress( int, unsigned int * )
  7. HdmiCecTx( int, const unsigned char * , int, int * )
    
Blocking calls should complete within a reasonable time period in accordance with any relevant CEC specification. Any calls that can fail due to the lack of a response from connected device should have a timeout period in accordance with any relevant CEC specification. If any of the blocking call is prematurely terminated after successful HdmiCecOpen(int * ) call, client is responsible to call HdmiCecClose(int) and retry the steps in Theory of operation.

## Internal Error Handling

All the HDMI CEC HAL APIs should return error synchronously as a return argument. HAL is responsible to handle system errors(e.g. out of memory) internally.

## Persistence Model

There is no requirement for HAL to persist any setting information. HDMI CEC Client is responsible to persist any settings related to HDMI CEC feature.


# Nonfunctional requirements

## Logging and debugging requirements

HDMI CEC HAL component should log all the error and critical informative messages which helps to debug/triage the issues and understand the functional flow of the system.

## Memory and performance requirements

HDMI CEC HAL should not cause excessive memory and CPU utilization.

## Quality Control

HDMI CEC HAL implementation should perform static analysis, our preferred tool is Coverity.
Copyright validation should be performed, our preferred tool is Black duck.
- Have a zero-warning policy with regards to compiling. All warnings should be treated as error.
- Use of memory analysis tools like Valgrind are encouraged, to identify leaks/corruptions.
- `HAL` Tests will endeavour to create worst case scenarios to assist investigations.

## Licensing

HDMI CEC HAL implementation is expected to released under the Apache License 2.0. 

## Build Requirements

HDMI CEC HAL source code should be built independently.
  

## Variability Management

Any changes in the APIs should be reviewed and approved by COMCAST.

## Platform or Product Customization

None

# Interface API Documentation


## Theory of operation and key concepts

HDMI CEC HAL should be initialized by HDMI CEC client process. HDMI CEC client is expected to have complete control over the life cycle of the Hdmi CEC HAL module.

  1. Initialize the driver using function: HdmiCecOpen(int * ) before making any other CEC HAL API calls. This call performs the CEC driver open functionality, discovers the physical address based on the connection topology. In case of source devices HdmiCecOpen should initiate the logical address discovery part of this routine. In case of sink devices logical address will be fixed and set using the HdmiCecAddLogicalAddress api by the client module. If HdmiCecOpen( int * ) calls fails, CEC HAL should return the respective error code, so that the client can retry the operation.

  2. Once logical address and physical address is assigned, CEC module should able to send and receive the respective CEC commands

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

Covered as per "Description" sections.

### UML Diagrams

#### Sequence Diagram

```mermaid
%%{ init : { "theme" : "default", "flowchart" : { "curve" : "stepBefore" }}}%%
   sequenceDiagram
    HdmiCecDemon->>HdmiCecHal:HdmiCecOpen
    HdmiCecHal-->>HdmiCecDemon:return
    Note over HdmiCecHal: Once open is completed soc will discover <br> physical and logical address internally, <br> based on device type and connection topology
    HdmiCecDemon->>HdmiCecHal:HdmiCecSetRxCallback
    HdmiCecHal-->>HdmiCecDemon:return
    HdmiCecDemon ->>HdmiCecHal:HdmiCecGetLogicalAddress
    HdmiCecHal-->>HdmiCecDemon:return
    HdmiCecDemon ->>HdmiCecHal:HdmiCecGetPhysicalAddress
    HdmiCecHal-->>HdmiCecDemon:return
    Note over HdmiCecDemon,HdmiCecHal: sync cec transmit message
    HdmiCecDemon ->>HdmiCecHal:HdmiCecTx
    HdmiCecHal-->>HdmiCecDemon:return
    Note over HdmiCecHal: for the above message if response expected or message originated <br> from the remote device HdmiCecSetRxCallback fn will trigger
    HdmiCecHal-->>HdmiCecDemon:HdmiCecSetCallback triggered
    Note over HdmiCecDemon,HdmiCecHal: Close the cec driver. Cec hal operation finished
    HdmiCecDemon ->>HdmiCecHal:HdmiCecClose
    HdmiCecHal-->>HdmiCecDemon:return
 ```

#### State Diagram

```mermaid
%%{ init : { "theme" : "default", "flowchart" : { "curve" : "stepBefore" }}}%%
    stateDiagram-v2
        [*] --> Closed
        Closed --> OpenState:HdmiCecOpen <br> will do cec driver open
        OpenState --> ReadyState:Logical and physical address <br> allocated
        note right of ReadyState : Ready state we can send and <br> receive messages.
        OpenState --> Closed : HdmiCecClose
        ReadyState --> Closed : HdmiCecClose

```
