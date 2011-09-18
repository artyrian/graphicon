#include <QtGui>
#include "imageprocessing.h"
#include "filterdialog.h"
#include <QRgb>
#include <time.h>


// BT.709: Y = 0.2125·R + 0.7154·G + 0.0721·B
const double coeffRed = 0.2125;
const double coeffGreen = 0.7154;
const double coeffBlue = 0.0721;
const double pi = 3.1415;
const int sharpSigma = 1;

Position::Position(int x0, int y0, int x1, int y1)
{
	this->x0 = x0;
	this->y0 = y0;
	this->x1 = x1;
	this->y1 = y1;
}

struct PixelRGBSpecific
{
	double red;
	double green;
	double blue;
	double sumNormal;

	PixelRGBSpecific ()
		: red(0), green(0), blue(0), sumNormal(0)
	{};

	void add(double r, double g, double b)
	{
		red += r;
		green += g;
		blue += b;
	}

	void addSumNormal(double sum)
	{
		sumNormal += sum;
	}

	void normalize()
	{
		red /= sumNormal;
		green /= sumNormal;
		blue /= sumNormal;
	}
};

ImgInfo::ImgInfo (double r, double g, double b)
	: coeffR (r), coeffG(g), coeffB(b)
{
	intensityMin = 255.0;
	intensityMax = 0.0;
};

