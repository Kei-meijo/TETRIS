#include "board.h"

#include "../opencv/cv_util.h"
#include <math.h>
#include <iostream>

Board::Board() : img_size(40), already_hold(false), hold_block(0), next_size_max(6), is_game_over(false), can_action(false),
	score(0), down_count(0), set_count(0), delete_line_wait(false), delete_line_wait_count(0),
	last_action(Config::NONE), last_action_key(Config::NONE), last_action_count(0), ren(0), back_to_back(0),
	go_dir(0), go_x(1), go_y(0), go_c(0), need_calc_score(false), is_started(false), attack(0),
	attack_yellow_type(2), attack_red_type(4), can_put_block(true), put_block_state(false), fps(30), x(10), y(20) {

	this->x = 10;
	this->y = 20;
	this->highest_line = 0;
	this->attack_max = 14;

	this->images = nullptr;
	this->mino_counts = nullptr;
	this->mino_number = 0;
	this->minos = nullptr;
	this->rawboard = nullptr;
	this->srs_type = 0;
	this->tspin_status = T_SPIN_NONE;
}

//�R���X�g���N�^
Board::Board(int x, int y, Config& lvl) :
	img_size(40), already_hold(false), hold_block(0), next_size_max(6), is_game_over(false), can_action(false),
	score(0), down_count(0), set_count(0), delete_line_wait(false), delete_line_wait_count(0),
	last_action(Config::NONE), last_action_key(Config::NONE), last_action_count(0), ren(0), back_to_back(0),
	go_dir(0), go_x(1), go_y(0), go_c(0), levels(lvl), need_calc_score(false), is_started(false), attack(0),
	attack_yellow_type(2), attack_red_type(4), can_put_block(true), put_block_state(false), x(x), y(y), 
	highest_line(0) {

	//�{�[�h�T�C�Y������
	this->x = x;
	this->y = y;
	this->highest_line = 0;
	this->attack_max = y - 6;
	if (attack_max < 1) attack_max = 1;

	//�摜�ǂݍ���
	loadimg();

	//������
	init();

	//�~�m�ݒ�
	this->mino_number = 8;
	this->minos = new Blocks[mino_number];
	this->mino_counts = new int[mino_number];

	for (int i = 0; i < mino_number; i++) {
		this->minos[i] = Blocks();
		this->mino_counts[i] = 0;

		//�u���b�N�����ʒu
		this->minos[i].x = this->x / 2;
		this->minos[i].y = this->y + 1;
	}

	//�u���b�N�̌`��ݒ�
	this->minos[0] = this->minos[0].setType(0).setRot(1).add(0, 0).add(0, 0).add(0, 0);//NULL
	this->minos[1] = this->minos[1].setType(1).setRot(2).add(0, 1).add(0, 2).add(0, -1);//I
	this->minos[2] = this->minos[2].setType(2).setRot(1).add(1, 0).add(0, 1).add(1, 1);//O
	this->minos[3] = this->minos[3].setType(3).setRot(2).add(-1, 0).add(0, 1).add(1, 1);//S
	this->minos[4] = this->minos[4].setType(4).setRot(2).add(1, 0).add(0, 1).add(-1, 1);//Z
	this->minos[5] = this->minos[5].setType(5).setRot(4).add(0, 1).add(1, 0).add(2, 0);//J
	this->minos[6] = this->minos[6].setType(6).setRot(4).add(0, 1).add(-1, 0).add(-2, 0);//L
	this->minos[7] = this->minos[7].setType(7).setRot(4).add(0, 1).add(1, 0).add(-1, 0);//T

	//T�X�s���p�����蔻��
	this->minos[7] = this->minos[7].addStatus(-1, -1, 0).addStatus(1, -1, 0).addStatus(-1, 1, 1).addStatus(1, 1, 1);

	//Level �ݒ�
	//Level1��Level2
	for (auto& level : lvl.levels) {
		if (level.level == 1) {
			now_level = level;
		} else if (level.level == 2) {
			next_level = level;
		}
	}

	//�����_�}�C�U������
	std::random_device randomDevice;
	std::vector<uint32_t> randomSeedVector(10);
	std::generate(randomSeedVector.begin(), randomSeedVector.end(), std::ref(randomDevice));
	std::seed_seq randomSeed(randomSeedVector.begin(), randomSeedVector.end());

	randomEngine = std::mt19937(randomSeed);
	randomBlockDistribution = std::uniform_int_distribution<int>(1, this->mino_number - 1);
	randomlineDistribution = std::uniform_int_distribution<int>(1, this->x);
}



//�������J��
Board::~Board() {
	for (int i = 0; i < y + 5; i++) {
		if (this->rawboard[i])delete[] this->rawboard[i];
	}

	if (this->rawboard)delete[] this->rawboard;
	if (this->minos)delete[] this->minos;
	if (this->images)delete[] this->images;
	if (this->mino_counts)delete[] this->mino_counts;
}

//������
void Board::init() {
	//�������m��
	std::cout << "x = " << this->x << std::endl;
	std::cout << "y = " << this->y << std::endl;
	this->rawboard = new int* [this->y + 5];
	for (int i = 0; i < this->y + 5; i++) {
		this->rawboard[i] = new int[this->x + 2];

		for (int j = 0; j < this->x + 2; j++) {
			this->rawboard[i][j] = 0;
		}
	}

	for (int i = 0; i < this->y + 5; i++) {
		this->rawboard[i][0] = WALL;
		this->rawboard[i][this->x + 1] = WALL;
	}

	for (int i = 0; i < this->x + 2; i++) {
		this->rawboard[0][i] = WALL;
	}
}

//�Q�[���X�^�[�g
void Board::start() {
	if (!is_started) {
		//�Q�[���J�n
		//�V�K�u���b�N�ݒ�
		setNewBlock();
		is_started = true;
	}
}

//�u���b�N�������Ă��邩?
bool Board::isInterfere(Blocks& blocks) {
	//�~�m�̍��W�擾
	auto points = blocks.getPoints();
	for (auto& p : points) {
		//�~�m�̈ʒu�Ƀu���b�N������Ă��Ȃ����m�F
		if (this->rawboard[p.y][p.x] != 0) {
			return true;
		}
	}

	return false;
}

