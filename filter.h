#pragma once

#include <memory>
#include <cmath>
#include <QImage>

class Filter {
protected:
    virtual QColor calcNewPixelColor(const QImage &img, int x, int y) const = 0;
    float calcColorIntensity(const QColor &color) const;

public:
    virtual ~Filter() = default;
    virtual QImage process(const QImage &img) const;
};

class InvertFilter : public Filter {
protected:
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
};

class Kernel {
protected:
    std::unique_ptr<float[]> data;
    std::size_t radius;

    std::size_t getLen() const;

public:
    Kernel(std::size_t radius);
    Kernel(const Kernel &other);

    std::size_t getRadius() const;
    std::size_t getSize() const;
    float operator[](std::size_t id) const;
    float& operator[](std::size_t id);
};

class MatrixFilter : public Filter {
protected:
    Kernel mKernel;
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;

public:
    MatrixFilter(const Kernel &kernel);
    virtual ~MatrixFilter() = default;
};

class BlurKernel : public Kernel {
public:
    BlurKernel(std::size_t radius = 2);
};

class BlurFilter : public MatrixFilter {
public:
    BlurFilter(std::size_t radius = 2);
};

class GaussianKernel : public Kernel {
public:
    GaussianKernel(std::size_t radius = 2, float sigma = 3.f);
};

class GaussianFilter : public MatrixFilter {
public:
    GaussianFilter(std::size_t radius = 2, float sigma = 3.f);
};

class GrayScaleFilter : public Filter {
protected:
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
};

class SepiaFilter : public Filter {
protected:
    float coefficient;
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
public:
    SepiaFilter(float coefficient = 15.f);
};

class BrightnessFilter : public Filter {
protected:
    float coefficient;
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
public:
    BrightnessFilter(float coefficient = 100.f);
};

class SobelKernelX : public Kernel {
public:
    SobelKernelX();
};

class SobelKernelY : public Kernel {
public:
    SobelKernelY();
};

class SobelFilterX : public MatrixFilter {
public:
    SobelFilterX();
};

class SobelFilterY : public MatrixFilter {
public:
    SobelFilterY();
};

class SobelFilter : public Filter {
protected:
    SobelFilterX sobelX;
    SobelFilterY sobelY;
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
};

class SharpnessKernel : public Kernel {
public:
    SharpnessKernel();
};

class SharpnessFilter : public MatrixFilter {
public:
    SharpnessFilter();
};
