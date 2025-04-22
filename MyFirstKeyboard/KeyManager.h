#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include <Arduino.h>
#include <Keyboard.h>

// キーコードをuint8_tとして定義（16進テキストで表現）
typedef uint8_t keyCode;

class KeyObject {
public:
  KeyObject();
  void init(int pin); // ピンの初期化
  void update();
  
  void setKey(keyCode code);
  void setRandomKeys(keyCode codes[], int count);
  // Retrieve assigned key for persistence or querying
  keyCode getAssignedKey() const;
  
private:
  int pin;
  keyCode assignedKey;
  keyCode* randomKeyArray;
  int randomKeyCount;
  
  bool lastReading;
  unsigned long lastDebounceTime;
  const unsigned long debounceDelay = 30; // 30msデバウンス
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
  // Retrieve current key mapping
  keyCode getKey(int index) const;
  
private:
  int size;
  KeyObject* keys;
};

#endif // KEYMANAGER_H