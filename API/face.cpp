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
    m_center = calculateCenter();
}

Point Face::getCenter() const
{
    return m_center;
}

vector<Point> Face::getVertices() const
{
    return m_vertices;
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

double Face::getDistance(Face other) const
{
    return norm(m_center - other.getCenter());
}

double Face::getDistance(Point other) const
{
    return norm(m_center - other);
}

Mat Face::crop(Mat image)
{
    Mat mask = Mat::zeros(image.rows, image.cols, CV_8UC1);
    drawContours(mask, vector<vector<Point> >(1, m_vertices), -1, Scalar(255), CV_FILLED);
    polylines(mask, vector<vector<Point> >(1, m_vertices), true, Scalar(255), 3, CV_AA);

    Mat crop = Mat::zeros(image.rows, image.cols, CV_8UC1);
    image.copyTo(crop, mask);

    return crop;
}


Point Face::calculateCenter()
{
    Moments m = moments(m_vertices, true);
    return Point(m.m10/m.m00, m.m01/m.m00);
}
