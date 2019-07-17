MQTT Interface
==============

# MQTT Remote (fischertechnik Cloud)

## fischertechnik Cloud / Dashboard
| Component                      | topic SUBSCRIBE    |
| ------------------------------:|--------------------|
| Environment Sensor             | **i/bme680**       |
| Brightness Sensor              | **i/ldr**          |
| Camera Picture                 | **i/cam**          |
| Pos Pan-Tilt-Unit              | **i/ptu/pos**      |
| Alert Message                  | **i/alert**        |
| Broadcast                      | **i/broadcast**    |
| State HBW                      | **f/i/state/hbw**  |
| State VGR                      | **f/i/state/vgr**  |
| State MPO                      | **f/i/state/mpo**  | 
| State SLD                      | **f/i/state/sld**  | 
| State DSI (VGR)                | **f/i/state/dsi**  | 
| State DSO (VGR)                | **f/i/state/dso**  | 
| Stock HBW                      | **f/i/stock**      | 
| State Order(VGR)               | **f/i/order**      | 
| State NFC Device (VGR)         | **f/i/nfc/ds**     | 

| Component                      | topic PUBLISH      |
| ------------------------------:|--------------------|
| TXT Pairing Ack                | **c/link**         |
| Config Rate Environment Sensor | **c/bme680**       |
| Config Rate Brightness Sensor  | **c/ldr**          |
| Config Rate Camera Picture     | **c/cam**          |
| Control Buttons Pan-Tilt-Unit  | **o/ptu**          |
| Quit Button                    | **f/o/state/ack**  |
| Order Workpiece Buttons        | **f/o/order**      |
| Action Buttons NFC Module      | **f/o/nfc/ds**     |

# MQTT Programs

## TxtFactoryMain
| Component                      | topic SUBSCRIBE    |
| ------------------------------:|--------------------|
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

| Components                     | topic PUBLISH      |
| ------------------------------:|--------------------|
| Environment Sensor             | **i/bme680**       |
| Brightness Sensor              | **i/ldr**          |
| Camera Picture                 | **i/cam**          |
| Pos Pan-Tilt-Unit              | **i/ptu/pos**      |
| Alert Message                  | **i/alert**        |
| Broadcast                      | **i/broadcast**    |
| Joysticks                      | **fl/ssc/joy**     | 

## TxtFactoryMPO
| Components                     | topic SUBSCRIBE    |
| ------------------------------:|--------------------|
| Quit Button                    | **f/o/state/ack**  |
| VGR Trigger                    | **fl/vgr/do**      |
| Acknowledgment SLD             | **fl/sld/ack**     |

| Component                      | topic PUBLISH      |
| ------------------------------:|--------------------|
| State MPO                      | **f/i/state/mpo**  | 
| Acknowledgment MPO             | **fl/mpo/ack**     |

## TxtFactoryHBW
| Component                      | topic SUBSCRIBE    |
| ------------------------------:|--------------------|
| Quit Button                    | **f/o/state/ack**  |
| Joysticks                      | **fl/ssc/joy**     | 
| VGR Trigger                    | **fl/vgr/do**      |

| Component                      | topic PUBLISH      |
| ------------------------------:|--------------------|
| State HBW                      | **f/i/state/hbw**  | 
| Acknowledgment HBW             | **fl/hbw/ack**     |

## TxtFactoryVGR
| Component                      | topic SUBSCRIBE    |
| ------------------------------:|--------------------|
| Quit Button                    | **f/o/state/ack**  |
| Order Workpiece Buttons        | **f/o/order**      |
| Action Buttons NFC Module      | **f/o/nfc/ds**     |
| Joysticks                      | **fl/ssc/joy**     | 
| Acknowledgment MPO             | **fl/mpo/ack**     |
| Acknowledgment HBW             | **fl/hbw/ack**     |
| Acknowledgment SLD             | **fl/sld/ack**     |

| Component                      | topic PUBLISH      |
| ------------------------------:|--------------------|
| State VGR                      | **f/i/state/vgr**  |
| State DSI (VGR)                | **f/i/state/dsi**  | 
| State DSO (VGR)                | **f/i/state/dso**  | 
| VGR Trigger                    | **fl/vgr/do**      |

## TxtFactorySLD
| Component                      | topic SUBSCRIBE    |
| ------------------------------:|--------------------|
| Quit Button                    | **f/o/state/ack**  |
| Acknowledgment MPO             | **fl/mpo/ack**     |

| Component                      | topic PUBLISH      |
| ------------------------------:|--------------------|
| State SLD                      | **f/i/state/sld**  | 
| Acknowledgment SLD             | **fl/sld/ack**     |
