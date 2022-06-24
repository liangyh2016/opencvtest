#include "strategy.h"

#include <QVector2D>
#include <QLineF>
#include <QMap>

#include <QDebug>

using namespace cv;
using namespace std;

Strategy::Strategy(QObject *parent) : QObject(parent)
{

}

void Strategy::parse(const std::vector<cv::Vec4f> &src)
{
    clear();

    mSrcLines = std::move(src);

    QVector<QLineF> allHLines;
    QVector<QLineF> allVLines;

    classifyByHV(allHLines, allVLines);

    parseHLines(allHLines);
    parseVLines(allVLines);
}

void Strategy::clear()
{
    mSrcLines.clear();
    mMidLines.clear();
    mDstLines.clear();
}

void Strategy::classifyByHV(QVector<QLineF> &allHLines, QVector<QLineF> &allVLines)
{
    for (const Vec4f& line: mSrcLines) {
        QLineF l(line[0], line[1], line[2], line[3]);

        QVector2D v(l.p2() - l.p1());
        QVector2D nv = v.normalized();

        if (qAbs(nv.x()) >= 0.86602540378) {
            allHLines.push_back(l);
        }
        else if (qAbs(nv.y()) >= 0.86602540378) {
            allVLines.push_back(l);
        }
        else {
            qDebug() << "error" << nv;
        }
    }
}

void Strategy::parseHLines(const QVector<QLineF> &lines)
{
    QSet<float> ySet;

    for (const QLineF& line: lines) {
        ySet.insert(line.p1().y());
        ySet.insert(line.p2().y());
    }

    QList<float> ys = ySet.toList();
    qSort(ys);

    QVector<QVector<float>> yGroups;

    float lastY = -100000.;
    QVector<float> lastYGroup;
    for(int i = 0; i < ys.length(); ++i) {
        float y = ys.at(i);
        if (y - lastY > mThickGapThreshold) {
            if (!lastYGroup.isEmpty()) {
                yGroups.append(lastYGroup);
            }
            lastYGroup.clear();
        }

        lastY = y;
        lastYGroup.append(y);
    }

    if (!lastYGroup.isEmpty()) {
        yGroups.append(lastYGroup);
    }

    QVector<QVector<QLineF>> lineGroups;

    for (const QVector<float>& yGroup: yGroups) {
        QVector<QLineF> candidates;
        for (const QLineF& line: lines) {
            if (yGroup.contains(line.p1().y()) || yGroup.contains(line.p2().y())) {
                candidates.append(line);
            }
        }

        qSort(candidates.begin(), candidates.end(), [](const QLineF& l1, const QLineF& l2)->bool {
            return l1.p1().x() < l2.p1().x();
        });










    }



}

void Strategy::parseVLines(const QVector<QLineF> &lines)
{

}

