ImageProcessing::ImageProcessing()
{
	image = new QImage;
	imageLabel = new AreaImage;

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

AreaImage::AreaImage()
{
	rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
}

Position AreaImage::points()
{
	if (selectedByMouse) {
		return Position (firstCorner.x(), firstCorner.y(), secondCorner.x(), secondCorner.y());
	} else {
		return Position (0, 0, width(), height());
	}
}

void AreaImage::setSelectedByMouse(bool isSelect)
{
	selectedByMouse = isSelect;
}

void AreaImage::mousePressEvent(QMouseEvent *event)
 {
	if (selectedByMouse) {
		rubberBand->hide();
		firstCorner = event->pos();
		rubberBand->setGeometry(QRect(firstCorner, QSize()));
		rubberBand->show();
	}
 }

void AreaImage::mouseMoveEvent(QMouseEvent *event)
 {
	if (selectedByMouse) {
		rubberBand->setGeometry(QRect(firstCorner, event->pos()));
	}
 }

void AreaImage::mouseReleaseEvent(QMouseEvent *event)
 {
	if (selectedByMouse) {
		secondCorner = event->pos();
	}
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

	linearStretchingChannelsAct = new QAction(tr("&Linear RGB (in turn) histogram stretching"), this);
	connect(linearStretchingChannelsAct, SIGNAL(triggered()), this, SLOT(linearStretchingChannels()));

	grayWorldAct = new QAction(tr("Grayworld"), this);
	connect(grayWorldAct, SIGNAL(triggered()), this, SLOT(grayWorld()));

	filterSmoothingSimpleAct = new QAction(tr("Smoothing simple filter"), this);
	connect(filterSmoothingSimpleAct, SIGNAL(triggered()), this, SLOT(filterSmoothingSimple()));

	filterSmoothingAct = new QAction(tr("Smoothing filter..."), this);
	filterSmoothingAct->setShortcut((tr("Ctrl+6")));
	connect(filterSmoothingAct, SIGNAL(triggered()), this, SLOT(filterSmoothing()));

	filterSharpnessAct = new QAction(tr("Sharpness filter"), this);
	filterSharpnessAct->setShortcut((tr("Ctrl+7")));
	connect(filterSharpnessAct, SIGNAL(triggered()), this, SLOT(filterSharpness()));

	filterMedianAct = new QAction(tr("Median filter"), this);
	connect(filterMedianAct, SIGNAL(triggered()), this, SLOT(filterMedian()));

	filterOptionalAct = new QAction(tr("Optional filter"), this);
	connect(filterOptionalAct, SIGNAL(triggered()), this, SLOT(filterOptional()));

	wavesShortEffectAct = new QAction(tr("Waves (short)"), this);
	connect(wavesShortEffectAct, SIGNAL(triggered()), this, SLOT(wavesShortEffect()));

	wavesLongEffectAct = new QAction(tr("Waves (long)"), this);
	connect(wavesLongEffectAct, SIGNAL(triggered()), this, SLOT(wavesLongEffect()));

	glassEffectAct = new QAction(tr("&Glass"), this);
	connect(glassEffectAct, SIGNAL(triggered()), this, SLOT(glassEffect()));

	rotateAct = new QAction(tr("&Rotate..."), this);
	connect(rotateAct, SIGNAL(triggered()), this, SLOT(rotate()));

	selectByMouseAct = new QAction(tr("Select area by mouse"), this);
	selectByMouseAct->setCheckable(true);
	connect(selectByMouseAct, SIGNAL(triggered()), this, SLOT(selectByMouse()));

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

	filterMenu = new QMenu(tr("Fi&lters"), this);
	filterMenu->setDisabled(true);
	filterMenu->addAction(linearStretchingRGBAct);
	filterMenu->addAction(linearStretchingRAct);
	filterMenu->addAction(linearStretchingGAct);
	filterMenu->addAction(linearStretchingBAct);
	filterMenu->addAction(linearStretchingChannelsAct);
	filterMenu->addSeparator();
	filterMenu->addAction(grayWorldAct);
	filterMenu->addSeparator();
	filterMenu->addAction(filterSmoothingSimpleAct);
	filterMenu->addAction(filterSmoothingAct);
	filterMenu->addAction(filterSharpnessAct);
	filterMenu->addAction(filterMedianAct);
	filterMenu->addAction(filterOptionalAct);

	effectsMenu = new QMenu(tr("&Effects"), this);
	effectsMenu->setDisabled(true);
	effectsMenu->addAction(wavesShortEffectAct);
	effectsMenu->addAction(wavesLongEffectAct);
	effectsMenu->addAction(glassEffectAct);

	transformationMenu = new QMenu(tr("&Transorm"), this);
	transformationMenu->setDisabled(true);
	transformationMenu->addAction(rotateAct);

	optionsMenu = new QMenu(tr("&Options"), this);
	optionsMenu->setDisabled(true);
	optionsMenu->addAction(selectByMouseAct);

        menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(filterMenu);
	menuBar()->addMenu(effectsMenu);
	menuBar()->addMenu(transformationMenu);
	menuBar()->addMenu(optionsMenu);
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
	filterMenu->setEnabled(true);
	effectsMenu->setEnabled(true);
	transformationMenu->setEnabled(true);
	optionsMenu->setEnabled(true);
	imageLabel->rubberBand->hide();
	imageLabel->firstCorner = QPoint (0,0);
	imageLabel->secondCorner = QPoint (0,0);
    }
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

	if (!(img.intensityMax - img.intensityMin)) {
		return;
	}

	Position pos = imageLabel->points();
//	(imageLabel->firstCorner.x(), imageLabel->firstCorner.x(), imageLabel->secondCorner.x(), imageLabel->secondCorner.x());
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

void ImageProcessing::linearStretchingChannels()
{
	linearStretchingR();
	linearStretchingG();
	linearStretchingB();
}

void ImageProcessing::linearStretchingR()
{
	ImgInfo img(1, 0 , 0);
	extremumsIntensity (&img);

	if (!(img.intensityMax - img.intensityMin)) {
		return;
	}

	Position pos = imageLabel->points();

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

	Position pos = imageLabel->points();
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

	Position pos = imageLabel->points();
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

void ImageProcessing::grayWorld()
{
	double coeffPixels = 1.0 / (image->height()*image->width());
	double Rsum = 0;
	double Gsum = 0;
	double Bsum = 0;

	Position pos = imageLabel->points();
	for (int y = 0; y < image->height(); y++) {
		for (int x = 0; x < image->width(); x++) {
			Rsum += qRed(image->pixel(x, y));
			Gsum += qGreen(image->pixel(x, y));
			Bsum += qBlue(image->pixel(x, y));
		}
	}

	Rsum *= coeffPixels;
	Gsum *= coeffPixels;
	Bsum *= coeffPixels;

	double avg = (Rsum + Gsum + Bsum)/ 3.0;
	for (int y = 0; y < image->height(); y++) {
		for (int x = 0; x < image->width(); x++) {
			double R = avg * qRed(image->pixel(x, y)) / Rsum;
			double G = avg * qGreen(image->pixel(x, y)) / Gsum;
			double B = avg * qBlue(image->pixel(x, y)) / Bsum;
			image->setPixel(x, y, qRgb(normalizeColorValue(R),
						normalizeColorValue(G),
						normalizeColorValue(B)));
		}
	}

	imageLabel->setPixmap(QPixmap::fromImage(*image));
}

int ImageProcessing::normalizeColorValue (double color)
{
	if (color > 255) {
		return 255;
	} else if (color < 0 ) {
		return 0;
	} else return color;

}

void ImageProcessing::extremumsIntensity(ImgInfo * img)
{
	Position pos = imageLabel->points();
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

double **ImageProcessing::kernelGauss2D(double sigma, double coeffSharp)
{
	double s2 = 2 * sigma * sigma;
	int partLine = round(3 * sigma);
	int size = 2 * partLine + 1;

	double **kernel = new double* [size * sizeof(double*)];
	for(int i = 0; i < size; i++) {
		kernel[i] = new double [size * sizeof(double)];
	}

	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			kernel[x][y] = coeffSharp * (1.0/(pi*s2)) * exp (-(pow(partLine - x, 2)+ pow(partLine-y, 2))/ s2);
		}
	}
	return kernel;
}

void ImageProcessing::gaussBlurSimple(double sigma, double **kernel)
{
	int partLine = round(3 * sigma);
	int size = 2*partLine + 1;

	Position pos = imageLabel->points();
	QImage *imageBuf = new QImage (pos.x1-pos.x0, pos.y1-pos.y0, QImage::Format_RGB888);

	for(int y = pos.y0; y < pos.y1; y++) {
		for(int x = pos.x0; x < pos.x1; x++) {
			PixelRGBSpecific pix;
			for (int m = 0; m < size; m++) {
				int j = y + m - partLine;
				for (int n = 0; n < size; n++) {
					int i = x + n - partLine;
					if (i >= pos.x0 && i < pos.x1 && j >= pos.y0 && j < pos.y1) {
						QRgb neibPix = image->pixel(i, j);
						pix.add(qRed(neibPix) * kernel[n][m],
							qGreen(neibPix) * kernel[n][m],
							qBlue(neibPix) * kernel[n][m]);
						pix.addSumNormal(kernel[n][m]);
					}
				}
			}
			pix.normalize();
			imageBuf->setPixel(x - pos.x0, y - pos.y0, qRgb(normalizeColorValue(pix.red),
									normalizeColorValue(pix.green),
									normalizeColorValue(pix.blue)));
		 }
	}

	copyImageBuf(imageBuf);
	delete(imageBuf);
}

void ImageProcessing::filterSmoothingSimple()
{
	bool statusOk;
	int size = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
				     tr("Power of blur:"), 1, 1, 3, 1, &statusOk);
	if (statusOk) {
		double sigma = size * 1.0 / 3;
		double **kernel= kernelGauss2D(sigma, 1);
		gaussBlurSimple(sigma, kernel);
		imageLabel->setPixmap(QPixmap::fromImage(*image));
	}
}

