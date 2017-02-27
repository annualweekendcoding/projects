--LUA_RUN_SCRIPT=/lua/auto_upload.lua
--LUA_SD_EVENT=/lua/auto_upload.lua
--oder Aufruf des Scriptes über z.B. http://flashair/lua/auto_upload.lua?upload_all=1

-- Globale Variablen werden aus den cfg-Dateien und Parametern gelesen
-- Hier sind die Default-Werte
write_log = false -- log in eine log-Datei schreiben
print_log = true -- log auf Standard-Ausgabe
ftp_uri = "" -- ftp://user:pass@host:port/dir
lastupload = "" -- kennzeichnet die letzte übertragene Datei
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
    elseif (key == "singlefile") then
        -- singlefile=1
        singlefile = tonumber(val)>0
    elseif (key == "upload_all") then
        -- upload_all=1
        upload_all = tonumber(val)>0
    else
        writeLog("unbekannter Parameter: "..key.."="..val.."\n")
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

local function writeLastupload(cfg_name, _dir, _file)
    -- schreibt den Wert für die letzte Bilddatei in eine Datei
    local wFile = io.open(cfg_name, "w")
    if (wFile == nil) then return end

    lastupload = _dir:sub(1, 3).._file:sub(5, 8)
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
                if lfs.attributes(filePath, "mode") ~= "file" then
                    goto continue
                end
                if aFile:sub(-4) ~= ".JPG" then
                    -- alles was keine JPG-Dateien sind übergehen
				    goto continue
                end				
                if not upload_all then
                    local photoNum = tonumber(aFile:sub(5, 8))
                    if lastPhoto >= photoNum then
                        -- zu kleine Fotonummer übergehen
                        goto continue
                    else
                        lastPhoto = photoNum
                    end
                end
                local res = fa.ftp("put",ftp_uri..aFile,filePath)
                if res==1 then
					writeLog("FTP-Upload Erfolg "..ftp_uri..aFile.." "..filePath)
                    writeLastupload("/lua/lastupload.cfg", aDirectory, aFile)
				else
					writeLog("FTP-Upload Fehler "..ftp_uri..aFile.." "..filePath)
				    -- Bei FTP-Fehler eine 1 zurückliefern, damit wiederholt wird
                    return 1
                end
                if singlefile then
                    return 0
                end

                ::continue::
            end
            lastPhoto = 0
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

--readConfig("/daten/annualweekendcoding/projects/flashair/auto_upload/auto_upload.cfg")
readConfig("/lua/auto_upload.cfg")
readConfig("/lua/lastupload.cfg")
readArgs()
main()

--[[
-- Hauptprogramm mit locking

-- Lockfile erzeugen
--writeLog("lock")
local lock, message = lfs.mkdir("/lua/lock")
if lock then
    writeLog("lock ok")
    -- nur wenn Lock erfolgreich, ausführen
    local status, err = pcall(main())
    -- Lock wieder freigeben
    lfs.rmdir("/lua/lock")
    if status then
        writeLog("Exception: "..err)
    end
else
    writeLog("Lock Failed")
end
--]]