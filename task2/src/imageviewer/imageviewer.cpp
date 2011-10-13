#include <QtGui>

#include "imageviewer.h"
#include <iostream>
#include <string.h>

ImageViewer::ImageViewer()
	: reader()
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
    statusFile = false;

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

	pathImage = new char [strlen(fileName.toAscii()) + 1];
	strcpy(pathImage, fileName.toAscii());
    }
    detectAct->setEnabled(statusImage && statusFile);
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

	pathFileModel = new char [strlen(fileName.toAscii()) + 1];
	strcpy(pathFileModel, fileName.toAscii());
	statusFile = true;

    detectAct->setEnabled(statusImage && statusFile);
}

void ImageViewer::detect()
{
	loadModelFromFile();

	QImage img(pathImage);

	reader.setInstancesNumber(0);
	for (int x0 = 0; x0 < img.width() - X_PIXEL; x0 += STEP_X_DETECTING) {
		reader.processInstance(x0, 0, x0 + X_PIXEL, Y_PIXEL, img);
		reader.incInstancesNumber();
	}

	classify(img);
}

void ImageViewer::loadModelFromFile()
{
	if((modelPedestrian = load_model(pathFileModel))==0)
	{
	  std:: cerr << "can't load model from file " << std::endl;
	  exit(1);
	}
}

void ImageViewer::classify(QImage &img)
{
	struct feature_node* x = Malloc(struct feature_node, NUM_FEATURES+1);
	x[NUM_FEATURES].index = -1;  // -1 marks the end of list
	std::vector<double> vPredictValues;
	double prob_estimates[1];

	for (size_t i = 0; i < reader.getInstancesNumber(); i++) {
		for (int j = 0; j < NUM_FEATURES; j++) {
			x[j].index = 1+j;	// 1-based feature number
			x[j].value = reader.instancesFeatures[i][j];
		}
		int predict_label = predict(modelPedestrian, x);
		if (predict_label == 1) {
			predict_values(modelPedestrian, x, prob_estimates);
		} else {
			prob_estimates[0] = 0;
		}
		vPredictValues.push_back(prob_estimates[0]);
	}
	suppression(img, vPredictValues);
	reader.instancesFeatures.clear();
	free(x);
}

void ImageViewer::detectedPedestrian(int i, QImage &img)
{
	for(int y = 0; y < img.height(); y++) {
		img.setPixel(STEP_X_DETECTING * i, y, qRgb(0,255,0));
	}
	for(int y = 0; y < img.height(); y++) {
		img.setPixel(STEP_X_DETECTING * i + X_PIXEL, y, qRgb(0,255,0));
	}
	imageLabel->setPixmap(QPixmap::fromImage(img));
	std::cout<< "Found pedestrian at " << STEP_X_DETECTING * i << std::endl;
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


void ImageViewer::suppression(QImage &img,  std::vector<double> &vect)
{
	double max = vect[0];
	int max_index = 0;

	findMaximum(vect, &max, &max_index);
	while (max != 0) {
		detectedPedestrian(max_index, img);
		vect[max_index] = 0;
		nullInArea(vect, max_index);
		max = vect[0];
		max_index = 0;
		findMaximum(vect, &max, &max_index);
	}
}

void ImageViewer::findMaximum(std::vector<double> &vect, double *max, int *max_index)
{
	for(uint i = 0; i < vect.size(); i++) {
		if (vect[i] > *max) {
			*max = vect[i];
			*max_index = i;
		}
	}
}


void ImageViewer::nullInArea(std::vector<double> &vect, int index)
{
	int localArea = X_PIXEL / (2 * STEP_X_DETECTING);
	for(int i = - localArea; i <= localArea;  i++) {
		if ((index + i >= 0) && (index + i < (int)vect.size())) {
			vect[index + i] = 0;
		}
	}
}
