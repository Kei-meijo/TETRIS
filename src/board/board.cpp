#include "board.h"

#include "../opencv/cv_util.h"
#include <math.h>
#include <iostream>

Board::Board() : img_size(40), already_hold(false), hold_block(0), next_size_max(6), is_game_over(false), can_action(false),
	score(0), down_count(0), set_count(0), delete_line_wait(false), delete_line_wait_count(0),
	last_action(Config::NONE), last_action_key(Config::NONE), last_action_count(0), ren(0), back_to_back(0),
	go_dir(0), go_x(1), go_y(0), go_c(0), need_calc_score(false), is_started(false), attack(0),
	attack_yellow_type(2), attack_red_type(4), can_put_block(true), put_block_state(false), fps(30), x(10), y(20) {

	this->x = 10;
	this->y = 20;
	this->highest_line = 0;
	this->attack_max = 14;

	this->images = nullptr;
	this->mino_counts = nullptr;
	this->mino_number = 0;
	this->minos = nullptr;
	this->rawboard = nullptr;
	this->srs_type = 0;
	this->tspin_status = T_SPIN_NONE;
}

//コンストラクタ
Board::Board(int x, int y, Config& lvl) :
	img_size(40), already_hold(false), hold_block(0), next_size_max(6), is_game_over(false), can_action(false),
	score(0), down_count(0), set_count(0), delete_line_wait(false), delete_line_wait_count(0),
	last_action(Config::NONE), last_action_key(Config::NONE), last_action_count(0), ren(0), back_to_back(0),
	go_dir(0), go_x(1), go_y(0), go_c(0), levels(lvl), need_calc_score(false), is_started(false), attack(0),
	attack_yellow_type(2), attack_red_type(4), can_put_block(true), put_block_state(false), x(x), y(y), 
	highest_line(0) {

	//ボードサイズ初期化
	this->x = x;
	this->y = y;
	this->highest_line = 0;
	this->attack_max = y - 6;
	if (attack_max < 1) attack_max = 1;

	//画像読み込み
	loadimg();

	//初期化
	init();

	//ミノ設定
	this->mino_number = 8;
	this->minos = new Blocks[mino_number];
	this->mino_counts = new int[mino_number];

	for (int i = 0; i < mino_number; i++) {
		this->minos[i] = Blocks();
		this->mino_counts[i] = 0;

		//ブロック初期位置
		this->minos[i].x = this->x / 2;
		this->minos[i].y = this->y + 1;
	}

	//ブロックの形状設定
	this->minos[0] = this->minos[0].setType(0).setRot(1).add(0, 0).add(0, 0).add(0, 0);//NULL
	this->minos[1] = this->minos[1].setType(1).setRot(2).add(0, 1).add(0, 2).add(0, -1);//I
	this->minos[2] = this->minos[2].setType(2).setRot(1).add(1, 0).add(0, 1).add(1, 1);//O
	this->minos[3] = this->minos[3].setType(3).setRot(2).add(-1, 0).add(0, 1).add(1, 1);//S
	this->minos[4] = this->minos[4].setType(4).setRot(2).add(1, 0).add(0, 1).add(-1, 1);//Z
	this->minos[5] = this->minos[5].setType(5).setRot(4).add(0, 1).add(1, 0).add(2, 0);//J
	this->minos[6] = this->minos[6].setType(6).setRot(4).add(0, 1).add(-1, 0).add(-2, 0);//L
	this->minos[7] = this->minos[7].setType(7).setRot(4).add(0, 1).add(1, 0).add(-1, 0);//T

	//Tスピン用当たり判定
	this->minos[7] = this->minos[7].addStatus(-1, -1, 0).addStatus(1, -1, 0).addStatus(-1, 1, 1).addStatus(1, 1, 1);

	//Level 設定
	//Level1とLevel2
	for (auto& level : lvl.levels) {
		if (level.level == 1) {
			now_level = level;
		} else if (level.level == 2) {
			next_level = level;
		}
	}

	//ランダマイザ初期化
	std::random_device randomDevice;
	std::vector<uint32_t> randomSeedVector(10);
	std::generate(randomSeedVector.begin(), randomSeedVector.end(), std::ref(randomDevice));
	std::seed_seq randomSeed(randomSeedVector.begin(), randomSeedVector.end());

	randomEngine = std::mt19937(randomSeed);
	randomBlockDistribution = std::uniform_int_distribution<int>(1, this->mino_number - 1);
	randomlineDistribution = std::uniform_int_distribution<int>(1, this->x);
}



//メモリ開放
Board::~Board() {
	for (int i = 0; i < y + 5; i++) {
		if (this->rawboard[i])delete[] this->rawboard[i];
	}

	if (this->rawboard)delete[] this->rawboard;
	if (this->minos)delete[] this->minos;
	if (this->images)delete[] this->images;
	if (this->mino_counts)delete[] this->mino_counts;
}

//初期化
void Board::init() {
	//メモリ確保
	std::cout << "x = " << this->x << std::endl;
	std::cout << "y = " << this->y << std::endl;
	this->rawboard = new int* [this->y + 5];
	for (int i = 0; i < this->y + 5; i++) {
		this->rawboard[i] = new int[this->x + 2];

		for (int j = 0; j < this->x + 2; j++) {
			this->rawboard[i][j] = 0;
		}
	}

	for (int i = 0; i < this->y + 5; i++) {
		this->rawboard[i][0] = WALL;
		this->rawboard[i][this->x + 1] = WALL;
	}

	for (int i = 0; i < this->x + 2; i++) {
		this->rawboard[0][i] = WALL;
	}
}

//ゲームスタート
void Board::start() {
	if (!is_started) {
		//ゲーム開始
		//新規ブロック設定
		setNewBlock();
		is_started = true;
	}
}

