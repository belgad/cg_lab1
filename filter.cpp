#include "filter.h"
#include <iostream>

template <typename T>
T clamp(T value, T min, T max) {
    if (value > max) return max;
    if (value < min) return min;
    return value;
}

QImage imageDifference(const QImage &img1, const QImage &img2) {
    if (img1.width() != img2.width() || img1.height() != img2.height()) throw;
    int width = img1.width(), height = img2.height();
    QImage result(img1);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            QColor color1 = img1.pixelColor(i, j), color2 = img2.pixelColor(i, j);
            result.setPixelColor(i, j, QColor(clamp(color1.red() - color2.red(), 0, 255), clamp(color1.green() - color2.green(), 0, 255), clamp(color1.blue() - color2.blue(), 0, 255)));
        }
    }
    return result;
}

float Filter::calcColorIntensity(const QColor &color) {
    float intensity = clamp(0.299f * color.red() + 0.587f * color.green() + 0.114f * color.blue(), 0.f, 255.f);
    return intensity;
}

QImage Filter::process(const QImage &img) const {
    QImage result(img);

    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            QColor color = calcNewPixelColor(img, x, y);
            result.setPixelColor(x, y, color);
        }
    }

    return result;
}

QColor InvertFilter::calcNewPixelColor(const QImage &img, int x, int y) const {
    QColor color = img.pixelColor(x, y);
    color.setRgb(255 - color.red(), 255 - color.green(), 255 - color.blue());
    return color;
}

std::size_t Kernel::getLen() const {
    return getSize() * getSize();
}

Kernel::Kernel() : radius(-1) {}

Kernel::Kernel(std::size_t radius) : radius(radius) {
    data = std::make_unique<float[]>(getLen());
}

Kernel::Kernel(const Kernel &other) : radius(other.getRadius()) {
    if (data) {
        data.reset();
    }
    data = std::make_unique<float[]>(getLen());
    std::copy(other.data.get(), other.data.get() + getLen(), data.get());
}

Kernel::Kernel (float* kernel, size_t radius) : radius(radius) {
    if (data) {
        data.reset();
    }
    data = std::make_unique<float[]>(getLen());
    std::copy(kernel, kernel + getLen(), data.get());
}

std::size_t Kernel::getRadius() const {
    return radius;
}

std::size_t Kernel::getSize() const {
    return 2 * radius + 1;
}

void Kernel::print() const {
    for (size_t i = 0; i < getLen(); i++) {
        printf("%.1f\t", data[i]);
    }
    printf("\n");
}

void Kernel::setKernel(float *kernel, size_t len) {
    radius = len;
    if (data) {
        data.reset();
    }
    data = std::make_unique<float[]>(getLen());
    std::copy(kernel, kernel + getLen(), data.get());
}

float Kernel::operator[](std::size_t id) const {
    return data[id];
}

float& Kernel::operator[](std::size_t id) {
    return data[id];
}

QColor MatrixFilter::calcNewPixelColor(const QImage &img, int x, int y) const {
    float returnR = 0, returnG = 0, returnB = 0;
    int size = mKernel.getSize();
    int radius = mKernel.getRadius();

    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {
            int idx = (i + radius) * size + j + radius;

            QColor color = img.pixelColor(clamp(x + j, 0, img.width() - 1), clamp(y + i, 0, img.height() - 1));

            returnR += color.red() * mKernel[idx];
            returnG += color.green() * mKernel[idx];
            returnB += color.blue() * mKernel[idx];
        }
    }

    return QColor(clamp(returnR, 0.f, 255.f), clamp(returnG, 0.f, 255.f), clamp(returnB, 0.f, 255.f));
}

MatrixFilter::MatrixFilter(const Kernel &kernel) : mKernel(kernel) {}

BlurKernel::BlurKernel(std::size_t radius) : Kernel(radius) {
    for (std::size_t i = 0; i < getLen(); i++) {
        data[i] = 1.f / getLen();
    }
}

