#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QVariantList>

namespace cv {
    class Mat;
}

class Tool : public QObject
{
    Q_OBJECT
public:
    explicit Tool(QObject *parent = nullptr);

    Q_PROPERTY(QVariantList step1 READ step1 WRITE setStep1 NOTIFY step1Changed)
    Q_PROPERTY(QVariantList step2 READ step2 WRITE setStep2 NOTIFY step2Changed)
    Q_PROPERTY(QVariantList step3 READ step3 WRITE setStep3 NOTIFY step3Changed)

public slots:
    bool parse(const QString& path);

private:
    cv::Mat preprocess(const cv::Mat& src);

    void classifyBy();

private:
    QVariantList step1() {
        return mStep1;
    }

    QVariantList step2() {
        return mStep2;
    }

    QVariantList step3() {
        return mStep3;
    }

    void setStep1(const QVariantList& v) {
        mStep1 = v;
    }

    void setStep2(const QVariantList& v) {
        mStep2 = v;
    }

    void setStep3(const QVariantList& v) {
        mStep3 = v;
    }

signals:
    void step1Changed();
    void step2Changed();
    void step3Changed();

private:
    QVariantList mStep1;
    QVariantList mStep2;
    QVariantList mStep3;
};

#endif // TOOL_H
