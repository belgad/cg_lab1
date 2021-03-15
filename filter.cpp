#include "filter.h"

template <typename T>
T clamp(T value, T min, T max) {
    if (value > max) return max;
    if (value < min) return min;
    return value;
}

float Filter::calcColorIntensity(const QColor &color) const {
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

Kernel::Kernel(std::size_t radius) : radius(radius) {
    data = std::make_unique<float[]>(getLen());
}

Kernel::Kernel(const Kernel &other) : radius(other.getRadius()) {
    if (data == nullptr) {
        data = std::make_unique<float[]>(other.getLen());
    }
    std::copy(other.data.get(), other.data.get() + getLen(), data.get());
}

std::size_t Kernel::getRadius() const {
    return radius;
}

std::size_t Kernel::getSize() const {
    return 2 * radius + 1;
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
            data[idx] = std::exp(-(x * x + y * y) / (sigma * sigma));
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
    data[0] = -1.f;
    data[1] = 0.f;
    data[2] = 1.f;
    data[3] = -2.f;
    data[4] = 0.f;
    data[5] = 2.f;
    data[6] = -1.f;
    data[7] = 0.f;
    data[8] = 1.f;
}

SobelKernelY::SobelKernelY() : Kernel(1) {
    data[0] = -1.f;
    data[1] = -2.f;
    data[2] = -1.f;
    data[3] = 0.f;
    data[4] = 0.f;
    data[5] = 0.f;
    data[6] = 1.f;
    data[7] = 2.f;
    data[8] = 1.f;
}

SobelFilterX::SobelFilterX() : MatrixFilter(SobelKernelX()) {}

SobelFilterY::SobelFilterY() : MatrixFilter(SobelKernelY()) {}

//QImage SobelFilter::process(const QImage &img) const {
////    imgX.sobelX.process(img), imgY = sobelY.process(img);
//    QImage result(img), imgX = sobelX.process(img), imgY = sobelY.process(img);

//    for (int x = 0; x < img.width(); x++) {
//        for (int y = 0; y < img.height(); y++) {
//            QColor color1 = imgX.pixelColor(x, y), color2 = imgY.pixelColor(x, y);
//            float returnR = std::sqrt(color1.red() * color1.red() + color2.red() * color2.red()), returnG = std::sqrt(color1.green() * color1.green() + color2.green() * color2.green()), returnB = std::sqrt(color1.blue() * color1.blue() + color2.blue() * color2.blue());
//            QColor color(clamp(returnR, 0.f, 255.f), clamp(returnG, 0.f, 255.f), clamp(returnB, 0.f, 255.f));
//            result.setPixelColor(x, y, color);
//        }
//    }

//    return result;
//}

QColor SobelFilter::calcNewPixelColor(const QImage &img, int x, int y) const {
//    QImage imgX = sobelX.process(img.copy(clamp(x - 1, 0, img.width() - 2), clamp(y - 1, 0, img.height() - 2), clamp(x + 3, 0, img.width() - 1) - x, clamp(y + 3, 0, img.height() - 1) - y));
//    QImage imgY = sobelY.process(img.copy(clamp(x, 0, img.width() - 1), clamp(y, 0, img.height() - 1), clamp(x + 3, 0, img.width() - 1) - x, clamp(y + 3, 0, img.height() - 1) - y));
    QImage imgX = sobelX.process(img.copy(x - 1, y - 1, 3, 3)), imgY = sobelY.process(img.copy(x - 1, y - 1, 3, 3));

    QColor color1 = imgX.pixelColor(1, 1), color2 = imgY.pixelColor(1, 1);
    float returnR = std::sqrt(color1.red() * color1.red() + color2.red() * color2.red()), returnG = std::sqrt(color1.green() * color1.green() + color2.green() * color2.green()), returnB = std::sqrt(color1.blue() * color1.blue() + color2.blue() * color2.blue());

    QColor color(clamp(returnR, 0.f, 255.f), clamp(returnG, 0.f, 255.f), clamp(returnB, 0.f, 255.f));

    return color;
}

SharpnessKernel::SharpnessKernel() : Kernel(1) {
    data[0] = 0.f;
    data[1] = -1.f;
    data[2] = 0.f;
    data[3] = -1.f;
    data[4] = 5.f;
    data[5] = -1.f;
    data[6] = 0.f;
    data[7] = -1.f;
    data[8] = 0.f;
}

SharpnessFilter::SharpnessFilter() : MatrixFilter(SharpnessKernel()) {}
