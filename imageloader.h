#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include <QModelIndex>

class DImage;
class ImageExplorer;

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(QObject* parent = 0);
    ImageLoader(ImageExplorer* explorer, QObject* parent = 0);

    DImage* get();

signals:
    void loaded(DImage* dimage);

public slots:
    void load(QString path);
    void load(QModelIndex index);

private:
    ImageExplorer* m_explorer;
    DImage* m_dimage;
};

#endif // IMAGELOADER_H
