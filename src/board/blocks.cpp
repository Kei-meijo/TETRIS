#include "blocks.h"

//ブロックの座標を取得
std::vector<Blocks::Point> Blocks::getPoints() {
	//ブロックの一覧を生成
	std::vector<Point> blocks;
	//回転中心を設定(中心は現在座標そのままの値)
	blocks.push_back(Point(this->x, this->y));

	//rotateを正になるように調整
	while (this->rotate < 0) {
		this->rotate += 4;
	}

	//何回回転させるか
	//O型なら rot_maxが1なので必ず0
	int rot = this->rotate % this->rot_max;

	//回転させる(ブロックそれぞれで同じ処理を行う)
	for (auto& point : this->blocks) {
		int px = point.x;
		int py = point.y;
		for (int r = 0; r < rot; r++) {
			//90回転の回転行列を掛けたときの式そのまま
			int nx = py;
			int ny = -px;

			px = nx;
			py = ny;
		}

		//今計算し終えたブロックを設定
		blocks.push_back(Point(px + this->x, py + this->y));
	}

	return blocks;
}

//T spin用の当たり判定の一覧を取得
	//内容は上の座標計算とほぼ同じ(中心座標は要らないのでその追加はしない)
std::vector<Blocks::Status> Blocks::getStats() {
	std::vector<Status> statuses;

	//そもそもstatusがなければば計算不要
	if (getStatSize() > 0) {

		//rotateを正に
		while (this->rotate < 0) {
			this->rotate += 4;
		}

		//ここは上の物と一緒
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

//コピー
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