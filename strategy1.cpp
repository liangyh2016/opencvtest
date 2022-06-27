#include "strategy1.h"

#include <QVector2D>
#include <QLineF>
#include <QMap>

#include <QDebug>

using namespace cv;
using namespace std;

struct LineGroup {
    float mFirst = 0;
    float mLast = 0;
    QVector<QLineF> mLines;

    LineGroup() {
        mFirst = 0;
        mLast = 0;
        mLines = {};
    }

    LineGroup(float first, float last, const QLineF& line) {
        if (first > last) {
            qSwap(first, last);
        }

        mFirst = first;
        mLast = last;
        mLines = {line};
    }

    bool merge(const LineGroup& other, int gapThreshold) {
        if ((mFirst <= other.mFirst && other.mFirst <= mLast) ||
            (mFirst <= other.mLast && other.mLast <= mLast) ||
            (other.mFirst <= mFirst && mFirst <= other.mLast) ||
            (other.mFirst <= mLast && mLast <= other.mLast) ||
            (qAbs(mFirst - other.mLast) <= gapThreshold) ||
            (qAbs(mLast - other.mFirst) <= gapThreshold)) {
            mFirst = qMin(mFirst, other.mFirst);
            mLast = qMax(mLast, other.mLast);
            mLines.append(other.mLines);
            return true;
        }
        return false;
    }
};

inline QDebug operator<< (QDebug& os, const LineGroup& lp) {
    return os << lp.mFirst << " " << lp.mLast << " " << lp.mLines.length();
}

Strategy1::Strategy1()
{

}

void Strategy1::parse(const std::vector<cv::Vec4f> &src)
{
    mSrcLines = std::move(src);

    {
        QVector<QLineF> allHLines;
        QVector<QLineF> allVLines;

        classifyByHV(mSrcLines, allHLines, allVLines);

        mDstLines.clear();

        parseHLines(allHLines, false);
        parseVLines(allVLines, false);
    }

    // 二次处理
    {
        QVector<QLineF> allHLines;
        QVector<QLineF> allVLines;

        classifyByHV(mDstLines, allHLines, allVLines);

        mDstLines.clear();

        parseHLines(allHLines, true);
        parseVLines(allVLines, true);
    }

    mMidLines = std::move(mDstLines);

    {
        QVector<QLineF> allHLines;
        QVector<QLineF> allVLines;

        classifyByHV(mMidLines, allHLines, allVLines);

        mDstLines.clear();

        closeHLineByVLine(allHLines, allVLines);
        closeVLineByHLine(allHLines, allVLines);
    }
}

void Strategy1::classifyByHV(const vector<Vec4f>& lines, QVector<QLineF> &allHLines, QVector<QLineF> &allVLines)
{
    for (const Vec4f& line: lines) {
        QLineF l(line[0], line[1], line[2], line[3]);

        QVector2D v(l.p2() - l.p1());
        QVector2D nv = v.normalized();

        if (qAbs(nv.x()) >= 0.707106781187) {
            allHLines.push_back(l);
        }
        else if (qAbs(nv.y()) >= 0.707106781187) {
            allVLines.push_back(l);
        }
        else {
            qDebug() << "error" << nv;
        }
    }
}