BlurFilter::BlurFilter(std::size_t radius) : MatrixFilter(BlurKernel(radius)) {}

GaussianKernel::GaussianKernel(std::size_t radius, float sigma) : Kernel(radius) {
    float norm = 0;
    int signed_radius = static_cast<int>(radius);

    for (int x = -signed_radius; x <= signed_radius; x++) {
        for (int y = -signed_radius; y <= signed_radius; y++) {
            std::size_t idx = (x + radius) * getSize() + y + radius;
            data[idx] = std::exp(-(x * x + y * y) / (2 * sigma * sigma));
            norm += data[idx];
        }
    }

    for (std::size_t i = 0; i < getLen(); i++) {
        data[i] /= norm;
    }
}

GaussianFilter::GaussianFilter(std::size_t radius, float sigma) : MatrixFilter(GaussianKernel(radius, sigma)) {}

QColor GrayScaleFilter::calcNewPixelColor(const QImage &img, int x, int y) const {
    QColor color = img.pixelColor(x, y);
    float intensity = calcColorIntensity(color);
    color.setRgb(intensity, intensity, intensity);
    return color;
}

QColor SepiaFilter::calcNewPixelColor(const QImage &img, int x, int y) const {
    QColor color = img.pixelColor(x, y);
    float intensity = calcColorIntensity(color);
    color.setRgb(clamp(intensity + 2.f * coefficient, 0.f, 255.f), clamp(intensity + 0.5f * coefficient, 0.f, 255.f), clamp(intensity - 1.f * coefficient, 0.f, 255.f));
    return color;
}

SepiaFilter::SepiaFilter(float coefficient) : coefficient(coefficient) {}

QColor BrightnessFilter::calcNewPixelColor(const QImage &img, int x, int y) const {
    QColor color = img.pixelColor(x, y);
    color.setRgb(clamp(color.red() + coefficient, 0.f, 255.f), clamp(color.green() + coefficient, 0.f, 255.f), clamp(color.blue() + coefficient, 0.f, 255.f));
    return color;
}

BrightnessFilter::BrightnessFilter(float coefficient) : coefficient(coefficient) {}

SobelKernelX::SobelKernelX() : Kernel(1) {
    data[0] = -1.f; data[1] = 0.f; data[2] = 1.f;
    data[3] = -2.f; data[4] = 0.f; data[5] = 2.f;
    data[6] = -1.f; data[7] = 0.f; data[8] = 1.f;
}

SobelKernelY::SobelKernelY() : Kernel(1) {
    data[0] = -1.f; data[1] = -2.f; data[2] = -1.f;
    data[3] = 0.f;  data[4] = 0.f;  data[5] = 0.f;
    data[6] = 1.f;  data[7] = 2.f;  data[8] = 1.f;
}

SobelFilterX::SobelFilterX() : MatrixFilter(SobelKernelX()) {}

SobelFilterY::SobelFilterY() : MatrixFilter(SobelKernelY()) {}

QColor DualFilter::calcNewPixelColor(const QImage &img, int x, int y) const {

    std::size_t sizeX = kernelX.getSize() * kernelX.getSize(), lengthX = kernelX.getSize(), sizeY = kernelY.getSize() * kernelY.getSize(), lengthY = kernelY.getSize();

    float redX = 0, greenX = 0, blueX = 0, redY = 0, greenY = 0, blueY = 0;
    for (std::size_t i = 0; i < sizeX; i++) {
        QColor tmp = img.pixelColor(clamp((int)(x + (i % lengthX) - 1), 0, img.width() - 1), clamp((int)(y + (i / lengthX) - 1), 0, img.height() - 1));
        redX += tmp.red() * kernelX[i];
        greenX += tmp.green() * kernelX[i];
        blueX += tmp.blue() * kernelX[i];
    }

    for (std::size_t i = 0; i < sizeY; i++) {
        QColor tmp = img.pixelColor(clamp((int)(x + (i % lengthY) - 1), 0, img.width() - 1), clamp((int)(y + (i / lengthY) - 1), 0, img.height() - 1));
        redY += tmp.red() * kernelY[i];
        greenY += tmp.green() * kernelY[i];
        blueY += tmp.blue() * kernelY[i];
    }

    float returnR = std::sqrt(redX * redX + redY * redY), returnG = std::sqrt(greenX * greenX + greenY * greenY), returnB = std::sqrt(blueX * blueX + blueY * blueY);

    QColor color(clamp(returnR, 0.f, 255.f), clamp(returnG, 0.f, 255.f), clamp(returnB, 0.f, 255.f));

    return color;
}

