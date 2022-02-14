#pragma once
#include <opencv2/opencv.hpp>
#include "blocks.h"
#include <vector>
#include "../config/config.h"
#include <random>


class Board {

public:
	//��
	static const int WALL = -1;
	//�~�m�̍ő啝
	static const int MINO_SIZE_MAX = 4;

	//T-spin�ꗗ
	static const int T_SPIN_NONE = 0;
	static const int T_SPIN_NORMAL = 1;
	static const int T_SPIN_MINI = 2;

	//�R���X�g���N�^
	Board();
	Board(int x, int y, Config& lvl) ;
	Board(const Board&) = delete;

	//�������J��
	~Board();

	//������
	void init();

	//�Q�[���X�^�[�g
	void start();

	//�Q�[���I��
	inline void end() {
		is_started = false;
	}

	bool isInterfere(Blocks& blocks);

	//�~�m�𓮂���
	bool move(int x, int y, int rot, bool action = true);

	//�z�[���h
	bool hold();

	//������܃u���b�N�ǉ�
	//1�i����
	bool addBlockFromDown();

	//�V�K�u���b�N��ݒ�
	bool setNewBlock();

	//���ۂɃQ�[����ʂ��o�͂��镔��
	cv::Mat show();

	//HOLD�����̕\��
	cv::Mat showHoldMino(double re_size = 1.0);

	//Next�̕\��
	cv::Mat showNextMino(double k, int h);

	//������܃u���b�N�G���A�̕`��
	cv::Mat showAttackMino(int margin);

	//���쒆�~�m���Œ肳����
	bool set();

	//���C������������Ƃ�����폜
	//set()���Ă񂾌�ɌĂ�
	int deleteBlock();

	//�������u���b�N��, ������
	void blockDown();

	//�Q�[���I�[�o�[����
	//�Ԃ�l�͏������I�������(�I����true)
	bool gameOver(bool flag = false);

	//Score���v�Z
	int calcScore();

	//����ւ̍U������
	int popAttack();

	//������܃u���b�N�̍����擾
	int getAttackHeight();

	//�U��
	void setAtack(int attack, int player_count);

	//���C�����[�v
	bool loop(int action);

	//Score�����邽�߂̓��ꕨ
	struct Score {

		Score() : score(0), ren(0), level(0), name1(""), name2("") {}

		int score;
		int ren;
		int level;
		std::string name1;
		std::string name2;
	};

	//Score�擾
	Score getScore();

	//Game Over���ǂ���
	inline bool isGameOver() {
		return this->is_game_over;
	}

private:

	//�摜�ǂݍ���
	void loadimg();

	//�S�[�X�g�̐ݒ�
	void ghost_set();

	//�~�m�̉摜�擾
	cv::Mat getMinoImage(int type, int type_c = -1);

	bool gameOver1(bool flag = false);
	bool gameOver2(bool flag = false);

#ifdef _Debug
	//Debug�p
	void Debug(int pattern);
#endif

	int x;				//�Ֆʂ̃T�C�Y ��
	int y;				//�Ֆʂ̃T�C�Y ����

	int** rawboard;		//�Ֆ�
	Blocks* minos;		//�~�m�̃e���v���[�g
	int* mino_counts;
	cv::Mat* images;	//�u���b�N�̉摜
	int mino_number;	//�~�m�̎��(NULL���܂߂�)
	Blocks now_block;	//���ݑ��쒆�̃u���b�N
	Blocks ghost;		//�����ʒu�ɕ\������S�[�X�g�u���b�N
	int hold_block;		//�z�[���h���Ă���u���b�N�̃^�C�v
	bool already_hold;	//���łɃz�[���h�����������
	std::vector<int> nexts;//NEXT�u���b�N
	int next_size_max;	//NEXT�ɕ\�����鐔
	int img_size;		//�u���b�N�̉摜�T�C�Y
	int last_action;	//�Ō�̑���

	//random
	//�~�m����p
	std::uniform_int_distribution<int> randomBlockDistribution;
	//���C������p
	std::uniform_int_distribution<int> randomlineDistribution;
	std::mt19937 randomEngine;

	//delete
	int highest_line;	//�ł������ʒu�̃u���b�N�̍���
	std::vector<int> delete_line;	//�폜�������C��

	int tspin_status;	//T�X�s���̎��
	int srs_type;		//Super Rotation System�̃^�C�v

	
	//game status
	//���x���֘A
	Config levels;		//���x���ꗗ
	Config::Level next_level;	//Next Level
	Config::Level now_level;	//Now Level
	int score;					//Score
	bool need_calc_score;		//Score�Čv�Z
	int ren;					//REN
	int back_to_back;			//back to back

	//�U���֘A
	int attack;					//�������������~�m�ɂ�鑊��ւ̍U����
	int attack_max;				//�������󂯂�ő�_���[�W
	int attack_yellow_type;		//�x���\��1�i�K�ڂŎg���u���b�N�̃^�C�v(�ӂ��͉��F : 2)
	int attack_red_type;		//�x���\��2�i�K�ڂŎg���u���b�N�̃^�C�v(�ӂ��͐ԐF : 4)
	bool can_put_block;			//�V�K�u���b�N�������\��? (���肠����A�j���[�V�������I�������)
	bool put_block_state;		//�V�K�u���b�N�������s����

	//�U����ۑ����邽�߂̃I�u�W�F�N�g
	struct AttackBlock {
		AttackBlock(int height) : time(0), height(height), type(8), state(0) {}
		AttackBlock() : time(0), height(0), type(8), state(0) {}

		AttackBlock operator++() {
			++this->time;
			return *this;
		}

		AttackBlock operator--() {
			--this->time;
			return *this;
		}

		const AttackBlock operator++(int) {
			AttackBlock atk = *this;
			++(*this);
			return atk;
		}

		const AttackBlock operator--(int) {
			AttackBlock atk = *this;
			--(*this);
			return atk;
		}

		int type;	//�\���Ɏg���u���b�N�̃^�C�v
		int height;	//������܃u���b�N�𐶐����邽����
		int time;	//�U�����󂯂Ă���o�߂�������
		int state;	//�ǂ��̃��C���ɋ󔒂���������ۑ����邽�߂̕ϐ�
	};
	std::vector<AttackBlock> attack_block;//���肩��̍U����ۑ�


	bool is_game_over;			//Game Over���ǂ���
	//game over���Ɏg�p����ϐ�
	int go_dir;	//�㏑������
	//���݂̍��W�@x��y
	int go_x;	
	int go_y;
	//�ǂ܂ł̋���(���̋����ɂȂ�����Ȃ���)
	int go_c;

	
	//���̉�����̓��C�����[�v�p�̕ϐ�
	int down_count;
	int fps;
	int set_count;
	bool can_action;
	int delete_line_wait_count;
	bool delete_line_wait;
	int last_action_key;
	int last_action_count;
	bool is_started;
};