# MyFirstKeyboard

このリポジトリは、MyFirstKeyboard専用PCB向けのファームウェアです。

## 説明

- このファームウェアは、MyFirstKeyboard基板を使って動作するカスタムキーボード用。
- キースイッチは7つで、ピンは A7, A6, A1, A9, A10, A8, A2 に接続。
- チャタリング抑止のためのデバウンス処理や、pressed/releasedのキーイベント処理を実装。

## 書き込み方法

1. Arduino IDEで「Seeeduino XIAO」を選択。（必要ならボードマネージャから追加してください）
2. このリポジトリをクローンまたはダウンロードして、Arduino IDEでMyFirstKeyboard.inoを開く。
3. MyFirstKeyboard専用PCBを接続し、ボードマネージャで Seeeduino XIAO を選択してスケッチをアップロード。

## TODO

- シリアルコマンドでキーの設定変更

## Key Remap Tool (ブラウザUI)
`KeyRemapTool` フォルダに、Web Serial API を使ったキーリマップ用のブラウザツールを同梱しています。
- 必要条件: Web Serial API 対応ブラウザ（Chrome, Edge 等）
- サーバ起動例:
  ```bash
  cd KeyRemapTool
  python3 -m http.server 8000
  ```
- ブラウザで `http://localhost:8000` を開き、以下を操作します。
  1. [Connect] ボタンで XIAO と接続
  2. [Refresh] ボタンで現在のマッピングを取得
  3. ドロップダウンから各キーを選択（`Random` でランダムモード）
  4. [Apply] ボタンで設定を送信・フラッシュに保存

## ライセンス

MIT
