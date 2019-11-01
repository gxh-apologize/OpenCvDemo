#ifndef CarPlateRecgnize_H
#define CarPlateRecgnize_H

#include <string>
#include <opencv2/opencv.hpp>
#include "CarPlateLocation.h"

using namespace cv;
using namespace std;

class CarPlateRecgnize{
public:
    CarPlateRecgnize();
    ~CarPlateRecgnize();

    string plateRecgnize(Mat src);

private:
    CarPlateLocation *plateLocation=0;
};