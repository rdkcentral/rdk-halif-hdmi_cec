@mainpage

# HDMI CEC HAL Documentation

## Version History

| Date | Author | Comment | Version |
| --- | --------- | --- | --- |
| 13/03/23 | Review Team | Edit  | 1.0.1 |
| 06/12/22| Amit Patel | First Release | 1.0.0 |

## Table of Contents

- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Description](#description)
- [Component Runtime Execution Requirements](#component-runtime-execution-requirements)
  - [Initialization and Startup](#initialization-and-startup)
  - [Threading Model](#threading-model)
  - [Process Model](#process-model)
  - [Memory Model](#memory-model)
  - [Power Management Requirements](#power-management-requirements)
  - [Asynchronous Notification Model](#asynchronous-notification-model)
  - [Blocking calls](#blocking-calls)
  - [Internal Error Handling](#internal-error-handling)
  - [Persistence Model](#persistence-model)
- [Non-functional requirements](#non-functional-requirements)
  - [Logging and debugging requirements](#logging-and-debugging-requirements)
  - [Memory and performance requirements](#memory-and-performance-requirements)
  - [Quality Control](#quality-control)
  - [Licensing](#licensing)
  - [Build Requirements](#build-requirements)
  - [Variability Management](#variability-management)
  - [Platform or Product Customization](#platform-or-product-customization)
- [Interface API Documentation](#interface-api-documentation)
  - [Theory of operation and key concepts](#theory-of-operation-and-key-concepts)
  - [Diagrams](#diagrams)

## Acronyms, Terms and Abbreviations

- `HDMI` - High-Definition Multimedia Interface
- `CEC`  - Consumer Electronics Control
- `HAL`  - Hardware Abstraction Layer
- `API`  - Application Programming Interface

## Description

The diagram below describes a high-level software architecture of the `HDMI` `CEC` module stack.

```mermaid
%%{ init : { "theme" : "forest", "flowchart" : { "curve" : "linear" }}}%%
flowchart TD
y[Caller]-->x[HDMI CEC HAL];
x[HDMI CEC HAL]-->z[HDMI CEC SOC Driver];
style y fill:#99CCFF,stroke:#333,stroke-width:0.3px,align:left
style z fill:#fcc,stroke:#333,stroke-width:0.3px,align:left
style x fill:#9f9,stroke:#333,stroke-width:0.3px,align:left

 ```

`HDMI` `CEC` HAL provides a set of `APIs` to facilitate communication through the driver for `CEC` messages with other `CEC` devices connected with HDMI cable.

The interface retrieves and discovers logical and physical address of the host device, it is responsibile for transmitting and receiving messages with remote device(s) synchronously / asynchronously.

## Component Runtime Execution Requirements

`CEC` message transmit operation should complete within one second. Desired `CEC` response time is **250 milliseconds** and maximum response time should be **1 second** as provided in the `CEC` specifications. Caller is responsible to perform retry operations as per the `CEC` specification requirements.

### Initialization and Startup

Caller should initialize by calling `HdmiCecOpen()` before calling any other `API`.

### Threading Model

This interface is not required to be thread safe. Any caller invoking the `APIs` should ensure calls are made in a thread safe manner.

### Process Model

This interface is required to support a single instantiation with a single process.

### Memory Model

For transmit messages, it is upto the caller to allocate and free the memory for the message buffer. For receive messages, the `HAL` is responsible for memory management. The memory allocated cannot exceed **20 bytes**.

### Power Management Requirements

Although this interface is not required to be involved in any of the power management operations, the state transitions MUST not affect its operation. e.g. on resumption from a low power state, the `HDMI` `CEC` should operate as if no transition has occurred.

### Asynchronous Notification Model

For asynchronous transmit and receive operations, the following `APIs` and callback registrations are used:

  1. For async transmit use: `HdmiCecTxAsync()`
  2. For async receive call back use: `HdmiCecSetRxCallback()`
  3. For async transmit ack use: `HdmiCecSetTxCallback()`

The caller is required to return the callback context as fast as possible.

### Blocking calls

There are no blocking calls. Synchronous calls should complete within a reasonable time period in accordance with any relevant `CEC` specification. Any call that can fail due to the lack of response from the connected device should have a timeout period in accordance with any relevant `CEC` specification and the function should return the relevant error code.

### Internal Error Handling

All the `APIs` must return error synchronously as a return argument. `HAL` is responsible for handling system errors (e.g. out of memory) internally.

### Persistence Model

There is no requirement for the interface to persist any setting information. Caller is responsible to persist any settings related to `HDMI` `CEC` feature.

## Non-functional requirements

### Logging and debugging requirements

This interface is required to support DEBUG, INFO and ERROR messages. DEBUG should be disabled by default and enabled when required.

### Memory and performance requirements

This interface is required to not cause excessive memory and CPU utilization.

### Quality Control

- This interface is required to perform static analysis, our preferred tool is Coverity.
- Open-source copyright validation is required to be performed, e.g.: Black duck, FossID.
- Have a zero-warning policy with regards to compiling. All warnings are required to be treated as errors.
- Use of memory analysis tools like Valgrind are encouraged, to identify leaks/corruptions.
- Tests will endeavour to create worst case scenarios to assist investigations.

### Licensing

The `HAL` implementation is expected to released under the Apache License 2.0. 

### Build Requirements

`The source code must build into a shared library and must be named as `libRCECHal.so`.
 
### Variability Management

Any changes in the `APIs` should be reviewed and approved by the component architects.

### Platform or Product Customization

None

## Interface API Documentation

`API` documentation will be provided by Doxygen which will be generated from the header files.

### Theory of operation and key concepts

The caller is expected to have complete control over the life cycle of the `HDMI` `CEC` `HAL`.

1. Initialize the `HAL` using function: `HdmiCecOpen()` before making any other `API` calls. This call also discovers the physical address based on the connection topology. In case of source devices, `HdmiCecOpen()` should initiate the logical address discovery as part of this routine. In case of sink devices, logical address will be fixed and set using the `HdmiCecAddLogicalAddress()`. If `HdmiCecOpen()` call fails, the `HAL` must return the respective error code, so that the caller can retry the operation.

2. Once logical address and physical address are assigned, the caller will be able to send and receive the respective `CEC` messages.

  - For asynchronous receive operations, use the callback function: `HdmiCecSetRxCallback()`. the caller must register a callback after initialisation.

  - For synchronous transmit, use the function: `HdmiCecTx()`.

  - For asynchronous transmit, use the function: `HdmiCecTxAsync()`. The caller must register a callback via `HdmiCecSetTxCallback()` in order to receive the status or acknowledgement.

3. De-intialise the `HAL` using the function: `HdmiCecClose()`

NOTE: The module would operate deterministically if the above call sequence is followed.

### Diagrams

#### Operational Call Sequence

```mermaid
%%{ init : { "theme" : "default", "flowchart" : { "curve" : "stepBefore" }}}%%
   sequenceDiagram
    participant HAL as HDMI CEC HAL
    Caller->>HAL:HdmiCecOpen()
    Note over HAL: SOC intialises and discovers <br> physical and logical address internally <br> based on device type and connection topology
    HAL-->>Caller:return
    Caller->>HAL:HdmiCecSetRxCallback()
    HAL-->>Caller:return
    Caller->>HAL:HdmiCecSetTxCallback()
    HAL-->>Caller:return
    Caller ->>HAL:HdmiCecGetLogicalAddress()
    HAL-->>Caller:return
    Caller ->>HAL:HdmiCecGetPhysicalAddress()
    HAL-->>Caller:return
    Caller ->>HAL:HdmiCecTx()
    Note over Caller,HAL: sync CEC transmit message
    HAL-->>Caller:return
    Note over HAL: For CEC message received from the remote device, HdmiCecSetRxCallback() will be triggered
    HAL-->>Caller:HdmiCecRxCallback() triggered
    Caller ->>HAL:HdmiCecClose()
    Note over Caller,HAL: SOC De-initialises 
    HAL-->>Caller:return
 ```

