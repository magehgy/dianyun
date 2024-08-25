#include "C_OpenCV.h"


int test()
{

    // 加载原始图像
    cv::Mat src = cv::imread("source_image.jpg");
    if (src.empty()) {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    // 设置仿射变换的源点和目标点
    cv::Point2f srcTri[3];
    srcTri[0] = cv::Point2f(0, 0);
    srcTri[1] = cv::Point2f(src.cols - 1, 0);
    srcTri[2] = cv::Point2f(0, src.rows - 1);

    cv::Point2f dstTri[3];
    dstTri[0] = cv::Point2f(0, src.rows * 0.33);
    dstTri[1] = cv::Point2f(src.cols * 0.85, src.rows * 0.25);
    dstTri[2] = cv::Point2f(src.cols * 0.15, src.rows * 0.7);

    // 计算仿射变换矩阵
    cv::Mat warp_mat = cv::getAffineTransform(srcTri, dstTri);

    // 对原始图像应用仿射变换
    cv::Mat warp_dst = cv::Mat::zeros(src.rows, src.cols, src.type());
    cv::warpAffine(src, warp_dst, warp_mat, warp_dst.size());

    // 显示结果
    cv::imshow("Original Image", src);
    cv::imshow("Warped Image", warp_dst);
    cv::waitKey(0);
    return 0;
}

C_OpenCV::C_OpenCV(string file)
{
    iniFile = file;
    //ini_manager = IniManager(iniFile);
    qrDecoder = QRCodeDetector();
    tk = time(NULL);
    Init();
}

void C_OpenCV::Init()
{
    IniManager ini_manager(iniFile);
    //cout << ini_manager["video"]["mediaType"];
    dstTri[0].x = ini_manager["affine"].toInt("dstX1");
    dstTri[0].y = ini_manager["affine"].toInt("dstY1");

    dstTri[1].x = ini_manager["affine"].toInt("dstX2");
    dstTri[1].y = ini_manager["affine"].toInt("dstY2");

    dstTri[2].x = ini_manager["affine"].toInt("dstX3");
    dstTri[2].y = ini_manager["affine"].toInt("dstY3");

    mediaSource = ini_manager["video"]["videosrc"];
    camNo = ini_manager["video"].toInt("camera");
    mediaType = ini_manager["video"].toInt("mediaType");
    cameraW = ini_manager["video"].toInt("cameraW");
    cameraH = ini_manager["video"].toInt("cameraH");

    scale = ini_manager["affine"].toDouble("scale");

    strQR1 = ini_manager["QRCode"]["strQR1"];
    strQR2 = ini_manager["QRCode"]["strQR2"];
    strQR3 = ini_manager["QRCode"]["strQR3"];

    OpenMedia();
}

//打开相机，设置图像大小
bool C_OpenCV::OpenMedia()
{
    //打开相机或视频流
    if (mediaType == 0) {
        capture = VideoCapture(camNo);
    }
    else {
        capture = VideoCapture(mediaSource);
    }
    if (capture.isOpened())
    {
        if (mediaType == 0)
        {
            capture.set(cv::CAP_PROP_FRAME_WIDTH, cameraW);
            capture.set(cv::CAP_PROP_FRAME_HEIGHT, cameraH);

        }
        if (!bSetSubPic)
        {
            SetSubPic();
        }
        return true;
    }
    return false;
}

//根据图像大小计算出分割后的图像大小
void C_OpenCV::SetSubPic()
{
    if (bSetSubPic)
        return;

    if (capture.read(image))
    {
        // 获取图像的大小
        cv::Size size = image.size();

        // 获取图像的宽度和高度
        w = size.width;
        h = size.height;
        subPicW = (int)(w * scale);
        subPicH = (int)(h * scale);
        roi[0] = Rect(0, 0, subPicW, subPicH);
        roi[1] = Rect(w - subPicW, 0, subPicW, subPicH);
        roi[2] = Rect(0, h - subPicH, subPicW, subPicH);
#ifndef AFFINE
        roi[3] = Rect(w - subPicW, h - subPicH, subPicW, subPicH);
#endif
        
        printf("\nw,h,subPicW,subPicH:%d,%d,%d,%d\n", w, h, subPicW, subPicH);

        bSetSubPic = true;
    }
}

//计算转换矩阵
void C_OpenCV::CaliTransMat()
{
    Mat frame, lastFrame;
    bool bGotNewFrame = false;

    bGotNewFrame = false;
    //获取最新一帧图像
    while (capture.read(frame))
    {
        bGotNewFrame = true;
        if (mediaType == 0)
            break;
        //lastFrame = frame;
    }
    if (!bGotNewFrame)
        return;

    imshow("frame", frame);

    if (!bSetSubPic)
        return;
    for (int i = 0; i < QRCODECNT; i++)
    {
        Mat img = frame(roi[i]);
        imshow(to_string(i), img);
        vector<Point> qrPoints;
        string qrStr = qrDecoder.detectAndDecode(img, qrPoints);
        if (qrStr.length() > 0) {
            if (i == 0)
            {
                srcTri[0].x = qrPoints[0].x;
                srcTri[0].y = qrPoints[0].y;
                printf("point1:%d,%d\n", qrPoints[0].x, qrPoints[0].y);
            }
            else if (i == 1)
            {
                srcTri[1].x = qrPoints[1].x + w - subPicW;
                srcTri[1].y = qrPoints[1].y;
                printf("point2:%d,%d\n", qrPoints[1].x, qrPoints[1].y);
            }
            else if (i == 2)
            {
                srcTri[i].x = qrPoints[3].x;
                srcTri[i].y = qrPoints[3].y + h - subPicH;
                printf("point3:%d,%d\n", qrPoints[3].x, qrPoints[3].y);
            }                
            else if (i == 3)//第四个二维码，透视变换时生效
            {
                srcTri[1].x = qrPoints[2].x + w - subPicW;
                srcTri[i].y = qrPoints[2].y + h - subPicH;
                printf("point4:%d,%d\n", qrPoints[2].x, qrPoints[2].y);
            }
                
            
            
            /*
            // 画出二维码区域和直线
            cv::Mat img = image.clone();
            for (int i = 0; i < corners.size(); i++) {
                cv::line(img, corners[i], corners[(i + 1) % corners.size()], cv::Scalar(0, 255, 0), 3);
            }
            cv::imshow("Straight QR Code", straight_qrcode);
            cv::imshow("Detected QR Code", img);
            cv::waitKey(0);*/
        }
        else {
            std::cout << "No QR Code detected." << std::endl;
            return;
        }
    }
    transMat = getAffineTransform(srcTri, dstTri);

    cout << "srcTri\n";
    printf("%f,%f,%f,%f,%f,%f\n", srcTri[0].x, srcTri[0].y, srcTri[1].x, srcTri[1].y, srcTri[2].x, srcTri[2].y);
    bGotTransMat = true;
    destroyAllWindows();
}

void C_OpenCV::warpAffineAFrame()
{
    if (!bGotTransMat) {
        return;
    }
    bool bGotNewFrame = false;
    //获取最新一帧图像
    while (capture.read(image))
    {
        bGotNewFrame = true;
        if (mediaType == 0) {
            break;
        }
        //lastFrame = frame;
    }
    if (!bGotNewFrame) {
        return;
    }
    cv::Mat warp_dst = cv::Mat::zeros(h, w, image.type());
    warpAffine(image, warp_dst, transMat, warp_dst.size());

    imshow("image", image);
    imshow("warp_dst", warp_dst);
}