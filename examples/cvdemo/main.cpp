#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "core.h"
#include "cube.h"
#include "dimage.h"
#include "face.h"

using namespace std;
using namespace cv;

void show(const char *id, Mat image)
{
    namedWindow(id);
    imshow(id, image);
}

int main(int argc, char** argv)
{
    DImage dimage("dice.png");
    dimage.printDetails();
    Mat image = dimage.getMat();
    show("Dice Image", image);

    // Show Edges
    show("Dice Bin", dimage.getBin());

    // Show Faces
    vector<Face> faces = dimage.collectFaces();
    Mat facesImg = dimage.getGray();
    for (size_t i = 0; i < faces.size(); ++i) {
        faces[i].printDetails();
        facesImg = faces[i].draw(facesImg);
    }
    show("Dice Faces", facesImg);

    // Collect Cubes
    vector<Cube> cubes = Cube::collectCubes(faces);
    Mat cubesImg = dimage.getGray();
    for (size_t i = 0; i < cubes.size(); ++i) {
        cubes[i].printDetails();
        Scalar color = (i%2) ? Scalar(255, 0, 0) : Scalar(0, 255, 255);
        cubesImg = cubes[i].draw(cubesImg, color);
    }
    show("Dice Cubes", cubesImg);

    // Collect pips on top faces
    vector<Pip> pips;
    for (size_t i = 0; i < cubes.size(); ++i) {
        Face topFace = cubes[i].getTopFace();
        printf("top: (%d, %d)\n", topFace.getCenter().x, topFace.getCenter().y);

        Image croppedFace(topFace.crop(dimage.getGray()));
        vector<Pip> pipsOfTopFace = croppedFace.collectPips();
        pips.insert(pips.end(), pipsOfTopFace.begin(), pipsOfTopFace.end());

        string windowId = SSTR("Dice Top " << i);
        show(windowId.c_str(), croppedFace.getMat());
    }

    // Display result
    show("Dice Pips", dimage.drawPips(pips));
    printf("\nRESULT: %d\n", pips.size());

    printf("Press 'q' to quit...\n");
    while (1) {
        if (char(waitKey(1)) == 'q') break;
    }

    destroyAllWindows();
    return 0;
}
