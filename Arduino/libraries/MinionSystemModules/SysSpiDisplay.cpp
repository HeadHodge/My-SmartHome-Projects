//   Diagnostic test for the displayed colour order
//
// Written by Bodmer 17/2/19 for the TFT_eSPI library:
// https://github.com/Bodmer/TFT_eSPI

/* 
 Different hardware manufacturers use different colour order
 configurations at the hardware level.  This may result in
 incorrect colours being displayed.

 Incorrectly displayed colours could also be the result of
 using the wrong display driver in the library setup file.

 Typically displays have a control register (MADCTL) that can
 be used to set the Red Green Blue (RGB) colour order to RGB
 or BRG so that red and blue are swapped on the display.

 This control register is also used to manage the display
 rotation and coordinate mirroring. The control register
 typically has 8 bits, for the ILI9341 these are:

 Bit Function
 7   Mirror Y coordinate (row address order)
 6   Mirror X coordinate (column address order)
 5   Row/column exchange (for rotation)
 4   Refresh direction (top to bottom or bottom to top in portrait orientation)
 3   RGB order (swaps red and blue)
 2   Refresh direction (top to bottom or bottom to top in landscape orientation)
 1   Not used
 0   Not used

 The control register bits can be written with this example command sequence:
 
    _tft.writecommand(TFT_MADCTL);
    _tft.writedata(0x48);          // Bits 6 and 3 set
    
 0x48 is the default value for ILI9341 (0xA8 for ESP32 M5STACK)
 in rotation 0 orientation.
 
 Another control register can be used to "invert" colours,
 this swaps black and white as well as other colours (e.g.
 green to magenta, red to cyan, blue to yellow).
 
 To invert colours insert this line after _tft.init() or _tft.begin():

    _tft.invertDisplay( invert ); // Where invert is true or false

*/

#include <SPI.h>
#include <TFT_eSPI.h>       // Hardware-specific library

//My Required Libraries
#include <SysTools.h>
#include <SysSpiDisplay.h>

/////////////////////////////////////////////////////////////////////////////////////////////////
namespace SysSpiDisplay {

#define _currentTime millis()
TFT_eSPI _tft = TFT_eSPI();  // Invoke custom library
uint32_t _displayOffTime = 0; //millisecs

///////////////////////////////////////////////////////////
void refresh(void) {
///////////////////////////////////////////////////////////
  //SysTools::addLog("SysSpiDisplay::refresh, Refresh Display");

    if(_currentTime >= _displayOffTime && _displayOffTime > 0) {
        SysTools::addLog("SysSpiDisplay::refresh, Turn Display Off: %i", gpio_set_level(GPIO_NUM_9, 0));
        _displayOffTime = 0;
    };
}

///////////////////////////////////////////////////////////
bool enable(char* pConnectInfo[]) {
///////////////////////////////////////////////////////////
  SysTools::addLog("SysSpiDisplay::enable, Turn On Display, time: %lu", millis());
  SysTools::addLog("SysSpiDisplay::enable, ConnectInfo, ssid: %s, password: %s, address: %s", pConnectInfo[0], pConnectInfo[1], pConnectInfo[2]);

    //Init Display
    _tft.init();
    //_tft.invertDisplay( true ); // Where i is true or false
    SysTools::addLog("SysSpiDisplay::enable, Turn Display On: %i", gpio_set_level(GPIO_NUM_9, 1));

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
    _tft.drawCentreString(pConnectInfo[0], 120, 156, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawString("PSW:", 10, 182, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawCentreString(pConnectInfo[1], 120, 192, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawString("IP:", 10, 210, 1);  //_tft.println("Total Wifi Remote Control");
    _tft.drawCentreString(pConnectInfo[2], 120, 220, 1);  //_tft.println("Total Wifi Remote Control");

    _displayOffTime = _currentTime + 30000;
    
    return true;
}
} //namespace SysSpiDisplay
