#include "KeyManager.h"

KeyObject::KeyObject()
    : pin(-1), assignedKey(0), randomKeyArray(NULL), randomKeyCount(0), isString(false),
      lastReading(HIGH), lastDebounceTime(0) {
  strMapping[0] = '\0';
}

void KeyObject::init(int p) {
  pin = p;
  pinMode(pin, INPUT_PULLUP);
  lastDebounceTime = millis() - debounceDelay;
}

void KeyObject::setKey(keyCode code) {
  isString = false; // 文字列マッピングを解除
  assignedKey = code;
  randomKeyCount = 0; // 通常キーの場合
}

void KeyObject::setRandomKeys(keyCode codes[], int count) {
  isString = false; // 文字列マッピングを解除
  randomKeyArray = codes;
  randomKeyCount = count;
}

void KeyObject::setStringMapping(const char* s) {
  isString = true;
  randomKeyCount = 0;
  strncpy(strMapping, s, MAX_STR_LEN - 1);
  strMapping[MAX_STR_LEN - 1] = '\0';
}

bool KeyObject::isStringMappingEnabled() const {
  return isString;
}

void KeyObject::update() {
  bool reading = digitalRead(pin);

  unsigned long currentTime = millis();
  if (reading != lastReading && (currentTime - lastDebounceTime) >= debounceDelay) {
    if (reading == LOW) { // LOW indicates a press with INPUT_PULLUP
      onKeyPressed();
    } else { // HIGH indicates a release
      onKeyReleased();
    }
    lastDebounceTime = currentTime;
  }
  lastReading = reading;
}

// UTF-32をUTF-8にエンコードするヘルパー関数を追加
static int utf8_encode(uint32_t cp, uint8_t *out) {
  if (cp <= 0x7F) { out[0] = cp; return 1; }
  if (cp <= 0x7FF) {
    out[0] = 0xC0 | (cp >> 6);
    out[1] = 0x80 | (cp & 0x3F);
    return 2;
  }
  if (cp <= 0xFFFF) {
    out[0] = 0xE0 | (cp >> 12);
    out[1] = 0x80 | ((cp >> 6) & 0x3F);
    out[2] = 0x80 | (cp & 0x3F);
    return 3;
  }
  out[0] = 0xF0 | (cp >> 18);
  out[1] = 0x80 | ((cp >> 12) & 0x3F);
  out[2] = 0x80 | ((cp >> 6) & 0x3F);
  out[3] = 0x80 | (cp & 0x3F);
  return 4;
}

void KeyObject::onKeyPressed() {
  if (isString) {
    Keyboard.print(strMapping);
    return;
  }
  if (randomKeyCount > 0) {
    int randIndex = random(randomKeyCount);
    activeKey = randomKeyArray[randIndex];
  } else {
    activeKey = assignedKey;
  }
  // モディファイアキー (0xE0=Ctrl, 0xE1=Shift, 0xE2=Alt, 0xE3=GUI)
  if (activeKey == 0xE0) {
    Keyboard.press(KEY_LEFT_CTRL);
  } else if (activeKey == 0xE1) {
    Keyboard.press(KEY_LEFT_SHIFT);
  } else if (activeKey == 0xE2) {
    Keyboard.press(KEY_LEFT_ALT);
  } else if (activeKey == 0xE3) {
    Keyboard.press(KEY_LEFT_GUI);
  }
  // Appleロゴ(0x00)用の特殊処理: Shift+Alt+'k'
  else if (activeKey == 0x00) {
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press('k');
  }
  // 通常キー(1バイト)は press
  else if (activeKey <= 0xFF) {
    Keyboard.press((uint8_t)activeKey);
  }
}

void KeyObject::onKeyReleased() {
  if (isString) return;
  // モディファイアキーのリリース
  if (activeKey == 0xE0) {
    Keyboard.release(KEY_LEFT_CTRL);
  } else if (activeKey == 0xE1) {
    Keyboard.release(KEY_LEFT_SHIFT);
  } else if (activeKey == 0xE2) {
    Keyboard.release(KEY_LEFT_ALT);
  } else if (activeKey == 0xE3) {
    Keyboard.release(KEY_LEFT_GUI);
  }
  // Appleロゴ(0x00)は Shift+Alt+'k'をリリース
  else if (activeKey == 0x00) {
    Keyboard.release(KEY_LEFT_SHIFT);
    Keyboard.release(KEY_LEFT_ALT);
    Keyboard.release('k');
  }
  // 通常キーは release
  else if (activeKey <= 0xFF) {
    Keyboard.release((uint8_t)activeKey);
  }
  activeKey = 0;
}
// Retrieve assigned key
keyCode KeyObject::getAssignedKey() const {
  return assignedKey;
}

KeyManager::KeyManager(const int pins[], int numPins) : size(numPins) {
  keys = new KeyObject[size];
  for (int i = 0; i < size; i++) {
    keys[i].init(pins[i]);
  }
}

void KeyManager::begin() {
  // 必要なら追加の初期化処理を記述
}

void KeyManager::setKey(int index, keyCode code) {
  if (index >= 0 && index < size) {
    keys[index].setKey(code);
  }
}

void KeyManager::setRandomKeys(int index, keyCode codes[], int count) {
  if (index >= 0 && index < size) {
    keys[index].setRandomKeys(codes, count);
  }
}

void KeyManager::setStringMapping(int index, const char* s) {
  if (index >= 0 && index < size) {
    keys[index].setStringMapping(s);
  }
}

void KeyManager::update() {
  for (int i = 0; i < size; i++) {
    keys[i].update();
  }
}
// Retrieve current key mapping for index
keyCode KeyManager::getKey(int index) const {
  if (index >= 0 && index < size) {
    return keys[index].getAssignedKey();
  }
  return 0;
}

// 追加: 文字列マッピング取得

const char* KeyObject::getStringMapping() const {
  return strMapping;
}

const char* KeyManager::getStringMapping(int index) const {
  if (index >= 0 && index < size && keys[index].isStringMappingEnabled()) {
    return keys[index].getStringMapping();
  }
  return "";
}

// 追加: 文字列モードか判定
bool KeyManager::isStringMappingEnabled(int index) const {
  if (index >= 0 && index < size) {
    return keys[index].isStringMappingEnabled();
  }
  return false;
}