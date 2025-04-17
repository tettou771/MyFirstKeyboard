#include "KeyManager.h"
#include <Keyboard.h>
#include <ofxSerialManager.h>

// Forward declaration for keyManager defined later
extern KeyManager keyManager;

// シリアルコマンド管理
ofxSerialManager serialManager;
// Flash storage for key mappings
#include <FlashStorage_SAMD.h>
#define MAX_KEYS 7
#define MAX_RANDOM 16
typedef struct {
  uint32_t magic;
  uint8_t isRandom[MAX_KEYS];
  uint8_t counts[MAX_KEYS];
  keyCode codes[MAX_KEYS][MAX_RANDOM];
} KeyMap;
#define KEYMAP_MAGIC 0x5A5A5A5A
FlashStorage(my_flash_store, KeyMap);

// Initialize flash mapping with defaults if needed
void initFlashMapping() {
  KeyMap km = my_flash_store.read();
  if (km.magic != KEYMAP_MAGIC) {
    km.magic = KEYMAP_MAGIC;
    for (int i = 0; i < MAX_KEYS; i++) {
      km.isRandom[i] = 0;
      km.counts[i] = 1;
      for (int j = 0; j < MAX_RANDOM; j++) km.codes[i][j] = 0;
    }
    // default mappings
    km.codes[0][0] = KEY_RETURN;
    km.codes[1][0] = ' ';
    km.codes[2][0] = 't';
    km.codes[3][0] = 'o';
    km.codes[4][0] = 'r';
    km.codes[5][0] = 'u';
    km.isRandom[6] = 1;
    km.counts[6] = 2;
    km.codes[6][0] = '!';
    km.codes[6][1] = '?';
    my_flash_store.write(km);
  }
}

// Write single key mapping to flash
void writeSingleMapping(int index, keyCode code) {
  KeyMap km = my_flash_store.read();
  km.isRandom[index] = 0;
  km.counts[index] = 1;
  km.codes[index][0] = code;
  my_flash_store.write(km);
}

// Write random key mapping to flash
void writeRandomMapping(int index, keyCode arr[], int count) {
  KeyMap km = my_flash_store.read();
  km.isRandom[index] = 1;
  km.counts[index] = count;
  for (int j = 0; j < count && j < MAX_RANDOM; j++) {
    km.codes[index][j] = arr[j];
  }
  my_flash_store.write(km);
}

// getコマンドのコールバック (JSON形式で返す)
void onGetKeyCommand(const char* payload, int length) {
  KeyMap km = my_flash_store.read();
  String resp = "{";
  for (int i = 0; i < MAX_KEYS; i++) {
    if (i) resp += ",";
    resp += "\"" + String(i) + "\":[";
    int cnt = (km.magic == KEYMAP_MAGIC ? km.counts[i] : 1);
    for (int j = 0; j < cnt; j++) {
      if (j) resp += ",";
      keyCode c = (km.magic == KEYMAP_MAGIC ? km.codes[i][j] : keyManager.getKey(i));
      resp += String(c);
    }
    resp += "]";
  }
  resp += "}";
  serialManager.send("get", resp.c_str());
}

// setkeyコマンドのコールバック
void onSetKeyCommand(const char* payload, int length) {
  char buf[256];
  int len = length < 255 ? length : 255;
  memcpy(buf, payload, len);
  buf[len] = '\0';
  char* token = strtok(buf, " ");
  if (!token) return;
  int index = atoi(token);
  keyCode keys[MAX_RANDOM];
  int keyCount = 0;
  while ((token = strtok(NULL, " ")) && keyCount < MAX_RANDOM) {
    keys[keyCount++] = (keyCode)token[0];
  }
  if (keyCount == 1) {
    keyManager.setKey(index, keys[0]);
    writeSingleMapping(index, keys[0]);
  } else if (keyCount > 1) {
    keyCode* arr = new keyCode[keyCount];
    for (int i = 0; i < keyCount; i++) arr[i] = keys[i];
    keyManager.setRandomKeys(index, arr, keyCount);
    writeRandomMapping(index, arr, keyCount);
  }
  serialManager.send("setkey", "ok");
}

// Seeeduino XIAO M0用カスタムキーボード
// 接続ピン順: A7, A6, A1, A9, A10, A8, A2
const int keyPins[7] = {A7, A6, A1, A9, A10, A8, A2};

// KeyManagerのインスタンス作成（キー数:7）
KeyManager keyManager(keyPins, 7);

void setup() {
  // シリアル通信開始
  Serial.begin(115200);
  // キーボードエミュレーション開始
  Keyboard.begin();
  // シリアルコマンドマネージャ初期化
  serialManager.setup(&Serial);
  serialManager.addListener("setkey", onSetKeyCommand);
  serialManager.addListener("get", onGetKeyCommand);
  
  // Initialize and load mapping from flash
  initFlashMapping();
  {
    KeyMap km = my_flash_store.read();
    for (int i = 0; i < MAX_KEYS; i++) {
      if (km.counts[i] > 1) {
        keyManager.setRandomKeys(i, km.codes[i], km.counts[i]);
      } else {
        keyManager.setKey(i, km.codes[i][0]);
      }
    }
  }
}

void loop() {
  keyManager.update();
  delay(10); // 小さな遅延でチャタリング対策
}