//ブロックが干渉しているか?
bool Board::isInterfere(Blocks& blocks) {
	//ミノの座標取得
	auto points = blocks.getPoints();
	for (auto& p : points) {
		//ミノの位置にブロックが被っていないか確認
		if (this->rawboard[p.y][p.x] != 0) {
			return true;
		}
	}

	return false;
}

//ミノを動かす
bool Board::move(int x, int y, int rot, bool action) {

	//動かせない時は何もしない
	//ブロック設置後とか
	if (!can_action)return true;
	Blocks tmp = now_block.clone();

	//一度に動かすのは一方向のみ
	if (x != 0) {
		//左右方向
		tmp.x += x;

		//ブロックが干渉しているか
		if (isInterfere(tmp)) {
			//移動不可
			//特に何もせず終了
			return false;
		} else {
			if (action) {
				//移動できるので,実際に移動
				now_block = tmp;

				//最後に動かした行動を保存
				if (x > 0) { last_action = Config::RIGHT; } else { last_action = Config::LEFT; }

				//ゴーストも動かす
				ghost_set();
			}
			return true;
		}
	} else if (rot != 0) {
		//回転
		tmp.rotate += rot;

		int tx = tmp.x;
		int ty = tmp.y;

		//ミノの座標取得
		auto points = tmp.getPoints();

		//Super Rotation 回転軸を動かしながら,設置できる位置に設置
		//符号のループ -1 or 1 y用
		for (int code_y = -1; code_y <= 1; code_y += 2) {

			//y座標 0, 1, 2, -1
			for (int ddy = 0; ddy <= 2; ddy++) {
				//2回目の0はスキップ(すでに計算済み)
				if (code_y == 1 && ddy == 0)continue;
				int dy = code_y * ddy;

				//符号のループ 1 or -1 x用
				for (int code_x = 1; code_x >= -1; code_x -= 2) {
					//中心位置 0～1(x方向)
					for (int ddx = 0; ddx <= 2; ddx++) {
						int dx = ddx * code_x;

						//xとyの距離が両方2以上は移動しない
						if (ddy > 1 && ddx > 1)continue;

						//std::cout << "(dx, dy) = (" << dx << "," << dy << ")" << std::endl;


						int nx = tx + dx;
						int ny = ty + dy;

						//回転中心が画面範囲外
						if (ny < 1 || nx < 1 || nx > this->x)break;

						//回転中心以外も確認
						bool can_place = true;
						for (auto& p : points) {
							nx = p.x + dx;
							ny = p.y + dy;

							//画面範囲内かどうか
							if (ny < 1 || nx < 1 || nx > this->x) {
								can_place = false;
								break;
							}

							//ブロックがないか
							if (this->rawboard[ny][nx] != 0) {
								can_place = false;
								break;
							}
						}

						//探索し終えてブロックが設置できるので設置
						if (can_place) {
							if (action) {
								tmp.x = tx + dx;
								tmp.y = ty + dy;

								//移動できるので,実際に移動
								now_block = tmp;

								//回転を0～3に収める
								int rot_dir = rot;
								while (rot_dir < 0) { rot_dir += 4; }
								while (rot_dir >= 4) { rot_dir -= 4; }

								//最後に操作した行動を保存
								if (rot_dir == 1) { last_action = Config::ROTATION_CW; }
								if (rot_dir == 3) { last_action = Config::ROTATION_CCW; }

								//mini Tspin判定用のSRS移動タイプ判別
								if (ddx == 1 && ddy == 0) {
									this->srs_type = 1;
								} else if (ddx == 1 && ddy == 1) {
									this->srs_type = 2;
								} else if (ddx == 0 && ddy == 2) {
									this->srs_type = 3;
								} else if (ddx == 1 && ddy == 2) {
									this->srs_type = 4;
								} else {
									this->srs_type = 0;
								}

								//ゴーストも動かす
								ghost_set();
							}
							return true;
						}
					}
				}
			}
		}

	} else if (y != 0) {
		//下方向
		tmp.y += y;

		//ブロックが干渉しているか
		if (isInterfere(tmp)) {
			//移動不可
			//特に何もせず終了
			return false;
		} else {
			if (action) {
				//移動できるので,実際に移動
				now_block = tmp;

				//最後に操作した行動を保存
				last_action = Config::DOWN;

				//下方向のみはゴーストは動かないので,ゴーストの更新はしない
			}
			return true;
		}
	}

	//移動できたのでtrueを返す
	return true;
}

//ホールド
bool Board::hold() {
	//すでにホールドしていたら、ホールドしない
	if (!already_hold) {

		//ホールドと操作中のものを入れ替える
		int tmp = this->hold_block;
		this->hold_block = now_block.getType();
		last_action = Config::HOLD;

		//ホールドが空だったら新規に生成しないと
		if (tmp == 0) {
			//ホールドが空
			//新規ブロックを生成
			setNewBlock();
		} else {
			//単純にホールドを呼び出す
			now_block = this->minos[tmp].clone();
		}

		//ゴーストを更新
		ghost_set();

		//ホールドしたことを設定
		already_hold = true;
		return true;
	}

	return false;
}

//おじゃまブロック追加
//1段ずつ
bool Board::addBlockFromDown() {

	//そもそもブロックがなければおく必要なし
	if (this->attack_block.empty())return true;

	//空のブロックがあれば削除
	while (!this->attack_block.empty() && this->attack_block[0].height <= 0) {
		this->attack_block.erase(this->attack_block.begin());
	}

	//まだ時間の余裕があるので, 設置不要
	if (this->attack_block[0].type != this->attack_red_type) return true;

	//規定は前回と同じライン
	int set_line = this->attack_block[0].state;
	if (this->attack_block[0].state == 0) {
		//前回が存在しない
		//完全ランダム
		set_line = randomlineDistribution(randomEngine);
	} else {
		//70%の確率で前回と同じ
		int tmp_ = randomlineDistribution(randomEngine);

		if (tmp_ > 7) {//8, 9, 10 で30%
			//前回とは違うラインへ
			while (set_line == this->attack_block[0].state) {
				set_line = randomlineDistribution(randomEngine);
			}
		}
	}

	//ライン追加
	//まずは空間を空ける
	for (int i = this->y + 4; i >= 2; i--) {
		for (int j = 1; j <= this->x; j++) {
			this->rawboard[i][j] = this->rawboard[i - 1][j];
		}
	}
	for (int j = 1; j <= this->x; j++) {
		//ライン追加
		if (set_line == j) { this->rawboard[1][j] = 0; } else { this->rawboard[1][j] = 8; }
	}



	//前回のラインを更新
	this->attack_block[0].state = set_line;
	this->attack_block[0].height--;
	printf("attack_block : %d\n", this->attack_block[0].height);
	while (!this->attack_block.empty() && this->attack_block[0].height <= 0) {
		this->attack_block.erase(this->attack_block.begin());
	}

	return false;
}

