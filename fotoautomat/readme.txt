Der Fotodrucker besteht aus 4 Komponenten:

1. Kamera mit FlashAir SD-Karte
- Das Programm auto_upload.lua l�dt auf Anforderung die Fotos auf einen FTP-Server
Installation: 
- WLAN auf der Karte konfigurieren
- Die Dateien aus dem flashair-Verzeichnis auf die Karte spielen

2. Raspberry-Pi
- Stellt den FTP-Server f�r den Upload der Fotos zur Verf�gung
- Durch das Programm camera_download.pl wird der Upload gesteuert.
- camera_printer.py hat eine Reihe von Aufgaben:
* beobachtet das Upload-Verzeichnis
* zeigt die Fotos und Statusmeldungen auf dem Display an
* �ber einen Button der an einen GPIO angeschlossen wird, kann der Druck ausgel�st werden
* Kann auch �ber einige Tastaturbefehle bedient werden
* F�r das Drucken wird das Foto per SMB auf einen anderen Rechner �bertragen
Installation:
- WLAN konfigurieren
- FTP-Server konfigurieren
- OpenCV 2 installieren (ggf. weitere fehlende Pakete)
- ggf. Programm zum Drucken im Quellcode anpassen
- camera_download.pl als Dienst ausf�hren
- camera_printer.py auf einem Display ausf�hren

3. Druckprogramm auf Windows-PC
- Da das Drucken von Fotos unter Windows einfacher zu realisieren war habe ich mich daf�r entschieden
- Ein kleines VBScript-Programm l�uft als Job und Druckt Fotos die in einem Verzeichnis auftauchen.
Installation:
- IrfanView installieren und Druckeinstellungen dort vornehmen
- Freigabe erstellen auf ein leeres Verzeichnis C:\printpic\print
- printjob.vbs ausf�hren
