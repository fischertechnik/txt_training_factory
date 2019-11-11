# Factory Software Update
This How-To describes the steps how to upgrade the software of the fischertechnik model *Training Factory Industry 4.0*.

> **!!! Caution !!!**: Upgrading the TXT controllers and the C programs of the Training Factory Industry 4.0 model takes about 1-2 hours. Please follow the instructions step by step. If you have any questions, please contact: fischertechnik-technik@fischer.de

## 1. Backup
Backup the calibration and configuration files of all 5 master TXT controllers in `/opt/knobloch/Data` directories. You can use the TXT [WEB server](WEBServer.md) to copy the files. *Alternatively*: Copy calibration files from the delivered USB stick.

## 2. Update TXT Firmware
Upgrade the TXT controller firmware of all 5 master TXT controllers **via ROBO Pro software**. Install [ROBO Pro 4.6.6](https://github.com/fischertechnik/FT-TXT/releases/download/v4.6.6/ROBOPro466.msi) and connect each TXT controller with the computer via USB cable. Start ROBO Pro, select TXT controller and USB interface and open the Test-Interface window. Be patient, the TXT firmware update takes about 5-10 minutes per TXT controller. DO NOT interrupt the power supply, because otherwise the controller is defective and needs to be repaired. All the files on the TXT controller will be overwritten after this step.

## 3. TXT Settings
Change TXT settings:
  - **Role**: TXT0: Cloud Device, TXT1,2,3,4: Master, TXT1b: Extension
  - **Security settings**: Enable WEB Server and SSH Daemon
  - **Network settings**: disable Bluetooth, activate WLAN Client, setup [network WLAN settings](Network_Config.md) for the corresponding TXT
  - **Connect Nano Router** to the 5 master TXT controllers TXT0-4

## 4. Deploy C Programs
Copy C programs via the TXT [WEB server](WEBServer.md) and set *AutoLoad* for the default c program

## 5. Restore
Restore calibration and configuration files using the [WEB server](WEBServer.md) from step 1.

## 6. Power Off and On
Switch off and on all the TXT controllers in the training model.

## 7. Start and Check
Start the factory. Please check if the calibration of the training model is correct.


# Hints
- The version of the TXT firmware can be found on the TXT controller in the menue `Settings -> info`
- The version of the ROBO Pro can be found in menue `Help -> About...`
