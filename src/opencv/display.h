#pragma once
#include "../board/board.h"

static const int h = 6;		//—]”’
static const int nh = 15;	//next ‚Æ hold ‚Æ attack‚Ì‰¡‚Ì—]”’
static const int dh = 50;	//next ‚Æ hold‚Ìã‚ÌŠÔŠu
static const int th = 20;	//OŠp‚Ì‘å‚«‚³
static const int hh = 20;	//next ‚Æ hold‚Ìc‚Ì—]”’
static const int len = 30;	//next ‚Æ score‚ÌŠÔ‚Ì•
static const cv::Scalar color = cv::Scalar(238, 240, 9);//˜g‚ÌF


//‰æ–Ê(å‚É˜g)‚ğ•`‰æ
//“Á‚Éà–¾‚µ‚È‚¢
void createDisplay(Board& board, cv::Mat& display);