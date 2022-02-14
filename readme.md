# TETRIS

<<<<<<< HEAD
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
=======
openCV繧剃ｽｿ逕ｨ縺励◆繝�繝医Μ繧ｹ縺ｧ縺�.
謫堺ｽ懷�･蜉帙↓縺ｯ繧ｲ繝ｼ繝�繝代ャ繝峨ｒ菴ｿ逕ｨ縺励∪縺�.

# Build
Windows 10  
Visual Srudio 2019(v142)  
C++ 14  

# Features
>>>>>>> origin/master

Hold繝ｻNext縺ｪ縺ｩ蝓ｺ譛ｬ逧�縺ｪ讖溯�ｽ縺ゅｊ
T-spin繝ｻTetris蛻､螳壹≠繧�
謚�繧�豸医＠縺溘Λ繧､繝ｳ謨ｰ縺ｫ繧医ｋ繧ｹ繧ｳ繧｢險育ｮ励≠繧�
險ｭ螳壹ヵ繧｡繧､繝ｫ縺ｫ繧医ｊ, 閾ｪ逕ｱ縺ｫ繧ｹ繧ｳ繧｢螟画峩繧�繝ｬ繝吶Ν霑ｽ蜉�蜿ｯ閭ｽ
繧ｲ繝ｼ繝�繝代ャ繝画磁邯夂憾豕√↓繧医▲縺ｦ繝槭Ν繝√�励Ξ繧､蜿ｯ閭ｽ

荳願ｨ倥☆縺ｹ縺ｦ縺昴ｍ縺｣縺ｦ縺�縺ｪ縺�縺ｨ蜍穂ｽ懊＠縺ｾ縺帙ｓ縺ｮ縺ｧ縺疲ｳｨ諢上￥縺�縺輔＞.

## Requirement
 
<<<<<<< HEAD
*  openCV 4.x (https://github.com/opencv/opencv)
*  nlohmann/json (https://github.com/nlohmann/json)

## Note

使用する画像は入っておりませんので, imageフォルダ内に入れてください.

画像の詳細  
256x256px (これ以外でも動作しますが, このサイズを推奨します)  
「0.png」〜「8.png」  
それぞれ  
*  0.png : 背景 (黒)  
*  1.png : I-テトリミノ (水色)  
*  2.png : O-テトリミノ (黄色)  
*  3.png : S-テトリミノ (緑色)  
*  4.png : Z-テトリミノ (赤色)  
*  5.png : J-テトリミノ (青色)  
*  6.png : L-テトリミノ (オレンジ色)  
*  7.png : T-テトリミノ (紫色)  
*  8.png : おじゃまブロック (灰色)  
=======
* openCV 4.x (https://github.com/opencv/opencv)
* nlohmann/json (https://github.com/nlohmann/json)

# Note

菴ｿ逕ｨ縺吶ｋ逕ｻ蜒上�ｯ蜈･縺｣縺ｦ縺翫ｊ縺ｾ縺帙ｓ縺ｮ縺ｧ, image繝輔か繝ｫ繝�蜀�縺ｫ蜈･繧後※縺上□縺輔＞.

逕ｻ蜒上�ｮ隧ｳ邏ｰ  
256x256px (縺薙ｌ莉･螟悶〒繧ょ虚菴懊＠縺ｾ縺吶′, 縺薙�ｮ繧ｵ繧､繧ｺ繧呈耳螂ｨ縺励∪縺�)  
縲�0.png縲搾ｽ槭��8.png縲�  
縺昴ｌ縺槭ｌ  
* 0.png : 閭梧勹 (鮟�)  
* 1.png : I-繝�繝医Μ繝溘ヮ (豌ｴ濶ｲ)  
* 2.png : O-繝�繝医Μ繝溘ヮ (鮟�濶ｲ)  
* 3.png : S-繝�繝医Μ繝溘ヮ (邱題牡)  
* 4.png : Z-繝�繝医Μ繝溘ヮ (襍､濶ｲ)  
* 5.png : J-繝�繝医Μ繝溘ヮ (髱定牡)  
* 6.png : L-繝�繝医Μ繝溘ヮ (繧ｪ繝ｬ繝ｳ繧ｸ濶ｲ)  
* 7.png : T-繝�繝医Μ繝溘ヮ (邏ｫ濶ｲ)  
* 8.png : 縺翫§繧�縺ｾ繝悶Ο繝�繧ｯ (轣ｰ濶ｲ)  
>>>>>>> origin/master

## License
 
TETRIS is under [MIT license](https://en.wikipedia.org/wiki/MIT_License).

<<<<<<< HEAD
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
=======
# References

繝�繝医Μ繧ｹ 
繧ｹ繧ｳ繧｢險育ｮ� http://www.terasol.co.jp/%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0/6841  
轣ｫ蜉幄ｨ育ｮ�1 https://ch-random.net/post/96/  
轣ｫ蜉幄ｨ育ｮ�2 https://w.atwiki.jp/puyopuyo_tetris/pages/98.html  
縺翫§繧�縺ｾ繝悶Ο繝�繧ｯ縺ｮ莉墓ｧ� https://tetris-matome.com/garbage/  
T-spin蛻､螳� https://tetris-matome.com/judgment/  
SRS https://tetrisch.github.io/main/srs.html  

openCV   
逕ｻ蜒剰ｲｼ繧贋ｻ倥¢ https://kougaku-navi.hatenablog.com/entry/20160108/p1  
繧ｦ繧｣繝ｳ繝峨え菴咲ｽｮ螟画峩 https://qiita.com/Kazuhito/items/b2ebd9f9010f1ffcac5b  

繧ｲ繝ｼ繝�繝代ャ繝牙�･蜉�
XInput1 http://gameprogrammingunit.web.fc2.com/xinput/index.html  
XInput2 http://pc98.skr.jp/post/2019/0819/  

>>>>>>> origin/master