double *ImageProcessing::kernelGauss1D (double sigma)
{
	int s2 = 2 * sigma * sigma;
	int partLine = round(3 * sigma);
	double *result = new double [(2 * partLine + 1) * sizeof(double)];

	//result[partLine] = 1;
	for(int i = 0; i <= partLine; i++) {
		result[partLine + i] = result[partLine - i] = (1.0/(sqrt(2*pi)*sigma)) * exp (-i*i/s2);
	}

	return result;
}

void ImageProcessing::gaussBlur(double sigma)
{
	int partLine = round (3*sigma);
	double *normalLine = kernelGauss1D(sigma);

	Position pos = imageLabel->points();

	// Horizontal;
	QRgb *pixHArray = new QRgb [(pos.x1 - pos.x0) * sizeof(QRgb)];
	for(int y = pos.y0; y < pos.y1; y++) {
		for(int x = pos.x0; x < pos.x1; x++) {
			PixelRGBSpecific pix;
			for (int i = 0; i <= 2 * partLine; i++) {
				int j = x + i - partLine;
				if (j >= pos.x0 && j < pos.x1) {
					QRgb neibPix = image->pixel(j, y);
					pix.add(qRed(neibPix) * normalLine[i],
						qGreen(neibPix) * normalLine[i],
						qBlue(neibPix) * normalLine[i]);
					pix.addSumNormal(normalLine[i]);
				}
			}
			pix.normalize();
			pixHArray[x] = qRgb(pix.red, pix.green, pix.blue);
		}
		for (int x = pos.x0; x < pos.x1; x++) {
			image->setPixel(x, y, pixHArray[x]);
		}
	}

	// Vertical;
	QRgb *pixVArray = new QRgb [(pos.y1 - pos.y0) * sizeof(QRgb)];
	for(int x = pos.x0; x < pos.x1; x++) {
		for(int y = pos.y0; y < pos.y1; y++) {
			PixelRGBSpecific pix;
			for (int i = 0; i <= 2 * partLine; i++) {
				int j = y + i - partLine;
				if (j >= pos.y0 && j < pos.y1) {
					QRgb neibPix = image->pixel(x, j);
					pix.add(qRed(neibPix) * normalLine[i],
						qGreen(neibPix) *normalLine[i],
						qBlue(neibPix) * normalLine[i]);
					pix.addSumNormal(normalLine[i]);
				}
			}
			pix.normalize();
			pixVArray[y] = qRgb(pix.red, pix.green, pix.blue);
		}
		for (int y = pos.y0; y < pos.y1; y++) {
			image->setPixel(x, y, pixVArray[y]);
		}
	}

	delete(pixHArray);
	delete(pixVArray);
}

