#include <QtGui>
#include "imageprocessing.h"
#include "filterdialog.h"
#include <QRgb>

// BT.709: Y = 0.2125·R + 0.7154·G + 0.0721·B
const double coeffRed = 0.2125;
const double coeffGreen = 0.7154;
const double coeffBlue = 0.0721;
const double pi = 3.1415;


ImgInfo::ImgInfo (double r, double g, double b)
	: coeffR (r), coeffG(g), coeffB(b)
{
	intensityMin = 255.0;
	intensityMax = 0.0;
};

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
	linearStretchingRGBAct->setShortcut((tr("Ctrl+1")));
	connect(linearStretchingRGBAct, SIGNAL(triggered()), this, SLOT(linearStretchingRGB()));

	linearStretchingRAct = new QAction(tr("&Linear R histogram stretching"), this);
	connect(linearStretchingRAct, SIGNAL(triggered()), this, SLOT(linearStretchingR()));

	linearStretchingGAct = new QAction(tr("&Linear G histogram stretching"), this);
	connect(linearStretchingGAct, SIGNAL(triggered()), this, SLOT(linearStretchingG()));

	linearStretchingBAct = new QAction(tr("&Linear B histogram stretching"), this);
	connect(linearStretchingBAct, SIGNAL(triggered()), this, SLOT(linearStretchingB()));

	filterSmoothingAct = new QAction(tr("Smoothing filter"), this);
	connect(filterSmoothingAct, SIGNAL(triggered()), this, SLOT(filterSmoothing()));

	filterSharpnessAct = new QAction(tr("Sharpness filter"), this);
	connect(filterSharpnessAct, SIGNAL(triggered()), this, SLOT(filterSharpness()));

	wavesHAct = new QAction(tr("Waves (horizontal)"), this);
	connect(wavesHAct, SIGNAL(triggered()), this, SLOT(wavesH()));

	wavesVAct = new QAction(tr("Waves (vertical)"), this);
	connect(wavesVAct, SIGNAL(triggered()), this, SLOT(wavesV()));

	exitAct = new QAction(tr("E&xit"), this);
        exitAct->setShortcut(tr("Ctrl+Q"));
        connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

}

void ImageProcessing::createMenus()
{
	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(openAct);
        fileMenu->addAction(saveAct);
	fileMenu->addSeparator();
        fileMenu->addAction(exitAct);

        contrastMenu = new QMenu(tr("&Contrast"), this);
	contrastMenu->setDisabled(true);
	contrastMenu->addAction(linearStretchingRGBAct);
	contrastMenu->addAction(linearStretchingRAct);
	contrastMenu->addAction(linearStretchingGAct);
	contrastMenu->addAction(linearStretchingBAct);
	contrastMenu->addSeparator();
	contrastMenu->addAction(filterSmoothingAct);
	contrastMenu->addAction(filterSharpnessAct);


	effectsMenu = new QMenu(tr("&Effects"), this);
	effectsMenu->setDisabled(true);
	effectsMenu->addAction(wavesHAct);
	effectsMenu->addAction(wavesVAct);

        menuBar()->addMenu(fileMenu);
        menuBar()->addMenu(contrastMenu);

	menuBar()->addMenu(effectsMenu);
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

	setWorkArea (0, 0, image->width(), image->height());

        saveAct->setEnabled(true);
	contrastMenu->setEnabled(true);
	effectsMenu->setEnabled(true);
    }
}

void ImageProcessing::setWorkArea(int x0, int y0, int x1, int y1)
{
	pos.x0 = x0;
	pos.y0 = y0;
	pos.x1 = x1;
	pos.y1 = y1;
}

void ImageProcessing::save()
{
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        tr("Save File"),
                                                        QDir::currentPath());

	if (fileName.isEmpty()) {
		return;
	}

	if (!fileName.endsWith(".bmp")) {
		fileName.append (".bmp");
	}

	image->save(fileName, "BMP");
}

