#include "cv_util.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <math.h>

// �摜���摜�ɓ\��t����֐�
void paste(cv::Mat& dst, cv::Mat src, int x, int y, int width, int height) {
	blend_paste(dst, src, x, y, width, height, false);
}

// �摜���摜�ɓ\��t����֐��i�T�C�Y�w����ȗ������o�[�W�����j
void paste(cv::Mat& dst, cv::Mat src, int x, int y) {
	paste(dst, src, x, y, src.cols, src.rows);
}

// �摜���摜�ɓ\��t����֐�(�u�����h)
void blend_paste(cv::Mat& dst, cv::Mat src, int x, int y, int width, int height, bool blend) {
	cv::Mat resized_img;
	cv::resize(src, resized_img, cv::Size(width, height));

	if (x >= dst.cols || y >= dst.rows) return;
	int w = (x >= 0) ? std::min(dst.cols - x, resized_img.cols) : std::min(std::max(resized_img.cols + x, 0), dst.cols);
	int h = (y >= 0) ? std::min(dst.rows - y, resized_img.rows) : std::min(std::max(resized_img.rows + y, 0), dst.rows);
	int u = (x >= 0) ? 0 : std::min(-x, resized_img.cols - 1);
	int v = (y >= 0) ? 0 : std::min(-y, resized_img.rows - 1);
	int px = std::max(x, 0);
	int py = std::max(y, 0);

	cv::Mat roi_dst = dst(cv::Rect(px, py, w, h));
	cv::Mat roi_resized = resized_img(cv::Rect(u, v, w, h));
	if (blend) {
		cv::addWeighted(roi_dst, 0.65, roi_resized, 0.35, 0.0, roi_dst);
	} else {
		roi_resized.copyTo(roi_dst);
	}
}

//�摜����������֐�
void concat(cv::Mat& dst, cv::Mat left, cv::Mat right, int margin) {
	int width = left.cols + right.cols + margin;
	int height = std::max(left.rows, right.rows);
	dst = cv::Mat(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

	cv::Mat roi_left = dst(cv::Rect(0, 0, left.cols, left.rows));
	cv::Mat roi_right = dst(cv::Rect(left.cols + margin, 0, right.cols, right.rows));

	left.copyTo(roi_left);
	right.copyTo(roi_right);
}

// �摜���摜�ɓ\��t����֐�(�u�����h)�i�T�C�Y�w����ȗ������o�[�W�����j
void blend_paste(cv::Mat dst, cv::Mat src, int x, int y) {
	blend_paste(dst, src, x, y, src.cols, src.rows);
}