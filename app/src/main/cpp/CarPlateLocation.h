//定位

#ifndef CarPlateLocation_H
#define CarPlateLocation_H

#include <string>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class CarPlateLocation{
public:
    CarPlateLocation();
    ~CarPlateLocation();

    /**
     * 参数一：要定位的图片
     * 参数二：输出
     */
    void location(Mat src,Mat& dst);
};