//�~�m�𓮂���
bool Board::move(int x, int y, int rot, bool action) {

	//�������Ȃ����͉������Ȃ�
	//�u���b�N�ݒu��Ƃ�
	if (!can_action)return true;
	Blocks tmp = now_block.clone();

	//��x�ɓ������͈̂�����̂�
	if (x != 0) {
		//���E����
		tmp.x += x;

		//�u���b�N�������Ă��邩
		if (isInterfere(tmp)) {
			//�ړ��s��
			//���ɉ��������I��
			return false;
		} else {
			if (action) {
				//�ړ��ł���̂�,���ۂɈړ�
				now_block = tmp;

				//�Ō�ɓ��������s����ۑ�
				if (x > 0) { last_action = Config::RIGHT; } else { last_action = Config::LEFT; }

				//�S�[�X�g��������
				ghost_set();
			}
			return true;
		}
	} else if (rot != 0) {
		//��]
		tmp.rotate += rot;

		int tx = tmp.x;
		int ty = tmp.y;

		//�~�m�̍��W�擾
		auto points = tmp.getPoints();

		//Super Rotation ��]���𓮂����Ȃ���,�ݒu�ł���ʒu�ɐݒu
		//�����̃��[�v -1 or 1 y�p
		for (int code_y = -1; code_y <= 1; code_y += 2) {

			//y���W 0, 1, 2, -1
			for (int ddy = 0; ddy <= 2; ddy++) {
				//2��ڂ�0�̓X�L�b�v(���łɌv�Z�ς�)
				if (code_y == 1 && ddy == 0)continue;
				int dy = code_y * ddy;

				//�����̃��[�v 1 or -1 x�p
				for (int code_x = 1; code_x >= -1; code_x -= 2) {
					//���S�ʒu 0�`1(x����)
					for (int ddx = 0; ddx <= 2; ddx++) {
						int dx = ddx * code_x;

						//x��y�̋���������2�ȏ�͈ړ����Ȃ�
						if (ddy > 1 && ddx > 1)continue;

						//std::cout << "(dx, dy) = (" << dx << "," << dy << ")" << std::endl;


						int nx = tx + dx;
						int ny = ty + dy;

						//��]���S����ʔ͈͊O
						if (ny < 1 || nx < 1 || nx > this->x)break;

						//��]���S�ȊO���m�F
						bool can_place = true;
						for (auto& p : points) {
							nx = p.x + dx;
							ny = p.y + dy;

							//��ʔ͈͓����ǂ���
							if (ny < 1 || nx < 1 || nx > this->x) {
								can_place = false;
								break;
							}

							//�u���b�N���Ȃ���
							if (this->rawboard[ny][nx] != 0) {
								can_place = false;
								break;
							}
						}

						//�T�����I���ău���b�N���ݒu�ł���̂Őݒu
						if (can_place) {
							if (action) {
								tmp.x = tx + dx;
								tmp.y = ty + dy;

								//�ړ��ł���̂�,���ۂɈړ�
								now_block = tmp;

								//��]��0�`3�Ɏ��߂�
								int rot_dir = rot;
								while (rot_dir < 0) { rot_dir += 4; }
								while (rot_dir >= 4) { rot_dir -= 4; }

								//�Ō�ɑ��삵���s����ۑ�
								if (rot_dir == 1) { last_action = Config::ROTATION_CW; }
								if (rot_dir == 3) { last_action = Config::ROTATION_CCW; }

								//mini Tspin����p��SRS�ړ��^�C�v����
								if (ddx == 1 && ddy == 0) {
									this->srs_type = 1;
								} else if (ddx == 1 && ddy == 1) {
									this->srs_type = 2;
								} else if (ddx == 0 && ddy == 2) {
									this->srs_type = 3;
								} else if (ddx == 1 && ddy == 2) {
									this->srs_type = 4;
								} else {
									this->srs_type = 0;
								}

								//�S�[�X�g��������
								ghost_set();
							}
							return true;
						}
					}
				}
			}
		}

	} else if (y != 0) {
		//������
		tmp.y += y;

		//�u���b�N�������Ă��邩
		if (isInterfere(tmp)) {
			//�ړ��s��
			//���ɉ��������I��
			return false;
		} else {
			if (action) {
				//�ړ��ł���̂�,���ۂɈړ�
				now_block = tmp;

				//�Ō�ɑ��삵���s����ۑ�
				last_action = Config::DOWN;

				//�������݂̂̓S�[�X�g�͓����Ȃ��̂�,�S�[�X�g�̍X�V�͂��Ȃ�
			}
			return true;
		}
	}

	//�ړ��ł����̂�true��Ԃ�
	return true;
}

//�z�[���h
bool Board::hold() {
	//���łɃz�[���h���Ă�����A�z�[���h���Ȃ�
	if (!already_hold) {

		//�z�[���h�Ƒ��쒆�̂��̂����ւ���
		int tmp = this->hold_block;
		this->hold_block = now_block.getType();
		last_action = Config::HOLD;

		//�z�[���h���󂾂�����V�K�ɐ������Ȃ���
		if (tmp == 0) {
			//�z�[���h����
			//�V�K�u���b�N�𐶐�
			setNewBlock();
		} else {
			//�P���Ƀz�[���h���Ăяo��
			now_block = this->minos[tmp].clone();
		}

		//�S�[�X�g���X�V
		ghost_set();

		//�z�[���h�������Ƃ�ݒ�
		already_hold = true;
		return true;
	}

	return false;
}

//������܃u���b�N�ǉ�
//1�i����
bool Board::addBlockFromDown() {

	//���������u���b�N���Ȃ���΂����K�v�Ȃ�
	if (this->attack_block.empty())return true;

	//��̃u���b�N������΍폜
	while (!this->attack_block.empty() && this->attack_block[0].height <= 0) {
		this->attack_block.erase(this->attack_block.begin());
	}

	//�܂����Ԃ̗]�T������̂�, �ݒu�s�v
	if (this->attack_block[0].type != this->attack_red_type) return true;

	//�K��͑O��Ɠ������C��
	int set_line = this->attack_block[0].state;
	if (this->attack_block[0].state == 0) {
		//�O�񂪑��݂��Ȃ�
		//���S�����_��
		set_line = randomlineDistribution(randomEngine);
	} else {
		//70%�̊m���őO��Ɠ���
		int tmp_ = randomlineDistribution(randomEngine);

		if (tmp_ > 7) {//8, 9, 10 ��30%
			//�O��Ƃ͈Ⴄ���C����
			while (set_line == this->attack_block[0].state) {
				set_line = randomlineDistribution(randomEngine);
			}
		}
	}

	//���C���ǉ�
	//�܂��͋�Ԃ��󂯂�
	for (int i = this->y + 4; i >= 2; i--) {
		for (int j = 1; j <= this->x; j++) {
			this->rawboard[i][j] = this->rawboard[i - 1][j];
		}
	}
	for (int j = 1; j <= this->x; j++) {
		//���C���ǉ�
		if (set_line == j) { this->rawboard[1][j] = 0; } else { this->rawboard[1][j] = 8; }
	}



	//�O��̃��C�����X�V
	this->attack_block[0].state = set_line;
	this->attack_block[0].height--;
	printf("attack_block : %d\n", this->attack_block[0].height);
	while (!this->attack_block.empty() && this->attack_block[0].height <= 0) {
		this->attack_block.erase(this->attack_block.begin());
	}

	return false;
}

