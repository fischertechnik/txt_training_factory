[TOC]

Introduction
=============
The C/C++ library *TxtSmartFactoryLib* can be used to control the "Training Factory Industry 4.0" fischertechnik models. Current sources can be found at [Github](https://www.github.com/fischertechnik).

The factory contains 6 TXT controllers. The pin assignment for these TXT controllers is described in the following [section](PinAssignmentTXT.md).

The MQTT interface for the TXT controllers is described in the following [section](MqttInterface.md)

# Training Model Stations
The factory consists of the following stations:
- **SSC**: Sensor Station with Camera
- **HBW**: High-Bay Warehouse
- **VGR**: Vacuum Gripper Robot
- **DPS**: Delivery and Pickup Station
- **MPO**: Multi-Processing Station with Oven
- **SLD**: Sorting Line with Color Detection

<!--
Kommentar

# Example

```cpp
...
ft::TxtSortingLine sortLine = new ft::TxtSortingLine(pTArea);
sortLine.startThread();
...
```
-->