DualFilter::DualFilter(Kernel kernelX, Kernel kernelY) : kernelX(kernelX), kernelY(kernelY) {}

SharpnessKernel::SharpnessKernel() : Kernel(1) {
    data[0] = 0.f;  data[1] = -1.f; data[2] = 0.f;
    data[3] = -1.f; data[4] = 5.f;  data[5] = -1.f;
    data[6] = 0.f;  data[7] = -1.f; data[8] = 0.f;
}

SharpnessFilter::SharpnessFilter() : MatrixFilter(SharpnessKernel()) {}

QColor GrayWorldFilter::calcNewPixelColor(const QImage &img, int x, int y) const {
    QColor color = img.pixelColor(x, y);
    color.setRgb(clamp(avgFull / avgR * color.red(), 0.f, 255.f), clamp(avgFull / avgG * color.green(), 0.f, 255.f), clamp(avgFull / avgB * color.blue(), 0.f, 255.f));
    return color;
}

QImage GrayWorldFilter::process(const QImage &img) {
    avgR = 0.f; avgG = 0.f; avgB = 0.f; avgFull = 0.f;
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            QColor temp = img.pixelColor(x, y);
            avgR += temp.red();
            avgG += temp.green();
            avgB += temp.blue();
        }
    }
    avgR /= img.width() * img.height();
    avgG /= img.width() * img.height();
    avgB /= img.width() * img.height();
    avgFull = (avgR + avgG + avgB) / 3;

    return Filter::process(img);
}

QColor PerfectReflectorFilter::calcNewPixelColor(const QImage &img, int x, int y) const {
    QColor color = img.pixelColor(x, y);
    color.setRgb(clamp(255.f / maxR * color.red(), 0.f, 255.f), clamp(255.f / maxG * color.green(), 0.f, 255.f), clamp(255.f / maxB * color.blue(), 0.f, 255.f));
    return color;
}

QImage PerfectReflectorFilter::process(const QImage &img) {
    maxR = 0.f; maxG = 0.f; maxB = 0.f;
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            QColor temp = img.pixelColor(x, y);
            if (maxR < temp.red()) {
                maxR = temp.red();
            }
            if (maxG < temp.green()) {
                maxG = temp.green();
            }
            if (maxB < temp.green()) {
                maxB = temp.blue();
            }
        }
    }

    return Filter::process(img);
}

QColor HistogramLinearChange::calcNewPixelColor(const QImage &img, int x, int y) const {
    QColor color = img.pixelColor(x, y);
    color.setRgb(clamp(255.f * (color.red() - minR) / deltaR, 0.f, 255.f), clamp(255.f * (color.green() - minG) / deltaG, 0.f, 255.f), clamp(255.f * (color.blue() - minG) / deltaG, 0.f, 255.f));
    return color;
}

QImage HistogramLinearChange::process(const QImage &img) {
    deltaR = 0.f; deltaG = 0.f; deltaB = 0.f; minR = 255.f; minG = 255.f; minB = 255.f;
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            QColor temp = img.pixelColor(x, y);
            if (deltaR < temp.red()) {
                deltaR = temp.red();
            }
            if (minR > temp.red()) {
                minR = temp.red();
            }
            if (deltaG < temp.green()) {
                deltaG = temp.green();
            }
            if (minG > temp.green()) {
                minG = temp.green();
            }
            if (deltaB < temp.green()) {
                deltaB = temp.blue();
            }
            if (minB > temp.blue()) {
                minB = temp.blue();
            }
        }
    }
    deltaR -= minR; deltaG -= minG; deltaB -= minB;

    return Filter::process(img);
}

