#pragma once
#include<vector>

//���삷��~�m�̃N���X
class Blocks {
public:

	//���݂̈ʒu�Ɖ���]���Ă��邩
	int x;
	int y;
	int rotate;

	//�u���b�N�̉�]���S����̈ʒu����ۑ����邽�߂̃N���X
	struct Point {
		Point(int x, int y) {
			this->x = x;
			this->y = y;
		}

		Point() {
			this->x = 0;
			this->y = 0;
		}

		int x;
		int y;
	};

	//T spin�̓����蔻��p��Point�ɉ�����Status��ۑ��ł���悤�ɂ�������
	struct Status {

		Status(int x, int y, int status) {
			this->p = Point(x, y);
			this->status = status;
		}

		Status() {
			this->p = Point(0, 0);
			this->status = 0;
		}

		Point p;
		int status;
	};

	//Block�̃R���X�g���N�^
	Blocks() : x(0), y(0), rotate(0), type(0), rot_max(1){}

	~Blocks() {}


	//�u���b�N�̒ǉ�
	inline Blocks add(int x, int y) {
		this->blocks.push_back(Point(x, y));

		return *this;
	}

	//��]�ł���񐔂̐ݒ�
	inline Blocks setRot(int rot) {
		this->rot_max = rot;
		return *this;
	}

	//�u���b�N�̃^�C�v��ݒ�
	inline Blocks setType(int type) {
		this->type = type;
		return *this;
	}

	//T�^�p��T-spin�p�̓����蔻���ǉ�
	inline Blocks addStatus(int x, int y, int st) {
		this->stas.push_back(Status(x, y, st));

		return *this;
	}

	//�u���b�N�̍��W���擾
	std::vector<Point> getPoints();

	//T spin�p�̓����蔻��̈ꗗ���擾
	//���e�͏�̍��W�v�Z�Ƃقړ���(���S���W�͗v��Ȃ��̂ł��̒ǉ��͂��Ȃ�)
	std::vector<Status> getStats();

	//�R�s�[
	Blocks clone();


	//��������getter��

	inline int getType() {
		return this->type;
	}

	inline int getRotMax() {
		return this->rot_max;
	}

	inline int getStatSize() {
		return (int)this->stas.size();
	}

private:
	int rot_max;
	int type;
	std::vector<Point> blocks;
	std::vector<Status> stas;
};