//新規ブロックを設定
bool Board::setNewBlock() {
	int rand = 0;
	put_block_state = false;
	can_put_block = false;

	//Next + 1が埋まるまで生成し続ける
	//+ 1なのは, 新規生成時には操作中ブロックがないため
	//その分も確保しないといけない
	while (this->nexts.size() <= next_size_max + 1) {
		//乱択の構造は
		//1～7までのリストを構成
		//例
		//番号				1　2　3　4　5　6　7
		//後何回出せるか	1　1　0　2　2　1　1
		//上の場合タイプ3は出さないようにして,他のブロックを生成する


		int count_ = 0;		//出せるブロックの数
		int c_number = -1;	//出せるブロックの番号
		//番号				1　2　3　4　5　6　7
		//後何回出せるか	0　0　0　1　0　1　0
		//上の場合
		//count_ = 2
		//c_number = 6
		for (int i = 1; i < this->mino_number; i++) {
			if (this->mino_counts[i] > 0) {
				count_++;
				c_number = i;
			}
		}

		//count_が0の場合,ブロックが出せないので,リストを再生成
		if (count_ == 0) {
			count_ = this->mino_number - 1;//再生成するので,当然全部出せるのでカウント値を最大に

			//fillは再生成のとき何で埋めるか
			//fillが1の場合		この場合,ブロックが連続しなくなる
			//番号				1　2　3　4　5　6　7
			//後何回出せるか	1　1　1　1　1　1　1
			//ずっと上のような状況は面白くないので,たまには連続するように2で埋める時も作る
			//fillが1の場合		この場合,ブロックが連続する可能性あり
			//番号				1　2　3　4　5　6　7
			//後何回出せるか	2　2　2　2　2　2　2
			//ただ,ずっと2だと連続しすぎるので,1と2をランダムにする
			//↓は1多めのランダム(1～7のランダムの奇数を1偶数を2にしている)
			int fill = (randomBlockDistribution(randomEngine) - 1) % 2 + 1;
			//ゲーム開始時は運要素を減らすため, 必ず1にする
			//ゲーム開始時はNEXTが0なので, それを利用して判別している
			if (this->nexts.size() < 1) {
				fill = 1;
			}
			
			//fillを埋める
			for (int i = 1; i < this->mino_number; i++) {
				this->mino_counts[i] = fill;
			}
		}

		//count_が2以上の時ランダムに抽出する
		//1の時は乱択の必要がないので, 事前に調べたタイプをそのまま入れる
		if (count_ > 1) {
			//1～count_のランダムになるような正規化を作る
			//count_が3の時は1, 2, 3のランダム
			std::uniform_int_distribution<int> dist = std::uniform_int_distribution<int>(1, count_);

			//乱数を取得　この値が使える番号の中で, 何番目かを示す
			//(例)tmp = 3
			//番号				1　2　3　4　5　6　7
			//後何回出せるか	1　1　0　2　0　1　1
			//上の時, タイプは4
			//(1番目は1, 2番目は2, 3番目は4, 4番目は6, 5番目は7)
			int tmp = dist(randomEngine);
			int now_count_ = 0;//探索中に今何番目か記録しておく

			//上で決めたtmp番目を探す
			for (int i = 1; i < this->mino_number; i++) {
				if (this->mino_counts[i] > 0) {//当たり前だけど, 使えないやつはカウントしないようにする
					if (++now_count_ == tmp) {//一番最初が1なので, 先に1足す 番目が一致していたらその番号が出力
						rand = i;
						break;
					}
				}
			}
		} else {
			//事前に調べたタイプをそのまま入れる
			rand = c_number;
		}

		//使ったやつは減らしておく(じゃないとリストで管理する意味がない)
		this->mino_counts[rand]--;

		//Nextに追加
		this->nexts.push_back(rand);
	}

	//nextの先頭を次の操作ミノに
	rand = this->nexts[0];
	//nextの先頭を削除（上の操作は単純にコピーしただけ）
	this->nexts.erase(this->nexts.begin());
	//実際にNextに代入
	now_block = this->minos[rand].clone();

	//新規ブロックのミノの座標取得
	//Game Over判定のたの, 新規ブロックが置ける判定に使用
	bool has_block = isInterfere(now_block);

	//最後に操作した行動のリセット
	last_action = Config::NONE;
	//操作可能にする
	can_action = true;

	//Game Over判定は新規ミノ生成時にブロックが被っていないか
	is_game_over = has_block;

	//ゴーストを更新
	ghost_set();
	return !has_block;
}

