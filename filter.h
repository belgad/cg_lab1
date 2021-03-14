#pragma once

#include <QImage>

class Filter {
protected:
    virtual QColor calcNewPixelColor(const QImage &img, int x, int y) const = 0;

public:
    virtual ~Filter() = default;
    virtual QImage process(const QImage &img) const;
};

class InvertFilter : public Filter {
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
};
