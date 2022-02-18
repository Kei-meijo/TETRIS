#pragma once

#include <opencv2/core/core.hpp>

// �摜���摜�ɓ\��t����֐�
void paste(cv::Mat& dst, cv::Mat src, int x, int y, int width, int height);

// �摜���摜�ɓ\��t����֐��i�T�C�Y�w����ȗ������o�[�W�����j
void paste(cv::Mat& dst, cv::Mat src, int x, int y);

// �摜���摜�ɓ\��t����֐�(�u�����h)
void blend_paste(cv::Mat& dst, cv::Mat src, int x, int y, int width, int height, bool blend = true);

//�摜����������֐�
void concat(cv::Mat& dst, cv::Mat left, cv::Mat right, int margin);

// �摜���摜�ɓ\��t����֐�(�u�����h)�i�T�C�Y�w����ȗ������o�[�W�����j
void blend_paste(cv::Mat dst, cv::Mat src, int x, int y);