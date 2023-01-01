#define US_KEYBOARD 1
#include <Arduino.h>
#include <MinionTools.h>

namespace MinionTools {
#define DEBUG_ON 1
#if DEBUG_ON
#define DBG_begin(...)    Serial.begin(__VA_ARGS__)
#define DBG_print(...)    Serial.print(__VA_ARGS__)
#define DBG_println(...)  Serial.println(__VA_ARGS__)
#define DBG_printf(...)   Serial.printf(__VA_ARGS__)
#else
#define DBG_begin(...)
#define DBG_print(...)
#define DBG_println(...)
#define DBG_printf(...)
#endif
    
char logBuffer[1024];

void addLog(const char* format, ...) { 
    if(Serial.availableForWrite() == 0) return;    
    memset (logBuffer, 0, 1024);
    
    va_list argptr;
    va_start(argptr, format);
    vsprintf(logBuffer, format, argptr);
    va_end(argptr);
    
    Serial.printf("%s \r\n", logBuffer);
    //delay(100);
}

void openLog(int baudRate)
{
    Serial.begin(115200);
    delay(1000);
    
    addLog("%s", "MinionTools::openLog Log Now Open");
}
}