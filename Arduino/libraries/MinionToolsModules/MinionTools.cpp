#define US_KEYBOARD 1
#include <Arduino.h>
#include <map>
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

std::map<std::string, hidKey>keyMap = {
    {"a",  {"keyboard", 0, 0x0004, "a", "{}", 5, 5, 0x001d}}, 
    {"b",  {"keyboard", 0, 0x0005, "b", "{}", 5, 5, 0x001e}}, 
    {"c",  {"keyboard", 0, 0x0006, "c", "{}", 5, 5, 0x001f}}, 
    {"d",  {"keyboard", 0, 0x0007, "d", "{}", 5, 5, 0x0020}}, 
    {"e",  {"keyboard", 0, 0x0008, "e", "{}", 5, 5, 0x0021}}, 
    {"f",  {"keyboard", 0, 0x0009, "f", "{}", 5, 5, 0x0022}}, 
    {"g",  {"keyboard", 0, 0x000a, "g", "{}", 5, 5, 0x0023}}, 
    {"h",  {"keyboard", 0, 0x000b, "h", "{}", 5, 5, 0x0024}}, 
    {"i",  {"keyboard", 0, 0x000c, "i", "{}", 5, 5, 0x0025}}, 
    {"j",  {"keyboard", 0, 0x000d, "j", "{}", 5, 5, 0x0026}}, 
    {"k",  {"keyboard", 0, 0x000e, "k", "{}", 5, 5, 0x0027}}, 
    {"l",  {"keyboard", 0, 0x000f, "l", "{}", 5, 5, 0x0028}}, 
    {"m",  {"keyboard", 0, 0x0010, "m", "{}", 5, 5, 0x0029}}, 
    {"n",  {"keyboard", 0, 0x0011, "n", "{}", 5, 5, 0x002a}}, 
    {"o",  {"keyboard", 0, 0x0012, "o", "{}", 5, 5, 0x002b}}, 
    {"p",  {"keyboard", 0, 0x0013, "p", "{}", 5, 5, 0x002c}}, 
    {"q",  {"keyboard", 0, 0x0014, "q", "{}", 5, 5, 0x002d}}, 
    {"r",  {"keyboard", 0, 0x0015, "r", "{}", 5, 5, 0x002e}}, 
    {"s",  {"keyboard", 0, 0x0016, "s", "{}", 5, 5, 0x002f}}, 
    {"t",  {"keyboard", 0, 0x0017, "t", "{}", 5, 5, 0x0030}}, 
    {"u",  {"keyboard", 0, 0x0018, "u", "{}", 5, 5, 0x0031}}, 
    {"v",  {"keyboard", 0, 0x0019, "v", "{}", 5, 5, 0x0032}}, 
    {"w",  {"keyboard", 0, 0x001a, "w", "{}", 5, 5, 0x0033}}, 
    {"x",  {"keyboard", 0, 0x001b, "x", "{}", 5, 5, 0x0034}}, 
    {"y",  {"keyboard", 0, 0x001c, "y", "{}", 5, 5, 0x0035}}, 
    {"z",  {"keyboard", 0, 0x001d, "z", "{}", 5, 5, 0x0036}},

    
    {"A",  {"keyboard", 2, 0x0004, "A", "{}", 5, 5, 0x001d}}, 
    {"B",  {"keyboard", 2, 0x0005, "B", "{}", 5, 5, 0x001e}}, 
    {"C",  {"keyboard", 2, 0x0006, "C", "{}", 5, 5, 0x001f}}, 
    {"D",  {"keyboard", 2, 0x0007, "D", "{}", 5, 5, 0x0020}}, 
    {"E",  {"keyboard", 2, 0x0008, "E", "{}", 5, 5, 0x0021}}, 
    {"F",  {"keyboard", 2, 0x0009, "F", "{}", 5, 5, 0x0022}}, 
    {"G",  {"keyboard", 2, 0x000a, "G", "{}", 5, 5, 0x0023}}, 
    {"H",  {"keyboard", 2, 0x000b, "H", "{}", 5, 5, 0x0024}}, 
    {"I",  {"keyboard", 2, 0x000c, "I", "{}", 5, 5, 0x0025}}, 
    {"J",  {"keyboard", 2, 0x000d, "J", "{}", 5, 5, 0x0026}}, 
    {"K",  {"keyboard", 2, 0x000e, "K", "{}", 5, 5, 0x0027}}, 
    {"L",  {"keyboard", 2, 0x000f, "L", "{}", 5, 5, 0x0028}}, 
    {"M",  {"keyboard", 2, 0x0010, "M", "{}", 5, 5, 0x0029}}, 
    {"N",  {"keyboard", 2, 0x0011, "N", "{}", 5, 5, 0x002a}}, 
    {"O",  {"keyboard", 2, 0x0012, "O", "{}", 5, 5, 0x002b}}, 
    {"P",  {"keyboard", 2, 0x0013, "P", "{}", 5, 5, 0x002c}}, 
    {"Q",  {"keyboard", 2, 0x0014, "Q", "{}", 5, 5, 0x002d}}, 
    {"R",  {"keyboard", 2, 0x0015, "R", "{}", 5, 5, 0x002e}}, 
    {"S",  {"keyboard", 2, 0x0016, "S", "{}", 5, 5, 0x002f}}, 
    {"T",  {"keyboard", 2, 0x0017, "T", "{}", 5, 5, 0x0030}}, 
    {"U",  {"keyboard", 2, 0x0018, "U", "{}", 5, 5, 0x0031}}, 
    {"V",  {"keyboard", 2, 0x0019, "V", "{}", 5, 5, 0x0032}}, 
    {"W",  {"keyboard", 2, 0x001a, "W", "{}", 5, 5, 0x0033}}, 
    {"X",  {"keyboard", 2, 0x001b, "X", "{}", 5, 5, 0x0034}}, 
    {"Y",  {"keyboard", 2, 0x001c, "Y", "{}", 5, 5, 0x0035}}, 
    {"Z",  {"keyboard", 2, 0x001d, "Z", "{}", 5, 5, 0x0036}},

    
    {"1",  {"keyboard", 0, 0x001e, "1", "{}", 5, 5, 0x0008}}, 
    {"2",  {"keyboard", 0, 0x001f, "2", "{}", 5, 5, 0x0009}}, 
    {"3",  {"keyboard", 0, 0x0020, "3", "{}", 5, 5, 0x000a}}, 
    {"4",  {"keyboard", 0, 0x0021, "4", "{}", 5, 5, 0x000b}}, 
    {"5",  {"keyboard", 0, 0x0022, "5", "{}", 5, 5, 0x000c}}, 
    {"6",  {"keyboard", 0, 0x0023, "6", "{}", 5, 5, 0x000d}}, 
    {"7",  {"keyboard", 0, 0x0024, "7", "{}", 5, 5, 0x000e}}, 
    {"8",  {"keyboard", 0, 0x0025, "8", "{}", 5, 5, 0x000f}}, 
    {"9",  {"keyboard", 0, 0x0026, "9", "{}", 5, 5, 0x0010}}, 
    {"0",  {"keyboard", 0, 0x0027, "0", "{}", 5, 5, 0x0007}},
    
    {"ENTER",  {"keyboard", 0, 0x0028, "ENTER", "{}", 5, 5, 0x0042}}, 
    {"ESCAPE",  {"keyboard", 0, 0x0029, "ESCAPE", "{}", 5, 5, 0x006f}}, 
    {"DPAD_RIGHT",  {"keyboard", 0, 0x004f, "DPAD_RIGHT", "{}", 5, 5, 0x0016}}, 
    {"DPAD_LEFT",  {"keyboard", 0, 0x0050, "DPAD_LEFT", "{}", 5, 5, 0x0015}}, 
    {"DPAD_DOWN",  {"keyboard", 0, 0x0051, "DPAD_DOWN", "{}", 5, 5, 0x0014}}, 
    {"DPAD_UP",  {"keyboard", 0, 0x0052, "DPAD_UP", "{}", 5, 5, 0x0013}},
    
    {"MENU",  {"consumer", 0, 0x0040, "MENU", "{}", 5, 5, 0x0052}}, 
    {"WINDOW",  {"consumer", 0, 0x0067, "WINDOW", "{}", 5, 5, 0x00ab}}, 
    {"PROG_RED",  {"consumer", 0, 0x0069, "PROG_RED", "{}", 5, 5, 0x00b7}}, 
    {"PROG_GREEN",  {"consumer", 0, 0x006a, "PROG_GREEN", "{}", 5, 5, 0x00b8}}, 
    {"PROG_BLUE",  {"consumer", 0, 0x006b, "PROG_BLUE", "{}", 5, 5, 0x00ba}}, 
    {"PROG_YELLOW",  {"consumer", 0, 0x006c, "PROG_YELLOW", "{}", 5, 5, 0x00b9}}, 
    {"LAST_CHANNEL",  {"consumer", 0, 0x0083, "LAST_CHANNEL", "{}", 5, 5, 0x00e5}}, 
    {"TV",  {"consumer", 0, 0x0089, "TV", "{}", 5, 5, 0x00aa}}, 
    {"EXPLORER",  {"consumer", 0, 0x008a, "EXPLORER", "{}", 5, 5, 0x0040}}, 
    {"CALL",  {"consumer", 0, 0x008c, "CALL", "{}", 5, 5, 0x0005}}, 
    {"GUIDE",  {"consumer", 0, 0x008d, "GUIDE", "{}", 5, 5, 0x00ac}}, 
    {"CAPTIONS",  {"consumer", 0, 0x0061, "CAPTIONS", "{}", 5, 5, 0x00af}}, 
    {"CHANNEL_UP",  {"consumer", 0, 0x009c, "CHANNEL_UP", "{}", 5, 5, 0x00a6}}, 
    {"CHANNEL_DOWN",  {"consumer", 0, 0x009d, "CHANNEL_DOWN", "{}", 5, 5, 0x00a7}}, 
    {"MEDIA_PLAY",  {"consumer", 0, 0x00b0, "MEDIA_PLAY", "{}", 5, 5, 0x007e}}, 
    {"BREAK",  {"consumer", 0, 0x00b1, "BREAK", "{}", 5, 5, 0x0079}}, 
    {"MEDIA_RECORD",  {"consumer", 0, 0x00b2, "MEDIA_RECORD", "{}", 5, 5, 0x0082}}, 
    {"MEDIA_FAST_FORWARD",  {"consumer", 0, 0x00b3, "MEDIA_FAST_FORWARD", "{}", 5, 5, 0x005a}}, 
    {"MEDIA_REWIND",  {"consumer", 0, 0x00b4, "MEDIA_REWIND", "{}", 5, 5, 0x0059}}, 
    {"MEDIA_NEXT",  {"consumer", 0, 0x00b5, "MEDIA_NEXT", "{}", 5, 5, 0x0057}}, 
    {"MEDIA_PREVIOUS",  {"consumer", 0, 0x00b6, "MEDIA_PREVIOUS", "{}", 5, 5, 0x0058}}, 
    {"MEDIA_STOP",  {"consumer", 0, 0x00b7, "MEDIA_STOP", "{}", 5, 5, 0x0056}}, 
    {"MEDIA_EJECT",  {"consumer", 0, 0x00b8, "MEDIA_EJECT", "{}", 5, 5, 0x0081}}, 
    {"MEDIA_PLAY_PAUSE",  {"consumer", 0, 0x00cd, "MEDIA_PLAY_PAUSE", "{}", 5, 5, 0x0055}}, 
    {"VOLUME_MUTE",  {"consumer", 0, 0x00e2, "VOLUME_MUTE", "{}", 5, 5, 0x00a4}}, 
    {"VOLUME_UP",  {"consumer", 0, 0x00e9, "VOLUME_UP", "{}", 5, 5, 0x0018}}, 
    {"VOLUME_DOWN",  {"consumer", 0, 0x00ea, "VOLUME_DOWN", "{}", 5, 5, 0x0019}}, 
    {"BOOKMARK",  {"consumer", 0, 0x0182, "BOOKMARK", "{}", 5, 5, 0x00ae}}, 
    {"MUSIC",  {"consumer", 0, 0x0183, "MUSIC", "{}", 5, 5, 0x00d1}}, 
    {"ENVELOPE",  {"consumer", 0, 0x018a, "ENVELOPE", "{}", 5, 5, 0x0041}}, 
    {"CONTACTS",  {"consumer", 0, 0x018d, "CONTACTS", "{}", 5, 5, 0x00cf}}, 
    {"CALENDAR",  {"consumer", 0, 0x018e, "CALENDAR", "{}", 5, 5, 0x00d0}}, 
    {"CALCULATOR",  {"consumer", 0, 0x0192, "CALCULATOR", "{}", 5, 5, 0x00d2}}, 
    {"EXPLORER",  {"consumer", 0, 0x0196, "EXPLORER", "{}", 5, 5, 0x0040}}, 
    {"POWER",  {"consumer", 0, 0x019e, "POWER", "{}", 5, 5, 0x001a}}, 
    {"HEADSETHOOK",  {"consumer", 0, 0x01b6, "HEADSETHOOK", "{}", 5, 5, 0x004f}}, 
    {"MUSIC",  {"consumer", 0, 0x01b7, "MUSIC", "{}", 5, 5, 0x00d1}}, 
    {"SEARCH",  {"consumer", 0, 0x0221, "SEARCH", "{}", 5, 5, 0x0054}}, 
    {"HOME",  {"consumer", 0, 0x0223, "HOME", "{}", 5, 5, 0x0003}}, 
    {"BACK",  {"consumer", 0, 0x0224, "BACK", "{}", 5, 5, 0x0004}}, 
    {"FORWARD",  {"consumer", 0, 0x0225, "FORWARD", "{}", 5, 5, 0x007d}}, 
    {"MEDIA_STOP",  {"consumer", 0, 0x0226, "MEDIA_STOP", "{}", 5, 5, 0x0056}}, 
    {"BOOKMARK",  {"consumer", 0, 0x022a, "BOOKMARK", "{}", 5, 5, 0x00ae}}, 
    {"PAGE_UP",  {"consumer", 0, 0x0233, "PAGE_UP", "{}", 5, 5, 0x005c}}, 
    {"PAGE_DOWN",  {"consumer", 0, 0x0234, "PAGE_DOWN", "{}", 5, 5, 0x005d}}, 
}; 