void Strategy1::parseHLines(const QVector<QLineF> &hlines, bool regularLine)
{
    QVector<QLineF> lines = std::move(hlines);
    qSort(lines.begin(), lines.end(), [](const QLineF& l1, const QLineF& l2)->bool {
        return (l1.p1().x() + l1.p1().y()) > (l2.p1().x() + l2.p1().y());
    });

    QVector<LineGroup> lineGroupsByY;

    for (const QLineF& line: lines) {
        LineGroup lp(line.p1().y(), line.p2().y(), line);
        bool isMerge = false;
        for (int i = 0; i < lineGroupsByY.length(); i++) {
            LineGroup& lineGroup = lineGroupsByY[i];
            if (lineGroup.merge(lp, mThickGapThreshold)) {
                isMerge = true;
                break;
            }
        }

        if (!isMerge) {
            lineGroupsByY.push_back(lp);
        }
    }

    qDebug() << lineGroupsByY.length() << lineGroupsByY;

    QVector<LineGroup> lineGroupsByXY;

    for (const LineGroup& lineGroup: qAsConst(lineGroupsByY)) {
        QVector<QLineF> glines = lineGroup.mLines;
        qSort(glines.begin(), glines.end(), [](const QLineF& l1, const QLineF& l2)->bool {
            return l1.p1().x() > l2.p1().x();
        });

        QVector<LineGroup> lineGroupsByX;

        for (const QLineF& line: glines) {
            LineGroup lp(line.p1().x(), line.p2().x(), line);
            bool isMerge = false;
            for (int i = 0; i < lineGroupsByX.length(); i++) {
                LineGroup& lineGroup = lineGroupsByX[i];
                if (lineGroup.merge(lp, mLengthGapThreshold)) {
                    isMerge = true;
                    break;
                }
            }

            if (!isMerge) {
                lineGroupsByX.push_back(lp);
            }
        }

        lineGroupsByXY.append(lineGroupsByX);
    }

    qDebug() << lineGroupsByXY.length() << lineGroupsByXY;

    for (const LineGroup& lineGroup: lineGroupsByXY) {
        float firstX = 100000.0;
        float lastX = -100000.0;

        vector<Point> points;
        for (const QLineF& line: lineGroup.mLines) {
            float x1 = line.p1().x();
            float y1 = line.p1().y();
            float x2 = line.p2().x();
            float y2 = line.p2().y();

            points.push_back(Point(x1, y1));
            points.push_back(Point(x2, y2));

            float minX = qMin(x1, x2);
            float maxX = qMax(x1, x2);

            if (firstX > minX) {
                firstX = minX;
            }

            if (lastX < maxX) {
                lastX = maxX;
            }
        }

        if (lastX - firstX < mLengthThreshold) {
            continue;
        }

        Vec4f lineInfo;
        fitLine(points, lineInfo, DIST_HUBER, 0, 1e-2, 1e-2);

        QPointF linePoint(lineInfo[2], lineInfo[3]);
        QVector2D lineVector(lineInfo[0], lineInfo[1]);

        float fx = firstX;
        float lx = lastX;
        float fy = -(lineVector.y()*(linePoint.x()-fx))/lineVector.x() + linePoint.y();
        float ly =  (lineVector.y()*(lx-linePoint.x()))/lineVector.x() + linePoint.y();

        if (regularLine) {
            fy = linePoint.y();
            ly = linePoint.y();
        }

        mDstLines.push_back(Vec4f(fx, fy, lx, ly));
    }
}

void Strategy1::parseVLines(const QVector<QLineF> &vlines, bool regularLine)
{
    QVector<QLineF> lines = std::move(vlines);
    qSort(lines.begin(), lines.end(), [](const QLineF& l1, const QLineF& l2)->bool {
        return (l1.p1().x() + l1.p1().y()) > (l2.p1().x() + l2.p1().y());
    });

    QVector<LineGroup> lineGroupsByX;

    for (const QLineF& line: lines) {
        LineGroup lp(line.p1().x(), line.p2().x(), line);
        bool isMerge = false;
        for (int i = 0; i < lineGroupsByX.length(); i++) {
            LineGroup& lineGroup = lineGroupsByX[i];
            if (lineGroup.merge(lp, mThickGapThreshold)) {
                isMerge = true;
                break;
            }
        }

        if (!isMerge) {
            lineGroupsByX.push_back(lp);
        }
    }

    qDebug() << lineGroupsByX.length() << lineGroupsByX;

    QVector<LineGroup> lineGroupsByXY;

    for (const LineGroup& lineGroup: qAsConst(lineGroupsByX)) {
        QVector<QLineF> glines = lineGroup.mLines;
        qSort(glines.begin(), glines.end(), [](const QLineF& l1, const QLineF& l2)->bool {
            return l1.p1().y() > l2.p1().y();
        });

        QVector<LineGroup> lineGroupsByY;

        for (const QLineF& line: glines) {
            LineGroup lp(line.p1().y(), line.p2().y(), line);
            bool isMerge = false;
            for (int i = 0; i < lineGroupsByY.length(); i++) {
                LineGroup& lineGroup = lineGroupsByY[i];
                if (lineGroup.merge(lp, mLengthGapThreshold)) {
                    isMerge = true;
                    break;
                }
            }

            if (!isMerge) {
                lineGroupsByY.push_back(lp);
            }
        }

        lineGroupsByXY.append(lineGroupsByY);
    }

    qDebug() << lineGroupsByXY.length() << lineGroupsByXY;

    for (const LineGroup& lineGroup: lineGroupsByXY) {
        float firstY = 100000.0;
        float lastY = -100000.0;

        vector<Point> points;
        for (const QLineF& line: lineGroup.mLines) {
            float x1 = line.p1().x();
            float y1 = line.p1().y();
            float x2 = line.p2().x();
            float y2 = line.p2().y();

            points.push_back(Point(x1, y1));
            points.push_back(Point(x2, y2));

            float minY = qMin(y1, y2);
            float maxY = qMax(y1, y2);

            if (firstY > minY) {
                firstY = minY;
            }

            if (lastY < maxY) {
                lastY = maxY;
            }
        }

        if (lastY - firstY < mLengthThreshold) {
            continue;
        }

        Vec4f lineInfo;
        fitLine(points, lineInfo, DIST_HUBER, 0, 1e-2, 1e-2);

        QPointF linePoint(lineInfo[2], lineInfo[3]);
        QVector2D lineVector(lineInfo[0], lineInfo[1]);

        float fy = firstY;
        float ly = lastY;
        float fx = -(lineVector.x()*(linePoint.y()-fy))/lineVector.y() + linePoint.x();
        float lx =  (lineVector.x()*(ly-linePoint.y()))/lineVector.y() + linePoint.x();

        if (regularLine) {
            fx = linePoint.x();
            lx = linePoint.x();
        }

        mDstLines.push_back(Vec4f(fx, fy, lx, ly));
    }
}

