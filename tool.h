#ifndef TOOL_H
#define TOOL_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <QObject>
#include <QVariantList>

class Strategy;
class Tool : public QObject
{
    Q_OBJECT
public:
    explicit Tool(QObject *parent = nullptr);

public slots:
    bool process(const QString& path);

    QVariantMap getResult();

private:
    std::vector<cv::Vec4f> preprocess(const cv::Mat& src);

private:
    Strategy* mStrategy = nullptr;
};

#endif // TOOL_H
