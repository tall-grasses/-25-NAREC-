const int ledPin = 9;       // 状態表示用のLEDピン
const int buttonPin = 2;    // ボタンが接続されているピン

#include <Keyboard.h>
String morseCode = "";      // モールス信号の記録
unsigned long lastInputTime = 0;  // 最後に入力された時間
unsigned long pressStartTime = 0; // ボタンが押され始めた時間
unsigned long lastDebounceTime = 0; // デバウンス用のタイマー
bool isPressing = false;         // ボタンが押されているかどうかのフラグ
bool lastButtonState = HIGH;     // 前回のボタン状態

const unsigned long debounceDelay = 50;   // デバウンスの遅延（ミリ秒）
const unsigned long dotThreshold = 200;  // ドットとダッシュの判定閾値（ミリ秒）
const unsigned long charPause = 800;     // 文字の区切り時間（ミリ秒）

// モールス信号 → アルファベットの対応表
const String morseAlphabet[] = { ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", 
                                 ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", 
                                 "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." };
const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// decodeMorse関数のプロトタイプ宣言
char decodeMorse(String code);

void setup() {
  Keyboard.begin();
  pinMode(buttonPin, INPUT_PULLUP); // ボタン入力（プルアップ設定）
  pinMode(ledPin, OUTPUT);          // LED出力
  Serial.begin(9600);               // シリアル通信開始
}

void loop() {
  int reading = digitalRead(buttonPin);
  unsigned long currentTime = millis();

  // デバウンス処理
  if (reading != lastButtonState) {
    lastDebounceTime = currentTime;
  }

  if ((currentTime - lastDebounceTime) > debounceDelay) {
    // ボタンが押された瞬間を検出
    if (reading == LOW && !isPressing) {
      isPressing = true;             // 押下開始を記録
      pressStartTime = currentTime;  // 押された時間を記録
      digitalWrite(ledPin, HIGH);    // LED点灯
    }

    // ボタンが離された瞬間を検出
    if (reading == HIGH && isPressing) {
      isPressing = false;              // 押下状態解除
      unsigned long pressDuration = currentTime - pressStartTime;

      // ドットとダッシュの判定
      if (pressDuration < dotThreshold) {
        morseCode += "."; // ドット
      } else {
        morseCode += "-"; // ダッシュ
      }

      lastInputTime = currentTime; // 最後の入力時間を記録
      digitalWrite(ledPin, LOW);   // LED消灯

      // デバッグ用：モールス信号を表示
      Serial.print("Current Morse Code: ");
      Serial.println(morseCode);
    }
  }

  lastButtonState = reading; // ボタンの状態を更新

  // 文字区切りの判定
  if (morseCode.length() > 0 && (currentTime - lastInputTime) > charPause) {
    char letter = decodeMorse(morseCode);
    Serial.print("Decoded Character: ");
    if (letter != '?') {
      Serial.println(letter);
      Keyboard.print(letter);
    } else {
      Serial.println("[?]");
      Keyboard.print("[?]");
    }
    morseCode = ""; // リセット
  }
}

// モールス信号をアルファベットに変換
char decodeMorse(String code) {
  for (int i = 0; i < 26; i++) {
    if (morseAlphabet[i] == code) {
      return alphabet[i];
    }
  }
  return '?'; // 対応なし
}