//�V�K�u���b�N��ݒ�
bool Board::setNewBlock() {
	int rand = 0;
	put_block_state = false;
	can_put_block = false;

	//Next + 1�����܂�܂Ő�����������
	//+ 1�Ȃ̂�, �V�K�������ɂ͑��쒆�u���b�N���Ȃ�����
	//���̕����m�ۂ��Ȃ��Ƃ����Ȃ�
	while (this->nexts.size() <= next_size_max + 1) {
		//�����̍\����
		//1�`7�܂ł̃��X�g���\��
		//��
		//�ԍ�				1�@2�@3�@4�@5�@6�@7
		//�㉽��o���邩	1�@1�@0�@2�@2�@1�@1
		//��̏ꍇ�^�C�v3�͏o���Ȃ��悤�ɂ���,���̃u���b�N�𐶐�����


		int count_ = 0;		//�o����u���b�N�̐�
		int c_number = -1;	//�o����u���b�N�̔ԍ�
		//�ԍ�				1�@2�@3�@4�@5�@6�@7
		//�㉽��o���邩	0�@0�@0�@1�@0�@1�@0
		//��̏ꍇ
		//count_ = 2
		//c_number = 6
		for (int i = 1; i < this->mino_number; i++) {
			if (this->mino_counts[i] > 0) {
				count_++;
				c_number = i;
			}
		}

		//count_��0�̏ꍇ,�u���b�N���o���Ȃ��̂�,���X�g���Đ���
		if (count_ == 0) {
			count_ = this->mino_number - 1;//�Đ�������̂�,���R�S���o����̂ŃJ�E���g�l���ő��

			//fill�͍Đ����̂Ƃ����Ŗ��߂邩
			//fill��1�̏ꍇ		���̏ꍇ,�u���b�N���A�����Ȃ��Ȃ�
			//�ԍ�				1�@2�@3�@4�@5�@6�@7
			//�㉽��o���邩	1�@1�@1�@1�@1�@1�@1
			//�����Ə�̂悤�ȏ󋵂͖ʔ����Ȃ��̂�,���܂ɂ͘A������悤��2�Ŗ��߂鎞�����
			//fill��1�̏ꍇ		���̏ꍇ,�u���b�N���A������\������
			//�ԍ�				1�@2�@3�@4�@5�@6�@7
			//�㉽��o���邩	2�@2�@2�@2�@2�@2�@2
			//����,������2���ƘA����������̂�,1��2�������_���ɂ���
			//����1���߂̃����_��(1�`7�̃����_���̊��1������2�ɂ��Ă���)
			int fill = (randomBlockDistribution(randomEngine) - 1) % 2 + 1;
			//�Q�[���J�n���͉^�v�f�����炷����, �K��1�ɂ���
			//�Q�[���J�n����NEXT��0�Ȃ̂�, ����𗘗p���Ĕ��ʂ��Ă���
			if (this->nexts.size() < 1) {
				fill = 1;
			}
			
			//fill�𖄂߂�
			for (int i = 1; i < this->mino_number; i++) {
				this->mino_counts[i] = fill;
			}
		}

		//count_��2�ȏ�̎������_���ɒ��o����
		//1�̎��͗����̕K�v���Ȃ��̂�, ���O�ɒ��ׂ��^�C�v�����̂܂ܓ����
		if (count_ > 1) {
			//1�`count_�̃����_���ɂȂ�悤�Ȑ��K�������
			//count_��3�̎���1, 2, 3�̃����_��
			std::uniform_int_distribution<int> dist = std::uniform_int_distribution<int>(1, count_);

			//�������擾�@���̒l���g����ԍ��̒���, ���Ԗڂ�������
			//(��)tmp = 3
			//�ԍ�				1�@2�@3�@4�@5�@6�@7
			//�㉽��o���邩	1�@1�@0�@2�@0�@1�@1
			//��̎�, �^�C�v��4
			//(1�Ԗڂ�1, 2�Ԗڂ�2, 3�Ԗڂ�4, 4�Ԗڂ�6, 5�Ԗڂ�7)
			int tmp = dist(randomEngine);
			int now_count_ = 0;//�T�����ɍ����Ԗڂ��L�^���Ă���

			//��Ō��߂�tmp�Ԗڂ�T��
			for (int i = 1; i < this->mino_number; i++) {
				if (this->mino_counts[i] > 0) {//������O������, �g���Ȃ���̓J�E���g���Ȃ��悤�ɂ���
					if (++now_count_ == tmp) {//��ԍŏ���1�Ȃ̂�, ���1���� �Ԗڂ���v���Ă����炻�̔ԍ����o��
						rand = i;
						break;
					}
				}
			}
		} else {
			//���O�ɒ��ׂ��^�C�v�����̂܂ܓ����
			rand = c_number;
		}

		//�g������͌��炵�Ă���(����Ȃ��ƃ��X�g�ŊǗ�����Ӗ����Ȃ�)
		this->mino_counts[rand]--;

		//Next�ɒǉ�
		this->nexts.push_back(rand);
	}

	//next�̐擪�����̑���~�m��
	rand = this->nexts[0];
	//next�̐擪���폜�i��̑���͒P���ɃR�s�[���������j
	this->nexts.erase(this->nexts.begin());
	//���ۂ�Next�ɑ��
	now_block = this->minos[rand].clone();

	//�V�K�u���b�N�̃~�m�̍��W�擾
	//Game Over����̂���, �V�K�u���b�N���u���锻��Ɏg�p
	bool has_block = isInterfere(now_block);

	//�Ō�ɑ��삵���s���̃��Z�b�g
	last_action = Config::NONE;
	//����\�ɂ���
	can_action = true;

	//Game Over����͐V�K�~�m�������Ƀu���b�N������Ă��Ȃ���
	is_game_over = has_block;

	//�S�[�X�g���X�V
	ghost_set();
	return !has_block;
}

