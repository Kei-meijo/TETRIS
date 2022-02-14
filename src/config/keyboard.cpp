#include "keyboard.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

KeyBoard::KeyBoard(const char* file_name) : is_open(false) {
	namespace nl = nlohmann;
	std::ifstream ifs(file_name);

	//操作優先度
	this->keys.push_back(Key("LEFT", Config::LEFT));
	this->keys.push_back(Key("RIGHT", Config::RIGHT));
	this->keys.push_back(Key("ROTATION CW", Config::ROTATION_CW));
	this->keys.push_back(Key("ROTATION CCW", Config::ROTATION_CCW));
	this->keys.push_back(Key("HOLD", Config::HOLD));
	this->keys.push_back(Key("DOWN", Config::DOWN));
	this->keys.push_back(Key("HARD DROP", Config::HARD_DROP));

	if (ifs.good()) {
		nl::json j;
		ifs >> j;

		std::cout << j["config name"].get<std::string>() << std::endl;

		//key code取得
		for (const auto& elem : j["key"].items()) {
			for (auto& key : keys) {
				//上で設定した中で, 一致する名前にキーコードを関連付ける
				if (elem.value()["key name"].get<std::string>() == key.name) {
					//listをすべて読み込んで設定
					for (const auto& key_code : elem.value()["key code"].items()) {
						key.key_code.push_back((int)key_code.value());
					}
				}
			}

		}

		//設定を読めたことを設定
		is_open = true;
	}
}

//cv::waitKeyからのキーコードを実際の操作コードに変換
int KeyBoard::getAction(int key_code) {
	int action = Config::NONE;

	//何らかのキー操作があったか判別
	if (key_code != -1) {

		//操作すべて確認(この際操作優先度順に探索される)
		for (const auto& key_ : this->keys) {
			//操作を行うキーコードすべて確認
			for (const auto& key_code_value : key_.key_code) {
				//キーコードが見つかったら, 関連付けられたアクションを返す
				if (key_code_value == key_code) {
					action = key_.action;
					break;
				}
			}

			//操作が見つかったら, 探索終了
			if (action != Config::NONE) {
				break;
			}
		}
		if (key_code == 27) { return -1; }
	}

	return action;
}

