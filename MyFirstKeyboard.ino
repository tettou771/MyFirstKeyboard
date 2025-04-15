#include "KeyManager.h"
#include <Keyboard.h>

// Seeeduino XIAO M0用カスタムキーボード
// 接続ピン順: A7, A6, A1, A9, A10, A8, A2
const int keyPins[7] = {A7, A6, A1, A9, A10, A8, A2};

// KeyManagerのインスタンス作成（キー数:7）
KeyManager keyManager(keyPins, 7);

void setup() {
  Keyboard.begin();
  
  // キーの初期設定
  keyManager.setKey(0, KEY_RETURN); // key 0: enter
  keyManager.setKey(1, ' ');          // key 1: space
  keyManager.setKey(2, 'r');          // key 2: r
  keyManager.setKey(3, 'i');          // key 3: i
  keyManager.setKey(4, 't');          // key 4: t
  keyManager.setKey(5, 'o');          // key 5: o
  
  // key 6: ランダムキー：'!' と '?' をランダムで送信
  keyCode randomKeys[2] = {'!', '?'};
  keyManager.setRandomKeys(6, randomKeys, 2);
}

void loop() {
  keyManager.update();
  delay(10); // 小さな遅延でチャタリング対策
}