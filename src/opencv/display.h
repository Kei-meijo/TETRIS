#pragma once
#include "../board/board.h"

static const int h = 6;		//�]��
static const int nh = 15;	//next �� hold �� attack�̉��̗]��
static const int dh = 50;	//next �� hold�̏�̊Ԋu
static const int th = 20;	//�O�p�̑傫��
static const int hh = 20;	//next �� hold�̏c�̗]��
static const int len = 30;	//next �� score�̊Ԃ̕�
static const cv::Scalar color = cv::Scalar(238, 240, 9);//�g�̐F


//���(��ɘg)��`��
//���ɐ������Ȃ�
void createDisplay(Board& board, cv::Mat& display);