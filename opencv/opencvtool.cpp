#include "opencvtool.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include <QSet>
#include <QDebug>

using namespace cv;
using namespace std;

OpenCVTool::OpenCVTool(QObject *parent) : QObject(parent)
{

}

QList<QLineF> OpenCVTool::loadLinesFromImage(const QString &path)
{
    QList<QLineF> lines = {};

    Mat source = imread("/Users/lyh/codes/opencvtest/1.jpg");
    if (source.empty()) {
        qDebug() << "fail step 1";
        return lines;
    }

    Mat temp = preprocess(source);


//    Mat source = imread("/Users/lyh/codes/opencvtest/1.jpg");
//    if (source.empty()) {
//        qDebug() << "fail step 1";
//        return lines;
//    }

//    Mat gray;
//    cvtColor(source, gray, COLOR_BGR2GRAY);

//    imwrite("/Users/lyh/codes/opencvtest/2.jpg", gray);

//    Mat gaus;
//    GaussianBlur(gray, gaus, {5, 5}, 0);

//    imwrite("/Users/lyh/codes/opencvtest/3.jpg", gaus);

//    // 去噪声与二值化
//    Mat binary;
//    Canny(gaus, binary, 80, 160, 3, true);

//    imwrite("/Users/lyh/codes/opencvtest/4.jpg", binary);

    return lines;
}

Mat OpenCVTool::preprocess(Mat source)
{
    qDebug() << source.rows;
    qDebug() << source.cols;

    QSet<QString> set;

    for (int i = 0; i < 10 /*source.rows*/; i++) {
        for (int j = 0; j < 10 /*source.cols*/; j++) {

            Vec3b bgr = source.at<Vec3b>(i, j);

//            QString color = QString("%1 %2 %3").arg((int)bgr[0], (int)bgr[1], (int)bgr[2]);

            qDebug() << (int)bgr[0] << (int)bgr[1] << (int)bgr[2];

//            set.insert(color);
        }
    }

    qDebug() << set;

    return source;
}