ScharrKernelX::ScharrKernelX() : Kernel(1) {
    data[0] = 3.f;  data[1] = 0.f; data[2] = -3.f;
    data[3] = 10.f; data[4] = 0.f; data[5] = -10.f;
    data[6] = 3.f;  data[7] = 0.f; data[8] = -3.f;
}

ScharrKernelY::ScharrKernelY() : Kernel(1) {
    data[0] = 3.f;  data[1] = 10.f;  data[2] = 3.f;
    data[3] = 0.f;  data[4] = 0.f;   data[5] = 0.f;
    data[6] = -3.f; data[7] = -10.f; data[8] = -3.f;
}

SobelFilter::SobelFilter() : DualFilter(SobelKernelX(), SobelKernelY()) {}

ScharrFilter::ScharrFilter() : DualFilter(ScharrKernelX(), ScharrKernelY()) {}

PrewittKernelX::PrewittKernelX() : Kernel(1) {
    data[0] = -1.f; data[1] = 0.f; data[2] = 1.f;
    data[3] = -1.f; data[4] = 0.f; data[5] = 1.f;
    data[6] = -1.f; data[7] = 0.f; data[8] = 1.f;
}

PrewittKernelY::PrewittKernelY() : Kernel(1) {
    data[0] = -1.f; data[1] = -1.f; data[2] = -1.f;
    data[3] = 0.f;  data[4] = 0.f;  data[5] = 0.f;
    data[6] = 1.f;  data[7] = 1.f;  data[8] = 1.f;
}

PrewittFilter::PrewittFilter() : DualFilter(PrewittKernelX(), PrewittKernelY()) {}

Sharpness2Kernel::Sharpness2Kernel() : Kernel(1) {
    data[0] = -1.f; data[1] = -1.f; data[2] = -1.f;
    data[3] = -1.f; data[4] = 9.f;  data[5] = -1.f;
    data[6] = -1.f; data[7] = -1.f; data[8] = -1.f;
}

Sharpness2Filter::Sharpness2Filter() : MatrixFilter(Sharpness2Kernel()) {}

QColor MathematicalMorphologyFilter::calcNewPixelColor(const QImage &img, int x, int y) const {
    int returnR = stdData.red, returnG = stdData.green, returnB = stdData.blue;
    int size = mKernel.getSize();
    int radius = mKernel.getRadius();

    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {
            int idx = (i + radius) * size + j + radius;
            if (mKernel[idx]) {
                QColor color = img.pixelColor(clamp(x + j, 0, img.width() - 1), clamp(y + i, 0, img.height() - 1));
                pixelProcess(color.red(), returnR);
                pixelProcess(color.green(), returnG);
                pixelProcess(color.blue(), returnB);
            }
        }
    }

    return QColor(clamp(returnR, 0, 255), clamp(returnG, 0, 255), clamp(returnB, 0, 255));
}

MathematicalMorphologyFilter::MathematicalMorphologyFilter(const Kernel &kernel) : MatrixFilter(kernel) {}

void Dilation::pixelProcess(int processData, int &storageData) const {
    storageData = std::max(processData, storageData);
}

Dilation::Dilation(const Kernel &kernel) : MathematicalMorphologyFilter(kernel) {
    stdData.red = 0; stdData.green = 0; stdData.blue = 0;
}

void Erosion::pixelProcess(int processData, int &storageData) const {
    storageData = std::min(processData, storageData);
}

Erosion::Erosion(const Kernel &kernel) : MathematicalMorphologyFilter(kernel) {
    stdData.red = 255; stdData.green = 255; stdData.blue = 255;
}

