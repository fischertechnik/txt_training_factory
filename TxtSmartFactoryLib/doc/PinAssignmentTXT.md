Pin Assignment TXT controllers
==============================

# TXT 1: Sensor Station with Camera (SSC)
| TXT-IO      | Function              |
| -----------:|-----------------------|
| I1          | Reference Switch Pan  |
| I2          | Reference Switch Tilt |
| I3          | Photoresistor         |
| I4          | -                     |
| I5          | Joystick X Front      |
| I6          | Joystick Y Front      |
| I7          | Joystick X Back       |
| I8          | Joystick Y Back       |
| C1          | Encoder Pan           |
| C2          | Encoder Tilt          |
| C3          | Joystick B Front      |
| C4          | Joystick B Back       |
| **M1**      | Motor Pan             |
| **M2**      | Motor Tilt            |
| **O5**      | LED camera red        |
| **O6**      | LED lights red        |
| **O7**      | LED lights yellow     |
| **O8**      | LED lights green      |
| *I2C*       | *Environmental Sensor*|

# TXT 2,3: Multi-Processing Station with Oven (MPO)
| TXT-IO                | Function                                      |
| ---------------------:|-----------------------------------------------|
| I1 (master)           | Reference switch turn-table (position vacuum) |
| I2 (master)           | Reference switch turn-table (position saw)    |
| I3 (master)           | Reference switch turn-table (position belt)   |
| I4 (master)           | Phototransistor end of conveyor belt          |
| I5 (master)           | Reference switch vacuum (position turn-table) |
| I6...I8 (master)      | -                                             |
| C1...C4 (master)      | -                                             |
| **M1** (master)       | Motor turn-table                              |
| **M2** (master)       | Motor saw                                     |
| **M3** (master)       | Motor conveyor belt                           |
| **O7** (master)       | Valve ejection                                |
| **O8** (master)       | Compressor                                    |
| *EXT (master)*        | *TXT Extension*                               |

| TXT-IO                | Function                                      |
| ---------------------:|-----------------------------------------------|
| I1 (extension)        | Reference switch oven feeder retract          |
| I2 (extension)        | Reference switch oven feeder inside extend    |
| I3 (extension)        | Reference switch vacuum (position oven)       |
| I4 (extension)        | -                                             |
| I5 (extension)        | Phototransistor                               |
| I6...I8 (extension)   | -                                             |
| C1...C4 (extension)   | -                                             |
| **M1** (extension)    | Motor oven feeder                             |
| **M2** (extension)    | Motor vacuum                                  |
| **O5** (extension)    | Valve vacuum                                  |
| **O6** (extension)    | Valve lowering                                |
| **O7** (extension)    | Valve oven door                               |
| **O8** (extension)    | Light oven                                    |
| *EXT (extension)*     | *TXT Master*                                  |

# TXT 4: High-Bay Warehouse (HBW)
| TXT-IO      | Function                            |
| -----------:|-------------------------------------|
| I1          | Phototransistor Outside             |
| I2...I3     | -                                   |
| I4          | Phototransistor Inside              |
| I5          | Reference Switch horizontal axis    |
| I6          | Reference Switch cantilever retract |
| I7          | Reference Switch cantilever extend  |
| I8          | Reference Switch vertical axis      |
| C1          | -                                   |
| C2          | Encoder horizontal axis             |
| C3          | -                                   |
| C4          | Encoder vertical axis               |
| **M1**      | Motor conveyor belt                 |
| **M2**      | Motor horizontal axis               |
| **M3**      | Motor cantilever                    |
| **M4**      | Motor vertical axis                 |

# TXT 5: Vacuum Gripper Robot (VGR), Delivery and Pickup Station (DPS)
| TXT-IO      | Function                         |
| -----------:|----------------------------------|
| I1          | Reference switch rotate          |
| I2          | Reference switch vertical axis   |
| I3          | Reference switch horizontal axis |
| I4...I6     | -                                |
| I7          | Phototransistor Delivery (DPS)   |
| I8          | Color Sensor (DPS)               |
| C1          | Encoder rotate                   |
| C2          | Encoder vertical axis            |
| C3          | Encoder horizontal axis          |
| C4          | Phototransistor Pickup (DPS)     |
| **M1**      | Motor rotate                     |
| **M2**      | Motor vertical axis              |
| **M3**      | Motor horizontal axis            |
| **O7**      | Compressor                       |
| **O8**      | Valve                            |
| *I2C*       | *NFC/RFID Module (DPS)*          |

# TXT 6: Sorting Line with Color Detection (SLD)
| TXT-IO      | Function                     |
| -----------:|------------------------------|
| I1          | Phototransistor Color Sensor |
| I2          | Color Sensor                 |
| I3          | Phototransistor Ejection     |
| I4...I5     | -                            |
| I6          | Phototransistor White        |
| I7          | Phototransistor Red          |
| I8          | Phototransistor Blue         |
| C1          | Pulse counter                |
| C2...C4     | -                            |
| **M1**      | Motor Conveyor Belt          |
| **M2**      | -                            |
| **O5**      | Valve Ejection White         |
| **O6**      | Valve Ejection Red           |
| **O7**      | Valve Ejection Blue          |
| **O8**      | Compressor                   |
