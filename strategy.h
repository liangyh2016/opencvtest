#ifndef STRATEGY_H
#define STRATEGY_H

#include "tool.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <QObject>

class Strategy : public QObject
{
    Q_OBJECT
public:
    explicit Strategy(QObject *parent = nullptr);

public:
    virtual void parse(const std::vector<cv::Vec4f>& src) = 0;

protected:
    std::vector<cv::Vec4f> mSrcLines;
    std::vector<cv::Vec4f> mMidLines;
    std::vector<cv::Vec4f> mDstLines;

private:
    friend QVariantMap Tool::getResult();
};

#endif // STRATEGY_H
