#pragma once
#include <vector>
#include "config.h"
#include <string>

class GamePad;

class KeyBoard {
	//�Q�[���p�b�h�ɂ�, ����D�揇�ʂ����L����
	friend GamePad;
public:
	KeyBoard(const char* file_name);

	//�ݒ肪�ǂݍ��߂����ǂ���
	inline bool isOpen() {
		return is_open;
	}

	//cv::waitKey����̃L�[�R�[�h�����ۂ̑���R�[�h�ɕϊ�
	int getAction(int key_code);

private:

	//�L�[�R�[�h��ۑ�����I�u�W�F�N�g
	struct Key {
		Key(std::string name, int action) : name(name), action(action) {}

		Key() : name(""), action(0) {}

		std::string name;			//���얼(Config�Ɠ����ɂ��Ă���)
		int action;					//����R�[�h
		std::vector<int> key_code;	//�L�[�R�[�h
	};
	std::vector<Key> keys;	//�L�[�R�[�h
	bool is_open;			//�ݒ肪�J������
};