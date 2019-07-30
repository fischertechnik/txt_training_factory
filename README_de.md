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
Die Programme können mit dem [WEB server](doc/WEBServer.md) auf den TXT Controller kopiert werden.

## API Reference
Die Doxygen-Dokumentation der C/C++ Bibliotheksklassen finden Sie in der [API-Referenz](https://fischertechnik.github.io/txt_training_factory/TxtSmartFactoryLib/doc/html/index.html).

## MQTT Interface
Die [MQTT Schnittstelle](TxtSmartFactoryLib/doc/MqttInterface.md)  beschreibt die Nutzdaten der MQTT-Clients.

Ein weiterer MQTT Client kann hinzugefügt werden, wobei folgende Parameter beachtet werden sollen.
* **Host** (default): 192.178.0.10
* **Port** (default): 1883
* **Benutzer** (default): txt
* **Passwort** (default): xtx
