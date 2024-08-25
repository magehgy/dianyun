#pragma once

#include <opencv2/opencv.hpp>
#include "inicpp.hpp"
#include <time.h>

using namespace std;
using namespace cv;
using namespace inicpp;

#define AFFINE //仿射变换还是透视变换
#ifdef AFFINE
#define QRCODECNT 3 //仿射变换需要3个点
#else QRCODECNT 4 //透视变换需要4个点
#endif
class C_OpenCV
{
public:
	//conf.ini

	int h = 0, w = 0;	//图像宽高
	Mat image;	//当前图像
	string mediaSource;	//视频文件|视频源
	int camNo;//相机编号
	int mediaType; //0:相机 1：视频流 2：视频文件
	int cameraW, cameraH;//设置相机图像宽高

	// 设置仿射变换的源点和目标点
	Point2f srcTri[3];
	Point2f dstTri[3];

	bool bSetSubPic = false;//设置好3|4个二维码区域
	bool bGotTransMat = false;//是否检测出二维码并得到转换矩阵
	Mat transMat;//转换矩阵

	//二维码检测器
	QRCodeDetector qrDecoder;

	void CaliTransMat();

	C_OpenCV(string iniFile);
	void Init();
	void warpAffineAFrame();
	void SetSubPic();

private:
	//时间戳，如果网络断开，定时重连视频服务器
	time_t tk;


	VideoCapture capture;//视频源

	double scale;
	int subPicW = 0, subPicH = 0;

	string strQR1, strQR2, strQR3;//二维码文本

	// 定义ROI区域，参数为x, y, width, height
	Rect roi[QRCODECNT];//左上，右上，左下，右下4个区域（仿射变换只需要3个点，透视变换需要4个点）

	//ini文件读写
	string iniFile;
	//IniManager ini_manager;

	//打开摄像头|视频流|视频文件
	bool OpenMedia();
};