Opening::Opening(const Kernel &kernel) : MatrixFilter(kernel) {}

QImage Opening::process(const QImage &img) const {
    Dilation dilation(mKernel);
    Erosion erosion(mKernel);
    return dilation.process(erosion.process(img));
}

Closing::Closing(const Kernel &kernel) : MatrixFilter(kernel) {}

QImage Closing::process(const QImage &img) const {
    Dilation dilation(mKernel);
    Erosion erosion(mKernel);
    return erosion.process(dilation.process(img));
}

MorphologicalGradient::MorphologicalGradient(const Kernel &kernel) : MatrixFilter(kernel) {}

QImage MorphologicalGradient::process(const QImage &img) const {
    Dilation dilation(mKernel);
    Erosion erosion(mKernel);

    return imageDifference(dilation.process(img), erosion.process(img));
}

MorphologicalTopHat::MorphologicalTopHat(const Kernel &kernel) : MatrixFilter(kernel) {}

QImage MorphologicalTopHat::process(const QImage &img) const {
    Opening opening(mKernel);

    return imageDifference(img, opening.process(img));
}

MorphologicalBlackHat::MorphologicalBlackHat(const Kernel &kernel) : MatrixFilter(kernel) {}

QImage MorphologicalBlackHat::process(const QImage &img) const {
    Closing closing(mKernel);

    return imageDifference(closing.process(img), img);
}

QColor MedianFilter::calcNewPixelColor(const QImage &img, int x, int y) const {
    int red[size], green[size], blue[size];
    for (int i = 0; i < diameter; i++) {
        for (int j = 0; j < diameter; j++) {
            QColor temp = img.pixelColor(clamp(x + i - radius, 0, img.width() - 1), clamp(y + j - radius, 0, img.width() - 1));
            red[i * diameter + j] = temp.red();
            green[i * diameter + j] = temp.green();
            blue[i * diameter + j] = temp.blue();
        }
    }

    std::sort(red, red + size);
    std::sort(green, green + size);
    std::sort(blue, blue + size);

    return QColor(red[size / 2], green[size / 2], blue[size / 2]);
}

MedianFilter::MedianFilter(size_t radius) : radius(radius), diameter(2 * radius + 1), size(diameter * diameter) {}

QColor BaseColorCorrection::calcNewPixelColor(const QImage &img, int x, int y) const {
    QColor color = img.pixelColor(x, y);
    color.setRgb(clamp(coeffR * color.red(), 0.f, 255.f), clamp(coeffG * color.green(), 0.f, 255.f), clamp(coeffB * color.blue(), 0.f, 255.f));
    return color;
}

BaseColorCorrection::BaseColorCorrection(float coeffR, float coeffG, float coeffB) : coeffR(coeffR), coeffG(coeffG), coeffB(coeffB) {}

BaseColorCorrection::BaseColorCorrection(int sourceR, int sourceG, int sourceB, int destR, int destG, int destB) : coeffR(float(destR) / float(sourceR)), coeffG(float(destG) / float(sourceG)), coeffB(float(destB) / float(sourceB)) {}

QImage BaseColorCorrection::process(const QImage &img) const {
    return Filter::process(img);
}

QImage BaseColorCorrection::process(const QImage &img, int sourceX, int sourceY, int destR, int destG, int destB) {
    float baseR = coeffR, baseG = coeffG, baseB = coeffB;
    QColor color = img.pixelColor(sourceX, sourceY);
    coeffR = (float(destR) / float(color.red()));
    coeffG = (float(destG) / float(color.green()));
    coeffB = (float(destB) / float(color.blue()));

    QImage result = Filter::process(img);
    coeffR = baseR; coeffG = baseG; coeffB = baseB;

    return result;
}

QColor MoveFilter::calcNewPixelColor(const QImage &img, int x, int y) const {
    x += deltaX; y += deltaY;
    if (clamp(x, 0, img.width() - 1) == x && clamp(y, 0, img.height() - 1) == y) {
        return img.pixelColor(x, y);
    } else {
        return QColor(0, 0, 0);
    }
}

