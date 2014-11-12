#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include <QModelIndex>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class ImageExplorer;


struct ImageInfo {
    QString path;
    int cols;
    int rows;
    int channels;
    QString depth;
};

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(QObject* parent = 0);
    ImageLoader(ImageExplorer* explorer, QObject* parent = 0);

    cv::Mat get();

    ImageInfo* getInfo();

signals:
    void loaded(cv::Mat);

public slots:
    void load(QString path);
    void load(QModelIndex index);

private:
    ImageExplorer* m_explorer;
    cv::Mat m_matImage;
    ImageInfo* m_info;

    void updateInfo(QString path);
};

#endif // IMAGELOADER_H
