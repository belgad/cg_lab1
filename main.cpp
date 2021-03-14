#include <string>
#include <QImage>

int main(int argc, char *argv[]) {

    std::string s;
    QImage img;

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-p") && (i + 1 < argc)) {
            s = argv[i + 1];
        }
    }

    img.load(QString(s.c_str()));
    img.save("Images/source.png");

    return 0;
}
