# TXT WEB Server
The TXT controller firmware version> = 4.5.0 contains an integrated WEB server that can be used to upload and download programs and files to the TXT. In this way, e.g. C programs are copied from the computer to the TXT controller and then started via the user interface of the TXT controller.

If the WEB server is not yet switched on, it must first be activated in the TXT menu (*Settings* -> *(down arrow)* -> *Security* -> *WEB Server*). When the WEB server is switched on, a "ws" symbol is displayed in the upper TXT status bar.

![Settings WEBServer](Settings_WEBServer0.png) ![Settings WEBServer](Settings_WEBServer1.png) ![Settings WEBServer](Settings_WEBServer2.png) ![Settings WEBServer](Settings_WEBServer_Off.png) ![Statusbar WS](Settings_WEBServer_On.png)

The TXT controller can be connected to the computer via *USB* / *WLAN* / *Bluetooth*.

The WEB page of the TXT controller can be accessed when the IP address is entered in the WEB browser (*Chrome* or *Firefox* are recommended). Depending on the interface, a different IP address must be entered.

* **USB**: 192.168.7.2
* **WLAN AP**: 192.168.8.2
* **Bluetooth**: 192.168.9.2
* **WLAN Client**: X.X.X.X

![WEBBrowser IP](WEBBrowser_IP.png)

The page of the TXT controller is password protected.

* **User**: *TXT*
* **Password**: Four-digit number of the TXT controller displayed in the upper status bar on the TXT display (e.g. *6892* if "TXT-6892" is displayed)

The following query of the password is displayed in the WEB browser:

![WEBBrowser Pass](WEBBrowser_Pass.png)

After entering the password, the page of the TXT controller should now be visible.

![TXT WEB Site](TXT_WEB_Site.png)

To copy C programs to the TXT controller, change to the directory "C-Program" by clicking on it with the mouse. In this directory C programs are stored on the TXT controller.

Files can be deleted using the Recycle Bin icon or added via the **+** icon.

If you click on **+**, a dialog box opens in which you can select files on the computer with "Upload files" and add them with "Add Files". If one or more files have been selected, they are uploaded to the TXT Controller with "Finish".

![WEBServer +](WEBServer_p.png) ![WEBServer Add](WEBServer_Add.png) ![WEBServer Upload](WEBServer_Upload.png) ![WEBServer Finish](WEBServer_Finish.png)

## C Programs - Training Factory Industry 4.0
Current TXT Training Factory Industry 4.0 C programs can be found on [GitHub](https://github.com/fischertechnik/txt_training_factory/tree/master/bin). Save the programs on the computer and download the appropriate C programs to the respective TXT controller as described above. Each TXT controller in TXT Training Factory Industry 4.0 opens its own WEB page. The cloud main application is copied to the "Cloud" folder, the rest of the C programs are copied to the "C-Program" folder.

* **Main**:

![WEBServer Main Prog](WEBServer_Main_Prog.png) ![TXT Main Prog](TXT_Main_Prog.png)

![WEBServer SSC Prog](WEBServer_SSC_Prog.png) ![TXT SSC Prog](TXT_SSC_Prog.png)

* **MPO**:

![WEBServer MPO Prog](WEBServer_MPO_Prog.png) ![TXT MPO Prog](TXT_MPO_Prog.png)

* **HBW**:

![WEBServer HBW Prog](WEBServer_HBW_Prog.png) ![TXT HBW Prog](TXT_HBW_Prog.png)

* **VGR**:

![WEBServer VGR Prog](WEBServer_VGR_Prog.png) ![TXT VGR Prog](TXT_VGR_Prog.png)

* **SLD**:

![WEBServer SLD Prog](WEBServer_SLD_Prog.png) ![TXT SLD Prog](TXT_SLD_Prog.png)

The C program is automatically loaded at power up when "Auto Load" is enabled. The program is loaded with "Load" as usual with the TXT controller and can then be started via the green button.

![TXT Autoload](TXT_Autoload0.png)
![TXT Autoload](TXT_Autoload1.png)
![TXT Autoload](TXT_Autoload2.png)

If the program is started, the button will change to red. The program can be stopped again with the red button.

![TXT Start](TXT_Start.png) ![TXT Stop](TXT_Stop.png)
