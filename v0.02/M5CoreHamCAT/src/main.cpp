/****************************************************
 *  M5CoreHamCAT main.cpp
 *  Ver0.01
 *  by JI1ORE
 ****************************************************/
#include <M5Unified.h>
#include <M5GFX.h>
#include "ui_display.h"
#include "globals.h"
#include "driver/i2s.h"
#include <HTTPClient.h>

M5GFX display;
HardwareSerial mySerial(1); // UART1使用（TX: GPIO14）
/*
const char *ssid = "UENO-a";
const char *pass = "hogehoge33";
const char *url = "http://192.168.1.13";
const int port = 50000;
*/
const char *url = "http://";
unsigned long lastSend = 0;

// M5Canvas canvas(&M5.Display);
MenuItem detectTouchedButton(int x, int y);

void handleWifiScreen();
void handleRigConnectScreen();
void handleMainUIScreen();
void handlePasswordScreen();
void handlePiConnectScreen();
void handleRigSelectScreen();
void handleFreqInputScreen();
void drawSplashScreen();

void setup()
{
  auto cfg = M5.config();

  // --- M5 本体の初期化 ---
  cfg.internal_mic = false;
  cfg.internal_spk = true;
  cfg.external_spk = false;
  M5.begin(cfg);
  mySerial.begin(115200, SERIAL_8N1, -1, 8); // TX = GPIO8 (PortB)
  Wire.begin(2, 1);                          // SDA=1, SCL=2 (CoreS3)
  encoder.begin(&Wire, 0x40);                // U135 の I2C アドレス
  lastEncVal = encoder.getEncoderValue();

  Serial.begin(115200);

  canvas.setColorDepth(8);       // 軽量化
  canvas.createSprite(320, 240); // CoreS3の画面サイズ
  drawSplashScreen();
  delay(2000); // 2秒表示
  canvas.setTextSize(2);
  canvas.setTextColor(WHITE);
  canvas.setFont(&fonts::efontJA_16);

  appState = STATE_WIFI;
}

void loop()
{
  M5.update();

  switch (appState)
  {
  case STATE_WIFI:
    handleWifiScreen();
    break;

  case STATE_PASSWORD:
    handlePasswordScreen();
    break;

  case STATE_PI_CONNECT:
    handlePiConnectScreen();
    break;

  case STATE_RIG_CONNECT:
    handleRigSelectScreen();
    break;

  case STATE_DEVICE_SELECT:
    handleRigConnectScreen();
    break;

  case STATE_MAIN_UI:
  {
    handleMainUIScreen();
    unsigned long now = millis();
    if (now - lastSend > 3000)
    {
      lastSend = now;
      String payload = "{\"ssid\":\"" + String(ssid) + "\",\"pass\":\"" + String(pass) + "\",\"url\":\"" + String(url) + String(raspiHost) + "\",\"port\":" + String(audioPort) + "}";
      mySerial.println(payload);
    }
    break;
  }
  case STATE_FREQ_INPUT:
    handleFreqInputScreen();
    return;
  }
}
