/****************************************************
 *  M5CoreHamCAT main.cpp
 *  Ver1.00
 *  by JI1ORE
 ****************************************************/
#include <M5Unified.h>
#include <M5GFX.h>
#include "ui_display.h"
#include "ui_core.h"
#include "globals.h"
#include "driver/i2s.h"
#include <HTTPClient.h>
#include "audio_i2c.hpp"
#include "es8388.hpp"
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioOutputI2S.h"

#ifdef M5CORE2
#define I2C_SDA_ENCODER 32
#define I2C_SCL_ENCODER 33
#define I2C_SDA_AUDIO 21
#define I2C_SCL_AUDIO 22
#define SYS_I2S_MCLK_PIN 0
#define SYS_I2S_SCLK_PIN 19
#define SYS_I2S_LRCK_PIN 27
#define SYS_I2S_DOUT_PIN 2
#define SYS_I2S_DIN_PIN 34
#elif M5CORES3
#define I2C_SDA_ENCODER 2
#define I2C_SCL_ENCODER 1
#define I2C_SDA_AUDIO 12
#define I2C_SCL_AUDIO 11
#define SYS_I2S_MCLK_PIN 0
#define SYS_I2S_SCLK_PIN 7
#define SYS_I2S_LRCK_PIN 6
#define SYS_I2S_DOUT_PIN 13
#define SYS_I2S_DIN_PIN 14
#endif

AudioOutputI2S *out = nullptr;
AudioFileSourceICYStream *file = nullptr;
AudioFileSourceBuffer *buff = nullptr;
TaskHandle_t streamTaskHandle = nullptr;
AppState lastAppState = STATE_WIFI;
bool lastspkEnabled = false;
int retries = 3;
bool connected = false;

AudioI2c device;
ES8388 es8388(&Wire1, I2C_SDA_AUDIO, I2C_SCL_AUDIO);

M5GFX display;
const char *url = "http://";
unsigned long lastSend = 0;
unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 600000;

i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 16,
    .dma_buf_len = 128,
    .use_apll = true,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0};

i2s_pin_config_t pin_config = {
    .mck_io_num = SYS_I2S_MCLK_PIN,
    .bck_io_num = SYS_I2S_SCLK_PIN,
    .ws_io_num = SYS_I2S_LRCK_PIN,
    .data_out_num = SYS_I2S_DOUT_PIN,
    .data_in_num = SYS_I2S_DIN_PIN,
};

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
void statusTask(void *param);
void streamTask(void *param);
void playbackTask(void *param);
void stopPlayback();

void setup()
{
  auto cfg = M5.config();

  // --- M5 本体の初期化 ---
  cfg.internal_mic = false;
  cfg.internal_spk = false;
  cfg.external_spk = true;

  Serial.begin(115200);

  M5.begin(cfg);
  Wire.begin(I2C_SDA_ENCODER, I2C_SCL_ENCODER); // SDA=32, SCL=33 (Core2)
  encoder.begin(&Wire, 0x40);                   // U135 の I2C アドレス
  lastEncVal = encoder.getEncoderValue();

  device.begin(&Wire1, I2C_SDA_AUDIO, I2C_SCL_AUDIO);
  device.setHPMode(AUDIO_HPMODE_NATIONAL);
  if (!es8388.init())
  {
    Serial.println("ES8388 init failed!");
  }
  else
  {
    Serial.println("ES8388 init OK!");
  }

  es8388.setADCVolume(100);
  es8388.setDACVolume(80);
  es8388.setDACOutput(DAC_OUTPUT_OUT1);
  es8388.setBitsSample(ES_MODULE_ADC, BIT_LENGTH_16BITS);
  es8388.setSampleRate((es_sample_rate_t)44100);

  // i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  // i2s_set_pin(I2S_NUM_0, &pin_config);

  canvas.setColorDepth(8);       // 軽量化
  canvas.createSprite(320, 240); // CoreS3SEの画面サイズ
  drawSplashScreen();
  delay(2000); // 2秒表示
  canvas.setTextSize(2);
  canvas.setTextColor(WHITE);
  canvas.setFont(&fonts::efontJA_16);

  xTaskCreatePinnedToCore(statusTask, "StatusTask", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(playbackTask, "PlaybackTask", 8192, NULL, 5, NULL, 1);

  appState = STATE_WIFI;
  lastReconnectAttempt = millis();
}

void loop()
{
  M5.update();

  if (appState != lastAppState || lastspkEnabled != spkEnabled)
  {
    if (appState == STATE_MAIN_UI && !connected && streamTaskHandle == nullptr && spkEnabled == true)
    {
      static unsigned long lastAttempt = 0;
      if (millis() - lastAttempt > 5000)
      { // 5秒に1回だけ再試行
        lastAttempt = millis();
        Serial.println("streamTask 起動！");
        xTaskCreatePinnedToCore(streamTask, "streamTask", 4096, NULL, 10, &streamTaskHandle, 1);
      }
    }
    else if (lastAppState == STATE_MAIN_UI && appState != STATE_MAIN_UI || spkEnabled == false)
    {
      stopPlayback();
      connected = false;
    }
    lastAppState = appState;
    lastspkEnabled = spkEnabled;
  }

  if (appState == STATE_MAIN_UI && !connected && streamTaskHandle == nullptr && spkEnabled == true)
  {
    static unsigned long lastAttempt = 0;
    if (millis() - lastAttempt > 5000)
    {
      lastAttempt = millis();
      Serial.println("streamTask 起動！");
      xTaskCreatePinnedToCore(streamTask, "streamTask", 4096, NULL, 10, &streamTaskHandle, 1);
    }
  }
  if (streamTaskHandle != nullptr)
  {
    eTaskState state = eTaskGetState(streamTaskHandle);
    if (state == eDeleted)
    {
      streamTaskHandle = nullptr;
    }
  }

  if (appState == STATE_MAIN_UI && spkEnabled && connected)
  {
    if (millis() - lastReconnectAttempt > reconnectInterval)
    {
      Serial.printf("🔄 [%lu ms] 10分経過、再接続を試みます\n", millis());
      stopPlayback();
      connected = false;
      lastReconnectAttempt = millis();
    }
  }

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
    break;
  }
  case STATE_FREQ_INPUT:
    handleFreqInputScreen();
    return;
  }
}

