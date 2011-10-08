#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include "../src-core/Instances.hpp"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

//! [0]
class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    ImageViewer();

private slots:
    void openImage();
    void openFile();
    void detect();

private:
    void createActions();
    void createMenus();
    void updateActions();
    void loadModelFromFile();
    void classify(QImage &);
    void detectedPedestrian(int, QImage &);

    QScrollArea *scrollArea;
    QLabel *imageLabel;

	struct model* modelPedestrian;

    QAction *openImageAct;
    QAction *openFileAct;
    QAction *detectAct;
    QAction *exitAct;
    QMenu *fileMenu;
    bool statusImage;
    char *pathImage;
    char *pathFileModel;

    InstancesData reader;
};

#endif
