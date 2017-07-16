' Script wartet auf Dateien im Ordner strFolder
' Dateien werden dann mit IrfanView gedruckt
' Datei mit Namen stop.* beendet das Script

strFolder = "C:\printpic\print"
strIview = "C:\Program Files (x86)\IrfanView\i_view32.exe"

Set objFSO = CreateObject("Scripting.FileSystemObject") 
Set objShell  = CreateObject("Shell.Application")
Set objWshShell = WScript.CreateObject("WScript.Shell")

' Create an object for the specified folder
Set objFolder = objShell.Namespace(strFolder)

bStop = false
bPrint = false

While not bStop
  ' Create a collection for the folder's contents
  Set colFiles  = objFolder.Items

  ' Loop through the collection to find the file specified
  If colFiles.Count > 0 Then
    For Each objFile In colFiles
      'WScript.Echo objFile.Name
      'WScript.Echo objFile.Path
      If objFile.Name = "stop" Then
        bStop = true
      Else
	    If not FileInUse(objFile.Path) Then
          bPrint = true
          ' Datei mit IrfanView drucken
	      ' Vorher dort die Druckeinstellungen vornehmen
          CMDRun = AddQuotes(strIview) & " " & AddQuotes(objFile.Path) & " /print /silent"
          objWshShell.Run CMDRun, 2, true
          ' WScript.Echo objFile.Path
          ' delete file
          objFSO.DeleteFile objFile.Path
		Else
          ' WScript.Echo "Gesperrt: " & objFile.Path		
        End If
      End If
    Next
  End If
  If bPrint Then
    'Wenn gedruckt wurde, dann ggf. noch offene IrfanViews killen
    CMDRun = AddQuotes(strIview) & " /killmesoftly /silent"
    objWshShell.Run CMDRun, 2, true
    bPrint = false
  End If
  WScript.Sleep 1000
Wend

WScript.Echo "printjob gestoppt."
 
Function AddQuotes(s)
  AddQuotes = Chr(34) & s & Chr(34)
End Function

' Prüfen, ob Datei in Benutzung
Function FileInUse(sFile)
  On Error Resume Next
  ' Versuch, Datei EXKLUSIV zu öffnen
  Set f = objFSO.OpenTextFile(sFile,8,true)
  ' Bei Fehler 70 ist die Datei in Benutzung
  FileInUse = (Err.Number = 70)
  f.close
  On Error Goto 0
End Function
 