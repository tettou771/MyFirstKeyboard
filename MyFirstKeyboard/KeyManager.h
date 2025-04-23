#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include <Arduino.h>
#include <Keyboard.h>

// キーコードをuint8_tとして定義（16進テキストで表現）
typedef uint8_t keyCode;

#define MAX_STR_LEN 64 // 最大文字列長

class KeyObject {
public:
  KeyObject();
  void init(int pin); // ピンの初期化
  void setStringMapping(const char* s);
  void update();
  
  void setKey(keyCode code);
  void setRandomKeys(keyCode codes[], int count);
  // Retrieve assigned key for persistence or querying
  keyCode getAssignedKey() const;
  bool isStringMappingEnabled() const; // 追加: 文字列マッピングか判定
  const char* getStringMapping() const; // 追加: 文字列マッピング取得
  
private:
  int pin;
  keyCode assignedKey;
  keyCode* randomKeyArray;
  int randomKeyCount;
  bool isString;                // 文字列マッピングか
  char strMapping[MAX_STR_LEN]; // 文字列マッピングバッファ
  
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
  void setStringMapping(int index, const char* s);
  bool isStringMappingEnabled(int index) const; // 追加: 文字列マッピングか判定 (KeyManager)
  const char* getStringMapping(int index) const; // 追加: キーの文字列マッピングを取得
  // Retrieve current key mapping
  keyCode getKey(int index) const;
  
private:
  int size;
  KeyObject* keys;
};

#endif // KEYMANAGER_H