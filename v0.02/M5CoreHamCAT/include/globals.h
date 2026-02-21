#pragma once
#include <Arduino.h>
#include <vector>
#include <Preferences.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <Unit_Encoder.h>

// ==== UI共通 ====
extern M5Canvas canvas;
extern Preferences prefs;
extern bool mainFirstDraw;
extern bool showErrorDialog;
extern String lastErrorMessage;

// ==== エンコーダ ====
extern Unit_Encoder encoder;
extern int32_t lastEncVal;

// ==== アプリ状態 ====
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
extern AppState appState;

// ==== メニュー項目 ====
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
extern int selectedItem;

// ==== Wi-Fi 関連 ====
struct WifiEntry
{
  String ssid;
  int rssi;
};
extern std::vector<WifiEntry> wifiList;
extern bool wifiScanned;
extern int selectedWifiIndex;
extern int wifiScrollOffset;
extern String ssid;
extern String pass;

// ==== パスワード入力・キーボード ====
extern bool passwordForWifi;
extern String inputPassword;
extern bool shiftOn;

enum EditField
{
  FIELD_NONE,
  FIELD_HOST,
  FIELD_API_PORT,
  FIELD_AUDIO_PORT,
  FIELD_BAUDRATE
};
extern EditField editingField;

enum KeyboardMode
{
  KB_QWERTY,
  KB_NUMPAD
};
extern KeyboardMode kbMode;

// ==== 周波数ステップ設定 ====
struct StepSetting
{
  const char *label;
  long stepHz;
};
extern StepSetting stepList[];
extern int selStep;

// ==== Raspberry Pi 接続設定 ====
extern bool useMDNS;
extern String raspiHost;
extern int apiPort;
extern int audioPort;
extern int baudRate;
extern bool useInternalSpeaker;

// ==== Rig / デバイス選択 ====
extern std::vector<int> rigIds;
extern std::vector<String> rigNames;
extern std::vector<String> catList;
extern std::vector<String> devList;
extern int selRig;
extern int selCat;
extern int selBaud;
extern std::vector<int> baudRates;
extern bool rigSelectFirstDraw;
extern bool rigConnectFirstDraw;

// ==== 無線機状態 ====
extern int64_t lastFreqHz;
extern String lastMode;
extern String lastModel;
extern int lastWidth;
extern int selWidthIndex;
extern std::vector<int> supportedWidths;

extern int txPowerWatt;
extern int txVolPercent;
extern int rxVolPercent;
extern int rxVolume;
extern bool bkinOn;
extern bool isTransmitting;
extern int signalStrength;

// ==== 関数 ====
String connectToRasPiServices();