void ImageProcessing::linearStretchingRGB()
{
	ImgInfo img(coeffRed, coeffGreen, coeffBlue);
	extremumsIntensity (&img);

	fprintf(stderr, "%f %f", img.intensityMin, img.intensityMax);

	if (!(img.intensityMax - img.intensityMin)) {
		return;
	}

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			double redValue = round(255.0 * (qRed(image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
			double greenValue = round(255.0 * (qGreen(image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
			double blueValue = round(255.0 * (qBlue(image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
			redValue = (redValue >= 0) ? redValue : 0;
			greenValue = (greenValue >= 0) ? greenValue : 0;
			blueValue = (blueValue >= 0) ? blueValue : 0;
			image->setPixel(x, y, qRgb(redValue, greenValue, blueValue));
		}
	}

        imageLabel->setPixmap(QPixmap::fromImage(*image));
}

void ImageProcessing::linearStretchingR()
{
	ImgInfo img(1, 0 , 0);
	extremumsIntensity (&img);

	if (!(img.intensityMax - img.intensityMin)) {
		return;
	}

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			double redValue = round(255 * (qRed(image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
			double greenValue = qGreen(image->pixel(x,y));
			double blueValue = qBlue(image->pixel(x,y));
			image->setPixel(x, y, qRgb(redValue, greenValue, blueValue));
		}
	}

	imageLabel->setPixmap(QPixmap::fromImage(*image));
}

void ImageProcessing::linearStretchingG()
{
	ImgInfo img(0, 1 , 0);
	extremumsIntensity (&img);

	if (!(img.intensityMax - img.intensityMin)) {
		return;
	}

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			int redValue = qRed(image->pixel(x,y));
			double greenValue = round(255 * (qGreen(image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
			int blueValue = qBlue(image->pixel(x,y));
			image->setPixel(x, y, qRgb(redValue, greenValue, blueValue));
		}
	}

	imageLabel->setPixmap(QPixmap::fromImage(*image));
}

void ImageProcessing::linearStretchingB()
{
	ImgInfo img(0, 0 , 1);
	extremumsIntensity (&img);

	if (!(img.intensityMax - img.intensityMin)) {
		return;
	}

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			int redValue = qRed(image->pixel(x,y));
			int greenValue = qGreen(image->pixel(x,y));
			double blueValue = round(255 * (qBlue(image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
			image->setPixel(x, y, qRgb(redValue, greenValue, blueValue));
		}
	}

	imageLabel->setPixmap(QPixmap::fromImage(*image));
}

void ImageProcessing::extremumsIntensity(ImgInfo * img)
{
	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			double intensity = img->coeffR * qRed(image->pixel(x,y)) +
					img->coeffG * qGreen(image->pixel(x,y)) +
					img->coeffB * qBlue(image->pixel(x,y));
			if (intensity > img->intensityMax) {
				img->intensityMax = intensity;
			}
			if (intensity < img->intensityMin) {
				img->intensityMin = intensity;
			}
		}
	}
}

void ImageProcessing::filterSmoothing()
{
	int sigma = 2;
	int s2 = 2 * sigma * sigma;
	int partLine = 3 * sigma;
	QImage line = QImage(2*partLine + 1, 1, QImage::Format_RGB888);
	int *normalLine = new int [(2 * partLine + 1) * sizeof(int)];

	for (int i = partLine; i < 2 * partLine; i++) {
		normalLine[2 * partLine - i] = normalLine[i] = exp (- i * i / s2);

	}

	for (int i = 0; i < 2 * partLine; i++) {
		fprintf (stderr, "%d:5:10", normalLine[i]);
	}

	// Horizontal;
	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			int sumPixels = 0;
			QRgb pix = qRgb (0, 0, 0);
			for (int i = 0; i < 2 * partLine; i++) {
				int j = x + i;
				if (j >= pos.x0 && j <= pos.x1) {
					QRgb neibPix = image->pixel(j, y);
					pix = qRgb (qRed(pix) + qRed(neibPix * normalLine[i]),
						    qGreen(pix) + qGreen(neibPix) * normalLine[i],
						    qBlue(pix) + qBlue(neibPix)) * normalLine[i];
					sumPixels += normalLine[i];
				}
			}
			pix = qRgb (qRed(pix/sumPixels), qGreen(pix/sumPixels), qBlue(pix/sumPixels));
			line.setPixel(x, 1, pix);
		}
		for (int x = pos.x0; x < pos.x1; x++) {
			image->setPixel(x, y, line.pixel(x, 1));
		}
	}

	for (int x = pos.x0; x < pos.x1; x++) {
		for (int y = pos.y0; y < pos.y1; y++) {
			int sumPixels = 0;
			QRgb pix = qRgb (0, 0, 0);
			for (int i = 0; i < 2 * partLine; i++) {
				int j = y + i;
				if (j >= pos.y0 && j <= pos.y1) {
					QRgb neibPix = image->pixel(j, y);
					pix = qRgb (qRed(pix) + qRed(neibPix * normalLine[i]),
						    qGreen(pix) + qGreen(neibPix) * normalLine[i],
						    qBlue(pix) + qBlue(neibPix)) * normalLine[i];
					sumPixels += normalLine[i];
				}
			}
			pix = qRgb (qRed(pix/sumPixels), qGreen(pix/sumPixels), qBlue(pix/sumPixels));
			line.setPixel(y, 1, pix);
		}
		for (int y = pos.y0; y < pos.y1; y++) {
			image->setPixel(x, y, line.pixel(x, 1));
		}
	}

	imageLabel->setPixmap(QPixmap::fromImage(*image));
}

void ImageProcessing::filterSharpness()
{
}

void ImageProcessing::wavesH()
{
	QImage *imageBuf = new QImage(image->width(), image->height(), QImage::Format_RGB888);

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			int newX = x + 20 * sin(2 * pi * x / 30);
			int newY = y;
			imageBuf->setPixel(x, y, image->pixel(newX, newY));
		}
	}

	delete (image);
	image = imageBuf;
	imageLabel->setPixmap(QPixmap::fromImage(*imageBuf));
}

void ImageProcessing::wavesV()
{
	QImage *imageBuf = new QImage(image->width(), image->height(), QImage::Format_RGB888);

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			int newX = x + 20 * sin(2 * pi * y / 128);
			int newY = y;
			imageBuf->setPixel(x, y, image->pixel(newX, newY));
		}
	}
	delete (image);
	image = imageBuf;
	imageLabel->setPixmap(QPixmap::fromImage(*imageBuf));
}
