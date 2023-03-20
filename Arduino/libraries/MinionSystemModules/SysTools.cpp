#define US_KEYBOARD 1
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>

#include <SysTools.h>

namespace SysTools {
 
char* _defaultMap = "settings";
char  _currentMap[32] = "";
Preferences _memMap;

void sendCommand(const char* pCommand) {
  if(Serial.availableForWrite() == 0) return;

    Serial.printf("\r\n%s\r\n\n", pCommand);
}

void addLog(const char* format, ...) {
    
    va_list argptr;
    va_start(argptr, format);
    printf("%s", "LOG: ");
    //vsprintf(logBuffer, format, argptr);
    vprintf(format, argptr);
    printf("%s", "\r\n");
    va_end(argptr);
    
};

void getOptions(const char* pMemKey, DynamicJsonDocument& pOptions) {
  const char* memKey;
  const char* memValue;
  DeserializationError jsonErr;
  //DynamicJsonDocument messageObj(128);
  
    memValue = const_cast<char*>(_memMap.getString(pMemKey, "{}\0").c_str());
    jsonErr = deserializeJson(pOptions, memValue);
        
    if(jsonErr) {
        SysTools::addLog("SysTools::getOptions Abort: %s", jsonErr.c_str());
        return;
    }
}

void setMemMap(const char* pMapName) {
    if(strcmp(pMapName, _currentMap) == 0) return;
    _memMap.begin(pMapName, false);
    strcpy(_currentMap, pMapName);
}

void lookupKeyWord(DynamicJsonDocument& pMessageObj, DynamicJsonDocument& pDeviceObj) { 
  SysTools::addLog("SysTools::lookupKeyWord: '%s'", (const char*)pMessageObj["required"]["keyWord"]);

  JsonObject keyOptions = pMessageObj["optional"].as<JsonObject>();
  const char* keyOption;
  const char* memKey;
  const char* memMap;
  String memValue;
  String memDefault = String("{\"keyUsage\": \"\", \"keyModifier\": 0, \"keyCode\": 0, \"keyWord\": \"\", \"keyData\": \"\", keyDuration: 0, keyDelay: 0}");
    
    if(pMessageObj["required"]["keyWord"] == nullptr) {
        SysTools::addLog("%s", "SysTools::lookupKeyWord Abort: 'keyWord' value missing in message");
        return;
    }
    
    if(pMessageObj["optional"]["keyMap"] == nullptr) pMessageObj["optional"]["keyMap"] = _defaultMap;

    memKey = (const char*)pMessageObj["required"]["keyWord"];
    memMap = (const char*)pMessageObj["optional"]["keyMap"];
    
    setMemMap(memMap);
    memValue = _memMap.getString(memKey, memDefault);
    SysTools::addLog("SysTools::lookupKeyWord memKey: %s, memMap: %s, memValue: %s", memKey, memMap, memValue.c_str());
    
    DeserializationError jsonErr = deserializeJson(pDeviceObj, memValue.c_str());
    
    if(jsonErr) {
        SysTools::addLog("SysTools::lookupKeyWord Abort: %s", jsonErr.c_str());
        return;
    }
    
    //Override keyOptions from pMessageObj
    for (JsonPair kv : keyOptions) {
        keyOption = kv.key().c_str();
        SysTools::addLog("keyOption: %s", keyOption);
        pDeviceObj[keyOption] = pMessageObj["optional"][keyOption];
    }
    
    SysTools::addLog("SysTools::lookupKeyWord: deviceOptions: %s %i %i, %i, %i", (const char*)pDeviceObj["keyWord"], (int)pDeviceObj["keyCode"], (int)pDeviceObj["keyModifier"], (int)pDeviceObj["keyDuration"], (int)pDeviceObj["keyDelay"]);
};

void receivedCommand(const char* pMessage) {
  addLog("SysTools::receivedCommand Message: %s", pMessage);
  DynamicJsonDocument messageObj(512);
  const char* memKey;
  const char* memMap;
  String memString;
      
    if(memcmp(pMessage, "SETOPTIONS:", 11) != 0) {
        SysTools::addLog("%s", "SysTools::receivedCommand Abort: Invalid Command.");
        return;
    };
    
    DeserializationError err = deserializeJson(messageObj, pMessage + 11);
        
    if(err) {
        SysTools::addLog("SysTools::receivedCommand Abort: %s", err.c_str());
        return;
    }

    if(messageObj["memKey"] == nullptr) {
        SysTools::addLog("%s", "SysTools::receivedCommand Abort: 'memKey' value missing in message");
        return;
    }

    if(messageObj["memMap"] == nullptr) messageObj["memMap"] = _defaultMap;
    
    memKey = (const char*)messageObj["memKey"];
    memMap = (const char*)messageObj["memMap"];
    
    char memValue[512];
    memset (memValue, 0, 512);
    JsonObject memValueObj = messageObj["memValue"].as<JsonObject>();
    serializeJson(memValueObj, memValue);
    
    addLog("SysTools::receivedCommand memKey: %s, memMap: %s, memValue: %s", memKey, memMap, memValue);
    
    setMemMap(memMap);
    _memMap.putString(memKey, memValue);
    memString = _memMap.getString(memKey);
    sendCommand("NOTIFY:{\"Command\":\"SETOPTIONS\", \"State\":\"Done\"}");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void dumpBuffer(const uint8_t* pBuffer, uint32_t pBufferSize, uint32_t pByteCount) {
  SysTools::addLog("\nSysTools::dumpBuffer, pBufferSize: %u, pByteCount: %u", pBufferSize, pByteCount);
    
    //Print First Few Buffer Bytes
    uint8_t lineCount = 0;
    
    Serial.printf("%s", "First: ");
    for(int i=0; i<pByteCount; ++i) {
        ++lineCount;
        if(lineCount > 10) {
            lineCount = 0;
            Serial.println("");
        }
        
        Serial.printf("0x%02X(%1c)", (pBuffer+i)[0], (pBuffer+i)[0]);
        if(i < (pByteCount - 1)) Serial.printf(",");
    }
     
    Serial.println("\n===========================================================================================================================================");
    
    //Print Last Few Buffer Bytes
    //SysTools::addLog("SysPartitionDisk::dumpBuffer, Last %i Bytes of %u Total", pByteCount, pBufferSize);
    
    lineCount = 0;
    
    Serial.printf("%s", " Last: ");
    for(int i=(pBufferSize - pByteCount); i<pBufferSize; ++i) {
        ++lineCount;
        if(lineCount > 10) {
            lineCount = 0;
            Serial.println("");
        }
        
        Serial.printf("0x%02X(%1c)", (pBuffer+i)[0], (pBuffer+i)[0]);
        if(i < (pBufferSize - 1)) Serial.printf(",");
    }
        
    Serial.println("\n");
}
}