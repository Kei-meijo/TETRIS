#include "blocks.h"

//�u���b�N�̍��W���擾
std::vector<Blocks::Point> Blocks::getPoints() {
	//�u���b�N�̈ꗗ�𐶐�
	std::vector<Point> blocks;
	//��]���S��ݒ�(���S�͌��ݍ��W���̂܂܂̒l)
	blocks.push_back(Point(this->x, this->y));

	//rotate�𐳂ɂȂ�悤�ɒ���
	while (this->rotate < 0) {
		this->rotate += 4;
	}

	//�����]�����邩
	//O�^�Ȃ� rot_max��1�Ȃ̂ŕK��0
	int rot = this->rotate % this->rot_max;

	//��]������(�u���b�N���ꂼ��œ����������s��)
	for (auto& point : this->blocks) {
		int px = point.x;
		int py = point.y;
		for (int r = 0; r < rot; r++) {
			//90��]�̉�]�s����|�����Ƃ��̎����̂܂�
			int nx = py;
			int ny = -px;

			px = nx;
			py = ny;
		}

		//���v�Z���I�����u���b�N��ݒ�
		blocks.push_back(Point(px + this->x, py + this->y));
	}

	return blocks;
}

//T spin�p�̓����蔻��̈ꗗ���擾
	//���e�͏�̍��W�v�Z�Ƃقړ���(���S���W�͗v��Ȃ��̂ł��̒ǉ��͂��Ȃ�)
std::vector<Blocks::Status> Blocks::getStats() {
	std::vector<Status> statuses;

	//��������status���Ȃ���΂Όv�Z�s�v
	if (getStatSize() > 0) {

		//rotate�𐳂�
		while (this->rotate < 0) {
			this->rotate += 4;
		}

		//�����͏�̕��ƈꏏ
		int rot = this->rotate % this->rot_max;
		for (auto& status : this->stas) {
			int px = status.p.x;
			int py = status.p.y;
			for (int r = 0; r < rot; r++) {
				int nx = py;
				int ny = -px;

				px = nx;
				py = ny;
			}

			statuses.push_back(Status(px + this->x, py + this->y, status.status));
		}
	}

	return statuses;
}

//�R�s�[
Blocks Blocks::clone() {
	Blocks block;
	block.x = this->x;
	block.y = this->y;
	block.rotate = this->rotate;
	block.rot_max = this->rot_max;
	block.type = this->type;

	for (auto& bl : this->blocks) {
		block.blocks.push_back(bl);
	}

	if (getStatSize() > 0) {
		for (auto& st : this->stas) {
			block.stas.push_back(st);
		}
	}

	return block;
}