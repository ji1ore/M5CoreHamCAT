#include "globals.h"

Unit_Encoder encoder;
int32_t lastEncVal = 0;


M5Canvas canvas(&M5.Display);
LGFX_Sprite listCanvas(&canvas);

StepSetting stepList[] = {
    {"10",    10},
    {"100",   100},
    {"1k",    1000},
    {"5k",    5000},
    {"10k",   10000},
    {"20k",   20000}
};
int selStep = 1;   // 初期値は 100Hz


AppState appState = STATE_WIFI;

std::vector<WifiEntry> wifiList;
bool wifiScanned = false;
bool showErrorDialog = false;
int selectedWifiIndex = -1;
int wifiScrollOffset = 0;

bool passwordForWifi = false;
String inputPassword = "";
bool shiftOn = false;

EditField editingField = FIELD_NONE;
KeyboardMode kbMode = KB_QWERTY;

bool useMDNS = true;
String raspiHost = "raspizero";
int apiPort = 8000;
int baudRate = 38400;

String lastErrorMessage = "";

std::vector<int> rigIds;
std::vector<String> rigNames;
std::vector<String> devList;
int selRig = 0;

int txPowerWatt = 0;
int txVolPercent = 50;
int rxVolPercent = 50;
bool bkinOn = false;
bool isTransmitting = false;
int signalStrength = 0;

int selectedItem = 0;

Preferences prefs;

bool rigSelectFirstDraw = true;

int selCat = 0;

int64_t lastFreqHz = 00000000;  
String lastMode = "USB";
String lastModel = "No_RIG";

TaskHandle_t audioTaskHandle = nullptr;

std::vector<String> catList;
std::vector<int> supportedWidths = {50, 100, 200, 300, 500, 1000, 1500, 2400, 2700, 3000};
int selWidthIndex = 0;
int lastWidth = 2400;
bool useInternalSpeaker = true;
std::vector<int> baudRates = {4800, 9600, 19200, 38400, 57600, 115200}; int selBaud = 3; // デフォルトは38400