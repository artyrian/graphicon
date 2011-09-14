#include <QtGui>
#include "imageprocessing.h"
#include <QRgb>

// BT.709: Y = 0.2125·R + 0.7154·G + 0.0721·B
const double coeffRed = 0.2125;
const double coeffGreen = 0.7154;
const double coeffBlue = 0.0721;

ImageProcessing::ImageProcessing()
{
        image = new QImage;
        imageLabel = new QLabel;

        imageLabel->setBackgroundRole(QPalette::Dark);
        imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        imageLabel->setScaledContents(true);

        scrollArea = new QScrollArea;
        scrollArea->setBackgroundRole(QPalette::Dark);
        scrollArea->setWidget(imageLabel);
        setCentralWidget(scrollArea);


        createActions();
        createMenus();

        setWindowTitle(tr("Image Processing"));
        resize (500, 400);
}

void ImageProcessing::createActions()
{
	openAct = new QAction(tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

        saveAct = new QAction(tr("&Save..."), this);
        saveAct->setShortcut(tr("Ctrl+S"));
        saveAct->setDisabled(true);
        connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	linearStretchingRGBAct = new QAction(tr("&Linear RGB histogram stretching"), this);
	connect(linearStretchingRGBAct, SIGNAL(triggered()), this, SLOT(linearStretchingRGB()));

	linearStretchingRAct = new QAction(tr("&Linear R histogram stretching"), this);
	connect(linearStretchingRAct, SIGNAL(triggered()), this, SLOT(linearStretchingR()));

	linearStretchingGAct = new QAction(tr("&Linear G histogram stretching"), this);
	connect(linearStretchingGAct, SIGNAL(triggered()), this, SLOT(linearStretchingG()));

	linearStretchingBAct = new QAction(tr("&Linear B histogram stretching"), this);
	connect(linearStretchingBAct, SIGNAL(triggered()), this, SLOT(linearStretchingB()));

	exitAct = new QAction(tr("E&xit"), this);
        exitAct->setShortcut(tr("Ctrl+Q"));
        connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

}

void ImageProcessing::createMenus()
{
	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(openAct);
        fileMenu->addAction(saveAct);
        fileMenu->addAction(exitAct);

        contrastMenu = new QMenu(tr("&Contrast"), this);
	contrastMenu->addAction(linearStretchingRGBAct);
	contrastMenu->addAction(linearStretchingRAct);
	contrastMenu->addAction(linearStretchingGAct);
	contrastMenu->addAction(linearStretchingBAct);
	contrastMenu->setDisabled(true);

        menuBar()->addMenu(fileMenu);
        menuBar()->addMenu(contrastMenu);
}

void ImageProcessing::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                    tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) {
	image->load(fileName, "BMP");
        if (image->isNull()) {
            QMessageBox::information(this, tr("Image Viewer"),
                                     tr("Cannot load %1.").arg(fileName));
            return;
        }

        imageLabel->setPixmap(QPixmap::fromImage(*image));
	imageLabel->adjustSize();

        saveAct->setEnabled(true);
	contrastMenu->setEnabled(true);
    }
}

void ImageProcessing::save()
{
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        tr("Save File"),
                                                        QDir::currentPath());

        if (!fileName.isEmpty()) {
                image->save(fileName, "BMP");
        }
}

void ImageProcessing::linearStretchingRGB()
{
	int intensityMin = 255;
	int intensityMax = 0;

	for (int y = 0; y < image->height(); y++) {
		for (int x = 0; x < image->width(); x++) {
			int intensity = coeffRed * qRed(image->pixel(x,y)) +
					coeffGreen * qGreen(image->pixel(x,y)) +
					coeffBlue * qBlue(image->pixel(x,y));
			if (intensity > intensityMax) {
				intensityMax = intensity;
			}
			if (intensity < intensityMin) {
				intensityMin = intensity;
			}
		}
	}

	if (!(intensityMax - intensityMin)) {
		return;
	}

	for (int y = 0; y < image->height(); y++) {
		for (int x = 0; x < image->width(); x++) {
			image->setPixel(x, y, qRgb(255 * (qRed(image->pixel(x,y)) - intensityMin) / (intensityMax - intensityMin),
						    255 * (qGreen(image->pixel(x,y)) - intensityMin) / (intensityMax - intensityMin),
						    255 * (qBlue(image->pixel(x,y)) - intensityMin) / (intensityMax - intensityMin)));
		}
	}

        imageLabel->setPixmap(QPixmap::fromImage(*image));
}

void ImageProcessing::linearStretchingR()
{
	int intensityMin = 255;
	int intensityMax = 0;

	for (int y = 0; y < image->height(); y++) {
		for (int x = 0; x < image->width(); x++) {
			int intensity = qRed(image->pixel(x,y));

			if (intensity > intensityMax) {
				intensityMax = intensity;
			}
			if (intensity < intensityMin) {
				intensityMin = intensity;
			}
		}
	}

	if (!(intensityMax - intensityMin)) {
		return;
	}

	for (int y = 0; y < image->height(); y++) {
		for (int x = 0; x < image->width(); x++) {
			image->setPixel(x, y, qRgb(255 * (qRed(image->pixel(x,y)) - intensityMin) / (intensityMax - intensityMin),
						    qGreen(image->pixel(x,y)),
						    qBlue(image->pixel(x,y))));
		}
	}

	imageLabel->setPixmap(QPixmap::fromImage(*image));
}

void ImageProcessing::linearStretchingG()
{	int intensityMin = 255;
	int intensityMax = 0;

	for (int y = 0; y < image->height(); y++) {
		for (int x = 0; x < image->width(); x++) {
			int intensity =	qGreen(image->pixel(x,y));
			if (intensity > intensityMax) {
				intensityMax = intensity;
			}
			if (intensity < intensityMin) {
				intensityMin = intensity;
			}
		}
	}

	if (!(intensityMax - intensityMin)) {
		return;
	}

	for (int y = 0; y < image->height(); y++) {
		for (int x = 0; x < image->width(); x++) {
			image->setPixel(x, y, qRgb(qRed(image->pixel(x,y)),
						    255 * (qGreen(image->pixel(x,y)) - intensityMin) / (intensityMax - intensityMin),
						    qBlue(image->pixel(x,y))));
		}
	}

	imageLabel->setPixmap(QPixmap::fromImage(*image));
}

void ImageProcessing::linearStretchingB()
{	int intensityMin = 255;
	int intensityMax = 0;

	for (int y = 0; y < image->height(); y++) {
		for (int x = 0; x < image->width(); x++) {
			int intensity = qBlue(image->pixel(x,y));
			if (intensity > intensityMax) {
				intensityMax = intensity;
			}
			if (intensity < intensityMin) {
				intensityMin = intensity;
			}
		}
	}

	if (!(intensityMax - intensityMin)) {
		return;
	}

	for (int y = 0; y < image->height(); y++) {
		for (int x = 0; x < image->width(); x++) {
			image->setPixel(x, y, qRgb(qRed(image->pixel(x,y)),
							qGreen(image->pixel(x,y)),
							255 * (qBlue(image->pixel(x,y)) - intensityMin) / (intensityMax - intensityMin)));
		}
	}

	imageLabel->setPixmap(QPixmap::fromImage(*image));
}
