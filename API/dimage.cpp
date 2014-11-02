#include "dimage.h"
#include "core.h"

#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

Image::Image()
{}

Image::Image(const char* imagePath)
{
    m_image = imread(imagePath, CV_LOAD_IMAGE_COLOR);
}

Image::Image(Mat image)
    : m_image(image)
{}

Mat Image::getMat()
{
    return m_image;
}

Mat Image::getGray()
{
    Mat gray = m_image.clone();

    if (gray.channels() != 1)
        cvtColor(gray, gray, CV_RGB2GRAY);

    return gray;
}

Mat Image::getBin()
{
    Mat bin = getGray();

    //Canny(bin, bin, 1, 300, 3);
    threshold(bin, bin, 127, 255, THRESH_BINARY);

    return bin;
}

vector<Pip> Image::collectPips()
{
    vector<Pip> pips;

    // Find contours
    vector<Contour> contours;
    findContours(getBin(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    vector<Point> approx;
    for (size_t i = 0; i < contours.size(); ++i) {
        approxPolyDP(
                Mat(contours[i]),
                approx,
                arcLength(Mat(contours[i]), true) * 0.02,
                true
        );

        if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
            continue;

        if (approx.size() >= 8)
            pips.push_back(approx);
    }

    return pips;
}

void Image::printDetails()
{
    printf("image size: %dx%d\n", m_image.cols, m_image.rows);
}


DImage::DImage()
    : Image()
{}

DImage::DImage(const char* imagePath)
    : Image(imagePath)
{}

DImage::DImage(cv::Mat image)
    : Image(image)
{}

vector<Face> DImage::collectFaces()
{
    vector<Face> faces;

    // Find contours
    vector<Contour> contours;
    findContours(getBin(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    vector<Point> approx;
    for (size_t i = 0; i < contours.size(); ++i) {
        approxPolyDP(
                Mat(contours[i]),
                approx,
                arcLength(Mat(contours[i]), true) * 0.02,
                true
        );

        if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx)) {
            continue;
        }

        if (approx.size() == 4) {
            // Do not store the border of the image
            if (approx[0].x == 1 && approx[0].y == 1 &&
                    approx[2].x >= m_image.cols-5 && approx[2].y >= m_image.rows-5) {
                continue;
            }
            faces.push_back(Face(approx));
        }
    }

    return faces;
}


Mat DImage::drawPips(vector<Pip> pips, Scalar color)
{
    Mat image = getGray();
    cvtColor(image, image, CV_GRAY2RGB);
    drawContours(image, pips, -1, color, 2);
    return image;
}
