#pragma once
#include <vector>
//#include "board.h"

class Config {
public:
	//操作一覧
	static const int NONE = 0;
	static const int LEFT = 1;
	static const int RIGHT = 2;
	static const int ROTATION_CW = 4;
	static const int ROTATION_CCW = 8;
	static const int HOLD = 16;
	static const int DOWN = 32;
	static const int HARD_DROP = 64;
	static const int START = 128;
	static const int BACK = 256;

	//コンストラクタ
	Config() : fps(0), height(0), width(0) {}
	Config(const char* file_name);


	//レベル保存用オブジェクト
	struct Level {

		Level() :
			level(0), last_action_count_max(0), set_count_max(0), delete_line_wait_count_max(0), down_speed(30), 
			attack_count_y(0), attack_count_r(0) {
		}

		//レベル
		int level;

		//スコアまとめ
		struct Score {
			Score() : 
				line(0), tetris(0), tspin_single(0), tspin_double(0), tspin_triple(0), ren(0), btb(0), bonus(1.0), lvlup_score(0),
				line2(0), line3(0), perfect_line(0), perfect_line2(0), perfect_line3(0), perfect_tetris(0), ren_max(0), tspin_mini(0) {}

			//普通のライン消し(+tetris)
			int line;
			int line2;
			int line3;
			int tetris;

			//普通のライン消し(+tetris)(perfect)
			int perfect_line;
			int perfect_line2;
			int perfect_line3;
			int perfect_tetris;

			//T-spin
			int tspin_single;
			int tspin_double;
			int tspin_triple;

			int tspin_mini;

			//REN
			int ren;
			int ren_max;

			//back to back
			double btb;


			double bonus;

			int lvlup_score;
		}score;

		//攻撃力
		struct Attack {

			Attack() :
				line(0), tetris(0), tspin_single(0), tspin_double(0), tspin_triple(0), ren(0), btb(0),
				line2(0), line3(0), perfect_line(0), perfect_line2(0), perfect_line3(0), perfect_tetris(0), tspin_mini(0) {
			}

			//普通のライン消し(+tetris)
			int line;
			int line2;
			int line3;
			int tetris;

			//普通のライン消し(+tetris)(perfect)
			int perfect_line;
			int perfect_line2;
			int perfect_line3;
			int perfect_tetris;

			//T-spin
			int tspin_single;
			int tspin_double;
			int tspin_triple;

			int tspin_mini;

			//REN
			std::vector<int> ren;

			//back to back
			int btb;
		}attack;

		//ゲーム速度関連
		int last_action_count_max;
		int set_count_max;
		int delete_line_wait_count_max;
		int down_speed;

		//警告までの時間
		int attack_count_y;
		int attack_count_r;
	};
	std::vector<Level> levels;

	int fps;	//fps
	int width;	//幅
	int height;	//高さ
};