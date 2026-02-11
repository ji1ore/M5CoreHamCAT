#pragma once
#include <Arduino.h>
#include <vector>
#include <Preferences.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <Unit_Encoder.h>

extern Unit_Encoder encoder;
extern int32_t lastEncVal;


extern M5Canvas canvas;

// ---- AppState ----
enum AppState
{
  STATE_WIFI,
  STATE_PASSWORD,
  STATE_PI_CONNECT,
  STATE_DEVICE_SELECT,
  STATE_RIG_CONNECT,
  STATE_MAIN_UI,
  STATE_FREQ_INPUT,
  STATE_CONNECT_FAILED 
};

enum MenuItem
{
  MENU_UP,
  MENU_DOWN,
  MENU_BACK,
  MENU_FREQ,
  MENU_MODE,
  MENU_POW,
  MENU_SQL,
  MENU_STEP,
  MENU_WIDTH,
  MENU_NONE
};

extern AppState appState;

// ---- Wi-Fi ----
struct WifiEntry
{
  String ssid;
  int rssi;
};
extern std::vector<WifiEntry> wifiList;

extern bool wifiScanned;
extern bool showErrorDialog;
extern int selectedWifiIndex;
extern int wifiScrollOffset;
extern std::vector<int> baudRates; extern int selBaud;

// ---- Password / Keyboard ----
extern bool passwordForWifi;
extern String inputPassword;
extern bool shiftOn;

enum EditField
{
  FIELD_NONE,
  FIELD_HOST,
  FIELD_API_PORT,
  FIELD_BAUDRATE
};
extern EditField editingField;

enum KeyboardMode
{
  KB_QWERTY,
  KB_NUMPAD
};
extern KeyboardMode kbMode;

struct StepSetting
{
  const char *label;
  long stepHz;
};

extern StepSetting stepList[];
extern int selStep;

// ---- RasPi Config ----
extern bool useMDNS;
extern String raspiHost;
extern int apiPort;
extern int baudRate;

extern String lastErrorMessage;

// ---- RigSelect ----
extern bool rigSelectFirstDraw;

// ---- DeviceSelect ----
extern std::vector<int> rigIds;
extern std::vector<String> rigNames;
extern std::vector<String> devList;
extern int selRig;

// ---- Main UI ----
extern int txPowerWatt;
extern int txVolPercent;
extern int rxVolPercent;
extern bool bkinOn;
extern bool isTransmitting;
extern int signalStrength;

extern int selectedItem;

// ---- Preferences ----
extern Preferences prefs;

// ---- Functions ----
String connectToRasPiServices();

// extern LGFX_Sprite listCanvas;

extern int selCat;

extern int64_t lastFreqHz;
extern String lastMode;
extern String lastModel;


extern int rxVolume;

extern std::vector<String> catList;
extern int lastWidth;
extern int selWidthIndex;
extern std::vector<int> supportedWidths;
extern bool useInternalSpeaker;
extern bool isTransmitting; 
