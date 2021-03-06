#ifndef FACE_H
#define FACE_H

#include <opencv2/core/core.hpp>

class Face {
public:
    Face() { }
    Face(std::vector<cv::Point> vertices);

    cv::Point getCenter() const;
    std::vector<cv::Point> getVertices() const;
    double getDistance(Face other) const;
    double getDistance(cv::Point other) const;
    cv::Mat crop(cv::Mat image);

    cv::Mat draw(cv::Mat image, cv::Scalar color = cv::Scalar(0, 0, 255), bool markCenter = true);
    void printDetails();

private:
    cv::Point calculateCenter();

    cv::Point m_center;
    std::vector<cv::Point> m_vertices;
};

#endif // FACE_H