//���ۂɃQ�[����ʂ��o�͂��镔��
cv::Mat Board::show() {
	cv::Mat display(this->y * this->img_size, this->x * this->img_size, CV_8UC3, cv::Scalar(0, 0, 0));

	//���쒆�̂��̂ƃS�[�X�g�̃~�m�̍��W�擾
	auto mino_block = now_block.getPoints();
	auto mino_block_g = ghost.getPoints();

	//openCV�̌��_������Ȃ̂ŁA�ォ��`��
	for (int i = this->y; i >= 1; i--) {
		//openCV�ł�y���W
		int opencv_y = (this->y - i) * this->img_size;
		for (int j = 1; j <= x; j++) {
			//openCV�ł�x���W
			int opencv_x = (j - 1) * this->img_size;
			//�{�[�h�̃u���b�N���
			int block_color = this->rawboard[i][j];
			bool is_ghost = false;

			//���T�����̃u���b�N�ɑ��쒆�̃~�m�����邩(���łɐݒu�����u���b�N���Ȃ��ꍇ�ɒT��)
			if (block_color == 0) {
				for (auto& point : mino_block) {
					if (point.x == j && point.y == i) {
						block_color = now_block.getType();
						break;
					}
				}
			}

			//���T�����̃u���b�N�ɃS�[�X�g�̃~�m�����邩(���łɐݒu�����u���b�N���Ȃ��ꍇ�ɒT��)
			if (block_color == 0) {
				for (auto& point : mino_block_g) {
					if (point.x == j && point.y == i) {
						block_color = now_block.getType();
						is_ghost = true;
						break;
					}
				}
			}

			//�S�[�X�g�ł���΁A���߂����ĕ\��
			if (!is_ghost) {
				//���̂܂ܕ`��
				paste(display, this->images[block_color].clone(), opencv_x, opencv_y);
			} else {
				//��U�w�i��`�悳��������, �S�[�X�g�𓧉߂����ĕ`��
				paste(display, this->images[0].clone(), opencv_x, opencv_y);
				blend_paste(display, this->images[block_color].clone(), opencv_x, opencv_y);
			}
		}
	}


	return display;
}

//HOLD�����̕\��
cv::Mat Board::showHoldMino(double re_size) {
	//�~�m���摜�ɂ���֐������̂܂܌Ă�ł���
	cv::Mat display_image;
	if (already_hold) {
		display_image = getMinoImage(this->hold_block, 8);
	} else {
		display_image = getMinoImage(this->hold_block);
	}

	if (re_size != 1.0) {
		cv::resize(display_image, display_image, cv::Size(), re_size, re_size);
	}

	return display_image;
}

//Next�̕\��
cv::Mat Board::showNextMino(double k, int h) {
	//Next�͒��߂̂��̂�傫���\������, ����ȊO�͈����Ɏw�肵���W���{�����T�C�Y�ɂ���
	int size = static_cast<int>(k * MINO_SIZE_MAX * this->img_size);//���������̉摜�T�C�Y
	int total_y_size = (size + h) * (this->next_size_max - 1) + MINO_SIZE_MAX * this->img_size;//�S�����킹���T�C�Y

	//�摜������
	cv::Mat display_mino(total_y_size, MINO_SIZE_MAX * this->img_size, CV_8UC3, cv::Scalar(0, 0, 0));

	//��ԏ�ɒ��߂�\��t��
	paste(display_mino, getMinoImage(this->nexts.empty() ? 0 : this->nexts[0]), 0, 0);

	//����ȊO��\��t��
	for (int i = 1; i < this->next_size_max; i++) {
		//�摜�擾
		cv::Mat next_img = getMinoImage(this->nexts.size() <= i ? 0 : this->nexts[i]);
		//�k��
		cv::resize(next_img, next_img, cv::Size(size, size));
		//�\��t��
		paste(display_mino, next_img, 0, MINO_SIZE_MAX * this->img_size + (i - 1) * size + i * h);
	}

	return display_mino;
}

//������܃u���b�N�G���A�̕`��
cv::Mat Board::showAttackMino(int margin) {
	//�摜������
	int height = (this->attack_max + 1) * this->img_size;
	cv::Mat display_mino(height, this->img_size, CV_8UC3, cv::Scalar(0, 0, 0));

	//��Ȃ�`�悵�Ȃ�
	if (!this->attack_block.empty()) {
		//������`��

		int y_ = 0;//y���W
		for (const auto& block : this->attack_block) {
			for (int i = 0; i < block.height; i++) {
				//�`��(1block)
				paste(display_mino, this->images[block.type].clone(), 0, height - y_ - this->img_size);
				//���W��1block�����炷
				y_ += this->img_size;
			}

			//�򂲂Ƃɋ󔒂��󂯂�
			y_ += margin;
		}
	}
	return display_mino;
}

//���쒆�~�m���Œ肳����
bool  Board::set() {
	//�z�[���h�ł���悤�ɂ���
	this->already_hold = false;
	//�~�m�𓮂����Ȃ��悤�ɂ���
	can_action = false;

	//���݂̃u���b�N�̈ʒu���擾
	auto points = now_block.getPoints();
	int type = now_block.getType();

	//���������ݒu�ł��邩�m�F
	//�u���b�N���d�Ȃ��Ă��Ȃ���
	if (isInterfere(now_block))return false;

	//T�X�s������
	this->tspin_status = T_SPIN_NONE;

	//T�~�m���ǂ���(T�X�s�����肪���݂��邩)
	if (this->now_block.getStatSize() > 0) {

		//�Ō�ɑ��삵���̂�,��]���삩�ǂ���
		if (this->last_action != Config::NONE && (this->last_action & (Config::ROTATION_CW | Config::ROTATION_CCW)) != 0) {
			//T spin�̓����蔻��̈ʒu���擾
			auto decision = now_block.getStats();

			int count = 0;//�����蔻��Ƀu���b�N�������邩
			int t_mini = 0;//T�̃u���b�N�̓ʑ��Ƀu���b�N�������邩
			for (auto& d : decision) {
				if (this->rawboard[d.p.y][d.p.x] != 0) {
					count++;
					t_mini += d.status;
				}
			}

			// �����蔻��Ƀu���b�N��3�ȏ�
			if (count >= 3) {
				//T-spin

				//T�̃u���b�N�̓ʑ��Ƀu���b�N��1�����Ȃ���, �Ō�̉�]���쎞��
				//Super Rotation System�̓��샂�[�h��4�ȊO((x, y) �ړ��ʂ�(+-1, +-2))����
				//T spin mini����(T spin����ł͂��邪, Score�͒ʏ�̃��C����������)
				//��L�ȊO��T spin(Score��T spin����)(normal�ƌď�)
				if (t_mini < 2 && this->srs_type != 4) {
					std::cout << "t spin mini" << std::endl;
					this->tspin_status = T_SPIN_MINI;
				} else {
					std::cout << "t spin normal" << std::endl;
					this->tspin_status = T_SPIN_NORMAL;
				}
			}
		}
	}

	//�{�[�h�Ƀu���b�N������������
	for (auto& p : points) {
		this->rawboard[p.y][p.x] = type;
	}

	//���쒆�ƃS�[�X�g��NULL�u���b�N�ɍ����ւ���, �����Ȃ��悤��
	this->now_block = this->minos[0].clone();
	this->ghost = this->minos[0].clone();

	//�ݒu���ł���
	return true;
}

