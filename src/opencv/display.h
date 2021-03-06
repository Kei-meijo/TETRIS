#pragma once
#include "../board/board.h"

static const int h = 6;		//余白
static const int nh = 15;	//next と hold と attackの横の余白
static const int dh = 50;	//next と holdの上の間隔
static const int th = 20;	//三角の大きさ
static const int hh = 20;	//next と holdの縦の余白
static const int len = 30;	//next と scoreの間の幅
static const cv::Scalar color = cv::Scalar(238, 240, 9);//枠の色


//画面(主に枠)を描画
//特に説明しない
void createDisplay(Board& board, cv::Mat& display);