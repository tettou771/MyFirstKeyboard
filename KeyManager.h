#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include <Arduino.h>
#include <Keyboard.h>

// キーコードをintとして定義（Keyboard.write()はintを受け取る）
typedef int keyCode;

class KeyObject {
public:
  KeyObject();
  void init(int pin); // ピンの初期化
  void update();
  
  void setKey(keyCode code);
  void setRandomKeys(keyCode codes[], int count);
  
private:
  int pin;
  keyCode assignedKey;
  keyCode* randomKeyArray;
  int randomKeyCount;
  
  bool lastReading;
  unsigned long lastDebounceTime;
  const unsigned long debounceDelay = 50; // 50msデバウンス
  void onKeyPressed();
  void onKeyReleased();
  keyCode activeKey;
};

class KeyManager {
public:
  // コンストラクタ：各キーのピン番号を配列で渡す
  KeyManager(const int pins[], int numPins);
  
  void begin(); // 初期化用（必要なら追加処理）
  void update();
  
  void setKey(int index, keyCode code);
  void setRandomKeys(int index, keyCode codes[], int count);
  
private:
  int size;
  KeyObject* keys;
};

#endif // KEYMANAGER_H