//実際にゲーム画面を出力する部分
cv::Mat Board::show() {
	cv::Mat display(this->y * this->img_size, this->x * this->img_size, CV_8UC3, cv::Scalar(0, 0, 0));

	//操作中のものとゴーストのミノの座標取得
	auto mino_block = now_block.getPoints();
	auto mino_block_g = ghost.getPoints();

	//openCVの原点が左上なので、上から描画
	for (int i = this->y; i >= 1; i--) {
		//openCVでのy座標
		int opencv_y = (this->y - i) * this->img_size;
		for (int j = 1; j <= x; j++) {
			//openCVでのx座標
			int opencv_x = (j - 1) * this->img_size;
			//ボードのブロック情報
			int block_color = this->rawboard[i][j];
			bool is_ghost = false;

			//今探索中のブロックに操作中のミノがあるか(すでに設置したブロックがない場合に探索)
			if (block_color == 0) {
				for (auto& point : mino_block) {
					if (point.x == j && point.y == i) {
						block_color = now_block.getType();
						break;
					}
				}
			}

			//今探索中のブロックにゴーストのミノがあるか(すでに設置したブロックがない場合に探索)
			if (block_color == 0) {
				for (auto& point : mino_block_g) {
					if (point.x == j && point.y == i) {
						block_color = now_block.getType();
						is_ghost = true;
						break;
					}
				}
			}

			//ゴーストであれば、透過させて表示
			if (!is_ghost) {
				//そのまま描画
				paste(display, this->images[block_color].clone(), opencv_x, opencv_y);
			} else {
				//一旦背景を描画させたあと, ゴーストを透過させて描画
				paste(display, this->images[0].clone(), opencv_x, opencv_y);
				blend_paste(display, this->images[block_color].clone(), opencv_x, opencv_y);
			}
		}
	}


	return display;
}

//HOLD部分の表示
cv::Mat Board::showHoldMino(double re_size) {
	//ミノを画像にする関数をそのまま呼んでいる
	cv::Mat display_image;
	if (already_hold) {
		display_image = getMinoImage(this->hold_block, 8);
	} else {
		display_image = getMinoImage(this->hold_block);
	}

	if (re_size != 1.0) {
		cv::resize(display_image, display_image, cv::Size(), re_size, re_size);
	}

	return display_image;
}

//Nextの表示
cv::Mat Board::showNextMino(double k, int h) {
	//Nextは直近のものを大きく表示して, それ以外は引数に指定した係数倍したサイズにする
	int size = static_cast<int>(k * MINO_SIZE_MAX * this->img_size);//小さい方の画像サイズ
	int total_y_size = (size + h) * (this->next_size_max - 1) + MINO_SIZE_MAX * this->img_size;//全部合わせたサイズ

	//画像初期化
	cv::Mat display_mino(total_y_size, MINO_SIZE_MAX * this->img_size, CV_8UC3, cv::Scalar(0, 0, 0));

	//一番上に直近を貼り付け
	paste(display_mino, getMinoImage(this->nexts.empty() ? 0 : this->nexts[0]), 0, 0);

	//それ以外を貼り付け
	for (int i = 1; i < this->next_size_max; i++) {
		//画像取得
		cv::Mat next_img = getMinoImage(this->nexts.size() <= i ? 0 : this->nexts[i]);
		//縮小
		cv::resize(next_img, next_img, cv::Size(size, size));
		//貼り付け
		paste(display_mino, next_img, 0, MINO_SIZE_MAX * this->img_size + (i - 1) * size + i * h);
	}

	return display_mino;
}

//おじゃまブロックエリアの描画
cv::Mat Board::showAttackMino(int margin) {
	//画像初期化
	int height = (this->attack_max + 1) * this->img_size;
	cv::Mat display_mino(height, this->img_size, CV_8UC3, cv::Scalar(0, 0, 0));

	//空なら描画しない
	if (!this->attack_block.empty()) {
		//下から描画

		int y_ = 0;//y座標
		for (const auto& block : this->attack_block) {
			for (int i = 0; i < block.height; i++) {
				//描画(1block)
				paste(display_mino, this->images[block.type].clone(), 0, height - y_ - this->img_size);
				//座標を1block分ずらす
				y_ += this->img_size;
			}

			//塊ごとに空白を空ける
			y_ += margin;
		}
	}
	return display_mino;
}

//操作中ミノを固定させる
bool  Board::set() {
	//ホールドできるようにする
	this->already_hold = false;
	//ミノを動かせないようにする
	can_action = false;

	//現在のブロックの位置を取得
	auto points = now_block.getPoints();
	int type = now_block.getType();

	//そもそも設置できるか確認
	//ブロックが重なっていないか
	if (isInterfere(now_block))return false;

	//Tスピン判定
	this->tspin_status = T_SPIN_NONE;

	//Tミノかどうか(Tスピン判定が存在するか)
	if (this->now_block.getStatSize() > 0) {

		//最後に操作したのが,回転操作かどうか
		if (this->last_action != Config::NONE && (this->last_action & (Config::ROTATION_CW | Config::ROTATION_CCW)) != 0) {
			//T spinの当たり判定の位置を取得
			auto decision = now_block.getStats();

			int count = 0;//当たり判定にブロックが何個あるか
			int t_mini = 0;//Tのブロックの凸側にブロックが何個あるか
			for (auto& d : decision) {
				if (this->rawboard[d.p.y][d.p.x] != 0) {
					count++;
					t_mini += d.status;
				}
			}

			// 当たり判定にブロックが3個以上
			if (count >= 3) {
				//T-spin

				//Tのブロックの凸側にブロックが1個しかなくて, 最後の回転操作時の
				//Super Rotation Systemの動作モードが4以外((x, y) 移動量が(+-1, +-2))だと
				//T spin mini判定(T spin判定ではあるが, Scoreは通常のライン消去判定)
				//上記以外はT spin(ScoreもT spin準拠)(normalと呼称)
				if (t_mini < 2 && this->srs_type != 4) {
					std::cout << "t spin mini" << std::endl;
					this->tspin_status = T_SPIN_MINI;
				} else {
					std::cout << "t spin normal" << std::endl;
					this->tspin_status = T_SPIN_NORMAL;
				}
			}
		}
	}

	//ボードにブロック情報を書き込む
	for (auto& p : points) {
		this->rawboard[p.y][p.x] = type;
	}

	//操作中とゴーストをNULLブロックに差し替えて, 見えないように
	this->now_block = this->minos[0].clone();
	this->ghost = this->minos[0].clone();

	//設置ができた
	return true;
}

