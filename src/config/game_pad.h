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

	//�ݒ肪�ǂݍ��߂����ǂ���
	inline bool isOpen() {
		return this->is_open;
	}

	//�Q�[���p�b�h�������ڑ�����Ă��邩
	inline int Size() {
		return this->stats.size();
	}

	//����ID���ڑ�����Ă��邩
	bool isConnected(int id);

	//�ڑ����\�z
	bool setConnection(int id);

	//�Q�[���p�b�h��ID���瑀��R�[�h�擾
	int getAction(int id);

private:
	struct PadInfo {

		PadInfo() : type(Pad_Input_Type::NONE) {}

		//�{�^���^�C�v
		enum Pad_Input_Type {
			Button,
			Trigger,
			Stick,
			NONE
		}type;


		//�ݒ�̃t�H�[�}�b�g
		class Config_Base {
		public:
			Config_Base(const char* name) : name(name) {}
			Config_Base() : name("") {}
			virtual ~Config_Base() {}
			std::string name;
		};

		//�{�^���p�ݒ�
		class Config_Button : public Config_Base {
		public:
			Config_Button(const char* name, int action) : Config_Base(name), action(action) {}
			Config_Button() : action(Config::NONE) {}

			int action;
		};

		//�g���K�[�p�ݒ�
		class Config_Trigger : public Config_Base {
		public:
			Config_Trigger(const char* name, int action, double sensitivity) : Config_Base(name), action(action), sensitivity(sensitivity) {}
			Config_Trigger() : action(Config::NONE), sensitivity(0.0) {}

			int action;
			double sensitivity;
		};

		//�X�e�B�b�N�p�ݒ�
		class Config_Stick : public Config_Base {
		public:
			Config_Stick(const char* name, double sensitivity) : Config_Base(name), sensitivity(sensitivity) {}
			Config_Stick() : sensitivity(0.0) {}

			double sensitivity;

			std::vector<Config_Button> input;
		};

		Config_Base* config;
	};

	std::vector<PadInfo> pad_keys;//�ݒ�ۑ�
	int push_max;//����������p

	//�Q�[���p�b�h�̌��ݏ��
	struct State {
		XINPUT_STATE state;
		int id;

		//����������p
		int last_action;
		int push_count;
	};
	std::vector<State> stats;

	//�}�X�N
	struct MaskInfo {

		MaskInfo() : key_name(""), mask_id(0) {}
		MaskInfo(std::string key_name, int mask_id) : key_name(key_name), mask_id(mask_id) {}

		std::string key_name;
		int mask_id;
	};
	std::vector<MaskInfo> mask;

	//�ݒ肪�ǂݍ��߂����ǂ���
	bool is_open;
};