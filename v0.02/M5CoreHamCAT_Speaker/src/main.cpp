#include <M5AtomS3.h>
#include <WiFi.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioOutputI2S.h"
#include <Adafruit_NeoPixel.h>

// UART設定
HardwareSerial mySerial(1); // RX: GPIO1, TX: GPIO2

constexpr uint8_t PIN_I2S_BCLK = 5;
constexpr uint8_t PIN_I2S_WS = 39;
constexpr uint8_t PIN_I2S_DOUT = 38;

int samplingRate = 48000;

#define LED_PIN 35
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

AudioFileSourceICYStream *file = nullptr;
AudioFileSourceBuffer *buff = nullptr;
AudioOutputI2S *out = nullptr;

String ssid, pass, url;
int newPort;
bool connected = false;
bool ready = false;
float volumeLevels[] = {0.2, 0.4, 0.6, 0.8, 1.0};
int volumeIndex = 2;

unsigned long buttonPressStart = 0;
bool buttonHeld = false;

bool tryStartStream(const String &fullUrl, int retries);

void startPlayback()
{
  String fullUrl = url + ":" + String(newPort) + "/radio/audio?rate=" + String(samplingRate);
  Serial.printf("接続先: %s\n", fullUrl.c_str());

  if (!tryStartStream(fullUrl, 3))
  {
    Serial.println("ストリーム接続失敗！");
    pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // 赤
    pixels.show();
    return;
  }

  buff = new AudioFileSourceBuffer(file, 1024); // ←バッファサイズはそのままでもOK

  out = new AudioOutputI2S();
  out->SetPinout(PIN_I2S_BCLK, PIN_I2S_WS, PIN_I2S_DOUT);
  out->SetRate(samplingRate);
  out->SetBitsPerSample(16);
  out->SetChannels(1);
  out->SetOutputModeMono(true);
  out->SetGain(volumeLevels[volumeIndex]);
  out->begin();

  pixels.setPixelColor(0, pixels.Color(0, 0, 255)); // 青
  pixels.show();
  Serial.println("再生開始！");
}

void stopPlayback()
{
  if (file)
    delete file;
  if (buff)
    delete buff;
  if (out)
    delete out;
  file = nullptr;
  buff = nullptr;
  out = nullptr;

  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // 赤
  pixels.show();
  Serial.println("再生停止・Wi-Fi切断");
}

// 🎧 再生専用タスク
void playbackTask(void *pvParameters)
{
  static uint8_t buffer[1024];
  while (true)
  {
    if (connected && buff && buff->isOpen() && out)
    {
      int len = buff->read(buffer, sizeof(buffer));
      if (len > 0)
      {
        int sampleCount = len / 2; // s16 = 2バイト/サンプル
        int16_t *samples16 = reinterpret_cast<int16_t *>(buffer);

        for (int i = 0; i < sampleCount; ++i)
        {
          int16_t sample[2] = {samples16[i], samples16[i]}; // モノラル→ステレオ
          while (!out->ConsumeSample(sample))
          {
            yield();
          }
        }
      }
      else
      {
        delay(1);
      }
    }
    else
    {
      delay(10);
    }
  }
}

void setup()
{
  auto cfg = M5.config();
  cfg.clear_display = true;
  cfg.external_spk = true;
  cfg.internal_spk = false;
  cfg.internal_mic = false;
  M5.begin(cfg);

  pixels.begin();
  pixels.setBrightness(128);
  pixels.setPixelColor(0, pixels.Color(255, 165, 0)); // オレンジ
  pixels.show();

  Serial.begin(115200);
  mySerial.begin(115200, SERIAL_8N1, 1, 2); // RX=1, TX=2

  WiFi.mode(WIFI_OFF);

  // 🎵 再生タスク起動！
  xTaskCreatePinnedToCore(
      playbackTask,
      "PlaybackTask",
      4096,
      NULL,
      5,
      NULL,
      1);
}

void loop()
{
  M5.update();

  // ① UART受信（再生中は無効）
  if (!connected && mySerial.available())
  {
    String json = mySerial.readStringUntil('\n');
    StaticJsonDocument<256> doc;
    if (deserializeJson(doc, json) == DeserializationError::Ok)
    {
      ssid = doc["ssid"].as<String>();
      pass = doc["pass"].as<String>();
      url = doc["url"].as<String>();
      newPort = doc["port"].as<int>();
      samplingRate = doc["sampling"].as<int>();

      if (ssid.length() > 0 && url.length() > 0)
      {
        ready = true;
        pixels.setPixelColor(0, pixels.Color(0, 255, 0)); // 緑
        pixels.show();
        Serial.println("接続情報受信完了");

        // ★ ゾンビセッション掃除
        if (file)
        {
          file->close();
          delete file;
          file = nullptr;
        }
        if (buff)
        {
          buff->close();
          delete buff;
          buff = nullptr;
        }
        if (out)
        {
          out->flush(); // バッファをクリア
          out->stop();
          delete out;
          out = nullptr;
        }
        delay(100); // 少し待ってから再生準備
      }

      else
      {
        ready = false;
        pixels.setPixelColor(0, pixels.Color(255, 165, 0)); // オレンジ
        pixels.show();
      }
    }
  }

  // ② ボタン長押し判定（再生開始/停止）
  if (M5.BtnA.isPressed())
  {
    if (!buttonHeld)
    {
      buttonPressStart = millis();
      buttonHeld = true;
    }
    else if (millis() - buttonPressStart > 1000)
    {
      buttonHeld = false;
      if (!connected && ready)
      {
        Serial.println("長押し：Wi-Fi接続＆再生開始");
        WiFi.begin(ssid.c_str(), pass.c_str());
        delay(1000);
        unsigned long start = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start < 10000)
        {
          delay(200);
          Serial.print(".");
        }
        if (WiFi.status() == WL_CONNECTED)
        {
          Serial.println("\nWi-Fi接続完了！");
          delay(500);
          connected = true;
          startPlayback();
        }
        else
        {
          Serial.println("\nWi-Fi接続失敗！");
          pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // 赤
          pixels.show();
        }
      }
      else if (connected)
      {
        Serial.println("長押し：再生停止");
        stopPlayback();
        connected = false;
        ready = false;
        pixels.setPixelColor(0, pixels.Color(255, 165, 0)); // オレンジ
        pixels.show();
      }
    }
  }
  else
  {
    buttonHeld = false;
  }

  // ③ ボタン短押しで音量切り替え
  if (M5.BtnA.wasPressed() && connected)
  {
    volumeIndex = (volumeIndex + 1) % 5;
    float vol = volumeLevels[volumeIndex];
    if (out)
      out->SetGain(vol);
    Serial.printf("音量変更：%.1f\n", vol);
  }

  delay(1);
}

bool tryStartStream(const String &fullUrl, int retries)
{
  for (int i = 0; i < retries; ++i)
  {
    Serial.printf("ストリーム接続試行 %d 回目...\n", i + 1);
    file = new AudioFileSourceICYStream(fullUrl.c_str());
    if (file->isOpen())
      return true;
    delete file;
    file = nullptr;
    delay(500);
  }
  return false;
}
