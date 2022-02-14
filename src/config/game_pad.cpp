#include <Windows.h>
#include "game_pad.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>


GamePad::GamePad(const char* file_name, KeyBoard& keyboard) : is_open(false), push_max(0){
	namespace nl = nlohmann;
	std::ifstream ifs(file_name);

	//�{�^���̃}�X�N��ݒ�
	//�\���L�[
	mask.push_back(MaskInfo("POV UP", XINPUT_GAMEPAD_DPAD_UP));
	mask.push_back(MaskInfo("POV DOWN", XINPUT_GAMEPAD_DPAD_DOWN));
	mask.push_back(MaskInfo("POV LEFT", XINPUT_GAMEPAD_DPAD_LEFT));
	mask.push_back(MaskInfo("POV RIGHT", XINPUT_GAMEPAD_DPAD_RIGHT));

	//�^�񒆂ɂ�����
	mask.push_back(MaskInfo("START", XINPUT_GAMEPAD_START));
	mask.push_back(MaskInfo("BACK", XINPUT_GAMEPAD_BACK));

	//�A�i���OOFF�̃g���K�[
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

		//���������莞��
		this->push_max = j["long press judgment"].get<int>();

		//�L�[�R�[�h�擾
		for (const auto& elem : j["key"].items()) {
			//���̓^�C�v���擾
			//�{�^���E�g���K�[�E�X�e�B�b�N
			std::string type = elem.value()["type"].get<std::string>();

			PadInfo config;

			if (type == "button") {//���̓^�C�v�{�^��
				//�{�^���p�̐ݒ���쐬
				config.type = PadInfo::Button;
				PadInfo::Config_Button* button = new PadInfo::Config_Button();

				//�L�[�̖��O�擾
				button->name = elem.value()["key name"].get<std::string>();
				//���얼���擾
				std::string action_name = elem.value()["action name"].get<std::string>();

				//�L�[�{�[�h�̃A�N�V�������g����, ���얼���瑀��R�[�h�擾
				button->action = Config::NONE;
				for (const auto& key : keyboard.keys) {
					if (key.name == action_name) {
						button->action = key.action;
					}
				}

				//�ݒ��ۑ�
				config.config = button;
			} else if (type == "stick") {//���̓^�C�v�X�e�B�b�N
				//�X�e�B�b�N�̐ݒ���쐬
				config.type = PadInfo::Stick;
				PadInfo::Config_Stick* stick = new PadInfo::Config_Stick();

				//�X�e�B�b�N�̖��O�擾
				stick->name = elem.value()["key name"].get<std::string>();
				//���슴�x�擾
				stick->sensitivity = elem.value()["sensitivity"].get<double>();

				//����������ƂɎ擾
				for (const auto& dir : elem.value()["action"].items()) {
					//�L�[�{�[�h�̃A�N�V�������g����, ���얼���瑀��R�[�h�擾
					for (const auto& key : keyboard.keys) {
						if (key.name == dir.value()["action name"].get<std::string>()) {
							PadInfo::Config_Button button;
							button.action = key.action;
							button.name = dir.value()["direction"].get<std::string>();
							stick->input.push_back(button);//�����ۑ�
						}
					}
				}

				//�ݒ��ۑ�
				config.config = stick;
			} else if (type == "trigger") {//���̓^�C�v�g���K�[
				//�g���K�[�̐ݒ���쐬
				config.type = PadInfo::Trigger;
				PadInfo::Config_Trigger* trigger = new PadInfo::Config_Trigger();

				//�L�[�̖��O�擾
				trigger->name = elem.value()["key name"].get<std::string>();
				//���얼���擾
				std::string action_name = elem.value()["action name"].get<std::string>();
				//���슴�x�擾
				trigger->sensitivity = elem.value()["sensitivity"].get<double>();

				//�L�[�{�[�h�̃A�N�V�������g����, ���얼���瑀��R�[�h�擾
				trigger->action = Config::NONE;
				for (const auto& key : keyboard.keys) {
					if (key.name == action_name) {
						trigger->action = key.action;
					}
				}

				//�ݒ��ۑ�
				config.config = trigger;
			}
			 
			//�ݒ��ǉ�
			this->pad_keys.push_back(config);
		}

		//�ݒ���J�������Ƃ�ۑ�����
		is_open = true;
	}
}

GamePad::~GamePad() {
	//�Q�[���p�b�h�I�u�W�F�N�g�폜�̍�, �Q�[���p�b�h�̏�Ԃ�OFF
	XInputEnable(false);
}

//����ID���ڑ�����Ă��邩
bool GamePad::isConnected(int id) {
	for (const auto& connection : this->stats) {
		if (connection.id == id) {
			return true;
		}
	}

	return false;
}

//�ڑ����\�z
bool GamePad::setConnection(int id) {
	//���łɐڑ�����Ă���Ή������Ȃ�
	if (isConnected(id))return true;

	//�X�e�[�^�X��p��
	State stat;
	stat.id = id;
	//���������Z�b�g
	ZeroMemory(&stat.state, sizeof(XINPUT_STATE));
	//���͂��擾�ł��� = �ڑ�����Ă���
	if (XInputGetState(stat.id, &stat.state) == ERROR_SUCCESS) {
		//�X�e�[�^�X��ۑ�
		this->stats.push_back(stat);

		return true;
	}

	//�ڑ�����Ă��Ȃ�
	return false;
}

