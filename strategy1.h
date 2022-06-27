#ifndef STRATEGY1_H
#define STRATEGY1_H

#include "strategy.h"

#include <QObject>
#include <QVector>

class Strategy1 : public Strategy
{
    Q_OBJECT
public:
    explicit Strategy1();

public:
    void parse(const std::vector<cv::Vec4f>& src);

private:
    void classifyByHV(const std::vector<cv::Vec4f>& lines, QVector<QLineF>& allHLines, QVector<QLineF>& allVLines);

    void parseHLines(const QVector<QLineF>& lines, bool regularLine = true);
    void parseVLines(const QVector<QLineF>& lines, bool regularLine = true);

    void closeHLineByVLine(const QVector<QLineF>& hlines, const QVector<QLineF>& vlines);
    void closeVLineByHLine(const QVector<QLineF>& hlines, const QVector<QLineF>& vlines);

private:
    int mLengthThreshold = 20;
    int mThickGapThreshold = 15;
    int mLengthGapThreshold = 100;
};

#endif // STRATEGY1_H
