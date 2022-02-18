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
	//Level �ݒ� �t�@�C���ǂݍ���
	Config config("param/tetris.json");

	//�L�[�{�[�h �ݒ� �t�@�C���ǂݍ���
	KeyBoard keyboard("param/keyboard.json");

	//PAD �ݒ� �t�@�C���ǂݍ���
	GamePad game_pad("param/game_pad.json", keyboard);

	//PAD �ڑ�
	for (int i = 0;; i++) {
		if (!game_pad.setConnection(i))break;
	}

	std::cout << "�v���C�l�� = " << game_pad.Size() << std::endl;

	//�Q�[����ʐ���
	std::vector<Board*> bs;
	for (int i = 0; i < max(game_pad.Size(), 1); i++) {
		bs.push_back(new Board(config.width, config.height, config));
	}

	//player �Q�[���p�b�hID�ݒ�
	int* start = new int[max(game_pad.Size(), 1)];
	for (int i = 0; i < max(game_pad.Size(), 1); i++) {
		start[i] = -1;
	}

	//���ʐݒ�
	int* rank = new int[max(game_pad.Size(), 1)];
	for (int i = 0; i < max(game_pad.Size(), 1); i++) {
		rank[i] = -1;
	}

	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	//FPS�ݒ�
	TimeBaseLoopExecuter exec(config.fps);
	bool game_state = false;
	for (int count = 0;; count++) {
		//GAME����
		int action = Config::NONE;


		cv::Mat display;
		for (int i = 0; i < bs.size(); i++) {
			cv::Mat tmp;

			//��ʕ`��
			createDisplay(*bs[i], tmp);

			//PRESS START�̕����̕`��
			if (start[i] < 0) {
				cv::putText(tmp, std::to_string(i + 1) + "P press start", cv::Point(150, tmp.rows / 2), cv::FONT_HERSHEY_COMPLEX | cv::FONT_ITALIC, 1.5, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
			}

			//���ʂ̕`��
			if (rank[i] > 0) {
				cv::putText(tmp, "RANK  " + std::to_string(rank[i]), cv::Point(tmp.cols / 2 - 100, tmp.rows / 2), cv::FONT_HERSHEY_COMPLEX | cv::FONT_ITALIC, 1.5, cv::Scalar(197, 43, 57), 2, cv::LINE_AA);
			}

			//��ʌ���
			if (display.empty()) {
				//��ʂɉ����`�悳��Ă��Ȃ��Ƃ�
				display = tmp;
			} else {
				//��ʉE���ɉ�ʂ�����
				concat(display, display, tmp, 10);
			}
		}

		//�{���̓_�u���o�b�t�@�Ƃ����Ȃ��Ƃ����Ȃ�����,
		//(�}���`�X���b�h����Ȃ�����)�`���Q�[���������ɉ�ʍX�V����Ȃ�
		//�̂�, �V���O���o�b�t�@�ł����C

		//�e�g���X�݂����Ȍy���Q�[������Ȃ��ƒʗp���Ȃ�
		cv::imshow("display", display);
		if (count == 0) {
			cv::moveWindow("display", (width - display.cols) / 2, (height - display.rows) / 2);
		}

		//�L�[�擾(�Ă΂Ȃ��Ɖ�ʕ`������Ȃ��݂���)
		//�ҋ@���Ԃ͍ŏ���1ms��
		int key = cv::waitKey(1);
		action = keyboard.getAction(key);
		if (action < 0)break;

		bool can_start = true;
		for (int i = 0; i < max(game_pad.Size(), 1); i++) {
			//i�̓v���C���[�ԍ�
			//i : 0  => 1P, i : 1 => 2P

			//��̑��ɃQ�[���p�b�h��ID������
			//0�ȏ�ɂȂ邪, �ǂ�ȕ��Ɍ��܂邩��, ����USB�̔F�����Ƃ�?
			//���R�Ɍ��߂邽�߂�START����������, 1P 2P �Ƃ�����
			//�̂�, �v���C���[�ԍ��ƃQ�[���p�b�h��ID���_��ɕR�Â�����K�v������


			//�O�������action�����p�����ƂŃv���C���[����l�̏ꍇ�̓L�[�{�[�h�ł�����\�ɂȂ�(�͂�)
			if (action == Config::NONE) {

				if (start[i] < 0) {
					//SATRT�{�^���������ĂȂ����ɃQ�[���p�b�h���ǂ�����U�邩���肷��
					//start �����ĂȂ������ŉ��Ԗڂ�(ID�Ⴂ��)
					int cc = 0;
					for (int j = 0; j < i; j++) {
						if (start[j] < 0)cc++;//start�����ĂȂ��v���C���[��-1�������Ă���. ���̑��̓Q�[���p�b�h��ID
					}

					//start �����ĂȂ�ID����n�Ԗڂ�ID���擾
					int select = 0;
					for (; select < max(game_pad.Size(), 1); select++) {
						//���T������ID��start�����������ׂ�
						bool contain = false;
						for (int j = 0; j < i; j++) {
							if (start[j] == select) {
								//start�����Ă���̂ŃX�L�b�v
								contain = true;
								break;
							}
						}

						//start�����ĂȂ��@���@n�Ԗ�
						if (!contain && cc <= 0) {
							action = game_pad.getAction(select);

							//start����������Ă��邩�擾
							if (action == Config::START) {
								//������Ă�����, ���X�g�̕ۑ��\�Ȓ��ł̈�Ԑ擪�ɕۑ�
								//���X�g�̕ۑ��\�Ȓ� : ��P���܂����܂��Ă��Ȃ��Ƃ���
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
							//�Ԗڂ�1�������Ƃ�,��v���肪0�Ɣ�r���邾���ł悭�Ȃ�
							//�J�E���^�p�̕ϐ��s�v
						}
					}
				} else {
					//SATRT�{�^���������Ă���̂�, �ۑ��ς݂�ID���Ăяo��
					action = game_pad.getAction(start[i]);
				}
			} else if(action == Config::START){
				//�L�[�{�[�h�p�X�^�[�g
				start[i] = 1;
			}

			if (action == Config::BACK) {
				break;
			}


			if (start[i] < 0) can_start = false;

			//�Q�[������
			bs[i]->loop(action);
			action = Config::NONE;
			//���̃v���C���[�ɉe�����Ȃ��悤
			//�L�[�{�[�h(���[�v�O)��1P : OK
			//1P��2P : NG

			//�U����
			int atk = bs[i]->popAttack();//�U���擾
			for (int j = 0; j < game_pad.Size(); j++) {
				if (i == j)continue;//�������g�ɂ͍U�����Ȃ�
				bs[j]->setAtack(atk, game_pad.Size());//��Ō��߂��U�����Z�b�g
			}
		}

		if (action == Config::BACK)break;

		//�}���`�v���C���̂�
		//���S�����Q�[���I�[�o�[�ŏI��
		if (game_pad.Size() >= 2) {
			int last_player = -1;
			int player_count = 0;
			for (int i = 0; i < game_pad.Size(); i++) {
				if (bs[i]->isGameOver()) {
					//���ʐݒ�
					//rank�����͖�����Ȃ̂�, ���ʂ������Ŋm�肳����
					if (rank[i] < 0) {
						//���łɃQ�[���I�[�o�[�ɂȂ����l�����J�E���g
						int finished_playing = 0;
						for (int j = 0; j < game_pad.Size(); j++) {
							if (rank[j] > 0)finished_playing++;
						}

						//���ʂ̓v���C���[�l�� - ���łɃQ�[���I�[�o�[�ɂȂ����l��
						//���łɃQ�[���I�[�o�[�ɂȂ����l����0�̎��͍ŉ���
						rank[i] = game_pad.Size() - finished_playing;
					}
				} else {
					//�v���C���[�l�����J�E���g
					//�����Ă���v���C���[�̔ԍ����ۑ�
					player_count++;
					last_player = i;
				}
			}

			//�����c�肪��l�̂Ƃ���, �����Ă���v���C���[��1�ʂŊm��
			if (player_count == 1) {
				if (rank[last_player] < 0)bs[last_player]->end();//�Q�[���I��
				rank[last_player] = 1;//�����Ă���v���C���[��1�ʂ�
			}
		}

		//�Q�[���X�^�[�g
		//�������������łɃX�^�[�g�ς݂Ȃ�����X�^�[�g�̖��߂͏o���Ȃ�
		if (can_start && !game_state) {
			for (auto& board : bs) {
				board->start();
			}
			game_state = true;//start��������Ă΂Ȃ��悤�ɂ���
		}

		//FPS����
		exec.TimeAdjustment();
	}




	for (auto& board : bs) {
		delete board;
	}
	delete[] start;
	delete[] rank;
}