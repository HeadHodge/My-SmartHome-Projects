#define US_KEYBOARD 1
#include <SPI.h>
#include <TFT_eSPI.h>       // Hardware-specific library

#include <SysTools.h>

namespace SysTools {
#define _currentTime millis()
TFT_eSPI _tft = TFT_eSPI();  // Invoke custom library
uint32_t _displayOffTime = 0; //millisecs
bool displayEnabled = false;

///////////////////////////////////////////////////////////////////
char* createPkgString(DynamicJsonDocument* pPkg) {
///////////////////////////////////////////////////////////////////
  SysTools::addLog("SysTools::createPkgString");
  uint16_t stringLength = measureJson(*pPkg) + 1;
  char* pkgString = (char*)malloc(stringLength);
    
    serializeJson(*pPkg, pkgString, stringLength);
    return pkgString;
}

///////////////////////////////////////////////////////////////////
DynamicJsonDocument* createPkg(char* pJsonString) {
///////////////////////////////////////////////////////////////////
  SysTools::addLog("SysTools::createPkg");
  DynamicJsonDocument* pkg = new DynamicJsonDocument(strlen(pJsonString) * 2);

    DeserializationError err = deserializeJson(*pkg, (const char*)pJsonString);
    
    if (err) {
        SysTools::addLog("SysTools::createPkg, ABORT: Invalid Pkg, Reason: %s", err.c_str());
        free(pkg);
        return nullptr;
    };
    
    pkg->shrinkToFit();
    SysTools::addLog("wsEndpoints::onPkgInput, pJsonString: %s, \nmemoryUsage: '%lu', capacity: '%lu'", pJsonString, pkg->memoryUsage(), pkg->capacity());
    return pkg;
}

///////////////////////////////////////////////////////////////////
DynamicJsonDocument* getOptionsObj(char* pFileName, char* pDirectory) {
///////////////////////////////////////////////////////////////////
    FILE *fp = NULL;
    char filePath[128];
    DynamicJsonDocument* optionsObj = nullptr;

    sprintf(filePath, "%s/%s", pDirectory, pFileName);
    fp = fopen(filePath,"r");
    
    if(fp == NULL) {
        SysTools::addLog("SysTools::getOptionsObj, ABORT: fopen failed: %s", filePath);
        return nullptr;
    };
    
    fseek(fp, 0, SEEK_END);     // seek to end of file
    uint32_t fsize = ftell(fp); // get current file pointer
    fseek(fp, 0, SEEK_SET);     // seek back to beginning of file
    char* optionsStr = (char*)malloc(fsize+1);
    memset(optionsStr, 0, fsize+1);
    uint16_t bytesRead = fread(optionsStr, 1, fsize, fp);
    fclose(fp);
    
    if(bytesRead != fsize) {
        SysTools::addLog("SysTools::getOptionsObj, ABORT: Incorrect fread Bytes Read: %lu", bytesRead);
        return nullptr;
    };
    
    SysTools::addLog("SysTools::getOptionsObj, optionsObj optionsStr: %s", optionsStr);
    optionsObj = new DynamicJsonDocument(fsize*2);
    DeserializationError deserializeErr = deserializeJson(*optionsObj, (const char*)optionsStr);
    free(optionsStr);
   
    if(deserializeErr) {
        SysTools::addLog("SysTools::getOptionsObj, ABORT: Deserialize File Failed, Reason: %s", deserializeErr.c_str());
        return nullptr;
    };
     
    optionsObj->shrinkToFit();
    SysTools::addLog("SysTools::getOptionsObj, optionsStr: %s, \nmemoryUsage: '%lu', capacity: '%lu'", optionsStr, optionsObj->memoryUsage(), optionsObj->capacity());

    return optionsObj;
}
    
///////////////////////////////////////////////////////////////////
bool saveOptionsObj(DynamicJsonDocument* pOptionsObject, char* pFileName, char* pDirectory) {
///////////////////////////////////////////////////////////////////
  SysTools::addLog("SysTools::saveOptionsObj, pDirectory: %s, pFileName: %s", pDirectory, pFileName);
  char optionsStr[1024];
  char filePath[128];
  
    if(serializeJson(*pOptionsObject, optionsStr) == 0){
        SysTools::addLog("saveOptionsObj::saveOptionsObj, ABORT: Serialize OptionsObj Failed");
        return false;
    };

    mkdir(pDirectory, 0777);

    FILE *fp = NULL;
    sprintf(filePath, "%s/%s", pDirectory, pFileName);
    fp = fopen(filePath,"w");
    
    if(fp == NULL) {
        SysTools::addLog("SysTools::saveOptionsObj, ABORT: fopen failed");
        return false;
    };
    
    fprintf(fp, "%s", optionsStr);
    fclose(fp);
  
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void dumpBuffer(const uint8_t* pBuffer, uint32_t pBufferSize, uint32_t pByteCount) {
///////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////
void enableDisplay() {
///////////////////////////////////////////////////////////
  SysTools::addLog("SysSpiDisplay::enableDisplay");

    //Init Display
    if(displayEnabled) return;
    
    _tft.init();
    displayEnabled = true;
}
///////////////////////////////////////////////////////////
void displayStationConnection() {
///////////////////////////////////////////////////////////
  SysTools::addLog("SysSpiDisplay::displayStationConnection, time: %lu", millis());

    enableDisplay();

    //Set the font colour
    _tft.fillScreen(TFT_GREEN);
    _tft.setTextColor(TFT_BLACK, TFT_GREEN);
    _tft.setTextPadding(1);
  
    _tft.setTextSize(3);
    _tft.drawCentreString("smartRemotes", 120, 0, 2);  //_tft.println("Total Wifi Remote Control");

    _tft.setTextSize(2);
    _tft.drawCentreString("v23.04 By:", 120, 49, 2);  //_tft.println("Total Wifi Remote Control");

    _tft.setTextSize(3);
    _tft.drawCentreString("minionLogic", 120, 78, 2);  //_tft.println("Total Wifi Remote Control");

    _tft.setTextSize(2);
    _tft.drawString("SSID:", 10, 134, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawCentreString("pConnectInfo", 120, 156, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawString("PSW:", 10, 182, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawCentreString("pConnectInfo[1]", 120, 192, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawString("IP:", 10, 210, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawCentreString("pConnectInfo[2]", 120, 220, 1);  //_tft.println("Total Wifi Remote Control");

    _displayOffTime = _currentTime + 30000;
}

///////////////////////////////////////////////////////////
void displayAPConnection() {
///////////////////////////////////////////////////////////
  SysTools::addLog("SysSpiDisplay::displayAPConnection, time: %lu", millis());

    enableDisplay();

    //Set the font colour
    _tft.fillScreen(TFT_RED);
    _tft.setTextColor(TFT_BLACK, TFT_GREEN);
    _tft.setTextPadding(1);
  
    _tft.setTextSize(3);
    _tft.drawCentreString("smartRemotes", 120, 0, 2);  //_tft.println("Total Wifi Remote Control");

    _tft.setTextSize(2);
    _tft.drawCentreString("v23.04 By:", 120, 49, 2);  //_tft.println("Total Wifi Remote Control");

    _tft.setTextSize(3);
    _tft.drawCentreString("minionLogic", 120, 78, 2);  //_tft.println("Total Wifi Remote Control");

    _tft.setTextSize(2);
    _tft.drawString("SSID:", 10, 134, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawCentreString("pConnectInfo", 120, 156, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawString("PSW:", 10, 182, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawCentreString("pConnectInfo[1]", 120, 192, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawString("IP:", 10, 210, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawCentreString("pConnectInfo[2]", 120, 220, 1);  //_tft.println("Total Wifi Remote Control");

    _displayOffTime = _currentTime + 30000;
}

///////////////////////////////////////////////////////////
void displayHome() {
///////////////////////////////////////////////////////////
  SysTools::addLog("SysSpiDisplay::displayHome, time: %lu", millis());

    enableDisplay();

    //Set the font colour
    _tft.fillScreen(TFT_WHITE);
    _tft.setTextColor(TFT_BLACK, TFT_GREEN);
    _tft.setTextPadding(1);
  
    _tft.setTextSize(3);
    _tft.drawCentreString("smartRemotes", 120, 0, 2);  //_tft.println("Total Wifi Remote Control");

    _tft.setTextSize(2);
    _tft.drawCentreString("v23.04 By:", 120, 49, 2);  //_tft.println("Total Wifi Remote Control");

    _tft.setTextSize(3);
    _tft.drawCentreString("minionLogic", 120, 78, 2);  //_tft.println("Total Wifi Remote Control");

    _tft.setTextSize(2);
    _tft.drawString("SSID:", 10, 134, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawCentreString("pConnectInfo", 120, 156, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawString("PSW:", 10, 182, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawCentreString("pConnectInfo[1]", 120, 192, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawString("IP:", 10, 210, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawCentreString("pConnectInfo[2]", 120, 220, 1);  //_tft.println("Total Wifi Remote Control");

    _displayOffTime = _currentTime + 30000;
}

///////////////////////////////////////////////////////////////////
void addLog(const char* format, ...) {
///////////////////////////////////////////////////////////////////
    va_list argptr;
    va_start(argptr, format);
    printf("%s", "LOG: ");
    vprintf(format, argptr);
    printf("%s", "\r\n");
    va_end(argptr);
}
} //namespace SysTools