#pragma once
#include<vector>

//操作するミノのクラス
class Blocks {
public:

	//現在の位置と何回転しているか
	int x;
	int y;
	int rotate;

	//ブロックの回転中心からの位置情報を保存するためのクラス
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

	//T spinの当たり判定用にPointに加えてStatusを保存できるようにしたもの
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

	//Blockのコンストラクタ
	Blocks() : x(0), y(0), rotate(0), type(0), rot_max(1){}

	~Blocks() {}


	//ブロックの追加
	inline Blocks add(int x, int y) {
		this->blocks.push_back(Point(x, y));

		return *this;
	}

	//回転できる回数の設定
	inline Blocks setRot(int rot) {
		this->rot_max = rot;
		return *this;
	}

	//ブロックのタイプを設定
	inline Blocks setType(int type) {
		this->type = type;
		return *this;
	}

	//T型用のT-spin用の当たり判定を追加
	inline Blocks addStatus(int x, int y, int st) {
		this->stas.push_back(Status(x, y, st));

		return *this;
	}

	//ブロックの座標を取得
	std::vector<Point> getPoints();

	//T spin用の当たり判定の一覧を取得
	//内容は上の座標計算とほぼ同じ(中心座標は要らないのでその追加はしない)
	std::vector<Status> getStats();

	//コピー
	Blocks clone();


	//ここからgetter等

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