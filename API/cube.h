#ifndef CUBE_H
#define CUBE_H

#include <opencv2/core/core.hpp>
#include "face.h"

class Cube {
public:
    Cube();

    void setHasPips(bool storePips);
    bool hasPips() const;
    void addFace(Face face);
    Face getTopFace() const;

    cv::Mat draw(cv::Mat image, cv::Scalar color = cv::Scalar(0, 0, 255), bool markCenter = true);
    void printDetails() const;

    static std::vector<Cube> collectCubes(std::vector<Face> faces, bool storePips = false);

private:
    std::vector<Face> m_faces;
    bool m_storePips;
    Face m_farthest;

    static std::vector<Face> getOppositeFaces(std::vector<Face> faces);
    static double meanOfFaces(std::vector<Face> faces);
    std::vector<Face> getTopPips() const;
};

#endif // CUBE_H
