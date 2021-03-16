#include <string>
#include <QImage>
#include "filter.h"

int main(int argc, char *argv[]) {

    std::string s;
    QImage img;

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-p") && (i + 1 < argc)) {
            s = argv[i + 1];
        }
    }

    if (s.empty()) {
        img.load(QString("images/source.png"));
    }
    else {
        img.load(QString(s.c_str()));
        img.save(QString("images/source.png"));
    }

    InvertFilter invert;
    invert.process(img).save("images/invert.png");

    BlurFilter blur;
    blur.process(img).save("images/blur.png");

    GaussianFilter gauss;
    gauss.process(img).save("images/gauss.png");

    GrayScaleFilter grayScale;
    grayScale.process(img).save("images/grayScale.png");

    SepiaFilter sepia;
    sepia.process(img).save("images/sepia.png");

    BrightnessFilter brightness;
    brightness.process(img).save("images/brightness.png");

    SobelFilterX sobelX;
    sobelX.process(img).save("images/sobelX.png");

    SobelFilterY sobelY;
    sobelY.process(img).save("images/sobelY.png");

    SobelFilter sobel;
    sobel.process(img).save("images/sobel.png");

    SharpnessFilter sharpness;
    sharpness.process(img).save("images/sharpness.png");

    return 0;
}
