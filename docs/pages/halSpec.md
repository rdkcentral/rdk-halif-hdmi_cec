@mainpage

# HDMI CEC HAL Documentation


## Version and Version History

### Version History

| Date | Author | Comment | Version |
| --- | --------- | --- | --- |
| 13/03/23 | Review Team | Edit  | 1.0.1 |
| 06/12/22| Amit Patel | First Release | 1.0.0 |

### Acronymns
1. HDMI - High-Definition Multimedia Interface
2. CEC  - Consumer Electronics Control

## Description
The diagram below describes a high-level software architecture of the HDMI CEC module stack. 
```mermaid
%%{ init : { "theme" : "forest", "flowchart" : { "curve" : "linear" }}}%%
flowchart TD
y[Caller]-->x[HDMI CEC HAL Lib];
x[HDMI CEC HAL Lib]-->z[HDMI CEC SOC Driver];
style y fill:#99CCFF,stroke:#333,stroke-width:0.3px,align:left
style z fill:#fcc,stroke:#333,stroke-width:0.3px,align:left
style x fill:#9f9,stroke:#333,stroke-width:0.3px,align:left

 ```

`HDMI CEC` HAL provides a set of APIs to communicate CEC messages with other CEC devices connected with HDMI cable. The purpose of the `HDMI CEC` HAL is to retrieve discovered logical and physical address of host device and to transmit and receive messages to and from the remote device synchronously / asynchronously.
 
## Component Runtime Execution Requirements

CEC message transmit operation should complete within one second. Desired CEC response time is 250 milliseconds and maximum response time should be 1 second as provided in the CEC specifications. Caller is responsible to perform retry operation as per the CEC specification requirements. 

### Initialization and Startup

Caller should initialize `HDMI CEC` HAL by calling `HdmiCecOpen()` API before calling any other `HDMI HAL` API.

### Threading Model

`HDMI CEC` HAL is not required to be thread safe. Any caller invoking the `HDMI CEC` HAL APIs should ensure calls are made in a thread safe manner.

### Process Model

The interface is expected to support a single instantiation with a single process.

### Memory Model

For transmit messages, it is upto the caller to allocate and free the memory for the message buffer. For receive messages, the HAL is responsible for memory management. The memory allocated cannot exceed 20 bytes.

### Power Management Requirements

Although the HAL is not involved in any of the power management operations, the state transitions MUST not affect its operation. e.g. on resumption from a low power state, the `HDMI CEC` should operate as if no transition has occurred.

### Asynchronous Notification Model

For asynchronous transmit and receive operations, the following APIs and callback registrations are used:

  1. For async transmit use the function: `HdmiCecTxAsync()`
  2. For async receive call back use the function: `HdmiCecSetRxCallback()`
  3. For async transmit ack use the function: `HdmiCecSetTxCallback()`

The caller will return the callback context as fast as possible.

### Blocking calls

There are no blocking calls. Synchronous calls should complete within a reasonable time period in accordance with any relevant CEC specification. Any call that can fail due to the lack of a response from the connected device should have a timeout period in accordance with any relevant CEC specification and the function should return the relevant error code.

### Internal Error Handling

All the APIs should return error synchronously as a return argument. HAL is responsible to handle system errors (e.g. out of memory) internally.

### Persistence Model

There is no requirement for HAL to persist any setting information. Caller is responsible to persist any settings related to `HDMI CEC` feature.

## Nonfunctional requirements

### Logging and debugging requirements

`HDMI CEC` HAL component must support DEBUG, INFO and ERROR messages. DEBUG should be disabled by default and enabled when required.

### Memory and performance requirements

The HAL should not cause excessive memory and CPU utilization.

### Quality Control

`HDMI CEC` HAL implementation should perform static analysis, our preferred tool is Coverity.
Copyright validation should be performed, our preferred tool is Black duck.
- Have a zero-warning policy with regards to compiling. All warnings should be treated as error.
- Use of memory analysis tools like Valgrind are encouraged, to identify leaks/corruptions.
- `HAL` Tests will endeavour to create worst case scenarios to assist investigations.

### Licensing

HDMI CEC HAL implementation is expected to released under the Apache License 2.0. 

## Build Requirements

`HDMI CEC` HAL source code must build into a shared library and must be named as `libRCECHal.so`.
 
### Variability Management

Any changes in the APIs should be reviewed and approved by the component architects.

## Platform or Product Customization

None

# Interface API Documentation


## Theory of operation and key concepts

The caller is expected to have complete control over the life cycle of the `HDMI CEC` HAL component.

1. Initialize the HAL using function: `HdmiCecOpen()` before making any other API calls. This call also discovers the physical address based on the connection topology. In case of source devices, `HdmiCecOpen()` should initiate the logical address discovery as part of this routine. In case of sink devices, logical address will be fixed and set using the `HdmiCecAddLogicalAddress()` API. If `HdmiCecOpen()` call fails, the HAL should return the respective error code, so that the caller can retry the operation.

2. Once logical address and physical address are assigned, the caller will be able to send and receive the respective `CEC` messages.

2.1. For asynchronous receive operations, use the callback function: `HdmiCecSetRxCallback()`. the caller must register a callback after initialisation.

2.2. For synchronous transmit, use the function: `HdmiCecTx()`.

2.3. For asynchronous transmit, use the function: `HdmiCecTxAsync()`. The caller must register a callback via `HdmiCecSetTxCallback()` in order to receive the status or acknowledgement.

3. De-intialise the HAL using the function: `HdmiCecClose()`

NOTE: The module would operate deterministically if the above call sequence is followed.

### Diagrams

#### Operational Call Sequence

```mermaid
%%{ init : { "theme" : "default", "flowchart" : { "curve" : "stepBefore" }}}%%
   sequenceDiagram
    Caller->>HDMICECHAL:HdmiCecOpen
    Note over HDMICECHAL: SOC intialises and discovers <br> physical and logical address internally <br> based on device type and connection topology
    HDMICECHAL-->>Caller:return
    Caller->>HDMICECHAL:HdmiCecSetRxCallback
    HDMICECHAL-->>Caller:return
    Caller->>HDMICECHAL:HdmiCecSetTxCallback
    HDMICECHAL-->>Caller:return
    Caller ->>HDMICECHAL:HdmiCecGetLogicalAddress
    HDMICECHAL-->>Caller:return
    Caller ->>HDMICECHAL:HdmiCecGetPhysicalAddress
    HDMICECHAL-->>Caller:return
    Caller ->>HDMICECHAL:HdmiCecTx
    Note over Caller,HDMICECHAL: sync CEC transmit message
    HDMICECHAL-->>Caller:return
    Note over HDMICECHAL: For CEC message received from the remote device, HdmiCecSetRxCallback() will be triggered
    HDMICECHAL-->>Caller:HdmiCecRxCallback triggered
    Caller ->>HDMICECHAL:HdmiCecClose
    Note over Caller,HDMICECHAL: SOC De-initialises 
    HDMICECHAL-->>Caller:return
 ```

