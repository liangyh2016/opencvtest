#ifndef STRATEGY2_H
#define STRATEGY2_H

#include "strategy.h"

#include <QObject>
#include <QVector>

class Strategy2 : public Strategy
{
    Q_OBJECT
public:
    explicit Strategy2();

public:
    void parse(const std::vector<cv::Vec4f>& src);

private:
    void classifyByHV(const std::vector<cv::Vec4f>& lines, QVector<QLineF>& allHLines, QVector<QLineF>& allVLines);

    void parseHLines(const QVector<QLineF>& lines);
    void parseVLines(const QVector<QLineF>& lines);

    void closeHLineByVLine(const QVector<QLineF>& hlines, const QVector<QLineF>& vlines);
    void closeVLineByHLine(const QVector<QLineF>& hlines, const QVector<QLineF>& vlines);

    void regularLines(QVector<QLineF>& hlines, QVector<QLineF>& vlines);

private:
    int mLengthThreshold = 20;
    int mThickGapThreshold = 15;
    int mLengthGapThreshold = 30;
};

#endif // STRATEGY2_H