//���C������������Ƃ�����폜
	//set()���Ă񂾌�ɌĂ�
int Board::deleteBlock() {
	this->highest_line = 0;//�ł������ʒu�ɂ���u���b�N��������
	this->delete_line.clear();//�폜�������C����������

	//������T��
	for (int y_ = 1; y_ <= this->y + 3; y_++) {

		bool complete_line = true;
		//x�����ɒT��
		for (int x_ = 1; x_ <= this->x; x_++) {
			//��̃u���b�N�������, ���C�������ł��Ȃ�
			if (this->rawboard[y_][x_] == 0) {
				complete_line = false;
				//�ł������ʒu�ɂ���u���b�N����T�����邽�߂�, 
				//���C�������ł��Ȃ����Ƃ��m�肵����, �T���𑱂��邽��break�͓���Ȃ�
			} else {
				this->highest_line = y_;
				//�ł������ʒu�ɂ���u���b�N��񂪒T�����I����.
				//�������łɃ��C�������ł��Ȃ����Ƃ��m�肵���ꍇ, �T���I��
				if (!complete_line)break;
			}
		}

		//���C�������\��
		if (complete_line) {
			//�\�ł���̂�, ���ۂɏ���
			for (int x_ = 1; x_ <= this->x; x_++) {
				this->rawboard[y_][x_] = 0;
			}
			//�폜�������C����������
			this->delete_line.push_back(y_);
		}
	}

	//�Ȃ����̒i�K�ł�, �u���b�N�͕�������ԂɂȂ�
	//���̌�blockDown() ���ĂԂ��Ƃ�, �폜�������C�����������邪, 
	//��������Ԃ���u�`�悷�邽��, �����ɂ͌Ă΂Ȃ�
	return (int)this->delete_line.size();
}

//�������u���b�N��, ������
void Board::blockDown() {

	//���̃��C����񂩂�����Ă���̂�, 0����T������Ή�����T���\
	for (int i = 0; i < this->delete_line.size(); i++) {

		//�폜���郉�C������(�ł������ʒu�ɂ���u���b�N������)������ɍ��
		for (int y = this->delete_line[i]; y <= this->highest_line; y++) {
			//��i��̏����R�s�[���Ă���
			for (int x = 1; x <= this->x; x++) {
				//�ǂ̓R�s�[���Ȃ��悤�ɂ���(�������Ȃ�����)
				if (this->rawboard[y + 1][x] != WALL) {
					this->rawboard[y][x] = this->rawboard[y + 1][x];
					//this->rawboard[y + 1][x] = 0;
				}
			}
		}
		//���̒i�̃��C�������̉e���ŏ�̕��̎c��̃��C������i�������Ă���̂�,
		//1�����Ă���
		for (int j = i + 1; j < this->delete_line.size(); j++) {
			this->delete_line[j]--;
		}
		//��ԍ����u���b�N����i�������Ă���̂�,1�����Ă���
		this->highest_line--;
	}
}

//�Q�[���I�[�o�[����
	//�Ԃ�l�͏������I�������(�I����true)
bool Board::gameOver(bool flag) {
#ifdef GAME_OVER_1
	return gameOver1(flag);
#else
	return gameOver2(flag);
#endif
}

//�Q�[���I�[�o�[����
	//�Ԃ�l�͏������I�������(�I����true)
bool Board::gameOver1(bool flag) {
	//�������玞�v���Ƀu���b�N��Ԃ��u���b�N�ɏ��������鏈��

	//dir��4�����ɂȂ�悤�ɒ���
	while (go_dir >= 4) { go_dir -= 4; }
	//go_c�������ǂ������I������ɂ��Ă���
	//go_c�͖{���͕ǂ܂ł̋���
	if (go_c < 0)return true;

	//dir���玟�̃u���b�N�ւ̕΍����v�Z
	//���̕ӂ̓~�m�̉�]�ƈꏏ(��]�s����g������)
	int g_x = 0;
	int g_y = 1;
	for (int r = 0; r < go_dir; r++) {
		int ng_x = g_y;
		int ng_y = -g_x;

		g_x = ng_x;
		g_y = ng_y;
	}

	//y�̕΍�������(y�����Ɉړ�)
	//���݂̈ʒu�ƕ΍��𑫂�������(���̃u���b�N�̈ʒu)�̕ǂ܂ł̋�����, �K��ȏ�
	//��L�̏ꍇ�������Ȃ���
	if (g_y != 0 && (g_y + go_y <= go_c || g_y + go_y > this->y - go_c)) {
		//�������Ȃ���
		go_dir++;
		//dir��3
		//(���̈ړ����������ړ��̏ꍇ)
		//�͕ǂ܂ł̋����̋K���1����
		if (go_dir == 3) {
			go_c++;
		}

		//flag��true�ŌĂ΂�鎞��,�Ȃ����A������
		//�܂�, ���ׂĖ��߂������Ƃ������ƂȂ̂�, �I������
		if (flag) {
			go_c = -1;
			return true;
		}

		//�܂��u���b�N�����������鏈�������Ă��Ȃ��̂�, 
		//�A���ŋȂ���Ȃ��Ƃ����Ȃ��̂��̔�������邽��,
		//������x�������������邽��, �������g���Ă�(flag �� true)
		return gameOver(true);
	}

	//x�̕΍�������(x�����Ɉړ�)
	//���݂̈ʒu�ƕ΍��𑫂�������(���̃u���b�N�̈ʒu)�̕ǂ܂ł̋�����, �K��ȏ�
	//��L�̏ꍇ�������Ȃ���
	if (g_x != 0 && (g_x + go_x <= go_c || g_x + go_x > this->x - go_c)) {
		//�������Ȃ���
		go_dir++;

		//flag��true�ŌĂ΂�鎞��,�Ȃ����A������
		//�܂�, ���ׂĖ��߂������Ƃ������ƂȂ̂�, �I������
		if (flag) {
			go_c = -1;
			return true;
		}

		//�܂��u���b�N�����������鏈�������Ă��Ȃ��̂�, 
		//�A���ŋȂ���Ȃ��Ƃ����Ȃ��̂��̔�������邽��,
		//������x�������������邽��, �������g���Ă�(flag �� true)
		return gameOver(true);
	}

	//���߂�ׂ��u���b�N�̈ʒu���v�Z(�΍��𑫂�����)
	go_x += g_x;
	go_y += g_y;

	//�u���b�N������ΐԂ����ߏグ��(type��4�ɕς��邾��)
	if (this->rawboard[go_y][go_x] != 0)this->rawboard[go_y][go_x] = 4;

	//�I�����Ă��Ȃ��̂�false
	return false;
}

