#include "keyboard.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

KeyBoard::KeyBoard(const char* file_name) : is_open(false) {
	namespace nl = nlohmann;
	std::ifstream ifs(file_name);

	//����D��x
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

		//key code�擾
		for (const auto& elem : j["key"].items()) {
			for (auto& key : keys) {
				//��Őݒ肵������, ��v���閼�O�ɃL�[�R�[�h���֘A�t����
				if (elem.value()["key name"].get<std::string>() == key.name) {
					//list�����ׂēǂݍ���Őݒ�
					for (const auto& key_code : elem.value()["key code"].items()) {
						key.key_code.push_back((int)key_code.value());
					}
				}
			}

		}

		//�ݒ��ǂ߂����Ƃ�ݒ�
		is_open = true;
	}
}

//cv::waitKey����̃L�[�R�[�h�����ۂ̑���R�[�h�ɕϊ�
int KeyBoard::getAction(int key_code) {
	int action = Config::NONE;

	//���炩�̃L�[���삪������������
	if (key_code != -1) {

		//���삷�ׂĊm�F(���̍ۑ���D��x���ɒT�������)
		for (const auto& key_ : this->keys) {
			//������s���L�[�R�[�h���ׂĊm�F
			for (const auto& key_code_value : key_.key_code) {
				//�L�[�R�[�h������������, �֘A�t����ꂽ�A�N�V������Ԃ�
				if (key_code_value == key_code) {
					action = key_.action;
					break;
				}
			}

			//���삪����������, �T���I��
			if (action != Config::NONE) {
				break;
			}
		}
		if (key_code == 27) { return -1; }
	}

	return action;
}

