#include "strategy2.h"

#include <QVector2D>
#include <QLineF>
#include <QMap>
#include <QtMath>

#include <QDebug>

using namespace cv;
using namespace std;

struct LineGroup {
    int mThickGapThreshold = 0;
    int mLengthGapThreshold = 0;
    QVector<QLineF> mLines = {};

    LineGroup(const QLineF& line, int lengthGapThreshold, int thickGapThreshold) {
        mLines = {line};
        mThickGapThreshold = thickGapThreshold;
        mLengthGapThreshold = lengthGapThreshold;
    }

    bool merge(const QLineF& line) {
        QPointF a1 = line.p1();
        QPointF a2 = line.p2();

        for (const QLineF& xline: mLines) {
            QPointF b1 = xline.p1();
            QPointF b2 = xline.p2();

            bool isNearest = false;

            if (position(a1, a2, b1) * position(a1, a2, b2) <= 0 && position(b1, b2, a1) * position(b1, b2, a2) <= 0) { // 相交
                isNearest = true;
            }
            else if (nearestSegment(a1, a2, b1) || nearestSegment(a1, a2, b2) || nearestSegment(b1, b2, a1) || nearestSegment(b1, b2, a2)) { // 距离很近
                isNearest = true;
            }

            if (isNearest) {
                QVector2D v1(a1 - a2);
                QVector2D v2(b1 - b2);

                v1.normalize();
                v2.normalize();

                float angle = qAcos(QVector2D::dotProduct(v1, v2)) / M_PI *180;
                if (qAbs(angle - 0) < 20 || qAbs(180 - angle) < 20) {
                    mLines.push_back(line);
                    return true;
                }
            }
        }

        return false;
    }

private:
    qreal dot(const QPointF& a, const QPointF& b, const QPointF& p) {
        QVector2D ab(b.x() - a.x(), b.y() - a.y());
        QVector2D ap(p.x() - a.x(), p.y() - a.y());
        return ab.x()*ap.x() + ab.y()*ap.y();
    }

    qreal cross(const QPointF& a, const QPointF& b, const QPointF& p) {
        QVector2D ab(b.x() - a.x(), b.y() - a.y());
        QVector2D ap(p.x() - a.x(), p.y() - a.y());
        return ab.x()*ap.y() - ab.y()*ap.x();
    }

    qreal distance(const QPointF& a, const QPointF& b) {
        return (a.x() - b.x()) * (a.x() - b.x()) + (a.y() - b.y()) * (a.y() - b.y());
    }

    int position(const QPointF& a, const QPointF& b, const QPointF& p) {
        if (cross(a, b, p) < 0) {
            return -1; // 逆时针
        }
        else if (cross(a, b, p) > 0) {
            return 1; // 顺时针
        }
        else if (dot(a, b, p) < 0) {
            return -2; // 反延长线
        }
        else if (dot(a, b, p) >= 0) {
            if (distance(a, b) < distance(a, p)) {
                return 2; // 延长线
            }
            return 0; // 在线上
        }

        qDebug() << "error";
        return 10;
    }

    bool nearestSegment(const QPointF& a, const QPointF& b, const QPointF& p) {
        qreal r = ((p.x() - a.x()) * (b.x() - a.x()) + (p.y() - a.y()) * (b.y() - a.y())) / distance(a, b);
        if (r <= 0) {
            qreal dist = qSqrt(distance(a, p));
            if (dist < mLengthGapThreshold) {
                return true;
            }
        }
        else if (r >= 1) {
            qreal dist = qSqrt(distance(b, p));
            if (dist < mLengthGapThreshold) {
                return true;
            }
        }
        else {
            qreal ac = r * qSqrt(distance(a, b));
            qreal dist = qSqrt(distance(a, p) - ac * ac);
            if (dist < mThickGapThreshold) {
                return true;
            }
        }

        return false;
    }
};

inline QDebug operator<< (QDebug& os, const LineGroup& lp) {
    return os << lp.mLines.length();
}

Strategy2::Strategy2()
{
}

