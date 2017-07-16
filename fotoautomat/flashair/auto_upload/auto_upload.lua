--LUA_RUN_SCRIPT=/lua/auto_upload.lua
--LUA_SD_EVENT=/lua/auto_upload.lua
--oder Aufruf des Scriptes über z.B. http://flashair/lua/auto_upload.lua?upload_all=1

-- Globale Variablen werden aus den cfg-Dateien und Parametern gelesen
-- Hier sind die Default-Werte
write_log = false -- log in eine log-Datei schreiben
print_log = false -- log auf Standard-Ausgabe
print_result = true -- result auf Standard-Ausgabe
ftp_uri = "" -- ftp://user:pass@host:port/dir
lastupload = "0000" -- kennzeichnet die letzte übertragene Datei
singlefile = true -- überträgt nur eine Datei und dann Ende, auch bei Fehler
upload_all = false -- überträgt alle Dateien unabhängig von lastupload

local function writeLog(str)
    -- schreibt eine Textzeile in eine Log-Datei
    if write_log then
        local wFile = io.open("/lua/auto_upload.log", "a")
        if (wFile == nil) then return end
        wFile:write(str.."\n")
        io.close(wFile)
    end
    if print_log then
        print(str)
    end
end

local function readConfigKeyValue(key,val)
    if (key == "lastupload") then
        lastupload = val
    elseif (key == "ftp_uri") then
        ftp_uri = val
    elseif (key == "write_log") then
        -- write_log=1
        write_log = tonumber(val)>0
    elseif (key == "print_log") then
        -- print_log=1
        print_log = tonumber(val)>0
    elseif (key == "print_result") then
        -- print_result=1
        print_result = tonumber(val)>0
    elseif (key == "singlefile") then
        -- singlefile=1
        singlefile = tonumber(val)>0
    elseif (key == "upload_all") then
        -- upload_all=1
        upload_all = tonumber(val)>0
    else
        if (print_result) then
            print ("ERROR unbekannter Parameter: "..key.."="..val)
        end
        writeLog("unbekannter Parameter: "..key.."="..val)
    end
end

local function readConfigLine(line)
    local s, e = line:find("=")
    if (s) then
        local key = line:sub(1, s - 1)
        local val = line:sub(e + 1)
        readConfigKeyValue(key,val)
    end
end

local function readConfig(file_name)
    -- liest Werte aus einer Konfigurationsdatei
    -- Die Zeilen müssen jeweils KEY=VALUE enthalten
    local f=io.open(file_name,"r")
    if f~=nil then
        io.input(f)
        repeat
            local line = io.read("*l")
            readConfigLine(line)
        until line:len()>0
        io.close(f)
    end
end

local function readArgs()
    -- liest die Kommandozeilenparameter ein
    -- Parameter werden auch am & getrennt, damit wird der Parameter vom http-request zerlegt
    for k,v in ipairs(arg) do
        if k>0 then
            while v:len()>0 do
                v = string.gsub(v,"\n","&")
                local s, e = v:find("&")
                if (s) then
                    local param = v:sub(1, s - 1)
                    readConfigLine(param)
                    v = v:sub(e + 1)
                else
                    readConfigLine(v)
                    v=""
                end
            end
        end
    end
end

local function writeLastupload(cfg_name)
    -- schreibt den Wert für die letzte Bilddatei in eine Datei
    local wFile = io.open(cfg_name, "w")
    if (wFile == nil) then return end
    wFile:write("lastupload="..lastupload.."\n")
    io.close(wFile)
end

local function autoUpload()
    local lastPhoto = 0
    local lastDirectory = 0
    if #lastupload > 0 then
        lastDirectory = tonumber(lastupload:sub(1, 3))
        lastPhoto = tonumber(lastupload:sub(4))
    end

    for aDirectory in lfs.dir("/DCIM") do
        local _dir_id = tonumber(aDirectory:sub(1, 3))
        if _dir_id ~= nil and _dir_id >= lastDirectory then
            for aFile in lfs.dir("/DCIM/"..aDirectory) do
                local filePath = "/DCIM/"..aDirectory.."/"..aFile
				writeLog("Teste "..filePath)
                -- Es muss eine Datei sein, aber nicht die FlashAir-Systemdatei
                if lfs.attributes(filePath, "mode") == "file" and aFile ~= "FA000001.JPG" then
                    -- verschiedene Dateinamensformate werden getestet und in eine Nummer umgewandelt
                    -- z.B. IMG_... für CANON
                    --      DSC ... für SONY
                    --      DSCF ... für FUJI
                    --      P ...
                    -- Es sollten eigentlich alle existierenden Varianten abgedeckt sein
                    -- zuerst Format mit einem Buchstaben testen
                    local photoNum = aFile:match("^[A-Z](%d+).JPE?G$")
                    if photoNum == nil then
                        -- Format mit 3 Zeichen testen
                        photoNum = aFile:match("^[A-Z0-9][A-Z0-9][A-Z0-9](%d+).JPE?G$")
                        if photoNum == nil then
                            -- Format mit 3 Zeichen und Unterstrich oder 4. Buchstaben testen
                            photoNum = aFile:match("^[A-Z0-9][A-Z0-9][A-Z0-9][_A-Z](%d+).JPE?G$")
                        end
                        if photoNum == nil then
                            writeLog("Dateiname "..aFile.." wurde nicht als Bild erkannt.")
                        end
                    end
                    --writeLog("photoNum="..photoNum)
                    if photoNum ~= nil and (upload_all or lastPhoto < tonumber(photoNum)) then
                        -- Dateinummer aus Verzeichnis und Nummer in der Datei zusammensetzen
                        local fileNum = aDirectory:sub(1, 3)..photoNum
                        -- als Zieldatei die lange Nummer verwenden
                        local destFile = "P"..fileNum..".JPG"
                        local res = fa.ftp("put",ftp_uri..destFile,filePath)
                        if res==1 then
                            if print_result then
                                print ("STORED "..destFile)
                            end
                            writeLog("FTP-Upload Erfolg "..ftp_uri..destFile.." "..filePath)
                            lastupload = fileNum
                            writeLastupload("/lua/lastupload.cfg")
                        else
                            if print_result then
                                print ("ERROR "..destFile)
                            end
                            writeLog("FTP-Upload Fehler "..ftp_uri..destFile.." "..filePath)
                            -- Bei FTP-Fehler eine 1 zurückliefern, damit wiederholt wird
                            return 1
                        end
                        if singlefile then
                            return 0
                        end
                    end
                end
            end
        end
    end
	return 0
end

local function waitWlanConnect()
    -- Wartet bis das WLAN verbunden ist
    while 1 do
        local res = fa.WlanLink()
        if res == 1 then
            break
        end
        sleep(2000)
    end
end

local function main()
    -- Main script
    collectgarbage("setstepmul", 24576)
    writeLog("start")
    waitWlanConnect()
    collectgarbage()
    writeLog("wlan-connected")

    while 1 do
      if autoUpload()==0 or singlefile then
        break
      end
      -- wiederholen bis fehlerfrei abgeschlossen
      writeLog("ftp-error repeat")
      sleep(10000)
      readConfig("/lua/lastupload.cfg")
    end
    writeLog("end")
end

readConfig("/lua/auto_upload.cfg")
readConfig("/lua/lastupload.cfg")
readArgs()
main()