//ラインがそろったところを削除
	//set()を呼んだ後に呼ぶ
int Board::deleteBlock() {
	this->highest_line = 0;//最も高い位置にあるブロック情報を入れる
	this->delete_line.clear();//削除したライン情報を入れる

	//下から探索
	for (int y_ = 1; y_ <= this->y + 3; y_++) {

		bool complete_line = true;
		//x方向に探索
		for (int x_ = 1; x_ <= this->x; x_++) {
			//空のブロックがあれば, ライン消去できない
			if (this->rawboard[y_][x_] == 0) {
				complete_line = false;
				//最も高い位置にあるブロック情報を探索するために, 
				//ライン消去できないことが確定したが, 探索を続けるためbreakは入れない
			} else {
				this->highest_line = y_;
				//最も高い位置にあるブロック情報が探索し終えた.
				//もうすでにライン消去できないことが確定した場合, 探索終了
				if (!complete_line)break;
			}
		}

		//ライン消去可能か
		if (complete_line) {
			//可能であるので, 実際に消去
			for (int x_ = 1; x_ <= this->x; x_++) {
				this->rawboard[y_][x_] = 0;
			}
			//削除したライン情報を入れる
			this->delete_line.push_back(y_);
		}
	}

	//なおこの段階では, ブロックは浮いた状態になる
	//この後blockDown() を呼ぶことで, 削除したラインを下げられるが, 
	//浮いた状態を一瞬描画するため, すぐには呼ばない
	return (int)this->delete_line.size();
}

//浮いたブロックを, 下げる
void Board::blockDown() {

	//下のライン情報から入っているので, 0から探索すれば下から探索可能
	for (int i = 0; i < this->delete_line.size(); i++) {

		//削除するラインから(最も高い位置にあるブロック高さへ)上方向に作業
		for (int y = this->delete_line[i]; y <= this->highest_line; y++) {
			//一段上の情報をコピーしてくる
			for (int x = 1; x <= this->x; x++) {
				//壁はコピーしないようにする(動かせないため)
				if (this->rawboard[y + 1][x] != WALL) {
					this->rawboard[y][x] = this->rawboard[y + 1][x];
					//this->rawboard[y + 1][x] = 0;
				}
			}
		}
		//下の段のライン下げの影響で上の方の残りのラインが一段下がっているので,
		//1引いておく
		for (int j = i + 1; j < this->delete_line.size(); j++) {
			this->delete_line[j]--;
		}
		//一番高いブロックも一段下がっているので,1引いておく
		this->highest_line--;
	}
}

//ゲームオーバー処理
	//返り値は処理が終わったか(終わればtrue)
bool Board::gameOver(bool flag) {
#ifdef GAME_OVER_1
	return gameOver1(flag);
#else
	return gameOver2(flag);
#endif
}

//ゲームオーバー処理
	//返り値は処理が終わったか(終わればtrue)
bool Board::gameOver1(bool flag) {
	//左下から時計回りにブロックを赤いブロックに書き換える処理

	//dirが4未満になるように調整
	while (go_dir >= 4) { go_dir -= 4; }
	//go_cが負かどうかを終了判定にしている
	//go_cは本来は壁までの距離
	if (go_c < 0)return true;

	//dirから次のブロックへの偏差を計算
	//この辺はミノの回転と一緒(回転行列を使うだけ)
	int g_x = 0;
	int g_y = 1;
	for (int r = 0; r < go_dir; r++) {
		int ng_x = g_y;
		int ng_y = -g_x;

		g_x = ng_x;
		g_y = ng_y;
	}

	//yの偏差がある(y方向に移動)
	//現在の位置と偏差を足したもの(次のブロックの位置)の壁までの距離が, 規定以上
	//上記の場合方向を曲げる
	if (g_y != 0 && (g_y + go_y <= go_c || g_y + go_y > this->y - go_c)) {
		//方向を曲げる
		go_dir++;
		//dirが3
		//(次の移動方向が左移動の場合)
		//は壁までの距離の規定を1足す
		if (go_dir == 3) {
			go_c++;
		}

		//flagがtrueで呼ばれる時は,曲げが連続する
		//つまり, すべて埋めきったということなので, 終了する
		if (flag) {
			go_c = -1;
			return true;
		}

		//まだブロックを書き換える処理をしていないのと, 
		//連続で曲がらないといけないのかの判定をするため,
		//もう一度同じ処理をするため, 自分自身を呼ぶ(flag は true)
		return gameOver(true);
	}

	//xの偏差がある(x方向に移動)
	//現在の位置と偏差を足したもの(次のブロックの位置)の壁までの距離が, 規定以上
	//上記の場合方向を曲げる
	if (g_x != 0 && (g_x + go_x <= go_c || g_x + go_x > this->x - go_c)) {
		//方向を曲げる
		go_dir++;

		//flagがtrueで呼ばれる時は,曲げが連続する
		//つまり, すべて埋めきったということなので, 終了する
		if (flag) {
			go_c = -1;
			return true;
		}

		//まだブロックを書き換える処理をしていないのと, 
		//連続で曲がらないといけないのかの判定をするため,
		//もう一度同じ処理をするため, 自分自身を呼ぶ(flag は true)
		return gameOver(true);
	}

	//埋めるべきブロックの位置を計算(偏差を足すだけ)
	go_x += g_x;
	go_y += g_y;

	//ブロックがあれば赤く染め上げる(typeを4に変えるだけ)
	if (this->rawboard[go_y][go_x] != 0)this->rawboard[go_y][go_x] = 4;

	//終了していないのでfalse
	return false;
}

