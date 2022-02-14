#include <Windows.h>
#include "game_pad.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>


GamePad::GamePad(const char* file_name, KeyBoard& keyboard) : is_open(false), push_max(0){
	namespace nl = nlohmann;
	std::ifstream ifs(file_name);

	//ボタンのマスクを設定
	//十字キー
	mask.push_back(MaskInfo("POV UP", XINPUT_GAMEPAD_DPAD_UP));
	mask.push_back(MaskInfo("POV DOWN", XINPUT_GAMEPAD_DPAD_DOWN));
	mask.push_back(MaskInfo("POV LEFT", XINPUT_GAMEPAD_DPAD_LEFT));
	mask.push_back(MaskInfo("POV RIGHT", XINPUT_GAMEPAD_DPAD_RIGHT));

	//真ん中にいるやつら
	mask.push_back(MaskInfo("START", XINPUT_GAMEPAD_START));
	mask.push_back(MaskInfo("BACK", XINPUT_GAMEPAD_BACK));

	//アナログOFFのトリガー
	mask.push_back(MaskInfo("BUTTON LT", XINPUT_GAMEPAD_LEFT_THUMB));
	mask.push_back(MaskInfo("BUTTON RT", XINPUT_GAMEPAD_RIGHT_THUMB));

	//LR
	mask.push_back(MaskInfo("BUTTON LB", XINPUT_GAMEPAD_LEFT_SHOULDER));
	mask.push_back(MaskInfo("BUTTON RB", XINPUT_GAMEPAD_RIGHT_SHOULDER));

	//ABXY
	mask.push_back(MaskInfo("BUTTON A", XINPUT_GAMEPAD_A));
	mask.push_back(MaskInfo("BUTTON B", XINPUT_GAMEPAD_B));
	mask.push_back(MaskInfo("BUTTON X", XINPUT_GAMEPAD_X));
	mask.push_back(MaskInfo("BUTTON Y", XINPUT_GAMEPAD_Y));

	if (ifs.good()) {
		nl::json j;
		ifs >> j;

		//長押し判定時間
		this->push_max = j["long press judgment"].get<int>();

		//キーコード取得
		for (const auto& elem : j["key"].items()) {
			//入力タイプを取得
			//ボタン・トリガー・スティック
			std::string type = elem.value()["type"].get<std::string>();

			PadInfo config;

			if (type == "button") {//入力タイプボタン
				//ボタン用の設定を作成
				config.type = PadInfo::Button;
				PadInfo::Config_Button* button = new PadInfo::Config_Button();

				//キーの名前取得
				button->name = elem.value()["key name"].get<std::string>();
				//操作名を取得
				std::string action_name = elem.value()["action name"].get<std::string>();

				//キーボードのアクションを使って, 操作名から操作コード取得
				button->action = Config::NONE;
				for (const auto& key : keyboard.keys) {
					if (key.name == action_name) {
						button->action = key.action;
					}
				}

				//設定を保存
				config.config = button;
			} else if (type == "stick") {//入力タイプスティック
				//スティックの設定を作成
				config.type = PadInfo::Stick;
				PadInfo::Config_Stick* stick = new PadInfo::Config_Stick();

				//スティックの名前取得
				stick->name = elem.value()["key name"].get<std::string>();
				//操作感度取得
				stick->sensitivity = elem.value()["sensitivity"].get<double>();

				//操作方向ごとに取得
				for (const auto& dir : elem.value()["action"].items()) {
					//キーボードのアクションを使って, 操作名から操作コード取得
					for (const auto& key : keyboard.keys) {
						if (key.name == dir.value()["action name"].get<std::string>()) {
							PadInfo::Config_Button button;
							button.action = key.action;
							button.name = dir.value()["direction"].get<std::string>();
							stick->input.push_back(button);//操作を保存
						}
					}
				}

				//設定を保存
				config.config = stick;
			} else if (type == "trigger") {//入力タイプトリガー
				//トリガーの設定を作成
				config.type = PadInfo::Trigger;
				PadInfo::Config_Trigger* trigger = new PadInfo::Config_Trigger();

				//キーの名前取得
				trigger->name = elem.value()["key name"].get<std::string>();
				//操作名を取得
				std::string action_name = elem.value()["action name"].get<std::string>();
				//操作感度取得
				trigger->sensitivity = elem.value()["sensitivity"].get<double>();

				//キーボードのアクションを使って, 操作名から操作コード取得
				trigger->action = Config::NONE;
				for (const auto& key : keyboard.keys) {
					if (key.name == action_name) {
						trigger->action = key.action;
					}
				}

				//設定を保存
				config.config = trigger;
			}
			 
			//設定を追加
			this->pad_keys.push_back(config);
		}

		//設定を開けたことを保存する
		is_open = true;
	}
}

GamePad::~GamePad() {
	//ゲームパッドオブジェクト削除の際, ゲームパッドの状態をOFF
	XInputEnable(false);
}

//そのIDが接続されているか
bool GamePad::isConnected(int id) {
	for (const auto& connection : this->stats) {
		if (connection.id == id) {
			return true;
		}
	}

	return false;
}

//接続を構築
bool GamePad::setConnection(int id) {
	//すでに接続されていれば何もしない
	if (isConnected(id))return true;

	//ステータスを用意
	State stat;
	stat.id = id;
	//メモリリセット
	ZeroMemory(&stat.state, sizeof(XINPUT_STATE));
	//入力が取得できる = 接続されている
	if (XInputGetState(stat.id, &stat.state) == ERROR_SUCCESS) {
		//ステータスを保存
		this->stats.push_back(stat);

		return true;
	}

	//接続されていない
	return false;
}

