#pragma once
#include <vector>
#include "config.h"
#include <string>

class GamePad;

class KeyBoard {
	//ゲームパッドには, 操作優先順位を共有する
	friend GamePad;
public:
	KeyBoard(const char* file_name);

	//設定が読み込めたかどうか
	inline bool isOpen() {
		return is_open;
	}

	//cv::waitKeyからのキーコードを実際の操作コードに変換
	int getAction(int key_code);

private:

	//キーコードを保存するオブジェクト
	struct Key {
		Key(std::string name, int action) : name(name), action(action) {}

		Key() : name(""), action(0) {}

		std::string name;			//操作名(Configと同名にしておく)
		int action;					//操作コード
		std::vector<int> key_code;	//キーコード
	};
	std::vector<Key> keys;	//キーコード
	bool is_open;			//設定が開けたら
};