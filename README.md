# Meeting Timer for M5Stack

M5Stack Gray v2.7 を使用した、シンプルで美しい物理会議タイマーです。
Mac版・Android版の `meeting-timer` プロジェクトの機能を M5Stack に移植しました。

## 主な機能

- **NTP 時刻同期**: Wi-Fi 経由でインターネットから正確な日本時間を取得し、待機中に表示します。
- **クイックタイマー**:
  - **Btn A**: 5分カウントダウン開始
  - **Btn B**: 10分カウントダウン開始
  - **Btn C**: ストップ / リセット
- **プレミアム UI**:
  - 滑らかなベクターフォント（FreeSansBold）を使用。
  - カウントダウンは視認性の高い 1.5倍サイズで表示。
  - ダークモード基調のモダンなデザイン。
- **カスタムアラーム**: 終了時に「ピピピ、ピピピ、ピピピ」と3連音を3回鳴らしてお知らせします。

## 開発環境

- **Framework**: Arduino
- **Library**: M5Unified, M5GFX
- **Toolchain**: PlatformIO

## セットアップ

1. `src/main.cpp` の冒頭にある Wi-Fi 設定をご自身の環境に合わせて書き換えてください。

   ```cpp
   const char *ssid = "YOUR_SSID";
   const char *password = "YOUR_PASSWORD";
   ```

2. PlatformIO を使用してビルドおよび書き込みを行います。

   ```bash
   pio run --target upload
   ```

## ハードウェア構成

- M5Stack Gray v2.7 (ESP32)
- 320x240 TFT LCD
- 3 Buttons (A, B, C)
- Internal Speaker
