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

## ライセンス

MIT
