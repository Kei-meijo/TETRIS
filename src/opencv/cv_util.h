#pragma once

#include <opencv2/core/core.hpp>

// 画像を画像に貼り付ける関数
void paste(cv::Mat& dst, cv::Mat src, int x, int y, int width, int height);

// 画像を画像に貼り付ける関数（サイズ指定を省略したバージョン）
void paste(cv::Mat& dst, cv::Mat src, int x, int y);

// 画像を画像に貼り付ける関数(ブレンド)
void blend_paste(cv::Mat& dst, cv::Mat src, int x, int y, int width, int height, bool blend = true);

//画像を結合する関数
void concat(cv::Mat& dst, cv::Mat left, cv::Mat right, int margin);

// 画像を画像に貼り付ける関数(ブレンド)（サイズ指定を省略したバージョン）
void blend_paste(cv::Mat dst, cv::Mat src, int x, int y);