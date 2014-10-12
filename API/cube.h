#ifndef CUBE_H
#define CUBE_H

#include <opencv2/core/core.hpp>
#include "face.h"

class Cube {
public:
    Cube() { }

    void addFace(Face face);
    Face getTopFace();

    cv::Mat draw(cv::Mat image, cv::Scalar color = cv::Scalar(0, 0, 255), bool markCenter = true);
    void printDetails();

    static std::vector<Cube> collectCubes(std::vector<Face> faces);

private:
    std::vector<Face> m_faces;

    static std::vector<Face> getOppositeFaces(std::vector<Face> faces);
};

#endif // CUBE_H
