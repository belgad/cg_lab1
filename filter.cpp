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
