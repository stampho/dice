#ifndef IMAGEEXPLORER_H
#define IMAGEEXPLORER_H

#include <QObject>
#include <QFileSystemModel>

namespace Ui {
class DiceWindow;
}

class ImageExplorer : public QObject
{
    Q_OBJECT

public:
    explicit ImageExplorer(QObject* parent = 0);
    ImageExplorer(Ui::DiceWindow* ui, QObject* parent = 0);

    QString getSelectedFile(QModelIndex index);

signals:

public slots:
    void dirSelected(QModelIndex);

private:
    Ui::DiceWindow* m_ui;
    QFileSystemModel* m_dirModel;
    QFileSystemModel* m_fileModel;
    QStringList m_imageFilter;

    void initDirModel();
    void initFileModel();
};

#endif // IMAGEEXPLORER_H