void ImageProcessing::filterSmoothing()
{
	bool statusOk;
	int value = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
				     tr("Power of smoothing:"), 1, 1, 3, 1, &statusOk);
	if (statusOk) {
		gaussBlur(value);
		imageLabel->setPixmap(QPixmap::fromImage(*image));
	}
}

void ImageProcessing::filterSharpness()
{
	double alpha = 0.1;
	double sigma = 1.0/3;
	int partLine =  round(3 * sigma);
	double **kernel = kernelGauss2D(sigma, -alpha);

	kernel[partLine][partLine] += 1 + alpha;

	gaussBlurSimple(sigma, kernel);

	imageLabel->setPixmap(QPixmap::fromImage(*image));
}

int compare(const void *a, const void *b)
{
	return (*(char*)a -*(char*)b);
}

void ImageProcessing::medianProcess(int size)
{
	Position pos = imageLabel->points();

	char *rArray = new char[size*size*(sizeof(char))];
	char *gArray = new char[size*size*(sizeof(char))];
	char *bArray = new char[size*size*(sizeof(char))];

	QImage *imageBuf = new QImage (pos.x1-pos.x0, pos.y1-pos.y0, QImage::Format_RGB888);

	for(int y = pos.y0; y < pos.y1; y++) {
		for(int x = pos.x0; x < pos.x1; x++) {
			for(int m = 0; m < size; m++) {
				int j = y + m - size;
				for(int n = 0; n < size; n++) {
					int i = x + n - size;
					rArray[size*m + n] = qRed(realPixel(i, j));
					gArray[size*m + n] = qGreen(realPixel(i, j));
					bArray[size*m + n] = qBlue(realPixel(i, j));
				}
			}
			qsort(rArray, size*size, sizeof(char), compare);
			qsort(gArray, size*size, sizeof(char), compare);
			qsort(bArray, size*size, sizeof(char), compare);
			int red = rArray[size*size/2];
			int green = gArray[size*size/2];
			int blue = bArray[size*size/2];
			imageBuf->setPixel(x, y, qRgb(red, green, blue));
		}
	}
	copyImageBuf(imageBuf);
	delete(imageBuf);
	delete(rArray);
	delete(gArray);
	delete(bArray);
	fprintf (stderr, "x\n");
}