bool Board::gameOver2(bool flag) {
	this->now_block = this->minos[0];
	if (this->go_y > this->y) { return true; }

	if (++this->go_x % 3 == 0) {
		this->go_y++;
		for (int i = 1; i <= this->x; i++) {
			this->rawboard[this->go_y][i] = 0;
		}
	} else {
		int y = this->go_y + 1;
		for (int i = 1; i <= this->x; i++) {
			if(this->rawboard[y][i] != 0) this->rawboard[y][i] = 8;
		}
	}

	return false;
}

//Scoreを計算
int  Board::calcScore() {

	//Score計算不要であれば, 現在値をそのまま返す
	if (!need_calc_score)return this->score;

	bool perfect = this->delete_line.size() >= this->highest_line || this->highest_line == 0;

	//Levelボーナス
	double bonus = this->now_level.score.bonus;
	double btb_bonus = 1.0;
	//追加スコア
	int plus_score = 0;//+ this->back_to_back * this->now_level.btb;
	//基本
	int ren_ = this->ren - 1 < 0 ? 0 : this->ren - 1;
	int basic_score = 0;

	//攻撃力
	int perfect_attack = 0;
	int normal_attack = 0;

	//RENによるスコア加算
	basic_score = ren_ * this->now_level.score.ren;
	if (basic_score > this->now_level.score.ren_max) {
		basic_score = this->now_level.score.ren_max;
	}

	if (ren_ > 0) {
		normal_attack += this->now_level.attack.ren[std::min(ren_, (int)this->now_level.attack.ren.size()) - 1];
		//printf("REN = %d\n", normal_attack);
	}

	//back to backによるスコア加算
	if (this->back_to_back >= 2) {
		btb_bonus = this->now_level.score.btb;
		normal_attack += this->now_level.attack.btb;
	}

	//基本スコア
	//削除のされ方
	if (this->delete_line.size() >= 4) {
		//テトリス
		basic_score += this->now_level.score.tetris;
		normal_attack += this->now_level.attack.tetris;
	} else if (this->delete_line.size() >= 3 && this->tspin_status == T_SPIN_NORMAL) {
		//T-spin triple
		basic_score += this->now_level.score.tspin_triple;
		normal_attack += this->now_level.attack.tspin_triple;
	} else if (this->delete_line.size() == 2 && this->tspin_status == T_SPIN_NORMAL) {
		//T-spin double
		basic_score += this->now_level.score.tspin_double;
		normal_attack += this->now_level.attack.tspin_double;
	} else if (this->delete_line.size() == 1 && this->tspin_status == T_SPIN_NORMAL) {
		//T-spin single
		basic_score += this->now_level.score.tspin_single;
		normal_attack += this->now_level.attack.tspin_single;
	} else if (this->delete_line.size() > 0) {
		//通常のライン消し
		//T-spin miniもここ

		if (this->delete_line.size() >= 3) {
			basic_score += this->now_level.score.line3;
			normal_attack += this->now_level.attack.line3;
		} else if (this->delete_line.size() >= 2) {
			basic_score += this->now_level.score.line2;
			normal_attack += this->now_level.attack.line2;
		} else {
			basic_score += this->now_level.score.line;
			normal_attack += this->now_level.attack.line;
		}

		if (this->tspin_status == T_SPIN_MINI) {
			plus_score += this->now_level.score.tspin_mini;
		}
	}

	//追加スコア
	if (perfect) {
		if (this->delete_line.size() >= 4) {
			plus_score += this->now_level.score.perfect_tetris;
			perfect_attack += this->now_level.attack.perfect_tetris;
		} else if (this->delete_line.size() >= 3) {
			plus_score += this->now_level.score.perfect_line3;
			perfect_attack += this->now_level.score.perfect_line3;
		} else if (this->delete_line.size() >= 2) {
			plus_score += this->now_level.score.perfect_line2;
			perfect_attack += this->now_level.attack.perfect_line2;
		} else if (this->delete_line.size() >= 1) {
			plus_score += this->now_level.score.perfect_line;
			perfect_attack += this->now_level.score.perfect_line;
		}
	}

	if (perfect_attack > 0) {
		attack += perfect_attack;
	} else {
		attack += normal_attack;
	}
	//printf("( %d * %f + %d ) * %f\n", basic_score, btb_bonus, plus_score, bonus);
	//printf("attack = %d, perfect = %d, normal = %d\n", attack, perfect_attack, normal_attack);
	this->score += static_cast<int>((basic_score * btb_bonus + plus_score) * bonus);

	//Next Level が存在していて
	//現在のレベルがNext Leveに上がる条件を満たしている場合は
	//レベルを上げる
	//一度に2以上上がることを考慮してwhileで判定し続ける
	//Level1でRENのボーナス入った状態でT spin doubleなど
	while (this->next_level.level > 0 && this->score > this->next_level.score.lvlup_score) {
		this->now_level = this->next_level;//Levelを次のものへ

		//Next Levelを設定する
		//現在のレベル+1のレベル設定情報があるかの探索
		bool contain = false;
		for (auto& next_lvl : this->levels.levels) {
			if (next_lvl.level == this->now_level.level + 1) {
				contain = true;
				this->next_level = next_lvl;
			}
		}

		//レベルがない時は空情報を登録
		if (!contain) {
			this->next_level = Config::Level();
		}
	}

	//std::cout << "score = " << this->score << std::endl;
	//計算したので, もう一度Score計算されないよう計算必要性をfalseへ
	need_calc_score = false;
	return this->score;
}

//相手への攻撃判定
int Board::popAttack() {
	int attk = this->attack;
	this->attack = 0;

	//おじゃま削除
	int attk_tmp = attk;
	while (this->attack_block.size() > 0 && attk_tmp > 0) {
		if (attk_tmp >= this->attack_block[0].height) {
			attk_tmp -= this->attack_block[0].height;

			//削除
			this->attack_block.erase(this->attack_block.begin());
		} else {
			this->attack_block[0].height -= attk_tmp;
			attk_tmp = 0;
			break;
		}
	}

	return attk;
}