void statusTask(void *param)
{
  while (true)
  {
    if (appState == STATE_MAIN_UI)
    {
      RigStatus st = fetchRigStatus();
      if (st.valid)
      {
        // 共有変数にコピー（mutexがあるとより安全）
        sharedFreq = st.freq;
        sharedMode = st.mode;
        sharedModel = st.model;
        sharedSignal = st.signal;
        sharedTx = st.tx;
        needRedraw = true;
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); // 100msごとに取得
  }
}

void streamTask(void *param)
{
  Serial.println("streamTask 開始");
  connected = false;
  bool success = false;
  for (int i = 0; i < retries; ++i)
  {
    Serial.printf("ストリーム接続試行 %d 回目...\n", i + 1);
    String streamUrl = "http://" + HostName + ":" + String(audioPort) + "/radio/audio/?rate=" + String(samplingRates[selSampling]);
    file = new AudioFileSourceICYStream(streamUrl.c_str());

    if (file && file->isOpen())
    {
      success = true;
      break;
    }
    delete file;
    file = nullptr;
    delay(500);
  }

  if (!success)
  {
    Serial.println("ストリーム接続失敗！");
    connected = false;
    vTaskDelete(NULL);
    return;
  }

  if (file && file->isOpen())
  {
    Serial.println("ICYStream open OK!");

    delay(1000);
    if (!file->isOpen())
    {
      Serial.println("⚠️ ICYStream dropped after open!");
      delete file;
      file = nullptr;
      connected = false;
      streamTaskHandle = nullptr;
      vTaskDelete(NULL);
      return;
    }

    success = true;
  }

  buff = new AudioFileSourceBuffer(file, 2048);
  if (!buff)
  {
    Serial.println("AudioFileSourceBuffer 作成失敗！");
    delete file;
    file = nullptr;
    connected = false;
    streamTaskHandle = nullptr;
    vTaskDelete(NULL);
    return;
  }

  out = new AudioOutputI2S();
  if (!out)
  {
    Serial.println("AudioOutputI2S 作成失敗！");
    delete buff;
    delete file;
    buff = nullptr;
    file = nullptr;
    connected = false;
    streamTaskHandle = nullptr;
    vTaskDelete(NULL);
    return;
  }

  if (selSampling < 0 || selSampling >= samplingRates.size())
  {
    Serial.println("⚠️ selSampling が範囲外です！");
  }

  out->SetPinout(SYS_I2S_SCLK_PIN, SYS_I2S_LRCK_PIN, SYS_I2S_DOUT_PIN);
  out->SetOutputModeMono(false);
  out->SetChannels(2);
  out->SetRate(samplingRates[selSampling]);
  out->SetBitsPerSample(16);
  out->SetGain(currentVolume);
  if (!out->begin())
  {
    Serial.println("AudioOutputI2S begin failed!");
    delete out;
    out = nullptr;
    connected = false;
    vTaskDelete(NULL);
    return;
  }
  connected = true;
  Serial.println("再生開始！");
  streamTaskHandle = nullptr;
  vTaskDelete(NULL);
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

  Serial.println("再生停止");
}

// 🎧 再生専用タスク
void playbackTask(void *pvParameters)
{
  static uint8_t buffer[1024];
  while (true)
  {
    if (!connected || !out || !buff || !buff->isOpen())
    {
      delay(100);
      continue;
    }
    static bool wasPlaying = false;
    bool nowPlaying = connected && buff && buff->isOpen() && out;

    if (nowPlaying && !wasPlaying)
    {
      Serial.println("🎶 再生ループ開始！");
    }
    wasPlaying = nowPlaying;

    static bool lastOpen = false;
    bool nowOpen = file && file->isOpen();
    if (nowOpen != lastOpen)
    {
      Serial.printf("ICYStream open %s!\n", nowOpen ? "OK" : "failed");
      lastOpen = nowOpen;
    }

    if (buff && buff->isOpen() && out)
    {
      int len = buff->read(buffer, sizeof(buffer));
      if (len > 0 && out)
      {
        int sampleCount = len / 2;
        int16_t *samples16 = reinterpret_cast<int16_t *>(buffer);

        for (int i = 0; i < sampleCount; ++i)
        {
          int16_t sample[2] = {samples16[i], samples16[i]};
          int retry = 0;
          while (out && !out->ConsumeSample(sample))
          {
            delay(1);
            if (++retry > 1000)
            {
              Serial.println("⚠️ ConsumeSample timeout!");
              break;
            }
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