void Strategy1::closeHLineByVLine(const QVector<QLineF> &hlines, const QVector<QLineF> &vlines)
{
    for (const QLineF& hline: hlines) {

        float firstExtendLength = 100000.0;
        QPointF firstExtendPoint = hline.p1();

        float lastExtendLength = 100000.0;
        QPointF lastExtendPoint = hline.p2();

        for (const QLineF& vline: vlines) {
            QPointF ip;
            auto type = hline.intersect(vline, &ip);
            if (type != QLineF::NoIntersection) {
                float vDistance = qMin(QVector2D(vline.p1()-ip).length(), QVector2D(vline.p2()-ip).length());
                float vDistanceRefer = vDistance/5;

                {
                    QVector2D fv(hline.p1()-ip);
                    float extendLength = fv.length() + vDistanceRefer;
                    if (extendLength < firstExtendLength) {
                        firstExtendPoint = ip;
                        firstExtendLength = extendLength;
                    }
                }

                {
                    QVector2D lv(hline.p2()-ip);
                    float extendLength = lv.length() + vDistanceRefer;
                    if (extendLength < lastExtendLength) {
                        lastExtendPoint = ip;
                        lastExtendLength = extendLength;
                    }
                }
            }
        }

        if (firstExtendPoint == lastExtendPoint) {
            continue;
        }

        float fx = firstExtendPoint.x();
        float fy = firstExtendPoint.y();
        float lx = lastExtendPoint.x();
        float ly = lastExtendPoint.y();

        mDstLines.push_back(Vec4f(fx, fy, lx, ly));
    }
}

void Strategy1::closeVLineByHLine(const QVector<QLineF> &hlines, const QVector<QLineF> &vlines)
{
    for (const QLineF& vline: vlines) {

        float firstExtendLength = 100000.0;
        QPointF firstExtendPoint = vline.p1();

        float lastExtendLength = 100000.0;
        QPointF lastExtendPoint = vline.p2();

        for (const QLineF& hline: hlines) {
            QPointF ip;
            auto type = vline.intersect(hline, &ip);
            if (type != QLineF::NoIntersection) {
                float hDistance = qMin(QVector2D(hline.p1()-ip).length(), QVector2D(hline.p2()-ip).length());
                float hDistanceRefer = hDistance/5;

                {
                    QVector2D fv(vline.p1()-ip);
                    float extendLength = fv.length() + hDistanceRefer;
                    if (extendLength < firstExtendLength) {
                        firstExtendPoint = ip;
                        firstExtendLength = extendLength;
                    }
                }

                {
                    QVector2D lv(vline.p2()-ip);
                    float extendLength = lv.length() + hDistanceRefer;
                    if (extendLength < lastExtendLength) {
                        lastExtendPoint = ip;
                        lastExtendLength = extendLength;
                    }
                }
            }
        }

        if (firstExtendPoint == lastExtendPoint) {
            continue;
        }

        float fx = firstExtendPoint.x();
        float fy = firstExtendPoint.y();
        float lx = lastExtendPoint.x();
        float ly = lastExtendPoint.y();

        mDstLines.push_back(Vec4f(fx, fy, lx, ly));
    }
}
