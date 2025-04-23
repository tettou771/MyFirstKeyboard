# MyFirstKeyboard

**Author**: tettou771

## 概要

MyFirstKeyboard専用PCB（MFKB基板）向けのカスタムキーボードファームウェアと、Webベースのキーリマップツールを提供します。

### ファームウェア
- Seeeduino XIAO（M0搭載）用スケッチ
- 7キーのスイッチをDebounce処理付きで読み取り
- USB HIDキーボードとしてキー押下／リリースを送信
- シリアルコマンドでキー設定をフラッシュ保存

### キーリマップツール (KeyRemapTool)
- `KeyRemapTool/index.html` をChromeやEdgeなどのWeb Serial API対応ブラウザで開くだけ
- サーバ起動などは不要です

## ボードマネージャの追加

Arduino IDEでSeeeduino XIAOを使うには、環境設定の「追加のボードマネージャのURL」に以下を追加してください：

```
https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
```

## ファームウェア書き込み方法
1. Arduino IDEを起動し、環境設定でボードマネージャURLを追加
2. 「ツール」→「ボード」→「ボードマネージャ」を開き、Seeeduino XIAOをインストール
3. 本リポジトリをクローンまたはダウンロード
4. Arduino IDEで `MyFirstKeyboard.ino` を開く
5. MFKB基板をUSB接続し、「ツール」→「ボード」でSeeeduino XIAO、「ツール」→「ポート」で該当ポートを選択
6. 「スケッチ」→「アップロード」で書き込み

## Key Remap Toolの使い方
1. `KeyRemapTool/index.html` を対応ブラウザで開く
2. [Connect] を押してデバイスと接続
3. [Refresh] で現在のマッピングを取得
4. ドロップダウンやStr入力欄でキーを設定
5. [Apply] を押して設定を送信＆フラッシュ保存

## ライセンス

MIT License
