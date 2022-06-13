#include <QCoreApplication>

#include "opencvtool.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    OpenCVTool oc;
    oc.loadLinesFromImage("");


//    Mat source = imread("D:/Code/opencvtest2/share.jpg");
//    if (source.empty()) {
//        qDebug() << "fail step 1";
//        return -1;
//    }

//    qDebug() << source.depth();

//    Mat gray;
//    cvtColor(source, gray, COLOR_BGR2GRAY);

//    imwrite("D:/Code/opencvtest2/share1.jpg", gray);
//    qDebug() << gray.depth();

//    Mat gaus;
//    GaussianBlur(gray, gaus, {5, 5}, 0);

//    imwrite("D:/Code/opencvtest2/share2.jpg", gaus);

//    // 去噪声与二值化
//    Mat binary;
//    Canny(gaus, binary, 80, 160, 3, true);

//    imwrite("D:/Code/opencvtest2/share3.jpg", binary);

//    // 定义矢量结构存放检测出来的直线
//    vector<Vec2f> lines;

////    HoughLines(binary, lines, 1, CV_PI / 180, 30, 0, 0, 0, 0.1);
//    HoughLines(binary, lines, 1, CV_PI / 180, 60, 0, 0, CV_PI / 2 - 0.1, CV_PI / 2 + 0.1);

//    qDebug() << lines.size();

//    for (const auto& point: lines) {
//        qDebug() << point[0] << point[1];

//        float rho = point[0];
//        float theta = point[1];

//        float pa = cos(theta);
//        float pb = sin(theta);

//        float x0 = pa * rho;
//        float y0 = pb * rho;

//        int x1 = cvRound(x0 + 2000 * (-pb));
//        int y1 = cvRound(y0 + 2000 * ( pa));
//        int x2 = cvRound(x0 - 2000 * (-pb));
//        int y2 = cvRound(y0 - 2000 * ( pa));

//        qDebug() << QPoint({x1, y1}) << QPoint({x2, y2});
//        line(source, {x1, y1}, {x2, y2}, {43, 43, 43});
//    }

//    imwrite("D:/Code/opencvtest2/share4.jpg", source);

    return a.exec();
}
