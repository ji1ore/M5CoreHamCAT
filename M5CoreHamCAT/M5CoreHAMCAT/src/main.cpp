#include <M5Unified.h>
#include <M5GFX.h>
#include "ui_display.h"
#include "globals.h"
#include "driver/i2s.h"
#include <HTTPClient.h>

M5GFX display;
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

  Wire.begin(2, 1);           // SDA=1, SCL=2 (CoreS3)
  encoder.begin(&Wire, 0x40); // U135 の I2C アドレス
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
    handleMainUIScreen();
    break;

  case STATE_FREQ_INPUT:
    handleFreqInputScreen();
    return;
  }
}
