概要
====

i2c を使用した7セグメントLED表示器です。  
Raspberry PI は i2c が利用できるので Raspberry PI のコマンドラインから使用できます。  


### パーツ

- PIC18F14K50 (SOIC)

- TD62083 (SOIC)

- ダイナミック接続４桁高輝度赤色７セグメントＬＥＤ表示器　カソードコモン　カソード共通接続  
 http://akizukidenshi.com/catalog/g/gI-03955/

- チップ抵抗  
 270Ω  1/4W x 8  

- チップコンデンサ  
 0.1uF  
 0.22uF  


### I2C通信仕様

- 通信速度 100kHz
- i2C スレーブ
- ライト要求 (Master -> Slaveへのライト要求) のみ

### コマンド

| address | data   | description |
|---------|--------|-------------|
| 0x00    | なし   | リセット    |
| 0x01    | なし   | LCD消灯     |
| 0x02    | 1..8   | 最大ページ数設定  |
| 0x03    | 0..255 | スクロール速度設定 (0.1秒 * (指定値+1)) *1  |
| 0x04    | 0..255 | ページ切り替え速度設定 (0.1秒 * (指定値+1))  |
| 0x05    | 0..255 | 点滅速度設定 (0.1秒 * (指定値+1))  |
| 0x06    | 0..1   | スクロール設定 (0:disable 1:enable)|
| 0x07    | 0..1   | 点滅設定 (0:disable 1:enable) |
| 0x10    | 0..7   | カレントページ設定 *2 |
| 0x11    | 文字列 | 描画文字列を指定、終端は 0x00 を指定します *3 |

*1 ... 文字のスクロール速度になります。例えば 19 を指定した場合 0.1*(19+1) で2秒単位にスクロールします。
*2 ... 0 から数えるページ数を指定します。範囲は 0 から 最大ページ数-1 になります。
*3 ... 描画文字列は ASCII コードで指定します。表示可能文字は数字、スペース、ハイフン、アンダースコア、パイプ、アルファベット（大文字、小文字）、ピリオドです。
