# fischertechnik Training Factory Industry 4.0 (en)
This project contains the C/C++ library *TxtSmartFactoryLib* and the main (*TxtFactoryMain*) and client (*TxtFactoryClient*) programs
for the fischertechnik [**Training Factory Industry 4.0**](https://www.fischertechnik.de/en/service/elearning/teaching/lernfabrik-4). The library requires at least the TXT firmware version 4.5.0.

> [**fischertechnik Lernfabrik 4.0 (de)**](README_de.md)

## Overview
The factory consists of the following stations:
* **SSC**: Sensor Station with Camera
* **HBW**: High-Bay Warehouse
* **VGR**: Vacuum Gripper Robot
* **DPS**: Delivery and Pickup Station
* **MPO**: Multi-Processing Station with Oven
* **SLD**: Sorting Line with Color Detection

The next picture shows the network overview with the TXT controllers.
![Overview Network](doc/Overview_Network.PNG "Overview Network")

## Installation
The programs can be copied using the TXT [WEB server](doc/WEBServer.md).

## API Reference
The Doxygen documentation of the C / C ++ library classes can be found in the [API Reference](TxtSmartFactoryLib/doc/html/index.html).

## MQTT Interface
The [MQTT Interface](TxtSmartFactoryLib/doc/MqttInterface.md) describes the topics and the payload of the MQTT clients. 

Another local MQTT client can be added, taking note of the following parameters.
* **host** (default): 192.178.0.10
* **port** (default): 1883
* **user** (default): txt
* **password** (default): xtx
