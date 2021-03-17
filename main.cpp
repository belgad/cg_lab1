#include <string>
#include <iostream>
#include <fstream>
#include <QImage>
#include "filter.h"

int main(int argc, char *argv[]) {

    bool mathMorphology = false;
    std::string s, mathMorphologyKernelPath;
    int mathMorphologyKernelSize = 0;
    Kernel mathMorphologyKernel;

    mathMorphologyKernelPath = "images/mathMorphologyKernel"; mathMorphology = true;

    QImage img;

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-p") && (i + 1 < argc)) {
            s = argv[i + 1];
        }
        if (!strcmp(argv[i], "--mmkp") && (i + 1 < argc)) {
            mathMorphologyKernelPath = argv[i + 1];
            mathMorphology = true;
        }
        if (!strcmp(argv[i], "-m")) {
            mathMorphology = true;
        }
    }

    if (s.empty()) {
        img.load(QString("images/source.png"));
    }
    else {
        img.load(QString(s.c_str()));
        img.save(QString("images/source.png"));
    }

    if (mathMorphology) {
        std::unique_ptr<float[]> temp;

        if (mathMorphologyKernelPath.empty()) {
            printf("Input size of kernel for math morphology operations:\nSize: ");
            scanf("%d", &mathMorphologyKernelSize);

            temp = std::make_unique<float[]>(mathMorphologyKernelSize * mathMorphologyKernelSize);
            printf("Input kernel (%dx%d):\n", mathMorphologyKernelSize, mathMorphologyKernelSize);
            for (int i = 0; i < mathMorphologyKernelSize; i++) {
                for (int j = 0; j < mathMorphologyKernelSize; j++) {
                    scanf("%f", &temp[i * mathMorphologyKernelSize + j]);
                }
            }
        }
        else {
            FILE *mathMorphologyKernelFile = fopen(mathMorphologyKernelPath.c_str(), "r");

            fscanf(mathMorphologyKernelFile, "%d", &mathMorphologyKernelSize);

            temp = std::make_unique<float[]>(mathMorphologyKernelSize * mathMorphologyKernelSize);

            for (int i = 0; i < mathMorphologyKernelSize; i++) {
                for (int j = 0; j < mathMorphologyKernelSize; j++) {
                    fscanf(mathMorphologyKernelFile, "%f", &temp[i * mathMorphologyKernelSize + j]);
                }
            }
        }

        mathMorphologyKernel.setKernel(temp.get(), mathMorphologyKernelSize / 2);
        temp.reset();
    }

//    InvertFilter invert;
//    invert.process(img).save("images/invert.png");

//    BlurFilter blur;
//    blur.process(img).save("images/blur.png");

//    GaussianFilter gauss;
//    gauss.process(img).save("images/gauss.png");

//    GrayScaleFilter grayScale;
//    grayScale.process(img).save("images/grayScale.png");

//    SepiaFilter sepia;
//    sepia.process(img).save("images/sepia.png");

//    BrightnessFilter brightness;
//    brightness.process(img).save("images/brightness.png");

//    SobelFilterX sobelX;
//    sobelX.process(img).save("images/sobelX.png");

//    SobelFilterY sobelY;
//    sobelY.process(img).save("images/sobelY.png");

//    SharpnessFilter sharpness;
//    sharpness.process(img).save("images/sharpness.png");

//    GrayWorldFilter grayWorld;
//    grayWorld.process(img).save("images/grayWorld.png");

//    PerfectReflectorFilter perfectReflector;
//    perfectReflector.process(img).save("images/perfectReflector.png");

//    LinearHistogramChange linearHistogramChange;
//    linearHistogramChange.process(img).save("images/linearHistogramChange.png");

//    SobelFilter sobel;
//    sobel.process(img).save("images/sobel.png");

//    ScharrFilter scharr;
//    scharr.process(img).save("images/scharr.png");

//    PrewittFilter prewitt;
//    prewitt.process(img).save("images/prewitt.png");

//    Sharpness2Filter sharpness2;
//    sharpness2.process(img).save("images/sharpness2.png");

//    Dilation dilation(mathMorphologyKernel);
//    dilation.process(img).save("images/dilation.png");

//    Erosion erosion(mathMorphologyKernel);
//    erosion.process(img).save("images/erosion.png");

//    Opening opening(mathMorphologyKernel);
//    opening.process(img).save("images/opening.png");

//    Closing closing(mathMorphologyKernel);
//    closing.process(img).save("images/closing.png");

//    MorphologicalGradient morphGrad(mathMorphologyKernel);
//    morphGrad.process(img).save("images/morphGrad.png");

//    MorphologicalTopHat morphTopHat(mathMorphologyKernel);
//    morphTopHat.process(img).save("images/morphTopHat.png");

//    MorphologicalBlackHat morphBlackHat(mathMorphologyKernel);
//    morphBlackHat.process(img).save("images/morphBlackHat.png");

//    MedianFilter median;
//    median.process(img).save("images/median.png");

//    BaseColorCorrection baseColor;
//    baseColor.process(img).save("images/baseColor.png");

//    MoveFilter move;
//    move.process(img, 50, 0).save("images/move.png");

//    RotateFilter rotate;
//    rotate.process(img, img.width() / 2, img.height() / 2, M_PI_4).save("images/rotate.png");

//    WavesFilter waves;
//    waves.process(img, 60, 0).save("images/waves1.png");
//    waves.process(img, 30, 1).save("images/waves2.png");

    GlassFilter glass;
    glass.process(img).save("images/glass.png");

    MotionBlurFilter motionBlur;
    motionBlur.process(img).save("images/motionBlur.png");

    return 0;
}
