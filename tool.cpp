#include "tool.h"

#include "strategy1.h"
#include "strategy2.h"

using namespace cv;
using namespace std;

Tool::Tool(QObject *parent) : QObject(parent)
{
    mStrategy = new Strategy2();
}

std::vector<Vec4f> Tool::preprocess(const Mat &src)
{
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    Mat gauss;
    GaussianBlur(gray, gauss, {9, 9}, 0);

    Mat median;
    medianBlur(gauss, median, 3);

    Mat edges;
    Canny(median, edges, 50, 100);

    vector<Vec4f> lines;
    HoughLinesP(edges, lines, 1, CV_PI/180, 20, 5, 5);

    return lines;
}

bool Tool::process(const QString &path)
{
    Mat src = imread(path.toStdString());
    if (!src.data) {
        return false;
    }

    vector<Vec4f> lines = preprocess(src);

    mStrategy->parse(lines);

    return true;
}

static QVariantList toVariantList(const vector<Vec4f>& lines = {}) {
    QVariantList out;
    for (const Vec4f& line: lines) {
        out.append(QVariant(QVariantList{line[0], line[1], line[2], line[3]}));
    }
    return out;
}

static QVariantList toVariantList(const vector<vector<Vec4f>>& liness = {}) {
    QVariantList outs;
    for (const vector<Vec4f>& lines: liness) {
        outs.append(QVariant(toVariantList(lines)));
    }
    return outs;
}

QVariantMap Tool::getResult()
{
    QVariantMap result;
    result.insert("src", toVariantList(mStrategy->mSrcLines));
    result.insert("mid", toVariantList(mStrategy->mMidLines));
    result.insert("dst", toVariantList(mStrategy->mDstLines));
    return result;
}
