#include "KeyManager.h"

KeyObject::KeyObject()
    : pin(-1), assignedKey(0), randomKeyArray(NULL), randomKeyCount(0),
      lastReading(HIGH), lastDebounceTime(0) {}

void KeyObject::init(int p) {
  pin = p;
  pinMode(pin, INPUT_PULLUP);
  lastDebounceTime = millis() - debounceDelay;
}

void KeyObject::setKey(keyCode code) {
  assignedKey = code;
  randomKeyCount = 0; // 通常キーの場合
}

void KeyObject::setRandomKeys(keyCode codes[], int count) {
  randomKeyArray = codes;
  randomKeyCount = count;
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

void KeyObject::onKeyPressed() {
  if (randomKeyCount > 0) {
    int randIndex = random(randomKeyCount);
    activeKey = randomKeyArray[randIndex];
  } else {
    activeKey = assignedKey;
  }
  Keyboard.press(activeKey);
}

void KeyObject::onKeyReleased() {
  Keyboard.release(activeKey);
  activeKey = 0;
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

void KeyManager::update() {
  for (int i = 0; i < size; i++) {
    keys[i].update();
  }
}