bool Board::gameOver2(bool flag) {
	this->now_block = this->minos[0];
	if (this->go_y > this->y) { return true; }

	if (++this->go_x % 3 == 0) {
		this->go_y++;
		for (int i = 1; i <= this->x; i++) {
			this->rawboard[this->go_y][i] = 0;
		}
	} else {
		int y = this->go_y + 1;
		for (int i = 1; i <= this->x; i++) {
			if(this->rawboard[y][i] != 0) this->rawboard[y][i] = 8;
		}
	}

	return false;
}

//Score���v�Z
int  Board::calcScore() {

	//Score�v�Z�s�v�ł����, ���ݒl�����̂܂ܕԂ�
	if (!need_calc_score)return this->score;

	bool perfect = this->delete_line.size() >= this->highest_line || this->highest_line == 0;

	//Level�{�[�i�X
	double bonus = this->now_level.score.bonus;
	double btb_bonus = 1.0;
	//�ǉ��X�R�A
	int plus_score = 0;//+ this->back_to_back * this->now_level.btb;
	//��{
	int ren_ = this->ren - 1 < 0 ? 0 : this->ren - 1;
	int basic_score = 0;

	//�U����
	int perfect_attack = 0;
	int normal_attack = 0;

	//REN�ɂ��X�R�A���Z
	basic_score = ren_ * this->now_level.score.ren;
	if (basic_score > this->now_level.score.ren_max) {
		basic_score = this->now_level.score.ren_max;
	}

	if (ren_ > 0) {
		normal_attack += this->now_level.attack.ren[std::min(ren_, (int)this->now_level.attack.ren.size()) - 1];
		//printf("REN = %d\n", normal_attack);
	}

	//back to back�ɂ��X�R�A���Z
	if (this->back_to_back >= 2) {
		btb_bonus = this->now_level.score.btb;
		normal_attack += this->now_level.attack.btb;
	}

	//��{�X�R�A
	//�폜�̂����
	if (this->delete_line.size() >= 4) {
		//�e�g���X
		basic_score += this->now_level.score.tetris;
		normal_attack += this->now_level.attack.tetris;
	} else if (this->delete_line.size() >= 3 && this->tspin_status == T_SPIN_NORMAL) {
		//T-spin triple
		basic_score += this->now_level.score.tspin_triple;
		normal_attack += this->now_level.attack.tspin_triple;
	} else if (this->delete_line.size() == 2 && this->tspin_status == T_SPIN_NORMAL) {
		//T-spin double
		basic_score += this->now_level.score.tspin_double;
		normal_attack += this->now_level.attack.tspin_double;
	} else if (this->delete_line.size() == 1 && this->tspin_status == T_SPIN_NORMAL) {
		//T-spin single
		basic_score += this->now_level.score.tspin_single;
		normal_attack += this->now_level.attack.tspin_single;
	} else if (this->delete_line.size() > 0) {
		//�ʏ�̃��C������
		//T-spin mini������

		if (this->delete_line.size() >= 3) {
			basic_score += this->now_level.score.line3;
			normal_attack += this->now_level.attack.line3;
		} else if (this->delete_line.size() >= 2) {
			basic_score += this->now_level.score.line2;
			normal_attack += this->now_level.attack.line2;
		} else {
			basic_score += this->now_level.score.line;
			normal_attack += this->now_level.attack.line;
		}

		if (this->tspin_status == T_SPIN_MINI) {
			plus_score += this->now_level.score.tspin_mini;
		}
	}

	//�ǉ��X�R�A
	if (perfect) {
		if (this->delete_line.size() >= 4) {
			plus_score += this->now_level.score.perfect_tetris;
			perfect_attack += this->now_level.attack.perfect_tetris;
		} else if (this->delete_line.size() >= 3) {
			plus_score += this->now_level.score.perfect_line3;
			perfect_attack += this->now_level.score.perfect_line3;
		} else if (this->delete_line.size() >= 2) {
			plus_score += this->now_level.score.perfect_line2;
			perfect_attack += this->now_level.attack.perfect_line2;
		} else if (this->delete_line.size() >= 1) {
			plus_score += this->now_level.score.perfect_line;
			perfect_attack += this->now_level.score.perfect_line;
		}
	}

	if (perfect_attack > 0) {
		attack += perfect_attack;
	} else {
		attack += normal_attack;
	}
	//printf("( %d * %f + %d ) * %f\n", basic_score, btb_bonus, plus_score, bonus);
	//printf("attack = %d, perfect = %d, normal = %d\n", attack, perfect_attack, normal_attack);
	this->score += static_cast<int>((basic_score * btb_bonus + plus_score) * bonus);

	//Next Level �����݂��Ă���
	//���݂̃��x����Next Leve�ɏオ������𖞂����Ă���ꍇ��
	//���x�����グ��
	//��x��2�ȏ�オ�邱�Ƃ��l������while�Ŕ��肵������
	//Level1��REN�̃{�[�i�X��������Ԃ�T spin double�Ȃ�
	while (this->next_level.level > 0 && this->score > this->next_level.score.lvlup_score) {
		this->now_level = this->next_level;//Level�����̂��̂�

		//Next Level��ݒ肷��
		//���݂̃��x��+1�̃��x���ݒ��񂪂��邩�̒T��
		bool contain = false;
		for (auto& next_lvl : this->levels.levels) {
			if (next_lvl.level == this->now_level.level + 1) {
				contain = true;
				this->next_level = next_lvl;
			}
		}

		//���x�����Ȃ����͋����o�^
		if (!contain) {
			this->next_level = Config::Level();
		}
	}

	//std::cout << "score = " << this->score << std::endl;
	//�v�Z�����̂�, ������xScore�v�Z����Ȃ��悤�v�Z�K�v����false��
	need_calc_score = false;
	return this->score;
}

//����ւ̍U������
int Board::popAttack() {
	int attk = this->attack;
	this->attack = 0;

	//������܍폜
	int attk_tmp = attk;
	while (this->attack_block.size() > 0 && attk_tmp > 0) {
		if (attk_tmp >= this->attack_block[0].height) {
			attk_tmp -= this->attack_block[0].height;

			//�폜
			this->attack_block.erase(this->attack_block.begin());
		} else {
			this->attack_block[0].height -= attk_tmp;
			attk_tmp = 0;
			break;
		}
	}

	return attk;
}

