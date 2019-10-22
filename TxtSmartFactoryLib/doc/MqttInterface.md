MQTT Interface
==============

This document describes the MQTT interface of the local MQTT broker, the MQTT bridge and the remote MQTT broker.

# MQTT Bridge / MQTT Remote Broker

## fischertechnik Cloud
If you use the fischertechnik Cloud [https://www.fischertechnik-cloud.com](https://www.fischertechnik-cloud.com), then you have to pair your TXT controller by executing
> *Settings* -> *Network* -> *Cloud Setup* -> *Pairing New*.

## Other Clouds
If you want to connect your own remote MQTT broker, you can configure the MQTT bridge by creating/editing of the file `/etc/mosquitto/ft-txt-bridge-cloud.conf`.
> **Attention**: For this you need root access rights!

Content of `ft-txt-bridge-cloud.conf` (see documentation [https://mosquitto.org/](https://mosquitto.org/)):
```
connection YourOwnCloudName
address www.domain.com:8883
bridge_capath /etc/ssl/certs
notifications false
cleansession false #on connection dropping
remote_username YourOwnCloudMQTTUser
remote_password YourOwnCloudMQTTPassword
local_username txt
local_password xtx
topic i/# both 1 "" prefix_mqtt_remote_broker/
topic o/# both 1 "" prefix_mqtt_remote_broker/
topic c/# both 1 "" prefix_mqtt_remote_broker/
topic f/# both 1 "" prefix_mqtt_remote_broker/
try_private false
bridge_attempt_unsubscribe false
```

# MQTT Interface Dashboard
| Component SUBSCRIBE            | topic              | payload                      |
| ------------------------------:|--------------------|------------------------------|
| Environment Sensor             | **i/bme680**       | see *TxtFactoryMain PUBLISH* |               
| Brightness Sensor              | **i/ldr**          | see *TxtFactoryMain PUBLISH* |               
| Camera Picture                 | **i/cam**          | see *TxtFactoryMain PUBLISH* |               
| Pos Pan-Tilt-Unit              | **i/ptu/pos**      | see *TxtFactoryMain PUBLISH* |               
| Alert Message                  | **i/alert**        | see *TxtFactoryMain PUBLISH* |               
| Broadcast                      | **i/broadcast**    | see *TxtFactoryMain PUBLISH* |               
| State HBW                      | **f/i/state/hbw**  | see *TxtFactoryHBW PUBLISH*  |               
| State VGR                      | **f/i/state/vgr**  | see *TxtFactoryVGR PUBLISH*  |               
| State MPO                      | **f/i/state/mpo**  | see *TxtFactoryMPO PUBLISH*  |               
| State SLD                      | **f/i/state/sld**  | see *TxtFactorySLD PUBLISH*  |              
| State DSI (VGR)                | **f/i/state/dsi**  | see *TxtFactoryDSI PUBLISH*  |               
| State DSO (VGR)                | **f/i/state/dso**  | see *TxtFactoryDSO PUBLISH*  |               
| Stock HBW                      | **f/i/stock**      | see *TxtFactoryHBW PUBLISH*  |               
| State Order(VGR)               | **f/i/order**      | see *TxtFactoryVGR PUBLISH*  |                
| State NFC Device (VGR)         | **f/i/nfc/ds**     | see *TxtFactoryVGR PUBLISH*  |               

| Component PUBLISH              | topic              | payload                        |
| ------------------------------:|--------------------|--------------------------------|
| TXT Pairing Ack                | **c/link**         | internal usage                 |               
| Config Rate Environment Sensor | **c/bme680**       | see *TxtFactoryMain SUBSCRIBE* |               
| Config Rate Brightness Sensor  | **c/ldr**          | see *TxtFactoryMain SUBSCRIBE* |             
| Config Rate Camera Picture     | **c/cam**          | see *TxtFactoryMain SUBSCRIBE* |            
| Control Buttons Pan-Tilt-Unit  | **o/ptu**          | see *TxtFactoryMain SUBSCRIBE* |             
| Quit Button                    | **f/o/state/ack**  | see *TxtFactoryVGR SUBSCRIBE* |              
| Order Workpiece Buttons        | **f/o/order**      | see *TxtFactoryVGR SUBSCRIBE* |              
| Action Buttons NFC Module      | **f/o/nfc/ds**     | see *TxtFactoryVGR SUBSCRIBE* |           

# MQTT Interface Local Clients
Another local MQTT client can be added, taking note of the following parameters:
* **host** (default): 192.168.0.10
* **port** (default): 1883
* **user** (default): txt
* **password** (default): xtx

| abbreviation | payload example            | description                    |
|-------------:|----------------------------|--------------------------------|
| **ts**       | YYYY-MM-DDThh:mm:ss.fffZ   |time stamp according to ISO8601: year:YYYY, month:MM, day:DD, hour:hh, minute:mm, second:ss, fraction:fff |

## TxtFactoryMain
| Component SUBSCRIBE            | topic              | payload  | description   |
| ------------------------------:|--------------------|----------|---------------|
| TXT Pairing Ack                | **c/link**         |
| Config Rate Environment Sensor | **c/bme680**       |
| Config Rate Brightness Sensor  | **c/ldr**          |
| Config Rate Camera Picture     | **c/cam**          |
| Control Buttons Pan-Tilt-Unit  | **o/ptu**          |
| State HBW                      | **f/i/state/hbw**  |
| State VGR                      | **f/i/state/vgr**  |
| State MPO                      | **f/i/state/mpo**  | 
| State SLD                      | **f/i/state/sld**  | 
| State DSI (VGR)                | **f/i/state/dsi**  | 
| State DSO (VGR)                | **f/i/state/dso**  | 

| Component PUBLISH              | topic              | payload  | description   |
| ------------------------------:|--------------------|----------|---------------|
| Environment Sensor             | **i/bme680**       |`{ "ts":"YYYY-MM-DDThh:mm:ss.fffZ", "t":25.1, "rt":25.01, "h":40.0, "rh":38.01, "p":1000.15, "iaq":200, "aq":3, "gr":161000 }` | **t**/**rt**: temperature compensated / raw value [°C], **h**/ **rh**: relative humidity compensated / raw value [%], **p**: air pressure [hPa], **iaq**: index air quality 0-500 (0...50:*Good*, 51...100:*Moderate*, 101...150:*Unhealthy for Sensitive Groups*, 151...200:*Unhealthy*, 201...300:*Very Unhealthy*, 301...500:*Hazardous*), **aq**: air quality score 0-3 (0:IAQ invalid, 1:calibration necessary, 2:calibration done, 3:IAQ is calibrated), **gr**: gas resistance [Ohm] |
| Brightness Sensor              | **i/ldr**          |`{ "ts":"YYYY-MM-DDThh:mm:ss.fffZ", "br":100.0, "ldr":15000 }` | **br**: brightness 0-100.0 [%], **ldr**: value resistance 0-15000 [Ohm] |
| Camera Picture                 | **i/cam**          |`{ "ts":"YYYY-MM-DDThh:mm:ss.fffZ", "data":"data:image/jpeg;base64,<...>" }` | **data**: camera image as base64 string |
| Pos Pan-Tilt-Unit              | **i/ptu/pos**      |`{ "ts":"YYYY-MM-DDThh:mm:ss.fffZ", "pan":0.5, "tilt":-0.5}` | **pan**: relative position pan: -1.000...0.000...1.000, **tilt**: relative position tilt: -1.000...0.000...1.000 |
| Alert Message                  | **i/alert**        |`{ "ts":"YYYY-MM-DDThh:mm:ss.fffZ", "id":"bme680/t", "data":"data:image/jpeg;base64,<...>", "code":100 }` | **id**: bme680/t=temperature, bme680/h=humidity, bme680/p=pressure, bme680/iaq=air quality, ldr=brightness/photo resistor, cam=camera, **data**: sensor value / camera image as string, **code**: 100=Alarm: Movement detected!, 200=Alarm: danger of frost! temperature < 4.0 °C, 300=Alarm: Hohe Luftfeuchtigkeit! humidity > 80% |
| Broadcast                      | **i/broadcast**    | internal usage | |
| Joysticks                      | **fl/ssc/joy**     |                | |

## TxtFactoryMPO
| Component SUBSCRIBE            | topic              | payload                      | description   |
| ------------------------------:|--------------------|------------------------------|---------------|
| Quit Button                    | **f/o/state/ack**  |  |
| VGR Trigger                    | **fl/vgr/do**      | see *TxtFactoryVGR PUBLISH*  |
| Acknowledgment SLD             | **fl/sld/ack**     | see *TxtFactorySLD PUBLISH*  |

| Component PUBLISH              | topic              | payload  | description   |
| ------------------------------:|--------------------|----------|---------------|
| State MPO                      | **f/i/state/mpo**  | `{"ts":"YYYY-MM-DDThh:mm:ss.fffZ", "station":"mpo", "code":0, "description":"text", "active":1, "target":""}` |
| Acknowledgment MPO             | **fl/mpo/ack**     | `{"ts":"YYYY-MM-DDThh:mm:ss.fffZ", "code":0 }` | **code**: 0=MPO_EXIT, 1=MPO_STARTED, 2=MPO_PRODUCED |

## TxtFactoryHBW
| Component SUBSCRIBE            | topic              | payload                      | description   |
| ------------------------------:|--------------------|------------------------------|---------------|
| Quit Button                    | **f/o/state/ack**  | |
| Joysticks                      | **fl/ssc/joy**     | see *TxtFactoryMain PUBLISH* |
| VGR Trigger                    | **fl/vgr/do**      | see *TxtFactoryVGR PUBLISH*  |

| Component PUBLISH              | topic              | payload  | description   |
| ------------------------------:|--------------------|----------|---------------|
| State HBW                      | **f/i/state/hbw**  | `{"ts":"YYYY-MM-DDThh:mm:ss.fffZ", "station":"hbw", "code":0, "description":"text", "active":1, "target":""}` |
| Stock HBW                      | **f/i/stock**      | `{"ts":"YYYY-MM-DDThh:mm:ss.fffZ", "stockItems": [{ "workpiece": { "id":"123456789ABCDE", "type":"<BLUE/WHITE/RED>", "state":"<RAW/PROCESSED>" }, "location":"A1" },{ ... },{ "workpiece":null, "location":"B3" }] }` |
| Acknowledgment HBW             | **fl/hbw/ack**     | `{"ts":"YYYY-MM-DDThh:mm:ss.fffZ", "code":0, "workpiece":{...} }` | **code**: 0=HBW_EXIT, 1=HBW_FETCHED, 2=HBW_STORED, 3=HBW_CALIB_NAV, 4=HBW_CALIB_END |

## TxtFactoryVGR
| Component SUBSCRIBE            | topic              | payload                      | description               |
| ------------------------------:|--------------------|------------------------------|---------------------------|
| Quit Button                    | **f/o/state/ack**  | `{"ts":"YYYY-MM-DDThh:mm:ss.fffZ"}` | |
| Order Workpiece Buttons        | **f/o/order**      | `{"ts":"YYYY-MM-DDThh:mm:ss.fffZ", "type":"BLUE"}` | **type**: BLUE, WHITE, RED
| Action Buttons NFC Module      | **f/o/nfc/ds**     | `{"ts":"YYYY-MM-DDThh:mm:ss.fffZ", "cmd":"read"}` | **cmd**: delete, read |
| Joysticks                      | **fl/ssc/joy**     | see *TxtFactoryMain PUBLISH* |
| Acknowledgment MPO             | **fl/mpo/ack**     | see *TxtFactoryMPO PUBLISH*  |
| Acknowledgment HBW             | **fl/hbw/ack**     | see *TxtFactoryHBW PUBLISH*  |
| Acknowledgment SLD             | **fl/sld/ack**     | see *TxtFactorySLD PUBLISH*  |

| Component PUBLISH              | topic              | payload  | description   |
| ------------------------------:|--------------------|----------|---------------|
| State VGR                      | **f/i/state/vgr**  | `{"ts":"YYYY-MM-DDThh:mm:ss.fffZ", "station":"vgr", "code":0, "description":"text", "active":1, "target":"hbw"}` | |
| State DSI (VGR)                | **f/i/state/dsi**  | `{"ts":"YYYY-MM-DDThh:mm:ss.fffZ", "station":"dsi", "code":0, "description":"text", "active":1}` | |
| State DSO (VGR)                | **f/i/state/dso**  | `{"ts":"YYYY-MM-DDThh:mm:ss.fffZ", "station":"dso", "code":0, "description":"text", "active":1}` | |
| VGR Trigger                    | **fl/vgr/do**      | `{"ts":"YYYY-MM-DDThh:mm:ss.fffZ", "code":0, "workpiece":{...} }`| 	**code**: 0=VGR_EXIT, 1=VGR_HBW_FETCHCONTAINER, 2=VGR_HBW_STORE_WP, 3=VGR_HBW_FETCH_WP, 4=VGR_HBW_STORECONTAINER, 5=VGR_HBW_RESETSTORAGE, 6=VGR_HBW_CALIB, 7=VGR_MPO_PRODUCE, 8=VGR_SLD_START |

## TxtFactorySLD
| Component SUBSCRIBE            | topic              | payload                     | description   |
| ------------------------------:|--------------------|-----------------------------|---------------|
| Quit Button                    | **f/o/state/ack**  |
| Acknowledgment MPO             | **fl/mpo/ack**     | see *TxtFactoryMPO PUBLISH* |

| Component PUBLISH              | topic              | payload  | description   |
| ------------------------------:|--------------------|----------|---------------|
| State SLD                      | **f/i/state/sld**  | `{"ts":"YYYY-MM-DDThh:mm:ss.fffZ", "station":"sld", "code":0, "description":"text", "active":1, "target":"hbw"}` |
| Acknowledgment SLD             | **fl/sld/ack**     | `{"ts":"YYYY-MM-DDThh:mm:ss.fffZ", "code":0, "type":<>, "colorValue":<> }` | **code**: 0=SLD_EXIT, 1=SLD_STARTED, 2=SLD_SORTED |
