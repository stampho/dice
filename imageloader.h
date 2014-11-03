#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include <QModelIndex>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class ImageExplorer;

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(QObject* parent = 0);
    ImageLoader(ImageExplorer* explorer, QObject* parent = 0);

    cv::Mat get();

signals:
    void loaded(cv::Mat dimage);

public slots:
    void load(QString path);
    void load(QModelIndex index);

private:
    ImageExplorer* m_explorer;
    cv::Mat m_matImage;
};

#endif // IMAGELOADER_H
