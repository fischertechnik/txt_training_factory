MQTT Interface
==============
| abbreviation | payload example            | description                    |
|-------------:|----------------------------|--------------------------------|
| **ts**       | YYYY-MM-DDThh:mm:ss.fffZ   |time stamp according to ISO8601: year:YYYY, month:MM, day:DD, hour:hh, minute:mm, second:ss, fraction:fff |

# MQTT Remote (fischertechnik Cloud)

## fischertechnik Cloud / Dashboard
| Component SUBSCRIBE            | topic              | payload                      | description   |
| ------------------------------:|--------------------|------------------------------|---------------|
| Environment Sensor             | **i/bme680**       | see *TxtFactoryMain PUBLISH* |               |
| Brightness Sensor              | **i/ldr**          | see *TxtFactoryMain PUBLISH* |               |
| Camera Picture                 | **i/cam**          | see *TxtFactoryMain PUBLISH* |               |
| Pos Pan-Tilt-Unit              | **i/ptu/pos**      | see *TxtFactoryMain PUBLISH* |               |
| Alert Message                  | **i/alert**        | see *TxtFactoryMain PUBLISH* |               |
| Broadcast                      | **i/broadcast**    | see *TxtFactoryMain PUBLISH* |               |
| State HBW                      | **f/i/state/hbw**  | see *TxtFactoryHBW PUBLISH*  |               |
| State VGR                      | **f/i/state/vgr**  | see *TxtFactoryVGR PUBLISH*  |               |
| State MPO                      | **f/i/state/mpo**  | see *TxtFactoryMPO PUBLISH*  |               |
| State SLD                      | **f/i/state/sld**  | see *TxtFactorySLD PUBLISH*  |               | 
| State DSI (VGR)                | **f/i/state/dsi**  | see *TxtFactoryDSI PUBLISH*  |               | 
| State DSO (VGR)                | **f/i/state/dso**  | see *TxtFactoryDSO PUBLISH*  |               | 
| Stock HBW                      | **f/i/stock**      | see *TxtFactoryHBW PUBLISH*  |               | 
| State Order(VGR)               | **f/i/order**      | see *TxtFactoryVGR PUBLISH*  |               | 
| State NFC Device (VGR)         | **f/i/nfc/ds**     | see *TxtFactoryVGR PUBLISH*  |               | 

| Component PUBLISH              | topic              | payload                      | description   |
| ------------------------------:|--------------------|------------------------------|---------------|
| TXT Pairing Ack                | **c/link**         | internal usage               |               |
| Config Rate Environment Sensor | **c/bme680**       |                              |               |
| Config Rate Brightness Sensor  | **c/ldr**          |                              |               |
| Config Rate Camera Picture     | **c/cam**          |                              |               |
| Control Buttons Pan-Tilt-Unit  | **o/ptu**          |                              |               |
| Quit Button                    | **f/o/state/ack**  |                              |               |
| Order Workpiece Buttons        | **f/o/order**      |                              |               |
| Action Buttons NFC Module      | **f/o/nfc/ds**     |                              |               |

# MQTT Programs

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
| Component SUBSCRIBE            | topic              | payload  | description   |
| ------------------------------:|--------------------|----------|---------------|
| Quit Button                    | **f/o/state/ack**  |
| VGR Trigger                    | **fl/vgr/do**      |
| Acknowledgment SLD             | **fl/sld/ack**     |

| Component PUBLISH              | topic              | payload  | description   |
| ------------------------------:|--------------------|----------|---------------|
| State MPO                      | **f/i/state/mpo**  | 
| Acknowledgment MPO             | **fl/mpo/ack**     |

## TxtFactoryHBW
| Component SUBSCRIBE            | topic              | payload  | description   |
| ------------------------------:|--------------------|----------|---------------|
| Quit Button                    | **f/o/state/ack**  |
| Joysticks                      | **fl/ssc/joy**     | 
| VGR Trigger                    | **fl/vgr/do**      |

| Component PUBLISH              | topic              | payload  | description   |
| ------------------------------:|--------------------|----------|---------------|
| State HBW                      | **f/i/state/hbw**  | 
| Stock HBW                      | **f/i/stock**      | 
| Acknowledgment HBW             | **fl/hbw/ack**     |

## TxtFactoryVGR
| Component SUBSCRIBE            | topic              | payload  | description   |
| ------------------------------:|--------------------|----------|---------------|
| Quit Button                    | **f/o/state/ack**  |
| Order Workpiece Buttons        | **f/o/order**      |
| Action Buttons NFC Module      | **f/o/nfc/ds**     |
| Joysticks                      | **fl/ssc/joy**     | 
| Acknowledgment MPO             | **fl/mpo/ack**     |
| Acknowledgment HBW             | **fl/hbw/ack**     |
| Acknowledgment SLD             | **fl/sld/ack**     |

| Component PUBLISH              | topic              | payload  | description   |
| ------------------------------:|--------------------|----------|---------------|
| State VGR                      | **f/i/state/vgr**  |
| State DSI (VGR)                | **f/i/state/dsi**  | 
| State DSO (VGR)                | **f/i/state/dso**  | 
| VGR Trigger                    | **fl/vgr/do**      |

## TxtFactorySLD
| Component SUBSCRIBE            | topic              | payload  | description   |
| ------------------------------:|--------------------|----------|---------------|
| Quit Button                    | **f/o/state/ack**  |
| Acknowledgment MPO             | **fl/mpo/ack**     |

| Component PUBLISH              | topic              | payload  | description   |
| ------------------------------:|--------------------|----------|---------------|
| State SLD                      | **f/i/state/sld**  | 
| Acknowledgment SLD             | **fl/sld/ack**     |
