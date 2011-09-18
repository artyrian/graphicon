#include "sourceprocessing.h"

const double coeffRed = 0.2125;
const double coeffGreen = 0.7154;
const double coeffBlue = 0.0721;
const double pi = 3.1415;
const int sharpSigma = 1;

void linearStretchingRGB(ImageProcessing *app)
{
	ImgInfo img(coeffRed, coeffGreen, coeffBlue);
	extremumsIntensity (&img, app);

	if (!(img.intensityMax - img.intensityMin)) {
		return;
	}

	Position pos = app->imageLabel->points();
	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			double redValue = round(255.0 * (qRed(app->image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
			double greenValue = round(255.0 * (qGreen(app->image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
			double blueValue = round(255.0 * (qBlue(app->image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
			redValue = (redValue >= 0) ? redValue : 0;
			greenValue = (greenValue >= 0) ? greenValue : 0;
			blueValue = (blueValue >= 0) ? blueValue : 0;
			app->image->setPixel(x, y, qRgb(redValue, greenValue, blueValue));
		}
	}

	app->imageLabel->setPixmap(QPixmap::fromImage(*app->image));
}

void linearStretchingChannels()
{
	linearStretchingR();
	linearStretchingG();
	linearStretchingB();
}

void linearStretchingR()
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

void linearStretchingG()
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

void linearStretchingB()
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

void grayWorld()
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

int normalizeColorValue (double color)
{
	if (color > 255) {
		return 255;
	} else if (color < 0 ) {
		return 0;
	} else return color;

}

void extremumsIntensity(ImgInfo * img, ImageProcessing *app)
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

double *kernelGauss1D (int partLine, int sigma)
{
	int s2 = 2 * sigma * sigma;
	double *result = new double [(2 * partLine + 1) * sizeof(double)];

	//result[partLine] = 1;
	for(int i = 0; i <= partLine; i++) {
		result[partLine + i] = result[partLine - i] = (1/(sqrt(2*pi)*sigma)) * exp (-i*i/s2);
	}

	return result;
}

void gaussBlur(int sigma, QImage *img)
{
	int partLine = sigma;
	double *normalLine = kernelGauss1D(partLine, sigma);

	Position pos = imageLabel->points();

	fprintf(stderr, "start horizontal min: %d, max %d.\n", pos.x0, pos.x1);
	// Horizontal;
	QRgb *pixHArray = new QRgb [(pos.x1 - pos.x0) * sizeof(QRgb)];
	for(int y = pos.y0; y < pos.y1; y++) {
		for(int x = pos.x0; x < pos.x1; x++) {
			PixelRGBSpecific pix;
			for (int i = 0; i <= 2 * partLine; i++) {
				int j = x + i - partLine;
				if (j >= pos.x0 && j < pos.x1) {
					QRgb neibPix = img->pixel(j, y);
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
			img->setPixel(x, y, pixHArray[x]);
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
					QRgb neibPix = img->pixel(x, j);
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
			img->setPixel(x, y, pixVArray[y]);
		}
	}

	delete(pixHArray);
	delete(pixVArray);
}

void filterSmoothing()
{
	bool statusOk;
	int value = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
				     tr("Percentage:"), 3, 3, 7, 1, &statusOk);
	if (statusOk) {
		gaussBlur(value, image);
		imageLabel->setPixmap(QPixmap::fromImage(*image));
	}

}

void filterSharpness()
{
	double alpha = 0.05;
	double oneAlpha = 1 + alpha;

	Position pos = imageLabel->points();

	QImage blurImage = *image;
	gaussBlur(sharpSigma, &blurImage);
	for(int y = pos.y0; y < pos.y1; y++) {
		for(int x = pos.x0; x < pos.x1; x++) {
			QRgb pix = image->pixel(x,y);
			QRgb pixBlur = blurImage.pixel(x,y);
			image->setPixel(x, y, qRgb(oneAlpha*qRed(pix)-alpha*qRed(pixBlur),
						oneAlpha*qGreen(pix)-alpha*qGreen(pixBlur),
						oneAlpha*qBlue(pix)- alpha*qBlue(pixBlur))
					);
		}
	}
	imageLabel->setPixmap(QPixmap::fromImage(*image));
}

void wavesShortEffect()
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

void wavesLongEffect()
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

void glassEffect()
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

void rotate()
{

}
