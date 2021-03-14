#include "filter.h"

template <typename T>
T clamp(T value, T min, T max) {
    if (value > max) return max;
    if (value < min) return min;
    return value;
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

MatrixFilter::MatrixFilter(const Kernel &kernel) : mKernel(kernel) {};

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

GaussianFilter::GaussianFilter(std::size_t radius, float sigma) : MatrixFilter(GaussianKernel(radius, sigma)) {};
