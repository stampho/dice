#include "dicewindow.h"
#include "ui_dicewindow.h"
#include "imageexplorer.h"
#include "imageviewer.h"

DiceWindow::DiceWindow(QWidget* parent) :
    QMainWindow(parent),
    m_ui(new Ui::DiceWindow)
{
    m_ui->setupUi(this);

    ImageExplorer* imageExplorer = new ImageExplorer(m_ui);
    ImageViewer* imageViewer = new ImageViewer(m_ui, imageExplorer);

    connect(m_ui->dirBrowser, SIGNAL(clicked(QModelIndex)), imageExplorer, SLOT(dirSelected(QModelIndex)));
    connect(m_ui->fileBrowser, SIGNAL(clicked(QModelIndex)), imageViewer, SLOT(load(QModelIndex)));
}

DiceWindow::~DiceWindow()
{
    delete m_ui;
}