//おじゃまブロックの高さ取得
int Board::getAttackHeight() {

	int height_ = 0;
	for (const auto& atk : this->attack_block) {
		height_ += atk.height;
	}

	return height_;
}

//攻撃
void Board::setAtack(int attack, int player_count) {
	//攻撃力0なら攻撃しない
	if (attack <= 0) {
		return;
	}

	//シングルプレイヤーなら攻撃判定とかはされない
	if (player_count <= 1) {
		return;
	}

	int attk = (int)(ceil((double)attack / (double)(player_count - 1)));

	//攻撃判定
	int margin = this->attack_max - getAttackHeight();
	if (margin <= 0)return;

	int set_block = (int)std::min(margin, attk);

	this->attack_block.push_back(AttackBlock(set_block));
}

//メインループ
bool  Board::loop(int action) {

	//ゲームオーバーの時はここだけで完結させる
	if (is_game_over) {
		return !gameOver();
	}

	if (!is_started) {
		return true;
	}

	if (!this->attack_block.empty()) {
		for (auto& blocks_ : this->attack_block) {
			++blocks_;

			if (blocks_.time >= this->now_level.attack_count_r) {
				blocks_.type = attack_red_type;
			} else if (blocks_.time >= this->now_level.attack_count_y) {
				blocks_.type = attack_yellow_type;
			}
		}
	}

	bool force_set = false;
	bool candown = true;
	if (can_action) {

		//キーを押し続けているか判定
		//レベルによって一応変えられる
		if (last_action_count > now_level.last_action_count_max) {
			last_action_count = 0;

			if ((last_action_key & (Config::ROTATION_CW | Config::ROTATION_CCW)) != 0) {
				//最後の操作が回転
				//回転操作は無操作状態もしくは異なる操作を挟まないと,回転の受け付けをしない
			} else {
				//キー情報を消すことで, 一定時間おきに再入力が行える
				last_action_key = Config::NONE;
			}

		} else if (last_action_key != Config::NONE) {
			//キーを押し続けているか判定のためのカウント値上昇
			last_action_count++;
		}

		//キー入力時に動作可能であれば, 動作させる
		//action != NONE は　キー入力があるかの確認
		//((action & LEFT) != 0) は指定のアクションかの確認(左の場合はLEFT=左移動)
		//last_action_key != LEFT はキーを押し続けて連続移動しないようにするもの
		//1回しか押してないのに2回反応するものも防げる
		if (action != Config::NONE && ((action & Config::LEFT) != 0) && last_action_key != Config::LEFT) {
			move(-1, 0, 0);					//左移動
			this->set_count = 0;			//キー入力を行ってからの時間計測よう
			this->last_action_key = Config::LEFT;	//最後に入力したキーを保存
			last_action_count = 0;			//最後に実際に動かした時からの時間計測
		} else if (action != Config::NONE && ((action & Config::RIGHT) != 0) && last_action_key != Config::RIGHT) {
			move(1, 0, 0);					//右移動
			this->set_count = 0;
			this->last_action_key = Config::RIGHT;
			last_action_count = 0;
		} else if (action != Config::NONE && ((action & Config::ROTATION_CW) != 0) && last_action_key != Config::ROTATION_CW) {
			move(0, 0, 1);					//時計周りに回転
			this->set_count = 0;
			this->last_action_key = Config::ROTATION_CW;
			last_action_count = 0;
		} else if (action != Config::NONE && ((action & Config::ROTATION_CCW) != 0) && last_action_key != Config::ROTATION_CCW) {
			move(0, 0, -1);					//反時計周りに回転
			this->set_count = 0;
			this->last_action_key = Config::ROTATION_CCW;
			last_action_count = 0;
		} else if (action != Config::NONE && ((action & Config::HOLD) != 0)) {
			hold();							//ホールド
			this->down_count = 0;
			this->set_count = 0;
			last_action_count = 0;
		} else if (this->down_count > this->now_level.down_speed || (action != Config::NONE && ((action & Config::DOWN) != 0)) && last_action_key != Config::DOWN) {
			//重力もしくは下移動入力の受け付け
			//上記条件に加えて重力での判定も加えている
			this->down_count = 0;
			this->set_count = 0;
			this->last_action_key = Config::DOWN;
			last_action_count = 0;

			this->score++;

			//下へ移動
			candown = move(0, -1, 0);
		} else if (action != Config::NONE && ((action & Config::HARD_DROP) != 0) && last_action_key != Config::HARD_DROP) {
			//ハードドロップ
			this->last_action_key = Config::HARD_DROP;
			this->score += 2 * (this->now_block.y - this->ghost.y);

			this->set_count = 0;
			this->down_count = 0;
			last_action_count = 0;
			now_block = ghost;//操作ブロックをゴーストに書き換えてsetする
			force_set = true;//強制的にset
		}

		this->down_count++;
	} else {
		//操作不可
		if (put_block_state) {
			//ブロック新規設置
			if (can_put_block) {
				setNewBlock();	//新しいブロックを生成
				can_action = true;//動かせるように
			} else {
				//ライン削除後はここに来るはず
				//主に攻撃用ブロック設置
				if (addBlockFromDown()) {
					//攻撃ブロック設置設置が終わったら, 新規ブロック生成へ
					can_put_block = true;
				}
			}
		} else if (delete_line_wait) {
			//ライン削除後の判定
			//消去した後一定時間待つ
			if (delete_line_wait_count > now_level.delete_line_wait_count_max) {
				blockDown();	//ブロックをさげて
				put_block_state = true;//新しいブロックを生成

				//今回使用した変数のリセット
				delete_line_wait = false;
				delete_line_wait_count = 0;
			}
			delete_line_wait_count++;
		}
	}

	if (action == Config::NONE && can_action) {
		//無操作
		this->set_count++;//無操作時間を更新
		last_action_key = Config::NONE;
		last_action_count = 0;
	}

	//無操作が一定時間続くか, HARDDROPしたとき
	if (this->set_count == this->now_level.set_count_max || force_set || !candown) {
		if (candown)candown = move(0, -1, 0, false);//した移動できるか確認(実際に動かさない)

		if (!candown) {
			//下移動不可
			this->set_count = 0;
			this->can_action = false;
			set();			//固定
			deleteBlock();	//ライン消去

			//Score用のBack to Back計算
			if (this->tspin_status != T_SPIN_NONE || delete_line.size() == 4) {
				//テトリス・T-Spin
				back_to_back++;
			} else if (delete_line.size() != 0) {
				//テトリス・T-Spinのどちらもない通常のライン揃え
				back_to_back = 0;
			}



			if (delete_line.size() == 0) {
				//ライン消去がなかった
				//すぐさま新しいブロックを生成(blockDown()を呼ぶ必要なし)
				put_block_state = true;
				ren = 0;
			} else {
				need_calc_score = true;//Score計算の要求
				//消去したラインがあった.(blockDown()を呼ぶ)
				delete_line_wait = true;
				delete_line_wait_count = 0;
				ren++;

				//消去したラインがあったのでScore計算
				calcScore();
			}
		}
	}

	return true;
}