MoveFilter::MoveFilter(int deltaX, int deltaY) : deltaX(deltaX), deltaY(deltaY) {}

QImage MoveFilter::process(const QImage &img) const {
    return Filter::process(img);
}

QImage MoveFilter::process(const QImage &img, int dX, int dY) {
    float baseDX = deltaX, baseDY = deltaY;
    deltaX = dX; deltaY = dY;
    QImage result = Filter::process(img);
    deltaX = baseDX; deltaY = baseDY;
    return result;
}

QColor RotateFilter::calcNewPixelColor(const QImage &img, int x, int y) const {
//    float tmpX = x + centerX * (cos(angle) - 1) - centerY * sin(angle), tmpY = y + centerX * sin(angle) + centerY * (cos(angle) - 1);
//    x = cos(angle) * tmpX + sin(angle) * tmpY;
//    y = cos(angle) * tmpY - sin(angle) * tmpX;
    int tmpX = (x - centerX) * cos(angle) - (y - centerY) * sin(angle) + centerY, tmpY = (x - centerX) * sin(angle) + (y - centerY) * cos(angle) + centerY;
    if (clamp(tmpX, 0, img.width() - 1) == tmpX && clamp(tmpY, 0, img.height() - 1) == tmpY) {
        return img.pixelColor(tmpX, tmpY);
    } else {
        return QColor(0, 0, 0);
    }

}

RotateFilter::RotateFilter(int centerX, int centerY, float angle) : centerX(centerX), centerY(centerY), angle(angle) {}

QImage RotateFilter::process(const QImage &img) const {
    return Filter::process(img);
}

QImage RotateFilter::process(const QImage &img, int cX, int cY, float ang) {
    int baseX = centerX, baseY = centerY;
    float baseAngle = angle;
    centerX = cX; centerY = cY; angle = ang;
    QImage result = Filter::process(img);
    centerX = baseX; centerY = baseY; angle = baseAngle;
    return result;
}

QColor WavesFilter::calcNewPixelColor(const QImage &img, int x, int y) const {
    int tmpX = x + 20 * sin(2 * M_PI * (filterType == 0 ? x : y) / coefficient);
    if (clamp(tmpX, 0, img.width() - 1) == tmpX) {
        return img.pixelColor(tmpX, y);
    }
    else {
        return QColor(0, 0, 0);
    }
}

WavesFilter::WavesFilter(float sigma, int filterType) : coefficient(sigma), filterType(static_cast<WavesFilterType>(filterType)) {}

QImage WavesFilter::process(const QImage &img) const {
    return Filter::process(img);
}

QImage WavesFilter::process(const QImage &img, float sigma, int filterAxis) {
    float baseCoeff = coefficient; WavesFilterType baseType = filterType;
    coefficient = sigma; filterType = static_cast<WavesFilterType>(filterAxis);
    QImage result = Filter::process(img);
    coefficient = baseCoeff; filterType = baseType;
    return result;
}

QColor GlassFilter::calcNewPixelColor(const QImage &img, int x, int y) const {
    int tmpX = x + 10 * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f), tmpY = y + 10 * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f);
    return img.pixelColor(clamp(tmpX, 0, img.width() - 1), clamp(tmpY, 0, img.height() - 1));
}

GlassFilter::GlassFilter() {
    srand(static_cast<unsigned int>(time(0)));
}

MotionBlurKernel::MotionBlurKernel(size_t n) : Kernel(n) {
    for (size_t i = 0; i < n ; i++) {
        for (size_t j = 0; j < n; j++) {
            if (i == j) {
                data[i * n + j] = 1.f / n;
            }
            else {
                data[i * n + j] = 0.f;
            }
        }
    }
}

MotionBlurFilter::MotionBlurFilter(size_t n) : MatrixFilter(MotionBlurKernel(n)) {}
