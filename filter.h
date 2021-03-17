#pragma once

#include <memory>
#include <cmath>
#include <QImage>

QImage imageDifference(const QImage &img1, const QImage &img2);

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
    Kernel();
    Kernel(std::size_t radius);
    Kernel(const Kernel &other);
    Kernel(float *kernel, size_t len);
    Kernel(std::string path);

    std::size_t getRadius() const;
    std::size_t getSize() const;
    void print() const;
    void setKernel(float *kernel, size_t len);
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

class DualFilter : public Filter {
protected:
    Kernel kernelX;
    Kernel kernelY;
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
public:
    DualFilter(Kernel kernelX, Kernel kernelY);
};

class SharpnessKernel : public Kernel {
public:
    SharpnessKernel();
};

class SharpnessFilter : public MatrixFilter {
public:
    SharpnessFilter();
};

class GrayWorldFilter : public Filter {
protected:
    float avgR, avgG, avgB, avgFull;
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
public:
    QImage process(const QImage &img);
};

class PerfectReflectorFilter : public Filter {
protected:
    float maxR, maxG, maxB;
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
public:
    QImage process(const QImage &img);
};

class LinearHistogramChange : public Filter {
protected:
    float deltaR, deltaG, deltaB, minR, minG, minB;
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
public:
    QImage process(const QImage &img);
};

class ScharrKernelX : public Kernel {
public:
    ScharrKernelX();
};

class ScharrKernelY : public Kernel {
public:
    ScharrKernelY();
};

class SobelFilter : public DualFilter {
public:
    SobelFilter();
};

class ScharrFilter : public DualFilter {
public:
    ScharrFilter();
};

class PrewittKernelX : public Kernel {
public:
    PrewittKernelX();
};

class PrewittKernelY : public Kernel {
public:
    PrewittKernelY();
};

class PrewittFilter : public DualFilter {
public:
    PrewittFilter();
};

class Sharpness2Kernel : public Kernel {
public:
    Sharpness2Kernel();
};

class Sharpness2Filter : public MatrixFilter {
public:
    Sharpness2Filter();
};

class MathematicalMorphologyFilter : public MatrixFilter {
protected:
    virtual void pixelProcess(int processData, int &storageData) const = 0;
    struct StdData {
        int red; int green; int blue;
    } stdData;
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
public:
    MathematicalMorphologyFilter(const Kernel &kernel);
};

class Dilation : public MathematicalMorphologyFilter {
protected:
    void pixelProcess(int processData, int &storageData) const;
public:
    Dilation(const Kernel &kernel);
};

class Erosion : public MathematicalMorphologyFilter {
protected:
    void pixelProcess(int processData, int &storageData) const;
public:
    Erosion(const Kernel &kernel);
};

class Opening : public MatrixFilter {
public:
    Opening(const Kernel &kernel);
    QImage process(const QImage &img) const override;
};

class Closing : public MatrixFilter {
public:
    Closing(const Kernel &kernel);
    QImage process(const QImage &img) const override;
};

class MorphologicalGradient : public MatrixFilter {
public:
    MorphologicalGradient(const Kernel &kernel);
    QImage process(const QImage &img) const override;
};

class MorphologicalTopHat : public MatrixFilter {
public:
    MorphologicalTopHat(const Kernel &kernel);
    QImage process(const QImage &img) const override;
};

class MorphologicalBlackHat : public MatrixFilter {
public:
    MorphologicalBlackHat(const Kernel &kernel);
    QImage process(const QImage &img) const override;
};

class MedianFilter : public Filter {
protected:
    int radius;
    int diameter;
    int size;
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
public:
    MedianFilter(size_t radius = 2);
};

class BaseColorCorrection : public Filter {
protected:
    float coeffR, coeffG, coeffB;
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
public:
    BaseColorCorrection(float coeffR = 1.f, float coeffG = 1.f, float coeffB = 1.f);
    BaseColorCorrection(int sourceR, int sourceG, int sourceB, int destR, int destG, int destB);
    QImage process(const QImage &img) const override;
    QImage process(const QImage &img, int sourceX, int sourceY, int destR, int destG, int destB);
};

class MoveFilter : public Filter {
protected:
    int deltaX, deltaY;
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
public:
    MoveFilter(int deltaX = 0, int deltaY = 0);
    QImage process(const QImage &img) const override;
    QImage process(const QImage &img, int dX, int dY);
};

class RotateFilter : public Filter {
    int centerX, centerY;
    float angle;
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
public:
    RotateFilter(int centerX = 0, int centerY = 0, float angle = 0);
    QImage process(const QImage &img) const override;
    QImage process(const QImage &img, int cX, int cY, float ang);
};

class WavesFilter : public Filter {
protected:
    typedef enum {x, y} WavesFilterType;
    float coefficient;
    WavesFilterType filterType;
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
public:
    WavesFilter(float sigma = 30.f, int filterType = 0);
    QImage process(const QImage &img) const override;
    QImage process(const QImage &img, float sigma, int filterType = 0);
};

class GlassFilter : public Filter {
protected:
    QColor calcNewPixelColor(const QImage &img, int x, int y) const override;
public:
    GlassFilter();
};

class MotionBlurKernel : public Kernel {
public:
    MotionBlurKernel(size_t n = 10);
};

class MotionBlurFilter : public MatrixFilter {
public:
    MotionBlurFilter(size_t n = 10);
};