void Strategy2::parse(const std::vector<cv::Vec4f> &src)
{
    mSrcLines = std::move(src);

    {
        QVector<QLineF> allHLines;
        QVector<QLineF> allVLines;

        classifyByHV(mSrcLines, allHLines, allVLines);

        mMidLines.clear();
        mDstLines.clear();

        parseHLines(allHLines);
        parseVLines(allVLines);
    }

    {
        vector<Vec4f> tempLines = std::move(mDstLines);

        QVector<QLineF> allHLines;
        QVector<QLineF> allVLines;

        classifyByHV(tempLines, allHLines, allVLines);

        mDstLines.clear();

        closeHLineByVLine(allHLines, allVLines);
        closeVLineByHLine(allHLines, allVLines);
    }
}

void Strategy2::classifyByHV(const vector<Vec4f>& lines, QVector<QLineF> &allHLines, QVector<QLineF> &allVLines)
{
    for (const Vec4f& line: lines) {
        QLineF l(line[0], line[1], line[2], line[3]);

        QVector2D v(l.p2() - l.p1());
        QVector2D nv = v.normalized();

        if (qAbs(nv.x()) >= 0.707107) {
            allHLines.push_back(l);
        }
        else if (qAbs(nv.y()) >= 0.707107) {
            allVLines.push_back(l);
        }
        else {
            qDebug() << "error" << nv;
        }
    }
}

static std::vector<cv::Vec4f> toVec4f(const QVector<QLineF> &lines) {
    vector<Vec4f> vec4fs;
    for (const QLineF& line: lines) {
        vec4fs.push_back(Vec4f(line.p1().x(), line.p1().y(), line.p2().x(), line.p2().y()));
    }
    return vec4fs;
}

void Strategy2::parseHLines(const QVector<QLineF> &hlines)
{
    QVector<QLineF> lines = std::move(hlines);
    qSort(lines.begin(), lines.end(), [](const QLineF& l1, const QLineF& l2)->bool {
        return qMin(l1.p1().x(), l1.p2().x()) < qMin(l2.p1().x(), l2.p2().x());
    });

    QList<LineGroup> lineGroups;

    for (const QLineF& line: lines) {
        bool isMerge = false;
        for (int i = 0; i < lineGroups.length(); i++) {
            LineGroup& lineGroup = lineGroups[i];
            if (lineGroup.merge(line)) {
                isMerge = true;
                break;
            }
        }

        if (!isMerge) {
            LineGroup lp(line, mLengthGapThreshold, mThickGapThreshold);
            lineGroups.push_back(lp);
        }
    }

//    qDebug() << lineGroups.length() << lineGroups;
//    for (const LineGroup& lineGroup: lineGroups) {
//        mMidLines.push_back(toVec4f(lineGroup.mLines));
//    }

    for (const LineGroup& lineGroup: lineGroups) {
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

        mDstLines.push_back(Vec4f(fx, fy, lx, ly));
    }
}

void Strategy2::parseVLines(const QVector<QLineF> &vlines)
{
    QVector<QLineF> lines = std::move(vlines);
    qSort(lines.begin(), lines.end(), [](const QLineF& l1, const QLineF& l2)->bool {
        return qMin(l1.p1().y(), l1.p2().y()) < qMin(l2.p1().y(), l2.p2().y());
    });

    QList<LineGroup> lineGroups;

    for (const QLineF& line: lines) {
        bool isMerge = false;
        for (int i = 0; i < lineGroups.length(); i++) {
            LineGroup& lineGroup = lineGroups[i];
            if (lineGroup.merge(line)) {
                isMerge = true;
                break;
            }
        }

        if (!isMerge) {
            LineGroup lp(line, mLengthGapThreshold, mThickGapThreshold);
            lineGroups.push_back(lp);
        }
    }

//    qDebug() << lineGroups.length() << lineGroups;
//    for (const LineGroup& lineGroup: lineGroups) {
//        mMidLines.push_back(toVec4f(lineGroup.mLines));
//    }

    for (const LineGroup& lineGroup: lineGroups) {
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

        mDstLines.push_back(Vec4f(fx, fy, lx, ly));
    }
}

void Strategy2::closeHLineByVLine(const QVector<QLineF> &hlines, const QVector<QLineF> &vlines)
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
                float vDistanceRefer = vDistance/10;

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

void Strategy2::closeVLineByHLine(const QVector<QLineF> &hlines, const QVector<QLineF> &vlines)
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
                float hDistanceRefer = hDistance/10;

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
