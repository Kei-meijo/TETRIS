#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
//#define GAME_OVER_1

//board
#include "board/board.h"

//display
#include "opencv/display.h"
#include "opencv/cv_util.h"

//fps
#include"fps/TimeBaseLoopExecuter.h"

//config
#include "config/config.h"
#include "config/game_pad.h"
#include "config/keyboard.h"

#ifdef _DEBUG
int main() {
#else
#include <Windows.h>

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
){ 
//int main() {
#endif // _DEBUG
	//Level 設定 ファイル読み込み
	Config config("param/tetris.json");

	//キーボード 設定 ファイル読み込み
	KeyBoard keyboard("param/keyboard.json");

	//PAD 設定 ファイル読み込み
	GamePad game_pad("param/game_pad.json", keyboard);

	//PAD 接続
	for (int i = 0;; i++) {
		if (!game_pad.setConnection(i))break;
	}

	std::cout << "プレイ人数 = " << game_pad.Size() << std::endl;

	//ゲーム画面生成
	std::vector<Board*> bs;
	for (int i = 0; i < max(game_pad.Size(), 1); i++) {
		bs.push_back(new Board(config.width, config.height, config));
	}

	//player ゲームパッドID設定
	int* start = new int[max(game_pad.Size(), 1)];
	for (int i = 0; i < max(game_pad.Size(), 1); i++) {
		start[i] = -1;
	}

	//順位設定
	int* rank = new int[max(game_pad.Size(), 1)];
	for (int i = 0; i < max(game_pad.Size(), 1); i++) {
		rank[i] = -1;
	}

	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	//FPS設定
	TimeBaseLoopExecuter exec(config.fps);
	bool game_state = false;
	for (int count = 0;; count++) {
		//GAME処理
		int action = Config::NONE;


		cv::Mat display;
		for (int i = 0; i < bs.size(); i++) {
			cv::Mat tmp;

			//画面描画
			createDisplay(*bs[i], tmp);

			//PRESS STARTの文字の描画
			if (start[i] < 0) {
				cv::putText(tmp, std::to_string(i + 1) + "P press start", cv::Point(150, tmp.rows / 2), cv::FONT_HERSHEY_COMPLEX | cv::FONT_ITALIC, 1.5, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
			}

			//順位の描画
			if (rank[i] > 0) {
				cv::putText(tmp, "RANK  " + std::to_string(rank[i]), cv::Point(tmp.cols / 2 - 100, tmp.rows / 2), cv::FONT_HERSHEY_COMPLEX | cv::FONT_ITALIC, 1.5, cv::Scalar(197, 43, 57), 2, cv::LINE_AA);
			}

			//画面結合
			if (display.empty()) {
				//画面に何も描画されていないとき
				display = tmp;
			} else {
				//画面右側に画面を結合
				concat(display, display, tmp, 10);
			}
		}

		//本当はダブルバッファとかしないといけないけど,
		//(マルチスレッドじゃないから)描画やゲーム処理中に画面更新されない
		//ので, シングルバッファでも平気

		//テトリスみたいな軽いゲームじゃないと通用しない
		cv::imshow("display", display);
		if (count == 0) {
			cv::moveWindow("display", (width - display.cols) / 2, (height - display.rows) / 2);
		}

		//キー取得(呼ばないと画面描画もしないみたい)
		//待機時間は最小の1msで
		int key = cv::waitKey(1);
		action = keyboard.getAction(key);
		if (action < 0)break;

		bool can_start = true;
		for (int i = 0; i < max(game_pad.Size(), 1); i++) {
			//iはプレイヤー番号
			//i : 0  => 1P, i : 1 => 2P

			//上の他にゲームパッドのIDがある
			//0以上になるが, どんな風に決まるかは, 多分USBの認識順とか?
			//自由に決めるためにSTART押した順に, 1P 2P としたい
			//ので, プレイヤー番号とゲームパッドのIDを柔軟に紐づけする必要がある


			//外部からのactionを引継ぐことでプレイヤーが一人の場合はキーボードでも操作可能になる(はず)
			if (action == Config::NONE) {

				if (start[i] < 0) {
					//SATRTボタンを押してない時にゲームパッドをどう割り振るか決定する
					//start 押してないうちで何番目か(ID若い順)
					int cc = 0;
					for (int j = 0; j < i; j++) {
						if (start[j] < 0)cc++;//start押してないプレイヤーは-1が入っている. その他はゲームパッドのID
					}

					//start 押してないIDからn番目のIDを取得
					int select = 0;
					for (; select < max(game_pad.Size(), 1); select++) {
						//今探索中のIDがstart押したか調べる
						bool contain = false;
						for (int j = 0; j < i; j++) {
							if (start[j] == select) {
								//start押しているのでスキップ
								contain = true;
								break;
							}
						}

						//start押してない　かつ　n番目
						if (!contain && cc <= 0) {
							action = game_pad.getAction(select);

							//startが今押されているか取得
							if (action == Config::START) {
								//押されていたら, リストの保存可能な中での一番先頭に保存
								//リストの保存可能な中 : 何Pかまだ決まっていないところ
								for (int j = 0; j < max(game_pad.Size(), 1); j++) {
									if (start[j] < 0) {
										start[j] = select;
										break;
									}
								}
							}
							break;
						} else if (!contain) {
							cc--;
							//番目を1引くことで,一致判定が0と比較するだけでよくなる
							//カウンタ用の変数不要
						}
					}
				} else {
					//SATRTボタンを押しているので, 保存済みのIDを呼び出す
					action = game_pad.getAction(start[i]);
				}
			} else if(action == Config::START){
				//キーボード用スタート
				start[i] = 1;
			}

			if (action == Config::BACK) {
				break;
			}


			if (start[i] < 0) can_start = false;

			//ゲーム処理
			bs[i]->loop(action);
			action = Config::NONE;
			//次のプレイヤーに影響しないよう
			//キーボード(ループ外)→1P : OK
			//1P→2P : NG

			//攻撃等
			int atk = bs[i]->popAttack();//攻撃取得
			for (int j = 0; j < game_pad.Size(); j++) {
				if (i == j)continue;//自分自身には攻撃しない
				bs[j]->setAtack(atk, game_pad.Size());//上で決めた攻撃をセット
			}
		}

		if (action == Config::BACK)break;

		//マルチプレイ時のみ
		//他全員がゲームオーバーで終了
		if (game_pad.Size() >= 2) {
			int last_player = -1;
			int player_count = 0;
			for (int i = 0; i < game_pad.Size(); i++) {
				if (bs[i]->isGameOver()) {
					//順位設定
					//rankが負は未決定なので, 順位をここで確定させる
					if (rank[i] < 0) {
						//すでにゲームオーバーになった人数をカウント
						int finished_playing = 0;
						for (int j = 0; j < game_pad.Size(); j++) {
							if (rank[j] > 0)finished_playing++;
						}

						//順位はプレイヤー人数 - すでにゲームオーバーになった人数
						//すでにゲームオーバーになった人数が0の時は最下位
						rank[i] = game_pad.Size() - finished_playing;
					}
				} else {
					//プレイヤー人数をカウント
					//生きているプレイヤーの番号も保存
					player_count++;
					last_player = i;
				}
			}

			//生き残りが一人のときは, 生きているプレイヤーが1位で確定
			if (player_count == 1) {
				if (rank[last_player] < 0)bs[last_player]->end();//ゲーム終了
				rank[last_player] = 1;//生きているプレイヤーを1位へ
			}
		}

		//ゲームスタート
		//ただしもうすでにスタート済みならもうスタートの命令は出さない
		if (can_start && !game_state) {
			for (auto& board : bs) {
				board->start();
			}
			game_state = true;//startを何回も呼ばないようにする
		}

		//FPS調整
		exec.TimeAdjustment();
	}




	for (auto& board : bs) {
		delete board;
	}
	delete[] start;
	delete[] rank;
}