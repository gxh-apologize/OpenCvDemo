#include "CarPlateLocation.h"

CarPlateLocation::CarPlateLocation() {

}

CarPlateLocation::~CarPlateLocation() {

}

void CarPlateLocation::location(Mat src,Mat& dst){

    //高斯模糊
    Mat blur;
    /**
     * 参数三:5,5是一个比较合适的值
     */
    GaussianBlur(src,blur,Size(5,5),0);

    //灰度化
    Mat gray;
    cvtColor(blur,gray,COLOR_BGR2GRAY);

    //边缘检测
    Mat sobel_16;
    Sobel(gray,sobel_16,CV_16S,1,0);
    Mat sobel_8;
    convertScaleAbs(sobel_16,sobel_8);

    //二值化
    Mat shold;
    threshold(sobel_8,shold,0,255,THRESH_OTSU+THRESH_BINARY);

    //闭操作
    //getStructuringElement();
}