//ゲームパッドのIDから操作コード取得
int GamePad::getAction(int id) {
	XINPUT_STATE* status = nullptr;
	int* last_action = nullptr;
	int* push_count = nullptr;

	//ステータスを取得
	for (int i = 0; i < this->stats.size(); i++) {
		if (this->stats[i].id == id) {
			status = &this->stats[i].state;
			last_action = &this->stats[i].last_action;
			push_count = &this->stats[i].push_count;
		}
	}

	//ステータスがない(そのIDが接続されていない)
	if (status == nullptr) {
		return Config::NONE;
	}

	//入力を拾う
	if (XInputGetState(id, status) == ERROR_SUCCESS) {

		//スタートボタン
		if (status->Gamepad.wButtons & XINPUT_GAMEPAD_START) {
			return Config::START;
		}

		//戻るボタン
		if (status->Gamepad.wButtons & XINPUT_GAMEPAD_BACK) {
			return Config::BACK;
		}

		//キーコードの中から一致するものを探索
		for (const auto& pad_key : this->pad_keys) {

			//キータイプがボタン
			if (pad_key.type == PadInfo::Button) {
				//操作設定をボタン用に変換
				PadInfo::Config_Button* btn = dynamic_cast<PadInfo::Config_Button*>(pad_key.config);
				//操作が設定されていない
				if (btn->action == Config::NONE)continue;

				//ボタン用マスクからマスク取得
				for (const auto& mask_info : this->mask) {
					//ボタン名とマスク名一致
					if (mask_info.key_name == btn->name) {
						//マスクを使って, ボタンが押されているか確認
						if (status->Gamepad.wButtons & mask_info.mask_id) {

							//長押しの判定
							if (*last_action == btn->action) {//	長押ししている
								(*push_count)++;

								//長時間押していたら
								if (*push_count >= push_max) {
									//操作を伝達
									return btn->action | Config::LONG_PRESS;
								} else {
									//短時間長押しは, 操作を空に
									return Config::LONG_PRESS;
								}
							} else {
								//単押し
								*last_action = btn->action;
								return btn->action;
							}
						}
					}
				}
			} else if (pad_key.type == PadInfo::Stick) {//キータイプがスティック
				//操作設定をスティック用に変換
				PadInfo::Config_Stick* stk = dynamic_cast<PadInfo::Config_Stick*>(pad_key.config);
				short x, y;

				//(X, Y)座標を取得
				if (stk->name == "JOY STICK 1") {
					//左パッドから取得
					x = status->Gamepad.sThumbLX;
					y = status->Gamepad.sThumbLY;

					//センター位置なら原点へ移動(デッドゾーン内)
					if (abs(x) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && abs(y) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
						x = 0;
						y = 0;
					}
				} else {
					//右パッドから取得(デッドゾーン内)
					x = status->Gamepad.sThumbRX;
					y = status->Gamepad.sThumbRY;

					//センター位置なら原点へ移動
					if (abs(x) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && abs(y) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
						x = 0;
						y = 0;
					}
				}

				//スティック操作を探索
				for (const auto& bar : stk->input) {
					//操作が設定されていない場合, 探索不要
					if (bar.action == Config::NONE)continue;

					//各操作ごとに判定
					int action = Config::NONE;
					if (bar.name == "LEFT") {
						//左側x座標値に感度を乗算したものより左側にあるか
						//以下同じような処理
						if (x < -32768 * stk->sensitivity) {
							action = bar.action;
						}
					} else if (bar.name == "RIGHT") {
						if (x > 32767 * stk->sensitivity) {
							action = bar.action;
						}
					} else if (bar.name == "DOWN") {
						if (y < -32768 * stk->sensitivity) {
							action = bar.action;
						}
					} else if (bar.name == "UP") {
						if (y > 32767 * stk->sensitivity) {
							action = bar.action;
						}
					}

					//長押しの判定(ボタン用とほぼ同じ)
					if (action != Config::NONE) {
						if (*last_action == action) {
							(*push_count)++;

							if (*push_count >= push_max) {
								//操作を伝達
								return action | Config::LONG_PRESS;
							} else {
								//短時間長押しは, 操作を空に
								return Config::LONG_PRESS;
							}
						} else {
							*last_action = action;
							return action;
						}
					}
				}
			} else if (pad_key.type == PadInfo::Trigger) {//キータイプがトリガー
				//操作設定をトリガー用に変換
				PadInfo::Config_Trigger* trg = dynamic_cast<PadInfo::Config_Trigger*>(pad_key.config);

				//各操作ごとに判定
				int trgger_val = 0;
				if (trg->name == "LT") {
					trgger_val = status->Gamepad.bLeftTrigger;
				} else if (trg->name == "RT") {
					trgger_val = status->Gamepad.bRightTrigger;
				}

				//最大値に感度を乗算したものより左側にあるか
				if (trgger_val > 255 * trg->sensitivity) {

					//長押しの判定(ボタン用とほぼ同じ)
					if (*last_action == trg->action) {
						(*push_count)++;

						if (*push_count >= push_max) {
							//操作を伝達
							return trg->action | Config::LONG_PRESS;
						} else {
							//短時間長押しは, 操作を空に
							return Config::LONG_PRESS;
						}
					} else {
						*last_action = trg->action;
						return trg->action;
					}
				}
			}
		}
	}

	//ここに来るときは無操作
	*push_count = 0;
	*last_action = Config::NONE;
	return Config::NONE;
}