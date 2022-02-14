# TETRIS

openCVを使用したテトリスです.
操作入力にはゲームパッドを使用します.

# Features

Hold・Nextなど基本的な機能あり
T-spin・Tetris判定あり
技や消したライン数によるスコア計算あり
設定ファイルにより, 自由にスコア変更やレベル追加可能
ゲームパッド接続状況によってマルチプレイ可能

上記すべてそろっていないと動作しませんのでご注意ください.

# Requirement
 
* openCV 4.0 (https://github.com/opencv/opencv)
* nlohmann/json (https://github.com/nlohmann/json)

# Note

使用する画像は入っておりませんので, imageフォルダ内に入れてください.

画像の詳細  
256x256px
「0.png」～「8.png」  
それぞれ  
*0.png : 背景 (黒)  
*1.png : I-テトリミノ (水色)  
*2.png : O-テトリミノ (黄色)  
*3.png : S-テトリミノ (緑色)  
*4.png : Z-テトリミノ (赤色)  
*5.png : J-テトリミノ (青色)  
*6.png : L-テトリミノ (オレンジ色)  
*7.png : T-テトリミノ (紫色)  
*8.png : おじゃまブロック (灰色)  

# License
 
TETRIS is under [MIT license](https://en.wikipedia.org/wiki/MIT_License).

# References

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