//Score取得
Board::Score Board::getScore() {
	Score s;
	s.level = now_level.level;
	s.ren = ren - 1 < 0 ? 0 : ren - 1 < 0;
	s.score = score;

	//削除のされ方
	//tetrisとかt spinとかしたときに画面に文字が表示されるようにする
	if (this->delete_line.size() >= 4) {
		//テトリス
		s.name1 = "Tetris";
		s.name2 = "";
	} else if (this->delete_line.size() >= 3 && (this->tspin_status == T_SPIN_NORMAL || this->tspin_status == T_SPIN_MINI)) {
		//T-spin triple
		s.name1 = "T spin";
		s.name2 = "   triple";
	} else if (this->delete_line.size() == 2 && (this->tspin_status == T_SPIN_NORMAL || this->tspin_status == T_SPIN_MINI)) {
		//T-spin double
		s.name1 = "T spin";
		s.name2 = "   double";
	} else if (this->delete_line.size() == 1 && (this->tspin_status == T_SPIN_NORMAL || this->tspin_status == T_SPIN_MINI)) {
		//T-spin single
		s.name1 = "T spin";
		s.name2 = "   single";
	} else {
		s.name1 = "";
		s.name2 = "";
	}

	return s;
}

//画像読み込み
void Board::loadimg() {
	int image_size = 9;
	this->images = new cv::Mat[image_size];

	for (int i = 0; i < image_size; i++) {
		cv::Mat tmp = cv::imread("images/" + std::to_string(i) + ".png");
		cv::resize(tmp, tmp, cv::Size(),
			static_cast<double>(img_size) / static_cast<double>(tmp.cols),
			static_cast<double>(img_size) / static_cast<double>(tmp.rows));
		images[i] = tmp.clone();
	}

	std::cout << "load images" << std::endl;
}

//ゴーストの設定
void Board::ghost_set() {
	//ゴーストが空だったり, 操作中のものとタイプが違ったりすれば, 更新する
	if (&ghost != nullptr || ghost.getType() != now_block.getType()) {
		ghost = now_block.clone();
	}

	//とりあえずゴーストを操作中のやつに重ねる(x座標は同じなため)
	ghost.x = now_block.x;
	ghost.y = now_block.y;
	//上（現在の一個下）から置けなくなる場所を下方向に探索する
	for (int gy = now_block.y - 1; gy >= 1; gy--) {
		ghost.y = gy;

		//ゴーストの座標取得
		auto points = ghost.getPoints();
		bool has_block = false;
		for (auto& p : points) {
			if (rawboard[p.y][p.x] != 0) {
				has_block = true;
				break;
			}
		}

		//ゴーストが置けなかったら, 1個上がゴースト位置
		if (has_block) {
			ghost.y = gy + 1;
			break;
		}
	}
}

//ミノの画像取得
cv::Mat Board::getMinoImage(int type, int type_c) {
	//中心位置を調整しながら描画する
	cv::Mat display_mino(MINO_SIZE_MAX * this->img_size, MINO_SIZE_MAX * this->img_size, CV_8UC3, cv::Scalar(0, 0, 0));

	if (type <= 0 || type >= this->mino_number) {
		return display_mino;
	}

	Blocks tmp = this->minos[type].clone();
	tmp.x = 0;
	tmp.y = 0;

	//最大位置・最小位置を保存
	cv::Size min(MINO_SIZE_MAX, MINO_SIZE_MAX);
	cv::Size max(-MINO_SIZE_MAX, -MINO_SIZE_MAX);

	//座標取得
	auto points = tmp.getPoints();
	//最大位置・最小位置を計算
	for (auto& p : points) {
		if (min.height > p.y) min.height = p.y;
		if (min.width > p.x) min.width = p.x;

		if (max.height < p.y) max.height = p.y;
		if (max.width < p.x) max.width = p.x;
	}

	//幅高さ計算
	int width = (max.width - min.width) + 1;
	int height = (max.height - min.height) + 1;

	//中心位置を計算して, ずらす量を出す
	//1引いているのは本家と合わせるため(ほんとは行かないほうがバグ生まないかも)
	int dx = MINO_SIZE_MAX / 2 - width / 2 - 1;
	int dy = MINO_SIZE_MAX / 2 - height / 2;

	//showと違って, ブロックの位置だけ描画する
	for (auto& p : points) {
		int px = p.x - min.width + dx;
		int py = p.y - min.height + dy;

		int image_x = px * this->img_size;
		int image_y = (MINO_SIZE_MAX - 1 - py) * this->img_size;

		if (type_c < 0)paste(display_mino, this->images[type].clone(), image_x, image_y);
		else paste(display_mino, this->images[type_c].clone(), image_x, image_y);
	}
	return display_mino;
}
