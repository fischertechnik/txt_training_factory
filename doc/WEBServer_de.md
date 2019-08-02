# TXT WEB Server
Die TXT Controller Firmware Version >=4.5.0 enthält einen integrierten WEB server, der dazu verwendet werden kann, Programme und Dateien auf den TXT hoch- und runterzuladen. Auf diese Weise können z.B. C-Programme vom Computer auf den TXT Controller kopiert werden und dann über die Benutzeroberfläche des TXT Controllers gestartet werden.

Falls der WEB Server noch nicht eingeschaltet ist, muss dieser zunächst im TXT Menü (*Einstellungen –> (Pfeil nach unten) –> Sicherheit -> WEB Server*) aktiviert werden. Bei eingeschaltetem WEB Server wird in der oberen TXT Statusleiste ein "ws" Symbol angezeigt.

![Settings WEBServer](Settings_WEBServer0.png) ![Settings WEBServer](Settings_WEBServer1.png) ![Settings WEBServer](Settings_WEBServer2.png) ![Settings WEBServer](Settings_WEBServer_Off.png) ![Statusbar WS](Settings_WEBServer_On.png)

Der TXT Controller wird über *USB / WLAN / Bluetooth* mit dem Computer verbunden.

Die WEB Seite des TXT Controllers kann aufgerufen werden, wenn im WEB Browser (Chrome oder Firefox wird empfohlen) die IP-Adresse eingegeben wird. Dabei muss je nach Schnittstelle eine andere IP-Adresse eingegeben werden.

* **USB**: 192.168.7.2
* **WLAN AP**: 192.168.8.2
* **Bluetooth**: 192.168.9.2
* **WLAN Client**: X.X.X.X

![WEBBrowser IP](WEBBrowser_IP.png)

Die Seite des TXT Controllers ist durch ein Passwort gesichert.

* **Benutzer**: *TXT*
* **Passwort**: Vierstellige Nummer des TXT Controllers, die in der oberen    Statusleiste am TXT Display angezeigt wird (z. B. *2427* wenn "TXT-2427" angezeigt wird)

Folgende Abfrage des Passworts wird im WEB Browser angezeigt:

![WEBBrowser Pass](WEBBrowser_Pass_de.png)
 
Nach Eingabe des Passworts sollte jetzt die Seite des TXT Controllers sichtbar sein.

![TXT WEB Site](TXT_WEB_Site.png)

Um C-Programme auf den TXT Controller zu kopieren, wechseln Sie in das Verzeichnis “C-Program”, indem Sie dieses mit der Maus anklicken. In diesem Verzeichnis werden C-Programme auf dem TXT Controller gespeichert.

Dateien können über das Symbol Papierkorb gelöscht oder über das Symbol **+** hinzugefügt werden.

Wenn Sie auf **+** klicken, öffnet sich ein Dialogfenster, in dem Sie mit “Upload files” Dateien auf dem Computer auswählen und mit “Add Files” hinzufügen können. Wenn eine oder mehrere Dateien ausgewählt wurden, werden diese mit "Finish" auf den TXT Controller hochgeladen. 

![WEBServer +](WEBServer_p.png) ![WEBServer Add](WEBServer_Add.png) ![WEBServer Upload](WEBServer_Upload.png) ![WEBServer Finish](WEBServer_Finish.png)

## C-Programme - Lernfabrik 4.0
Aktuelle C-Programme der Lernfabrik 4.0 finden Sie auf [GitHub](https://github.com/fischertechnik/txt_training_factory/tree/master/bin). Speichern Sie die Programme auf dem Computer und laden Sie die entsprechenden C-Programme wie oben bereits beschrieben auf den jeweiligen TXT Controller herunter. Dabei wird pro TXT Controller in der Lernfabrik 4.0 eine eigene WEB Seite geöffnet. Die Cloud-Hauptanwendung wird in den Ordner "Cloud", die restlichen C-Programme werden in den Ordner "C-Program" kopiert.

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

Das C-Programm wird beim Einschalten automatisch geladen, wenn "Auto Load" aktiviert ist. Das Programm wird mit "Load" wie beim TXT Controller üblich geladen und kann dann über den grünen Button gestartet werden.

![TXT Autoload](TXT_Autoload0.png)
![TXT Autoload](TXT_Autoload1.png)
![TXT Autoload](TXT_Autoload2.png)

 Wenn das Programm gestartet wurde, so wird der Button rot, mit dem das Programm wieder gestoppt werden kann.
 
![TXT Start](TXT_Start.png) ![TXT Stop](TXT_Stop.png)
