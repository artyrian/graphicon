#include <QtGui>

#include "imageviewer.h"


ImageViewer::ImageViewer()
{
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    setCentralWidget(scrollArea);

    createActions();
    createMenus();

    statusImage = false;

    setWindowTitle(tr("Image Viewer"));
    resize(400, 250);
}


void ImageViewer::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
				    tr("Open Image"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        QImage image(fileName);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Image Viewer"),
                                     tr("Cannot load %1.").arg(fileName));
            return;
        }
        imageLabel->setPixmap(QPixmap::fromImage(image));
	imageLabel->adjustSize();
	statusImage = true;
    }
}

void ImageViewer::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
				    tr("Open File"), QDir::currentPath());
    if (fileName.isEmpty()) {
	    QMessageBox::information(this, tr("Image Viewer"),
				     tr("Cannot load %1.").arg(fileName));
	    return;
    }

    detectAct->setEnabled(true && statusImage);
}

void ImageViewer::detect()
{
// start Predict for one image;
// need struct model and path to dir with image
//    Test detect(pathImage, pathFileModel, 1);
//    detect.imagesClassification();

}

void ImageViewer::createActions()
{
    openImageAct = new QAction(tr("&Open image..."), this);
    openImageAct->setShortcut(tr("Ctrl+I"));
    connect(openImageAct, SIGNAL(triggered()), this, SLOT(openImage()));

    openFileAct = new QAction(tr("&Open file..."), this);
    openFileAct->setShortcut(tr("Ctrl+F"));
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFile()));

    detectAct = new QAction(tr("&Detect"), this);
    detectAct->setShortcut(tr("Ctrl+D"));
    connect(detectAct, SIGNAL(triggered()), this, SLOT(detect()));
    detectAct->setDisabled(true);;

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void ImageViewer::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openImageAct);
    fileMenu->addAction(openFileAct);
    fileMenu->addAction(detectAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    menuBar()->addMenu(fileMenu);
}

void ImageViewer::updateActions()
{
}