//������܃u���b�N�̍����擾
int Board::getAttackHeight() {

	int height_ = 0;
	for (const auto& atk : this->attack_block) {
		height_ += atk.height;
	}

	return height_;
}

//�U��
void Board::setAtack(int attack, int player_count) {
	//�U����0�Ȃ�U�����Ȃ�
	if (attack <= 0) {
		return;
	}

	//�V���O���v���C���[�Ȃ�U������Ƃ��͂���Ȃ�
	if (player_count <= 1) {
		return;
	}

	int attk = (int)(ceil((double)attack / (double)(player_count - 1)));

	//�U������
	int margin = this->attack_max - getAttackHeight();
	if (margin <= 0)return;

	int set_block = (int)std::min(margin, attk);

	this->attack_block.push_back(AttackBlock(set_block));
}

//���C�����[�v
bool  Board::loop(int action) {

	//�Q�[���I�[�o�[�̎��͂��������Ŋ���������
	if (is_game_over) {
		return !gameOver();
	}

	if (!is_started) {
		return true;
	}

	if (!this->attack_block.empty()) {
		for (auto& blocks_ : this->attack_block) {
			++blocks_;

			if (blocks_.time >= this->now_level.attack_count_r) {
				blocks_.type = attack_red_type;
			} else if (blocks_.time >= this->now_level.attack_count_y) {
				blocks_.type = attack_yellow_type;
			}
		}
	}

	bool force_set = false;
	bool candown = true;
	if (can_action) {

		//�L�[�����������Ă��邩����
		//���x���ɂ���Ĉꉞ�ς�����
		if (last_action_count > now_level.last_action_count_max) {
			last_action_count = 0;

			if ((last_action_key & (Config::ROTATION_CW | Config::ROTATION_CCW)) != 0) {
				//�Ō�̑��삪��]
				//��]����͖������Ԃ������͈قȂ鑀������܂Ȃ���,��]�̎󂯕t�������Ȃ�
			} else {
				//�L�[�����������Ƃ�, ��莞�Ԃ����ɍē��͂��s����
				last_action_key = Config::NONE;
			}

		} else if (last_action_key != Config::NONE) {
			//�L�[�����������Ă��邩����̂��߂̃J�E���g�l�㏸
			last_action_count++;
		}

		//�L�[���͎��ɓ���\�ł����, ���삳����
		//action != NONE �́@�L�[���͂����邩�̊m�F
		//((action & LEFT) != 0) �͎w��̃A�N�V�������̊m�F(���̏ꍇ��LEFT=���ړ�)
		//last_action_key != LEFT �̓L�[�����������ĘA���ړ����Ȃ��悤�ɂ������
		//1�񂵂������ĂȂ��̂�2�񔽉�������̂��h����
		if (action != Config::NONE && ((action & Config::LEFT) != 0) && last_action_key != Config::LEFT) {
			move(-1, 0, 0);					//���ړ�
			this->set_count = 0;			//�L�[���͂��s���Ă���̎��Ԍv���悤
			this->last_action_key = Config::LEFT;	//�Ō�ɓ��͂����L�[��ۑ�
			last_action_count = 0;			//�Ō�Ɏ��ۂɓ�������������̎��Ԍv��
		} else if (action != Config::NONE && ((action & Config::RIGHT) != 0) && last_action_key != Config::RIGHT) {
			move(1, 0, 0);					//�E�ړ�
			this->set_count = 0;
			this->last_action_key = Config::RIGHT;
			last_action_count = 0;
		} else if (action != Config::NONE && ((action & Config::ROTATION_CW) != 0) && last_action_key != Config::ROTATION_CW) {
			move(0, 0, 1);					//���v����ɉ�]
			this->set_count = 0;
			this->last_action_key = Config::ROTATION_CW;
			last_action_count = 0;
		} else if (action != Config::NONE && ((action & Config::ROTATION_CCW) != 0) && last_action_key != Config::ROTATION_CCW) {
			move(0, 0, -1);					//�����v����ɉ�]
			this->set_count = 0;
			this->last_action_key = Config::ROTATION_CCW;
			last_action_count = 0;
		} else if (action != Config::NONE && ((action & Config::HOLD) != 0)) {
			hold();							//�z�[���h
			this->down_count = 0;
			this->set_count = 0;
			last_action_count = 0;
		} else if (this->down_count > this->now_level.down_speed || (action != Config::NONE && ((action & Config::DOWN) != 0)) && last_action_key != Config::DOWN) {
			//�d�͂������͉��ړ����͂̎󂯕t��
			//��L�����ɉ����ďd�͂ł̔���������Ă���
			this->down_count = 0;
			this->set_count = 0;
			this->last_action_key = Config::DOWN;
			last_action_count = 0;

			this->score++;

			//���ֈړ�
			candown = move(0, -1, 0);
		} else if (action != Config::NONE && ((action & Config::HARD_DROP) != 0) && last_action_key != Config::HARD_DROP) {
			//�n�[�h�h���b�v
			this->last_action_key = Config::HARD_DROP;
			this->score += 2 * (this->now_block.y - this->ghost.y);

			this->set_count = 0;
			this->down_count = 0;
			last_action_count = 0;
			now_block = ghost;//����u���b�N���S�[�X�g�ɏ���������set����
			force_set = true;//�����I��set
		}

		this->down_count++;
	} else {
		//����s��
		if (put_block_state) {
			//�u���b�N�V�K�ݒu
			if (can_put_block) {
				setNewBlock();	//�V�����u���b�N�𐶐�
				can_action = true;//��������悤��
			} else {
				//���C���폜��͂����ɗ���͂�
				//��ɍU���p�u���b�N�ݒu
				if (addBlockFromDown()) {
					//�U���u���b�N�ݒu�ݒu���I�������, �V�K�u���b�N������
					can_put_block = true;
				}
			}
		} else if (delete_line_wait) {
			//���C���폜��̔���
			//�����������莞�ԑ҂�
			if (delete_line_wait_count > now_level.delete_line_wait_count_max) {
				blockDown();	//�u���b�N��������
				put_block_state = true;//�V�����u���b�N�𐶐�

				//����g�p�����ϐ��̃��Z�b�g
				delete_line_wait = false;
				delete_line_wait_count = 0;
			}
			delete_line_wait_count++;
		}
	}

	if (action == Config::NONE && can_action) {
		//������
		this->set_count++;//�����쎞�Ԃ��X�V
		last_action_key = Config::NONE;
		last_action_count = 0;
	}

	//�����삪��莞�ԑ�����, HARDDROP�����Ƃ�
	if (this->set_count == this->now_level.set_count_max || force_set || !candown) {
		if (candown)candown = move(0, -1, 0, false);//�����ړ��ł��邩�m�F(���ۂɓ������Ȃ�)

		if (!candown) {
			//���ړ��s��
			this->set_count = 0;
			this->can_action = false;
			set();			//�Œ�
			deleteBlock();	//���C������

			//Score�p��Back to Back�v�Z
			if (this->tspin_status != T_SPIN_NONE || delete_line.size() == 4) {
				//�e�g���X�ET-Spin
				back_to_back++;
			} else if (delete_line.size() != 0) {
				//�e�g���X�ET-Spin�̂ǂ�����Ȃ��ʏ�̃��C������
				back_to_back = 0;
			}



			if (delete_line.size() == 0) {
				//���C���������Ȃ�����
				//�������ܐV�����u���b�N�𐶐�(blockDown()���ĂԕK�v�Ȃ�)
				put_block_state = true;
				ren = 0;
			} else {
				need_calc_score = true;//Score�v�Z�̗v��
				//�����������C����������.(blockDown()���Ă�)
				delete_line_wait = true;
				delete_line_wait_count = 0;
				ren++;

				//�����������C�����������̂�Score�v�Z
				calcScore();
			}
		}
	}

	return true;
}

