#include "tool.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <QVector2D>
#include <QLineF>
#include <QMap>
#include <QDebug>

using namespace cv;
using namespace std;

Tool::Tool(QObject *parent) : QObject(parent)
{

}

static void toVariantList(const vector<Vec4f>& lines, QVariantList& out) {
    out.clear();
    for (const Vec4f& line: lines) {
        out.append(QVariant(QVariantList{line[0], line[1], line[2], line[3]}));
    }
}

static void toVariantList(const QVector<QLineF>& lines, QVariantList& out) {
    out.clear();
    for (const QLineF& line: lines) {
        out.append(QVariant(QVariantList{line.p1().x(), line.p1().y(), line.p2().x(), line.p2().y()}));
    }
}

Mat Tool::preprocess(const Mat &src)
{
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    Mat gauss;
    GaussianBlur(gray, gauss, {9, 9}, 0);

    Mat median;
    medianBlur(gauss, median, 3);

    Mat edges;
    Canny(median, edges, 50, 100);

    return edges;
}

bool Tool::parse(const QString &path)
{
    Mat src = imread(path.toStdString());
    if (!src.data) {
        return false;
    }

    Mat edges = preprocess(src);

    vector<Vec4f> lines;
    HoughLinesP(edges, lines, 1, CV_PI/180, 20, 5, 5);

    // #
    toVariantList(lines, mStep1);

    //





//    /*
    QVector<QLineF> hlines;
    QVector<QLineF> vlines;

    for (const Vec4f& line: lines) {
        QLineF l(line[0], line[1], line[2], line[3]);

        QVector2D v(l.p2() - l.p1());
        QVector2D nv = v.normalized();

        if (nv.x() > 0.707106781187) {
            hlines.push_back(l);
        }
        else {
            vlines.push_back(l);
        }
    }

    qDebug() << lines.size();
    qDebug() << vlines;
    qDebug() << "";


    QVector<QLineF> als;
    vector<Vec4f> alines;

    // H
    if (1) {
        QSet<float> ys;

        for (const QLineF& hl: hlines) {
            ys.insert(hl.p1().y());
            ys.insert(hl.p2().y());
        }

        QList<float> list = ys.toList();
        qSort(list);

        QVector<QVector<float>> nys;

        int threshold = 20;
        float lastY = -100000.;
        QVector<float> lastYGroup;
        for(int i = 0; i < list.length(); ++i) {
            float y = list.at(i);
            if (y - lastY > threshold) {
                if (!lastYGroup.isEmpty()) {
                    nys.append(lastYGroup);
                }
                lastYGroup.clear();
            }

            lastY = y;
            lastYGroup.append(y);
        }

        if (!lastYGroup.isEmpty()) {
            nys.append(lastYGroup);
        }

        for (const QVector<float>& gys: nys) {
            QVector<QLineF> hls;

//            qDebug() << gys;

            for (const QLineF& hl: hlines) {
                if (gys.contains(hl.p1().y()) || gys.contains(hl.p2().y())) {
//                    qDebug() << hl.p1().y() << hl.p2().y();

                    hls.append(hl);
                }
            }

            als.append(hls);

            qDebug() << hls.length();

            float firstX = 100000.0;
            float firstY = 100000.0;

            float lastX = -100000.0;
            float lastY = -100000.0;

            vector<Point> points;
            for (const QLineF& hl: qAsConst(hls)) {
                points.push_back(Point(hl.p1().x(), hl.p1().y()));
                points.push_back(Point(hl.p2().x(), hl.p2().y()));
//                qDebug() << hl.p1() << hl.p2();

                if (firstX > hl.p1().x()) {
                    firstX = hl.p1().x();
                    firstY = hl.p1().y();
                }

                if (lastX < hl.p2().x()) {
                    lastX = hl.p2().x();
                    lastY = hl.p2().y();
                }
            }

//            for (auto point: points) {
//                qDebug() << point.x << point.y;
//            }

            Vec4f lineInfo;
            fitLine(points, lineInfo, DIST_HUBER, 0, 1e-2, 1e-2);

            //
//            QVector2D v(lineInfo[0], lineInfo[1]);
            QPointF p(lineInfo[2], lineInfo[3]);

//            float px = p.x();
            float py = p.y();

//            float fy = firstY;
//            float ly = lastY;

//            float fx = -(v.x()*(py-fy))/v.y() + px;
//            float lx =  (v.x()*(ly-py))/v.y() + px;

            float fx = firstX;
            float lx = lastX;

            alines.push_back(Vec4f(fx, py, lx, py));
        }
    }

    // V
    if (1) {
        QSet<float> xs;

        for (const QLineF& vl: vlines) {
            xs.insert(vl.p1().x());
            xs.insert(vl.p2().x());
        }

        QList<float> list = xs.toList();
        qSort(list);

        QVector<QVector<float>> nxs;

        int threshold = 20;
        float lastX = -100000.;
        QVector<float> lastXGroup;
        for(int i = 0; i < list.length(); ++i) {
            float x = list.at(i);
            if (x - lastX > threshold) {
                if (!lastXGroup.isEmpty()) {
                    nxs.append(lastXGroup);
                }
                lastXGroup.clear();
            }

            lastX = x;
            lastXGroup.append(x);
        }

        if (!lastXGroup.isEmpty()) {
            nxs.append(lastXGroup);
        }

        qDebug() << vlines.length();

        for (const QVector<float>& gxs: nxs) {
            QVector<QLineF> vls;

//            qDebug() << gxs;

            for (const QLineF& vl: vlines) {
                if (gxs.contains(vl.p1().x()) || gxs.contains(vl.p2().x())) {
//                    qDebug() << vl.p1().x() << vl.p2().x();

                    vls.append(vl);
                }
            }

            als.append(vls);

            qDebug() << vls.length();

            float firstX = 100000.0;
            float firstY = 100000.0;

            float lastX = -100000.0;
            float lastY = -100000.0;

            vector<Point> points;
            for (const QLineF& vl: qAsConst(vls)) {
                points.push_back(Point(vl.p1().x(), vl.p1().y()));
                points.push_back(Point(vl.p2().x(), vl.p2().y()));
//                qDebug() << hl.p1() << hl.p2();

                if (firstY > vl.p1().y()) {
                    firstX = vl.p1().x();
                    firstY = vl.p1().y();
                }

                if (lastY < vl.p2().y()) {
                    lastX = vl.p2().x();
                    lastY = vl.p2().y();
                }
            }

//            for (auto point: points) {
//                qDebug() << point.x << point.y;
//            }

            Vec4f lineInfo;
            fitLine(points, lineInfo, DIST_HUBER, 0, 1e-2, 1e-2);

            //
//            QVector2D v(lineInfo[0], lineInfo[1]);
            QPointF p(lineInfo[2], lineInfo[3]);

            float px = p.x();
//            float py = p.y();

//            float fx = firstX;
//            float lx = lastX;

//            float fy = -(v.y()*(px-fx))/v.x() + py;
//            float ly =  (v.y()*(lx-px))/v.x() + py;

            float fy = firstY;
            float ly = lastY;

            alines.push_back(Vec4f(px, fy, px, ly));
        }
    }

    toVariantList(als, mStep2);
    toVariantList(alines, mStep3);
//    */

    return true;
}