//�Q�[���p�b�h��ID���瑀��R�[�h�擾
int GamePad::getAction(int id) {
	XINPUT_STATE* status = nullptr;
	int* last_action = nullptr;
	int* push_count = nullptr;

	//�X�e�[�^�X���擾
	for (int i = 0; i < this->stats.size(); i++) {
		if (this->stats[i].id == id) {
			status = &this->stats[i].state;
			last_action = &this->stats[i].last_action;
			push_count = &this->stats[i].push_count;
		}
	}

	//�X�e�[�^�X���Ȃ�(����ID���ڑ�����Ă��Ȃ�)
	if (status == nullptr) {
		return Config::NONE;
	}

	//���͂��E��
	if (XInputGetState(id, status) == ERROR_SUCCESS) {

		//�X�^�[�g�{�^��
		if (status->Gamepad.wButtons & XINPUT_GAMEPAD_START) {
			return Config::START;
		}

		//�߂�{�^��
		if (status->Gamepad.wButtons & XINPUT_GAMEPAD_BACK) {
			return Config::BACK;
		}

		//�L�[�R�[�h�̒������v������̂�T��
		for (const auto& pad_key : this->pad_keys) {

			//�L�[�^�C�v���{�^��
			if (pad_key.type == PadInfo::Button) {
				//����ݒ���{�^���p�ɕϊ�
				PadInfo::Config_Button* btn = dynamic_cast<PadInfo::Config_Button*>(pad_key.config);
				//���삪�ݒ肳��Ă��Ȃ�
				if (btn->action == Config::NONE)continue;

				//�{�^���p�}�X�N����}�X�N�擾
				for (const auto& mask_info : this->mask) {
					//�{�^�����ƃ}�X�N����v
					if (mask_info.key_name == btn->name) {
						//�}�X�N���g����, �{�^����������Ă��邩�m�F
						if (status->Gamepad.wButtons & mask_info.mask_id) {

							//�������̔���
							if (*last_action == btn->action) {//	���������Ă���
								(*push_count)++;

								//�����ԉ����Ă�����
								if (*push_count >= push_max) {
									//�����`�B
									return btn->action | Config::LONG_PRESS;
								} else {
									//�Z���Ԓ�������, ��������
									return Config::LONG_PRESS;
								}
							} else {
								//�P����
								*last_action = btn->action;
								return btn->action;
							}
						}
					}
				}
			} else if (pad_key.type == PadInfo::Stick) {//�L�[�^�C�v���X�e�B�b�N
				//����ݒ���X�e�B�b�N�p�ɕϊ�
				PadInfo::Config_Stick* stk = dynamic_cast<PadInfo::Config_Stick*>(pad_key.config);
				short x, y;

				//(X, Y)���W���擾
				if (stk->name == "JOY STICK 1") {
					//���p�b�h����擾
					x = status->Gamepad.sThumbLX;
					y = status->Gamepad.sThumbLY;

					//�Z���^�[�ʒu�Ȃ猴�_�ֈړ�(�f�b�h�]�[����)
					if (abs(x) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && abs(y) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
						x = 0;
						y = 0;
					}
				} else {
					//�E�p�b�h����擾(�f�b�h�]�[����)
					x = status->Gamepad.sThumbRX;
					y = status->Gamepad.sThumbRY;

					//�Z���^�[�ʒu�Ȃ猴�_�ֈړ�
					if (abs(x) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && abs(y) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
						x = 0;
						y = 0;
					}
				}

				//�X�e�B�b�N�����T��
				for (const auto& bar : stk->input) {
					//���삪�ݒ肳��Ă��Ȃ��ꍇ, �T���s�v
					if (bar.action == Config::NONE)continue;

					//�e���삲�Ƃɔ���
					int action = Config::NONE;
					if (bar.name == "LEFT") {
						//����x���W�l�Ɋ��x����Z�������̂�荶���ɂ��邩
						//�ȉ������悤�ȏ���
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

					//�������̔���(�{�^���p�Ƃقړ���)
					if (action != Config::NONE) {
						if (*last_action == action) {
							(*push_count)++;

							if (*push_count >= push_max) {
								//�����`�B
								return action | Config::LONG_PRESS;
							} else {
								//�Z���Ԓ�������, ��������
								return Config::LONG_PRESS;
							}
						} else {
							*last_action = action;
							return action;
						}
					}
				}
			} else if (pad_key.type == PadInfo::Trigger) {//�L�[�^�C�v���g���K�[
				//����ݒ���g���K�[�p�ɕϊ�
				PadInfo::Config_Trigger* trg = dynamic_cast<PadInfo::Config_Trigger*>(pad_key.config);

				//�e���삲�Ƃɔ���
				int trgger_val = 0;
				if (trg->name == "LT") {
					trgger_val = status->Gamepad.bLeftTrigger;
				} else if (trg->name == "RT") {
					trgger_val = status->Gamepad.bRightTrigger;
				}

				//�ő�l�Ɋ��x����Z�������̂�荶���ɂ��邩
				if (trgger_val > 255 * trg->sensitivity) {

					//�������̔���(�{�^���p�Ƃقړ���)
					if (*last_action == trg->action) {
						(*push_count)++;

						if (*push_count >= push_max) {
							//�����`�B
							return trg->action | Config::LONG_PRESS;
						} else {
							//�Z���Ԓ�������, ��������
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

	//�����ɗ���Ƃ��͖�����
	*push_count = 0;
	*last_action = Config::NONE;
	return Config::NONE;
}