#include "cube.h"
#include "face.h"

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

void Cube::addFace(Face face)
{
    m_faces.push_back(face);
}

Mat Cube::draw(Mat image, Scalar color, bool markCenter)
{
    if (image.channels() == 1)
        cvtColor(image, image, CV_GRAY2RGB);

    // Mark the center of the farthest face
    if (markCenter) {
        circle(image, m_faces[0].getCenter(), 2, Scalar(0, 0, 255), 2);
        circle(image, m_faces[0].getCenter(), 8, Scalar(128, 128, 0), 4);
    }

    int num = m_faces.size();
    for (int i = 0; i < num; ++i)
        m_faces[i].draw(image, color, false);

    return image;
}

void Cube::printDetails()
{
    printf("cube's faces:");
    int num = m_faces.size();
    for (int i = 0; i < num; ++i) {
        printf(" (%d, %d)", m_faces[i].getCenter().x, m_faces[i].getCenter().y);
    }
    printf("\n");
}

Face Cube::getTopFace()
{
    Face topFace = m_faces[0];
    int y, minY = topFace.getTopVertex().y;

    int num = m_faces.size();
    // Skip first face here since topFace is already initialized with it
    for (int i = 1; i < num; ++i) {
        y = m_faces[i].getTopVertex().y;
        if (y < minY) {
            minY = y;
            topFace = m_faces[i];
        }
    }

    return topFace;
}

vector<Cube> Cube::collectCubes(vector<Face> faces)
{
    // Supposing that there only two dices on the picture
    vector<Cube> cubes(2);

    vector<Face> opposites = Cube::getOppositeFaces(faces);
    assert(opposites.size() == 2);
    cubes[0].addFace(opposites[0]);
    cubes[1].addFace(opposites[1]);

    int num = faces.size();
    for (int i = 0; i < num; ++i) {
        double dist0 = opposites[0].getDistance(faces[i]);
        double dist1 = opposites[1].getDistance(faces[i]);

        if (dist0 == 0.0 || dist1 == 0.0)
            continue;

        int cIndex = (dist0 < dist1) ? 0 : 1;
        cubes[cIndex].addFace(faces[i]);
    }

    return cubes;
}

vector<Face> Cube::getOppositeFaces(vector<Face> faces)
{
    vector<Face> result(2);

    double dist, maxDist = 0;
    int num = faces.size();
    for (int i = 0; i < num-1; ++i) {
        for (int j = i+1; j < num; ++j) {
            dist = faces[i].getDistance(faces[j]);
            if (dist > maxDist) {
                maxDist = dist;
                result[0] = faces[i];
                result[1] = faces[j];
            }
        }
    }

    return result;
}