//Score�擾
Board::Score Board::getScore() {
	Score s;
	s.level = now_level.level;
	s.ren = ren - 1 < 0 ? 0 : ren - 1 < 0;
	s.score = score;

	//�폜�̂����
	//tetris�Ƃ�t spin�Ƃ������Ƃ��ɉ�ʂɕ������\�������悤�ɂ���
	if (this->delete_line.size() >= 4) {
		//�e�g���X
		s.name1 = "Tetris";
		s.name2 = "";
	} else if (this->delete_line.size() >= 3 && (this->tspin_status == T_SPIN_NORMAL || this->tspin_status == T_SPIN_MINI)) {
		//T-spin triple
		s.name1 = "T spin";
		s.name2 = "   triple";
	} else if (this->delete_line.size() == 2 && (this->tspin_status == T_SPIN_NORMAL || this->tspin_status == T_SPIN_MINI)) {
		//T-spin double
		s.name1 = "T spin";
		s.name2 = "   double";
	} else if (this->delete_line.size() == 1 && (this->tspin_status == T_SPIN_NORMAL || this->tspin_status == T_SPIN_MINI)) {
		//T-spin single
		s.name1 = "T spin";
		s.name2 = "   single";
	} else {
		s.name1 = "";
		s.name2 = "";
	}

	return s;
}

//�摜�ǂݍ���
void Board::loadimg() {
	int image_size = 9;
	this->images = new cv::Mat[image_size];

	for (int i = 0; i < image_size; i++) {
		cv::Mat tmp = cv::imread("images/" + std::to_string(i) + ".png");
		cv::resize(tmp, tmp, cv::Size(),
			static_cast<double>(img_size) / static_cast<double>(tmp.cols),
			static_cast<double>(img_size) / static_cast<double>(tmp.rows));
		images[i] = tmp.clone();
	}

	std::cout << "load images" << std::endl;
}

//�S�[�X�g�̐ݒ�
void Board::ghost_set() {
	//�S�[�X�g���󂾂�����, ���쒆�̂��̂ƃ^�C�v��������肷���, �X�V����
	if (&ghost != nullptr || ghost.getType() != now_block.getType()) {
		ghost = now_block.clone();
	}

	//�Ƃ肠�����S�[�X�g�𑀍쒆�̂�ɏd�˂�(x���W�͓����Ȃ���)
	ghost.x = now_block.x;
	ghost.y = now_block.y;
	//��i���݂̈���j����u���Ȃ��Ȃ�ꏊ���������ɒT������
	for (int gy = now_block.y - 1; gy >= 1; gy--) {
		ghost.y = gy;

		//�S�[�X�g�̍��W�擾
		auto points = ghost.getPoints();
		bool has_block = false;
		for (auto& p : points) {
			if (rawboard[p.y][p.x] != 0) {
				has_block = true;
				break;
			}
		}

		//�S�[�X�g���u���Ȃ�������, 1�オ�S�[�X�g�ʒu
		if (has_block) {
			ghost.y = gy + 1;
			break;
		}
	}
}

//�~�m�̉摜�擾
cv::Mat Board::getMinoImage(int type, int type_c) {
	//���S�ʒu�𒲐����Ȃ���`�悷��
	cv::Mat display_mino(MINO_SIZE_MAX * this->img_size, MINO_SIZE_MAX * this->img_size, CV_8UC3, cv::Scalar(0, 0, 0));

	if (type <= 0 || type >= this->mino_number) {
		return display_mino;
	}

	Blocks tmp = this->minos[type].clone();
	tmp.x = 0;
	tmp.y = 0;

	//�ő�ʒu�E�ŏ��ʒu��ۑ�
	cv::Size min(MINO_SIZE_MAX, MINO_SIZE_MAX);
	cv::Size max(-MINO_SIZE_MAX, -MINO_SIZE_MAX);

	//���W�擾
	auto points = tmp.getPoints();
	//�ő�ʒu�E�ŏ��ʒu���v�Z
	for (auto& p : points) {
		if (min.height > p.y) min.height = p.y;
		if (min.width > p.x) min.width = p.x;

		if (max.height < p.y) max.height = p.y;
		if (max.width < p.x) max.width = p.x;
	}

	//�������v�Z
	int width = (max.width - min.width) + 1;
	int height = (max.height - min.height) + 1;

	//���S�ʒu���v�Z����, ���炷�ʂ��o��
	//1�����Ă���͖̂{�Ƃƍ��킹�邽��(�ق�Ƃ͍s���Ȃ��ق����o�O���܂Ȃ�����)
	int dx = MINO_SIZE_MAX / 2 - width / 2 - 1;
	int dy = MINO_SIZE_MAX / 2 - height / 2;

	//show�ƈ����, �u���b�N�̈ʒu�����`�悷��
	for (auto& p : points) {
		int px = p.x - min.width + dx;
		int py = p.y - min.height + dy;

		int image_x = px * this->img_size;
		int image_y = (MINO_SIZE_MAX - 1 - py) * this->img_size;

		if (type_c < 0)paste(display_mino, this->images[type].clone(), image_x, image_y);
		else paste(display_mino, this->images[type_c].clone(), image_x, image_y);
	}
	return display_mino;
}
