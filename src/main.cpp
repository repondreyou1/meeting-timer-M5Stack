#include "secrets.h"
#include <M5Unified.h>
#include <WiFi.h>
#include <time.h>

// --- Wi-Fi 設定 (include/secrets.h で定義) ---
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
// ------------------

// NTP設定
const char *ntpServer = "ntp.jst.mfeed.ad.jp";
const long gmtOffset_sec = 9 * 3600; // JST (UTC+9)
const int daylightOffset_sec = 0;

// タイマー状態
enum TimerState { IDLE, COUNTDOWN, FINISHED };
TimerState currentState = IDLE;
long targetTimeMillis = 0;
long remainingSeconds = 0;

// デザイン設定
const uint32_t COLOR_BG = 0x000000;
const uint32_t COLOR_MAIN = 0xFFFFFF;
const uint32_t COLOR_5MIN = 0x00FF00;  // 元の鮮やかな緑
const uint32_t COLOR_10MIN = 0x00FFFF; // 元の鮮やかな水色
const uint32_t COLOR_STOP = 0xFF0000;  // 元の鮮やかな赤
const uint32_t COLOR_DIM = 0x444444;

// プロトタイプ宣言
void drawUI_Static();
void displayClock();
void displayTimer();
void startTimer(int minutes);

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  M5.Display.setRotation(1);
  M5.Display.setBrightness(128);
  M5.Display.fillScreen(COLOR_BG);
  M5.Display.setTextDatum(middle_center);

  // 接続中表示
  M5.Display.setFont(&fonts::FreeSansBold12pt7b);
  M5.Display.setTextColor(COLOR_MAIN);
  M5.Display.drawString("Connecting to Wi-Fi...", 160, 120);

  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }

  M5.Display.fillScreen(COLOR_BG);
  drawUI_Static();
}

void drawUI_Static() {
  // 下部のボタンガイド（元のシンプルなスタイルに戻す）
  M5.Display.setFont(&fonts::FreeSans9pt7b);
  M5.Display.setTextSize(1.0);

  // Btn A: 5min
  M5.Display.setTextColor(COLOR_5MIN, COLOR_BG);
  M5.Display.drawString("5 MIN", 64, 220);

  // Btn B: 10min
  M5.Display.setTextColor(COLOR_10MIN, COLOR_BG);
  M5.Display.drawString("10 MIN", 160, 220);

  // Btn C: Stop
  M5.Display.setTextColor(COLOR_STOP, COLOR_BG);
  M5.Display.drawString("STOP", 256, 220);
}

void displayClock() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[20];
    sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min,
            timeinfo.tm_sec);

    // 時刻のみ表示（日付は削除）
    M5.Display.setFont(&fonts::FreeSans12pt7b);
    M5.Display.setTextSize(1.0);
    M5.Display.setTextColor(COLOR_MAIN, COLOR_BG);
    M5.Display.drawString(timeStr, 160, 35);
  }
}

void displayTimer() {
  static long lastSec = -1;
  static TimerState lastState = IDLE;

  if (remainingSeconds == lastSec && currentState == lastState &&
      currentState != FINISHED)
    return;
  lastSec = remainingSeconds;
  lastState = currentState;

  int mins = remainingSeconds / 60;
  int secs = remainingSeconds % 60;
  char timerStr[10];
  sprintf(timerStr, "%02d:%02d", mins, secs);

  // カウントダウンタイマーは大きく（1.5倍）
  M5.Display.setFont(&fonts::FreeSansBold24pt7b);
  M5.Display.setTextSize(1.5);

  if (currentState == FINISHED) {
    uint32_t flashColor = (millis() / 500) % 2 == 0 ? COLOR_STOP : COLOR_BG;
    M5.Display.setTextColor(flashColor, COLOR_BG);
  } else if (currentState == COUNTDOWN) {
    M5.Display.setTextColor(COLOR_MAIN, COLOR_BG);
  } else {
    M5.Display.setTextColor(COLOR_DIM, COLOR_BG);
  }

  M5.Display.drawString(timerStr, 160, 120);
}

void startTimer(int minutes) {
  remainingSeconds = minutes * 60;
  targetTimeMillis = millis() + (remainingSeconds * 1000);
  currentState = COUNTDOWN;
  M5.Speaker.tone(1000, 50);
  M5.Display.fillScreen(COLOR_BG);
  drawUI_Static();
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed())
    startTimer(5);
  if (M5.BtnB.wasPressed())
    startTimer(10);
  if (M5.BtnC.wasPressed()) {
    currentState = IDLE;
    remainingSeconds = 0;
    M5.Speaker.tone(500, 100);
    M5.Display.fillScreen(COLOR_BG);
    drawUI_Static();
  }

  if (currentState == COUNTDOWN) {
    long now = millis();
    if (now >= targetTimeMillis) {
      currentState = FINISHED;
      remainingSeconds = 0;
      // ピピピ、ピピピ、ピピピと3連を3回鳴らす
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          M5.Speaker.tone(2000, 100);
          delay(200);
        }
        delay(400); // 3連の間の少し長い休み
      }
    } else {
      remainingSeconds = (targetTimeMillis - now) / 1000;
    }
  }

  displayClock();
  displayTimer();

  delay(1);
}
