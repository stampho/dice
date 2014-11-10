#include "cube.h"
#include "face.h"

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

Cube::Cube()
    : m_storePips(false)
{
}

void Cube::setHasPips(bool storePips)
{
    m_storePips = storePips;
}

bool Cube::hasPips() const
{
    return m_storePips;
}

void Cube::addFace(Face face)
{
    if (m_faces.empty()) {
        m_farthest = face;
        m_faces.push_back(face);
        return;
    }

    int newY = face.getCenter().y;
    int y;

    int i = 0;
    int size = m_faces.size();
    for (; i < size; ++i) {
        y = m_faces[i].getCenter().y;
        if (y >= newY)
            break;
    }

    int j = i;
    if (j == size)
        j--;
    if (face.getDistance(m_faces[j]) <= 2.0)
        return;
    if (j+1 < size)
        j++;
    if (face.getDistance(m_faces[j]) <= 2.0)
        return;

    m_faces.insert(m_faces.begin() + i, face);
}

Mat Cube::draw(Mat image, Scalar color, bool markCenter)
{
    if (image.channels() == 1)
        cvtColor(image, image, CV_GRAY2RGB);

    // Mark the center of the farthest face
    if (markCenter) {
        circle(image, m_farthest.getCenter(), 2, Scalar(0, 0, 255), 2);
        circle(image, m_farthest.getCenter(), 8, Scalar(128, 128, 0), 4);
    }

    int num = m_faces.size();
    for (int i = 0; i < num; ++i)
        m_faces[i].draw(image, color, false);

    return image;
}

void Cube::printDetails() const
{
    int num = m_faces.size();
    fprintf(stderr, "cube's faces (%d):", num);
    for (int i = 0; i < num; ++i) {
        fprintf(stderr, " (%d, %d)", m_faces[i].getCenter().x, m_faces[i].getCenter().y);
    }
    fprintf(stderr, "\n");
}

Face Cube::getTopFace() const
{
    if (!hasPips())
        return m_faces[0];

    vector<Face> pipFaces = getTopPips();

    std::vector<Point> pipPoints;
    std::vector<Point> newPoints;
    for (size_t i = 0; i < pipFaces.size(); ++i) {
        newPoints = pipFaces[i].getVertices();
        pipPoints.insert(pipPoints.end(), newPoints.begin(), newPoints.end());
    }
    RotatedRect rect = minAreaRect(Mat(pipPoints));

    Point2f points[4];
    rect.points(points);
    int x, y;
    vector<Point> result;
    for (int i = 0; i < 4; ++i) {
        x = round(points[i].x);
        y = round(points[i].y);
        result.push_back(Point(x, y));
    }

    return Face(result);
}

vector<Face> Cube::getTopPips() const
{
    vector<Face> pipFaces;

    int m = min(6, (int)m_faces.size());
    for (int i = 0; i < m; ++i) {
        pipFaces.push_back(m_faces[i]);
    }

    //printDetails();

    return pipFaces;
}

Face Cube::getNearestPip(Face ref) const
{
    Face nearest, curr;
    double minDist, dist;
    int num = m_faces.size();

    nearest = m_faces[0];
    if (nearest.getCenter() == ref.getCenter())
        nearest = m_faces[1];

    minDist = ref.getDistance(nearest);

    for (int i = 1; i < num; ++i) {
        curr = m_faces[i];
        dist = curr.getDistance(ref);
        if (dist != 0 && dist < minDist) {
            minDist = dist;
            nearest = curr;
        }
    }

    return nearest;
}

vector<Cube> Cube::collectCubes(vector<Face> faces, bool storePips)
{
    // Supposing that there only two dices on the picture
    vector<Cube> cubes(2);

    vector<Face> opposites = Cube::getOppositeFaces(faces);
    assert(opposites.size() == 2);
    cubes[0].addFace(opposites[0]);
    cubes[0].setHasPips(storePips);
    cubes[1].addFace(opposites[1]);
    cubes[1].setHasPips(storePips);

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

