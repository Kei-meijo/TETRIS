#include "display.h"

#include "cv_util.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//âÊñ (éÂÇ…òg)Çï`âÊ
//ì¡Ç…ê‡ñæÇµÇ»Ç¢
void createDisplay(Board& board, cv::Mat& display) {
	cv::Mat img = board.show();
	cv::Mat hold = board.showHoldMino(0.8);
	cv::Mat next = board.showNextMino(0.8, 2 * h);
	cv::Mat attack = board.showAttackMino(5);

	cv::resize(next, next, cv::Size(),
		static_cast<double>(img.rows / 2) / static_cast<double>(next.rows),
		static_cast<double>(img.rows / 2) / static_cast<double>(next.rows));

	cv::resize(hold, hold, cv::Size(), 0.75, 0.75);

	int width = 4 * h + hold.cols + img.cols + next.cols + 4 * nh;
	int height = 2 * h + img.rows;
	display = cv::Mat(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

	//hold
	paste(display, hold, h + nh, dh + hh);

	//board
	paste(display, img, 2 * h + 2 * nh + hold.cols, h);

	//attack
	paste(display, attack, h + nh + hold.cols - attack.cols, display.rows - attack.rows - h);

	//next
	paste(display, next, 3 * h + hold.cols + img.cols + 3 * nh, dh + hh);


	//hold
	cv::rectangle(display, cv::Rect(th, 0, hold.cols + h - th + 2 * nh, th), color, -1);
	cv::rectangle(display, cv::Rect(0, th, hold.cols + h + 2 * nh, dh - th), color, -1);
	cv::rectangle(display, cv::Rect(0, dh, h, hold.rows + h - th + 2 * hh), color, -1);
	cv::rectangle(display, cv::Rect(th, dh + hold.rows + 2 * hh, h + 2 * nh + hold.cols - th, h), color, -1);
	cv::fillConvexPoly(display, std::vector<cv::Point>{cv::Point(0, th), cv::Point(th, 0), cv::Point(th, th)}, color);
	cv::fillConvexPoly(display, std::vector<cv::Point>{cv::Point(h, dh + th), cv::Point(th + h, dh), cv::Point(h, dh)}, color);
	cv::fillConvexPoly(display, std::vector<cv::Point>{
		cv::Point(0, dh + hold.rows + 2 * hh + h - th), cv::Point(th, dh + hold.rows + 2 * hh + h),
			cv::Point(th + h, dh + hold.rows + 2 * hh), cv::Point(h, dh + hold.rows + 2 * hh - th)}
	, color);

	cv::putText(display, "HOLD", cv::Point(h + th + 30, dh - 20), cv::FONT_HERSHEY_COMPLEX | cv::FONT_ITALIC, 0.7, cv::Scalar(0, 0, 0), 2, cv::LINE_AA);

	//board
	cv::rectangle(display, cv::Rect(h + hold.cols + 2 * nh, 0, h, height), color, -1);
	cv::rectangle(display, cv::Rect(h + hold.cols + 2 * nh, 0, img.cols + h, h), color, -1);
	cv::rectangle(display, cv::Rect(h + hold.cols + 2 * nh, img.rows + h, img.cols + h, h), color, -1);
	cv::rectangle(display, cv::Rect(2 * h + hold.cols + img.cols + 2 * nh, 0, h, height), color, -1);

	//next

	int n_x = 3 * h + hold.cols + img.cols + 2 * nh;
	int n_w = next.cols + 2 * nh;
	int n_h = next.rows + 2 * hh;
	cv::rectangle(display, cv::Rect(n_x, 0, n_w - th + h, dh), color, -1);
	cv::rectangle(display, cv::Rect(n_x + n_w - th + h, th, th, dh - th), color, -1);
	cv::rectangle(display, cv::Rect(n_x + n_w, dh, h, n_h), color, -1);
	cv::rectangle(display, cv::Rect(n_x, dh + n_h, n_w + h, len), color, -1);

	cv::fillConvexPoly(display, std::vector<cv::Point>{
		cv::Point(n_x + n_w + h - th, 0), cv::Point(n_x + n_w + h, th), cv::Point(n_x + n_w + h - th, th)
	}, color);
	cv::fillConvexPoly(display, std::vector<cv::Point>{
		cv::Point(n_x + n_w - th, dh), cv::Point(n_x + n_w, dh + th), cv::Point(n_x + n_w, dh)
	}, color);
	cv::fillConvexPoly(display, std::vector<cv::Point>{
		cv::Point(n_x + n_w - th, dh + n_h), cv::Point(n_x + n_w, dh + n_h - th), cv::Point(n_x + n_w, dh + n_h)
	}, color);
	cv::putText(display, "NEXT", cv::Point(n_x + 15, dh - 20), cv::FONT_HERSHEY_COMPLEX | cv::FONT_ITALIC, 0.7, cv::Scalar(0, 0, 0), 2, cv::LINE_AA);

	//attack
	int a_y = dh + 2 * hh + h + hold.rows;
	int a_x = h + hold.cols - attack.cols;
	cv::rectangle(display, cv::Rect(a_x - h, a_y, h, display.rows - a_y - th), color, -1);
	cv::rectangle(display, cv::Rect(a_x - h + th, display.rows - h, attack.cols + nh * 2 + h - th, h), color, -1);
	cv::fillConvexPoly(display, std::vector<cv::Point>{
		cv::Point(a_x - h, display.rows - th), cv::Point(a_x - h + th, display.rows),
			cv::Point(a_x + th - h, display.rows - h), cv::Point(a_x - h, display.rows - th - h)}
	, color);

	//score
	int sx = 3 * h + hold.cols + img.cols + 2 * nh;
	int sy = dh + 2 * hh + next.rows + len;
	cv::Mat score = cv::Mat(display.rows - h - sy, display.cols - h - sx, CV_8UC3, cv::Scalar(0, 0, 0));


	Board::Score score_ = board.getScore();

	//ren
	cv::putText(score, "REN", cv::Point(10, 30), cv::FONT_HERSHEY_COMPLEX, 0.65, cv::Scalar(250, 250, 250), 1, cv::LINE_AA);
	cv::putText(score, std::to_string(score_.ren), cv::Point(60, 55), cv::FONT_HERSHEY_COMPLEX, 0.65, cv::Scalar(250, 250, 250), 1, cv::LINE_AA);

	//score
	cv::putText(score, "Score", cv::Point(10, 80), cv::FONT_HERSHEY_COMPLEX, 0.65, cv::Scalar(250, 250, 250), 1, cv::LINE_AA);
	cv::putText(score, std::to_string(score_.score) + "pts", cv::Point(20, 105), cv::FONT_HERSHEY_COMPLEX, 0.60, cv::Scalar(250, 250, 250), 1, cv::LINE_AA);

	//text
	//cv::putText(score, "Score", cv::Point(10, 200), cv::FONT_HERSHEY_COMPLEX, 0.65, cv::Scalar(250, 250, 250), 1, cv::LINE_AA);
	cv::putText(score, score_.name1, cv::Point(15, 165), cv::FONT_HERSHEY_COMPLEX, 0.60, cv::Scalar(250, 250, 250), 1, cv::LINE_AA);
	cv::putText(score, score_.name2, cv::Point(15, 195), cv::FONT_HERSHEY_COMPLEX, 0.60, cv::Scalar(250, 250, 250), 1, cv::LINE_AA);

	//level
	cv::putText(score, "Level " + std::to_string(score_.level), cv::Point(10, 260), cv::FONT_HERSHEY_COMPLEX, 0.65, cv::Scalar(250, 250, 250), 1, cv::LINE_AA);


	paste(display, score, sx, sy);
	cv::rectangle(display, cv::Rect(sx + score.cols, sy, h, score.rows + h - th), color, -1);
	cv::rectangle(display, cv::Rect(sx, sy + score.rows, h + score.cols - th, h), color, -1);
	cv::fillConvexPoly(display, std::vector<cv::Point>{
		cv::Point(sx + score.cols - th, sy), cv::Point(sx + score.cols, sy + th), cv::Point(sx + score.cols, sy)
	}, color);
	cv::fillConvexPoly(display, std::vector<cv::Point>{
		cv::Point(sx + score.cols + h - th, sy + score.rows + h), cv::Point(sx + score.cols + h, sy + score.rows + h - th),
			cv::Point(sx + score.cols, sy + score.rows - th), cv::Point(sx + score.cols - th, sy + score.rows)
	}, color);
}