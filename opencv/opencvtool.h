#ifndef OPENCVTOOL_H
#define OPENCVTOOL_H

#include <QLineF>
#include <QObject>

namespace cv {
class Mat;
}

class OpenCVTool : public QObject
{
    Q_OBJECT
public:
    explicit OpenCVTool(QObject *parent = nullptr);

public slots:
    QList<QLineF> loadLinesFromImage(const QString& path);

private:
    cv::Mat preprocess(cv::Mat source);
};

#endif // OPENCVTOOL_H
