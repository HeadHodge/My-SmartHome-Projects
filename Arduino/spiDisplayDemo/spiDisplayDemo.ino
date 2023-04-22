
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
 
    tft.writecommand(TFT_MADCTL);
    tft.writedata(0x48);          // Bits 6 and 3 set
    
 0x48 is the default value for ILI9341 (0xA8 for ESP32 M5STACK)
 in rotation 0 orientation.
 
 Another control register can be used to "invert" colours,
 this swaps black and white as well as other colours (e.g.
 green to magenta, red to cyan, blue to yellow).
 
 To invert colours insert this line after tft.init() or tft.begin():

    tft.invertDisplay( invert ); // Where invert is true or false

*/
//Required System Libraries
//#include <SPI.h>
//#include <TFT_eSPI.h>       // Hardware-specific library

//My Required Libraries
#include <SysTools.h>
#include <SysSpiDisplay.h>

///////////////////////////////////////////////////////////
//TFT_eSPI tft = TFT_eSPI();  // Invoke custom library
void setup(void) {
///////////////////////////////////////////////////////////
    SysSpiDisplay::enable();
}

///////////////////////////////////////////////////////////
void loop() {
///////////////////////////////////////////////////////////
    SysSpiDisplay::refresh();
}
