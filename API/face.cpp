#include "face.h"

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

Face::Face(vector<Point> vertices)
    : m_vertices(vertices)
{
    assert(vertices.size() == 4);
    m_center = calculateCenter();
}

Point Face::getCenter()
{
    return m_center;
}

Point Face::getTopVertex()
{
    Point topVertex;
    int y, minY = m_center.y;

    int num = m_vertices.size();
    for (int i = 0; i < num; ++i) {
        y = m_vertices[i].y;
        if (y < minY) {
            minY = y;
            topVertex = m_vertices[i];
        }
    }

    return topVertex;
}

Mat Face::draw(Mat image, Scalar color, bool markCenter)
{
    if (image.channels() == 1)
        cvtColor(image, image, CV_GRAY2RGB);

    drawContours(image, vector<vector<Point> >(1, m_vertices), -1, color, 2);
    if (markCenter)
        circle(image, m_center, 2, color, 2);

    return image;
}

void Face::printDetails()
{
    printf("vertices:");
    for (int i = 0; i < 4; ++i)
        printf(" (%d, %d)", m_vertices[i].x, m_vertices[i].y);
    printf("\n");

    printf("center: (%d, %d)\n", m_center.x, m_center.y);
}

double Face::getDistance(Face other)
{
    return norm(m_center - other.getCenter());
}

Mat Face::crop(Mat image)
{
    Mat mask = Mat::zeros(image.rows, image.cols, CV_8UC1);
    drawContours(mask, vector<vector<Point> >(1, m_vertices), -1, Scalar(255), CV_FILLED);

    Mat crop = Mat::zeros(image.rows, image.cols, CV_8UC1);
    crop.setTo(Scalar(255));
    image.copyTo(crop, mask);
    normalize(mask.clone(), mask, 0.0, 255.0, CV_MINMAX, CV_8UC1);

    return crop;
}


Point Face::calculateCenter()
{
    Moments m = moments(m_vertices, true);
    return Point(m.m10/m.m00, m.m01/m.m00);
}
