# HDMI CEC HAL Documentation

## Table of Contents

- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [References](#references)
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

- `HDMI`   - High-Definition Multimedia Interface
- `CEC`    - Consumer Electronics Control
- `HAL`    - Hardware Abstraction Layer
- `API`    - Application Programming Interface
- `Caller` - Any user of the interface via the `APIs`

## References

- `HDMI-CEC Specification` - High-Definition Multimedia Interface, Specification Version 1.4b, (https://www.hdmi.org/)
  - Downloadable via [this link](https://www.hdmi.org/spec/hdmi1_4b)
  - refer to Supplement 1 - Consumer Electronics Control (CEC) 

## Description

The diagram below describes a high-level software architecture of the module stack.

```mermaid
%%{ init : { "theme" : "forest", "flowchart" : { "curve" : "linear" }}}%%
flowchart TD
y[Caller]-->x[HDMI CEC HAL];
x[HDMI CEC HAL]-->z[HDMI CEC SOC Driver];
style y fill:#99CCFF,stroke:#333,stroke-width:0.3px,align:left
style z fill:#fcc,stroke:#333,stroke-width:0.3px,align:left
style x fill:#9f9,stroke:#333,stroke-width:0.3px,align:left
 ```

This interface provides a set of `APIs` to facilitate communication through the driver for `CEC` messages with other `CEC` devices connected with HDMI cable.

The interface retrieves and discovers logical and physical address of the host device, it is responsibile for transmitting and receiving messages with remote device(s) synchronously / asynchronously.

The `CEC` protocol responsibilities will lie between the `caller` and the `HAL`.

  - The `caller` must be responsible for `CEC` higher level protocol as defined in `HDMI-CEC Specification` Section `CEC 12`.
  - The `caller` must pass fully formed `CEC` messages to the `HAL` for the transmission.
  - The `HAL` must be responsible for physical device discovery and announcements on the `CEC` network as defined in the `HDMI-CEC Specification` Section `CEC 10`.
  - The driver layer is responsible for the physical layer as defined in the Section `CEC 4` (Electrical Specification) and Section `CEC 5` (Signalling and Bit Timings). The driver layer is out-of-scope for this document.

## Component Runtime Execution Requirements

`CEC` message transmit operation must complete within one second. Desired `CEC` response time is **200 milliseconds** and maximum response time must be **1 second** as provided in the `CEC` specifications (`HDMI-CEC Specification`). `Caller` is responsible to perform retry operations as per the `CEC` specification requirements. `Caller` will retry each transmission in line with a requirement as specified in Section `CEC 7.1` of the HMDI-CEC specification.

### Initialization and Startup

`Caller` must initialize by calling `HdmiCecOpen()` before calling any other `API`.

### Threading Model

This interface is not required to be thread safe. Any caller invoking the `APIs` must ensure calls are made in a thread safe manner.

### Process Model

This interface is required to support a single instantiation with a single process.

### Memory Model

For transmit messages, it is upto the caller to allocate and free the memory for the message buffer. For receive messages, the `HAL` is responsible for memory management. The memory allocated cannot exceed **20 bytes** (`HDMI-CEC Specification` Section `CEC 6`).

### Power Management Requirements

Although this interface is not required to be involved in any of the power management operations, the state transitions MUST not affect its operation. e.g. on resumption from a low power state, the interface must operate as if no transition has occurred.

### Asynchronous Notification Model

For asynchronous transmit and receive operations, the following `APIs` and callback registrations are used:

  1. For async transmit use: `HdmiCecTxAsync()`
  2. For async receive call back use: `HdmiCecSetRxCallback()`
  3. For async transmit ack use: `HdmiCecSetTxCallback()`

The caller is required to return the callback context as fast as possible.

### Blocking calls

There are no blocking calls. Synchronous calls must complete within a reasonable time period in accordance with any relevant `CEC` specification. Any call that can fail due to the lack of response from the connected device must have a timeout period in accordance with any relevant `CEC` specification and the function must return the relevant error code.

### Internal Error Handling

All the `APIs` must return error synchronously as a return argument. `HAL` is responsible for handling system errors (e.g. out of memory) internally.

### Persistence Model

There is no requirement for the interface to persist any setting information. `Caller` is responsible to persist any settings related to the `HAL`.

## Non-functional requirements

### Logging and debugging requirements

This interface is required to support DEBUG, INFO and ERROR messages. DEBUG and INFO must be disabled by default and enabled when required.

### Memory and performance requirements

This interface is required to not cause excessive memory and CPU utilization.

### Quality Control

- This interface is required to perform static analysis, our preferred tool is Coverity.
- Have a zero-warning policy with regards to compiling. All warnings are required to be treated as error.
- Copyright validation is required to be performed, e.g.: Black duck, FossID.
- Use of memory analysis tools like Valgrind are encouraged, to identify leaks/corruptions.
- `HAL` Tests will endeavour to create worst case scenarios to assist investigations.
- Improvements by any party to the testing suite are required to be fed back.

### Licensing

The `HAL` implementation is expected to released under the Apache License 2.0. 

### Build Requirements

The source code must build into a shared library and must be named as `libRCECHal.so`. The build mechanism must be independent of Yocto.
 
### Variability Management

Any changes in the `APIs` must be reviewed and approved by the component architects.

### Platform or Product Customization

None

## Interface API Documentation

`API` documentation will be provided by Doxygen which will be generated from the header files.

### Theory of operation and key concepts

The caller is expected to have complete control over the life cycle of the `HAL`.

1. Initialize the `HAL` using function: `HdmiCecOpen()` before making any other `API` calls. This call also discovers the physical address based on the connection topology. In case of source devices, `HdmiCecOpen()` must initiate the logical address discovery as part of this routine. In case of sink devices, logical address will be fixed and set using the `HdmiCecAddLogicalAddress()`. If `HdmiCecOpen()` call fails, the `HAL` must return the respective error code, so that the caller can retry the operation.

2. Once logical address and physical address are assigned, the caller will be able to send and receive the respective `CEC` messages.

  - For asynchronous receive operations, use the callback function: `HdmiCecSetRxCallback()`. the caller must register a callback after initialisation.

  - For synchronous transmit, use the function: `HdmiCecTx()`.

  - For asynchronous transmit, use the function: `HdmiCecTxAsync()`. The caller must register a callback via `HdmiCecSetTxCallback()` in order to receive the status or acknowledgement.

3.De-intialise the `HAL` using the function: `HdmiCecClose()`.

NOTE: The module would operate deterministically if the above call sequence is followed.

### Diagrams

#### Operational Call Sequence

```mermaid
%%{ init : { "theme" : "default", "flowchart" : { "curve" : "stepBefore" }}}%%
   sequenceDiagram
    participant Caller as Caller
    participant HAL as HDMI CEC HAL
    participant Driver as HDMI Device Control/Driver
    Caller->>HAL:HdmiCecOpen()
    Note over HAL: SOC intialises and discovers <br> physical address based on the connection topology <br> source devices discovers logical address internally  based on device type.
    HAL->>Driver: soc intialises
    Driver-->>HAL:return
    HAL-->>Caller:return
    Caller->>HAL:HdmiCecSetRxCallback()
    HAL->>Driver: Setting a receiver callback in soc
    Driver-->>HAL:return
    HAL-->>Caller:return
    Caller->>HAL:HdmiCecSetTxCallback()
    HAL->>Driver:Setting a transmit callback in soc
    Driver-->>HAL:return
    HAL-->>Caller:return
    Caller ->>HAL:HdmiCecAddLogicalAddress()
    Note over Caller: Sink devices must set the logical address using the API <br> HdmiCecAddLogicalAddress (Only for sink devices)
    HAL->>Driver: add logical address in soc
    Driver-->>HAL:return
    HAL-->>Caller:return
    Caller ->>HAL:HdmiCecGetLogicalAddress()
    HAL->>Driver:Get logical address from soc
    Driver-->>HAL:return
    HAL-->>Caller:return
    Caller ->>HAL:HdmiCecGetPhysicalAddress()
    HAL->>Driver:Get physical address in soc
    Driver-->>HAL:return
    HAL-->>Caller:return
    Caller ->>HAL:HdmiCecTx()
    Note over Caller,HAL: sync CEC transmit message
    HAL->>Driver: Transmit packet in soc side
    Driver-->>HAL:return
    HAL-->>Caller:return
    Driver-->>HAL:RxCallback()
    Note over HAL: For CEC message received from the remote device, HdmiCecSetRxCallback() will be triggered
    HAL-->>Caller:HdmiCecRxCallback() triggered
    Caller ->>HAL:HdmiCecClose()
    Note over Caller,HAL: SOC Un-initialises 
    HAL->>Driver: Soc Un-initialises
    Driver-->>HAL:return
    HAL-->>Caller:return
 ```
