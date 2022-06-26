#ifndef STRATEGY_H
#define STRATEGY_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "tool.h"

#include <QObject>
#include <QVector>

class Strategy : public QObject
{
    Q_OBJECT
public:
    explicit Strategy(QObject *parent = nullptr);

public:
    void parse(const std::vector<cv::Vec4f>& src);

private:
    void classifyByHV(const std::vector<cv::Vec4f>& lines, QVector<QLineF>& allHLines, QVector<QLineF>& allVLines);

    void parseHLines(const QVector<QLineF>& lines, bool regularLine = true);
    void parseVLines(const QVector<QLineF>& lines, bool regularLine = true);

    void closeHLineByVLine(const QVector<QLineF>& hlines, const QVector<QLineF>& vlines);
    void closeVLineByHLine(const QVector<QLineF>& hlines, const QVector<QLineF>& vlines);

private:
    std::vector<cv::Vec4f> mSrcLines;
    std::vector<cv::Vec4f> mMidLines;
    std::vector<cv::Vec4f> mDstLines;

private:
    int mLengthThreshold = 20;
    int mThickGapThreshold = 30;
    int mLengthGapThreshold = 100;

private:
    friend QVariantMap Tool::getResult();
};

#endif // STRATEGY_H
