#pragma once
#include <vector>
#include "config.h"
#include "keyboard.h"

#include <Xinput.h>

#pragma comment (lib, "xinput.lib")
#pragma warning(disable : 4995)


class GamePad {
public:
	GamePad(const char* file_name, KeyBoard& keyboard);

	~GamePad();

	//設定が読み込めたかどうか
	inline bool isOpen() {
		return this->is_open;
	}

	//ゲームパッドがいくつ接続されているか
	inline int Size() {
		return this->stats.size();
	}

	//そのIDが接続されているか
	bool isConnected(int id);

	//接続を構築
	bool setConnection(int id);

	//ゲームパッドのIDから操作コード取得
	int getAction(int id);

private:
	struct PadInfo {

		PadInfo() : type(Pad_Input_Type::NONE) {}

		//ボタンタイプ
		enum Pad_Input_Type {
			Button,
			Trigger,
			Stick,
			NONE
		}type;


		//設定のフォーマット
		class Config_Base {
		public:
			Config_Base(const char* name) : name(name) {}
			Config_Base() : name("") {}
			virtual ~Config_Base() {}
			std::string name;
		};

		//ボタン用設定
		class Config_Button : public Config_Base {
		public:
			Config_Button(const char* name, int action) : Config_Base(name), action(action) {}
			Config_Button() : action(Config::NONE) {}

			int action;
		};

		//トリガー用設定
		class Config_Trigger : public Config_Base {
		public:
			Config_Trigger(const char* name, int action, double sensitivity) : Config_Base(name), action(action), sensitivity(sensitivity) {}
			Config_Trigger() : action(Config::NONE), sensitivity(0.0) {}

			int action;
			double sensitivity;
		};

		//スティック用設定
		class Config_Stick : public Config_Base {
		public:
			Config_Stick(const char* name, double sensitivity) : Config_Base(name), sensitivity(sensitivity) {}
			Config_Stick() : sensitivity(0.0) {}

			double sensitivity;

			std::vector<Config_Button> input;
		};

		Config_Base* config;
	};

	std::vector<PadInfo> pad_keys;//設定保存
	int push_max;//長押し判定用

	//ゲームパッドの現在情報
	struct State {
		XINPUT_STATE state;
		int id;

		//長押し判定用
		int last_action;
		int push_count;
	};
	std::vector<State> stats;

	//マスク
	struct MaskInfo {

		MaskInfo() : key_name(""), mask_id(0) {}
		MaskInfo(std::string key_name, int mask_id) : key_name(key_name), mask_id(mask_id) {}

		std::string key_name;
		int mask_id;
	};
	std::vector<MaskInfo> mask;

	//設定が読み込めたかどうか
	bool is_open;
};