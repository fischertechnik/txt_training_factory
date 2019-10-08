# fischertechnik Lernfabrik 4.0 (de)
Dieses Projekt enthält die C/C++ Bibliothek *TxtSmartFactoryLib* und die Haupt- (*TxtFactoryMain*) und Client- (*TxtFactoryClient*) Programme,
mit denen die fischertechnik [**Lernfabrik 4.0**](https://www.fischertechnik.de/de-de/service/elearning/lehren/lernfabrik-4) gesteuert werden kann. Für die Bibliothek wird mindestens die TXT Firmware version 4.5.0 benötigt.

## Übersicht 
Die Lernfabrik 4.0 besteht aus folgenden Stationen:
* **SSC**: Sensorstation mit Kamera
* **HBW**: Hochregallager
* **VGR**: Vakuumsaugroboter
* **DPS**: Ein- und Auslagerungsstation
* **MPO**: Multibearbeitungsstation mit Ofen
* **SLD**: Sortierstrecke mit Farberkennung

Das nächste Bild zeigt die Netzwerkübersicht mit den TXT Controllern.
![Netzwerkübersicht](doc/Overview_Network.PNG "Netzwerkübersicht")

## Installation
Die Programme können mit dem [WEB server](doc/WEBServer_de.md) auf den TXT Controller kopiert werden.

## API Reference
Die Doxygen-Dokumentation der C/C++ Bibliotheksklassen finden Sie in der [API-Referenz](https://fischertechnik.github.io/txt_training_factory_doc/html/index.html).

## MQTT Interface
Die [MQTT Schnittstelle](TxtSmartFactoryLib/doc/MqttInterface.md)  beschreibt die Nutzdaten der MQTT-Clients und die Konfiguration der mosquitto MQTT Bridge.

## Zustandsmaschinen
* [FSM MPO](https://fischertechnik.github.io/txt_training_factory_doc/html/dot_TxtMultiProcessingStationRun.png)
* [FSM HBW](https://fischertechnik.github.io/txt_training_factory_doc/html/dot_TxtHighBayWarehouseRun.png)
* [FSM VGR](https://fischertechnik.github.io/txt_training_factory_doc/html/dot_TxtVacuumGripperRobotRun.png)
* [FSM SLD](https://fischertechnik.github.io/txt_training_factory_doc/html/dot_TxtSortingLineRun.png)
