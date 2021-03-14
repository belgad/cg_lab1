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

    return 0;
}