hidKey lookupKeyWord(DynamicJsonDocument& pKeyObj) { 
hidKey keyOptions = keyMap[(const char*)pKeyObj["settings"]["keyWord"]];
MinionTools::addLog("keyOptions: %s %s %i %i, %i, %i", "debug 2x00", keyOptions.keyWord, keyOptions.keyCode, keyOptions.keyModifier, keyOptions.keyDuration, keyOptions.keyDelay );

    if(pKeyObj["options"]["keyCode"] != nullptr) keyOptions.keyCode = (int)pKeyObj["options"]["keyCode"];
    if(pKeyObj["options"]["keyModifier"] != nullptr) keyOptions.keyModifier = (int)pKeyObj["options"]["keyModifier"];
    if(pKeyObj["options"]["keyDuration"] != nullptr) keyOptions.keyDuration = (int)pKeyObj["options"]["keyDuration"];
    if(pKeyObj["options"]["keyDelay"] != nullptr) keyOptions.keyDelay = (int)pKeyObj["options"]["keyDelay"];
    if(pKeyObj["options"]["keyUsage"] != nullptr) keyOptions.keyUsage = (const char*)pKeyObj["options"]["keyUsage"];
    MinionTools::addLog("keyOptions: %s %s %i %i, %i, %i", "debug 2x00", keyOptions.keyWord, keyOptions.keyCode, keyOptions.keyModifier, keyOptions.keyDuration, keyOptions.keyDelay );
    
    return keyOptions;
};

void addLog(const char* format, ...) { 
    if(Serial.availableForWrite() == 0) return;    
    memset (logBuffer, 0, 1024);
    
    va_list argptr;
    va_start(argptr, format);
    vsprintf(logBuffer, format, argptr);
    va_end(argptr);
    
    Serial.printf("%s \r\n", logBuffer);
    //delay(100);
};

void openLog(int baudRate) {
    Serial.begin(115200);
    delay(10000);
    
    addLog("%s", "MinionTools::openLog Log Now Open");
    delay(1000);
};
}