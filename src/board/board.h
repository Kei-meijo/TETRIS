#pragma once
#include <opencv2/opencv.hpp>
#include "blocks.h"
#include <vector>
#include "../config/config.h"
#include <random>


class Board {

public:
	//壁
	static const int WALL = -1;
	//ミノの最大幅
	static const int MINO_SIZE_MAX = 4;

	//T-spin一覧
	static const int T_SPIN_NONE = 0;
	static const int T_SPIN_NORMAL = 1;
	static const int T_SPIN_MINI = 2;

	//コンストラクタ
	Board();
	Board(int x, int y, Config& lvl) ;
	Board(const Board&) = delete;

	//メモリ開放
	~Board();

	//初期化
	void init();

	//ゲームスタート
	void start();

	//ゲーム終了
	inline void end() {
		is_started = false;
	}

	bool isInterfere(Blocks& blocks);

	//ミノを動かす
	bool move(int x, int y, int rot, bool action = true);

	//ホールド
	bool hold();

	//おじゃまブロック追加
	//1段ずつ
	bool addBlockFromDown();

	//新規ブロックを設定
	bool setNewBlock();

	//実際にゲーム画面を出力する部分
	cv::Mat show();

	//HOLD部分の表示
	cv::Mat showHoldMino(double re_size = 1.0);

	//Nextの表示
	cv::Mat showNextMino(double k, int h);

	//おじゃまブロックエリアの描画
	cv::Mat showAttackMino(int margin);

	//操作中ミノを固定させる
	bool set();

	//ラインがそろったところを削除
	//set()を呼んだ後に呼ぶ
	int deleteBlock();

	//浮いたブロックを, 下げる
	void blockDown();

	//ゲームオーバー処理
	//返り値は処理が終わったか(終わればtrue)
	bool gameOver(bool flag = false);

	//Scoreを計算
	int calcScore();

	//相手への攻撃判定
	int popAttack();

	//おじゃまブロックの高さ取得
	int getAttackHeight();

	//攻撃
	void setAtack(int attack, int player_count);

	//メインループ
	bool loop(int action);

	//Scoreを入れるための入れ物
	struct Score {

		Score() : score(0), ren(0), level(0), name1(""), name2("") {}

		int score;
		int ren;
		int level;
		std::string name1;
		std::string name2;
	};

	//Score取得
	Score getScore();

	//Game Overかどうか
	inline bool isGameOver() {
		return this->is_game_over;
	}

private:

	//画像読み込み
	void loadimg();

	//ゴーストの設定
	void ghost_set();

	//ミノの画像取得
	cv::Mat getMinoImage(int type, int type_c = -1);

	bool gameOver1(bool flag = false);
	bool gameOver2(bool flag = false);

#ifdef _Debug
	//Debug用
	void Debug(int pattern);
#endif

	int x;				//盤面のサイズ 幅
	int y;				//盤面のサイズ 高さ

	int** rawboard;		//盤面
	Blocks* minos;		//ミノのテンプレート
	int* mino_counts;
	cv::Mat* images;	//ブロックの画像
	int mino_number;	//ミノの種類(NULLを含める)
	Blocks now_block;	//現在操作中のブロック
	Blocks ghost;		//落下位置に表示するゴーストブロック
	int hold_block;		//ホールドしてあるブロックのタイプ
	bool already_hold;	//すでにホールド操作をしたか
	std::vector<int> nexts;//NEXTブロック
	int next_size_max;	//NEXTに表示する数
	int img_size;		//ブロックの画像サイズ
	int last_action;	//最後の操作

	//random
	//ミノ乱択用
	std::uniform_int_distribution<int> randomBlockDistribution;
	//ライン乱択用
	std::uniform_int_distribution<int> randomlineDistribution;
	std::mt19937 randomEngine;

	//delete
	int highest_line;	//最も高い位置のブロックの高さ
	std::vector<int> delete_line;	//削除したライン

	int tspin_status;	//Tスピンの種類
	int srs_type;		//Super Rotation Systemのタイプ

	
	//game status
	//レベル関連
	Config levels;		//レベル一覧
	Config::Level next_level;	//Next Level
	Config::Level now_level;	//Now Level
	int score;					//Score
	bool need_calc_score;		//Score再計算
	int ren;					//REN
	int back_to_back;			//back to back

	//攻撃関連
	int attack;					//自分がおいたミノによる相手への攻撃力
	int attack_max;				//自分が受ける最大ダメージ
	int attack_yellow_type;		//警告表示1段階目で使うブロックのタイプ(ふつうは黄色 : 2)
	int attack_red_type;		//警告表示2段階目で使うブロックのタイプ(ふつうは赤色 : 4)
	bool can_put_block;			//新規ブロック生成が可能か? (せりあがるアニメーションが終わったか)
	bool put_block_state;		//新規ブロック生成を行うか

	//攻撃を保存するためのオブジェクト
	struct AttackBlock {
		AttackBlock(int height) : time(0), height(height), type(8), state(0) {}
		AttackBlock() : time(0), height(0), type(8), state(0) {}

		AttackBlock operator++() {
			++this->time;
			return *this;
		}

		AttackBlock operator--() {
			--this->time;
			return *this;
		}

		const AttackBlock operator++(int) {
			AttackBlock atk = *this;
			++(*this);
			return atk;
		}

		const AttackBlock operator--(int) {
			AttackBlock atk = *this;
			--(*this);
			return atk;
		}

		int type;	//表示に使うブロックのタイプ
		int height;	//おじゃまブロックを生成するたかさ
		int time;	//攻撃を受けてから経過した時間
		int state;	//どこのラインに空白を作ったか保存するための変数
	};
	std::vector<AttackBlock> attack_block;//相手からの攻撃を保存


	bool is_game_over;			//Game Overかどうか
	//game over時に使用する変数
	int go_dir;	//上書き方向
	//現在の座標　xとy
	int go_x;	
	int go_y;
	//壁までの距離(この距離になったら曲がる)
	int go_c;

	
	//この下からはメインループ用の変数
	int down_count;
	int fps;
	int set_count;
	bool can_action;
	int delete_line_wait_count;
	bool delete_line_wait;
	int last_action_key;
	int last_action_count;
	bool is_started;
};