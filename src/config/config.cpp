#include "config.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

Config::Config(const char* file_name) : fps(0), height(0), width(0) {
	namespace nl = nlohmann;
	std::ifstream ifs(file_name);

	//jsonファイルからデータ読み込み
	if (ifs.good()) {
		nl::json j;
		ifs >> j;

		//設定読み込み
		std::cout << j["project name"].get<std::string>() << std::endl;
		this->fps = j["fps"].get<int>();
		this->width = j["width"].get<int>();
		this->height = j["height"].get<int>();


		//Level読み込み
		for (const auto& elem : j["levels"].items()) {
			Level l;
			l.level = elem.value()["level"].get<int>();
			l.score.bonus = elem.value()["bonus"].get<double>();
			l.score.lvlup_score = elem.value()["level up score"].get<int>();

			//speed
			l.last_action_count_max = elem.value()["speed"]["last action count"].get<int>();
			l.set_count_max = elem.value()["speed"]["set count"].get<int>();
			l.delete_line_wait_count_max = elem.value()["speed"]["delete line wait"].get<int>();
			l.down_speed = elem.value()["speed"]["down speed"].get<int>();
			l.attack_count_y = elem.value()["speed"]["attack count yellow"].get<int>();
			l.attack_count_r = elem.value()["speed"]["attack count red"].get<int>();

			//score
			l.score.line = elem.value()["score"]["line"].get<int>();
			l.score.line2 = elem.value()["score"]["line2"].get<int>();
			l.score.line3 = elem.value()["score"]["line3"].get<int>();
			l.score.tetris = elem.value()["score"]["tetris"].get<int>();

			l.score.perfect_line = elem.value()["score"]["perfect line"].get<int>();
			l.score.perfect_line2 = elem.value()["score"]["perfect line2"].get<int>();
			l.score.perfect_line3 = elem.value()["score"]["perfect line3"].get<int>();
			l.score.perfect_tetris = elem.value()["score"]["perfect tetris"].get<int>();

			l.score.tspin_single = elem.value()["score"]["t-spin single"].get<int>();
			l.score.tspin_double = elem.value()["score"]["t-spin double"].get<int>();
			l.score.tspin_triple = elem.value()["score"]["t-spin triple"].get<int>();
			l.score.ren = elem.value()["score"]["REN"].get<int>();
			l.score.ren_max = elem.value()["score"]["REN MAX"].get<int>();
			l.score.btb = elem.value()["score"]["back to back"].get<double>();

			//attack
			l.attack.line = elem.value()["attack"]["line"].get<int>();
			l.attack.line2 = elem.value()["attack"]["line2"].get<int>();
			l.attack.line3 = elem.value()["attack"]["line3"].get<int>();
			l.attack.tetris = elem.value()["attack"]["tetris"].get<int>();

			l.attack.perfect_line = elem.value()["attack"]["perfect line"].get<int>();
			l.attack.perfect_line2 = elem.value()["attack"]["perfect line2"].get<int>();
			l.attack.perfect_line3 = elem.value()["attack"]["perfect line3"].get<int>();
			l.attack.perfect_tetris = elem.value()["attack"]["perfect tetris"].get<int>();

			l.attack.tspin_single = elem.value()["attack"]["t-spin single"].get<int>();
			l.attack.tspin_double = elem.value()["attack"]["t-spin double"].get<int>();
			l.attack.tspin_triple = elem.value()["attack"]["t-spin triple"].get<int>();
			l.attack.btb = elem.value()["attack"]["back to back"].get<int>();

			for (const auto& elem_ren : elem.value()["attack"]["REN"].items()) {
				l.attack.ren.push_back(static_cast<int>(elem_ren.value()));
			}

			//this->levels.push_back(l);
			this->levels.push_back(l);
		}
	}
}