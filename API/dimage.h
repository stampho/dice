#ifndef DIMAGE_H
#define DIMAGE_H

#include "core.h"
#include "face.h"

class Image {
public:
    Image();
    Image(const char* imagePath);
    Image(cv::Mat image);

    cv::Mat getMat();
    cv::Mat getGray();
    cv::Mat getBin();

    std::vector<Pip> collectPips();

    void printDetails();

protected:
    cv::Mat m_image;
};

class DImage : public Image {
public:
    DImage();
    DImage(const char* imagePath);
    DImage(cv::Mat image);

    cv::Mat drawPips(std::vector<Pip> pips, cv::Scalar color = cv::Scalar(0, 0, 255));
    std::vector<Face> collectFaces();
};

#endif // DIMAGE_H
