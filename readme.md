![openCV ver](https://img.shields.io/badge/openCV-%3E%3D4.0-blue)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/0fe72812e4fe459cb0bb404f857cc47f)](https://www.codacy.com/gh/Kei-meijo/TETRIS/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Kei-meijo/TETRIS&amp;utm_campaign=Badge_Grade)
![Code Size](https://img.shields.io/github/languages/code-size/Kei-meijo/TETRIS)
![version](https://img.shields.io/badge/version-1.2.1-blue)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/Kei-meijo/TETRIS/blob/master/LICENSE.MIT)

# TETRIS

openCVを使用したテトリスです.
操作入力にはゲームパッドを使用します.

## Build
Windows 10  
Visual Srudio 2019(v142)  
C++ 14  

## Features

Hold・Nextなど基本的な機能あり
T-spin・Tetris判定あり
技や消したライン数によるスコア計算あり
設定ファイルにより, 自由にスコア変更やレベル追加可能
ゲームパッド接続状況によってマルチプレイ可能

上記すべてそろっていないと動作しませんのでご注意ください.

## Requirement
 
[openCV 4.x](https://github.com/opencv/opencv)  
[nlohmann/json](https://github.com/nlohmann/json)

## Note

使用する画像は入っておりませんので, imageフォルダ内に入れてください.

画像の詳細  
256x256px (これ以外でも動作しますが, このサイズを推奨します)  
「0.png」～「8.png」  
| ブロック名 | 画像名 |
| ------------- | ------------- |
| 背景 (黒)  | 0.png  |
| I-テトリミノ (水色)  | 1.png  |
| O-テトリミノ (黄色)  | 2.png  |
| S-テトリミノ (緑色)  | 3.png  |
| Z-テトリミノ (赤色)  | 4.png  |
| J-テトリミノ (青色)  | 5.png  |
| L-テトリミノ (オレンジ色)   | 6.png  |
| T-テトリミノ (紫色)   | 7.png  |
| おじゃまブロック (灰色)  | 8.png  |


## License
 
TETRIS is under [MIT license](https://en.wikipedia.org/wiki/MIT_License).

## References

テトリス 
スコア計算 http://www.terasol.co.jp/%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0/6841  
火力計算1 https://ch-random.net/post/96/  
火力計算2 https://w.atwiki.jp/puyopuyo_tetris/pages/98.html  
おじゃまブロックの仕様 https://tetris-matome.com/garbage/  
T-spin判定 https://tetris-matome.com/judgment/  
SRS https://tetrisch.github.io/main/srs.html  

openCV   
画像貼り付け https://kougaku-navi.hatenablog.com/entry/20160108/p1  
ウィンドウ位置変更 https://qiita.com/Kazuhito/items/b2ebd9f9010f1ffcac5b  

ゲームパッド入力
XInput1 http://gameprogrammingunit.web.fc2.com/xinput/index.html  
XInput2 http://pc98.skr.jp/post/2019/0819/  

FPS制御  
fps https://hakase0274.hatenablog.com/entry/2019/10/06/234413
