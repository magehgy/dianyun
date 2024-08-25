#pragma once

#include <opencv2/opencv.hpp>
#include "inicpp.hpp"
#include <time.h>

using namespace std;
using namespace cv;
using namespace inicpp;

#define AFFINE //����任����͸�ӱ任
#ifdef AFFINE
#define QRCODECNT 3 //����任��Ҫ3����
#else QRCODECNT 4 //͸�ӱ任��Ҫ4����
#endif
class C_OpenCV
{
public:
	//conf.ini

	int h = 0, w = 0;	//ͼ����
	Mat image;	//��ǰͼ��
	string mediaSource;	//��Ƶ�ļ�|��ƵԴ
	int camNo;//������
	int mediaType; //0:��� 1����Ƶ�� 2����Ƶ�ļ�
	int cameraW, cameraH;//�������ͼ����

	// ���÷���任��Դ���Ŀ���
	Point2f srcTri[3];
	Point2f dstTri[3];

	bool bSetSubPic = false;//���ú�3|4����ά������
	bool bGotTransMat = false;//�Ƿ������ά�벢�õ�ת������
	Mat transMat;//ת������

	//��ά������
	QRCodeDetector qrDecoder;

	void CaliTransMat();

	C_OpenCV(string iniFile);
	void Init();
	void warpAffineAFrame();
	void SetSubPic();

private:
	//ʱ������������Ͽ�����ʱ������Ƶ������
	time_t tk;


	VideoCapture capture;//��ƵԴ

	double scale;
	int subPicW = 0, subPicH = 0;

	string strQR1, strQR2, strQR3;//��ά���ı�

	// ����ROI���򣬲���Ϊx, y, width, height
	Rect roi[QRCODECNT];//���ϣ����ϣ����£�����4�����򣨷���任ֻ��Ҫ3���㣬͸�ӱ任��Ҫ4���㣩

	//ini�ļ���д
	string iniFile;
	//IniManager ini_manager;

	//������ͷ|��Ƶ��|��Ƶ�ļ�
	bool OpenMedia();
};