void ImageProcessing::filterMedian()
{
	bool statusOk;
	int size = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
				     tr("Size of median filter:"), 1, 1, 5, 1, &statusOk);
	if (statusOk) {
		medianProcess(size);
		imageLabel->setPixmap(QPixmap::fromImage(*image));
	}
}

void ImageProcessing::filterOptional()
{
	bool statusOk;
	int value = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
				  tr("Size of kernel:"), 1, 1, 10, 1, &statusOk);
	if (statusOk) {
		QTableWidget *filesTable = new QTableWidget(value, value);
		filesTable->setBaseSize(600, 600);
		filesTable->setShown(true);
	}
}

void ImageProcessing::wavesShortEffect()
{
	QImage *imageBuf = new QImage(image->width(), image->height(), QImage::Format_RGB888);

	Position pos = imageLabel->points();

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			int newX = x + 20 * sin(2 * pi * x / 30);
			int newY = y;
			if (newX >= pos.x0 && newX < pos.x1) {
				imageBuf->setPixel(x, y, image->pixel(newX, newY));
			}
		}
	}

	delete (image);
	image = imageBuf;
	imageLabel->setPixmap(QPixmap::fromImage(*imageBuf));
}

void ImageProcessing::wavesLongEffect()
{
	QImage *imageBuf = new QImage(image->width(), image->height(), QImage::Format_RGB888);

	Position pos = imageLabel->points();

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			int newX = x + 20 * sin(2 * pi * y / 128);
			int newY = y;
			if (newX >= pos.x0 && newX < pos.x1) {
				imageBuf->setPixel(x, y, image->pixel(newX, newY));
			}
		}
	}
	delete (image);
	image = imageBuf;
	imageLabel->setPixmap(QPixmap::fromImage(*imageBuf));
}

void ImageProcessing::glassEffect()
{
	QImage *imageBuf = new QImage(image->width(), image->height(), QImage::Format_RGB888);
	Position pos = imageLabel->points();
	srand((unsigned)time(0));

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			int newX = x + (rand()%2 - 0.5) * 10;
			int newY = y + (rand()%2 - 0.5) * 10;
			if (newX >= 0 && newX < image->width() && newY >= 0 && newY < image->height()) {
				imageBuf->setPixel(x, y, image->pixel(newX, newY));
			}
		}
	}
	copyImageBuf(imageBuf);
	delete(imageBuf);
	imageLabel->setPixmap(QPixmap::fromImage(*image));
}

void ImageProcessing::rotate()
{
}

void ImageProcessing::selectByMouse()
{
	bool selectByMouse = selectByMouseAct->isChecked();

	if (!selectByMouse) {
		selectByMouseAct->setChecked(false);
		imageLabel->rubberBand->hide();
		imageLabel->setSelectedByMouse(false);
	} else {
		selectByMouseAct->setChecked(true);
		imageLabel->setSelectedByMouse(true);
	}
}

void ImageProcessing::copyImageBuf(QImage *imgBuf)
{
	Position pos = imageLabel->points();
	for(int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			image->setPixel(x, y, imgBuf->pixel(x - pos.x0, y - pos.y0));
		}
	}
}

QRgb ImageProcessing::realPixel(int x, int y)
{
	return QRgb(image->pixel(realX(x), realY(y)));
}

int ImageProcessing::realX(int x)
{
	if (x < 0) {
		return realX(-x - 1);
	} else if (x >= image->width()){
		return realX(2 * image->width() - 1 - x);
	} else {
		return x;
	}
}

int ImageProcessing::realY(int y)
{
	if  (y < 0) {
		return realY(-y - 1);
	} else if (y >= image->height()) {
		return realY(2 * image->height() - 1 - y);
	} else {
		return y;
	}
}
