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
    void clear();

    void classifyByHV(QVector<QLineF>& allHLines, QVector<QLineF>& allVLines);

    void parseHLines(const QVector<QLineF>& lines);
    void parseVLines(const QVector<QLineF>& lines);

private:
    std::vector<cv::Vec4f> mSrcLines;
    std::vector<cv::Vec4f> mMidLines;
    std::vector<cv::Vec4f> mDstLines;

private:
    int mThickGapThreshold = 20;
    int mLengthGapThreshold = 100;

private:
    friend QVariantList Tool::getResult();
};

#endif // STRATEGY_H
