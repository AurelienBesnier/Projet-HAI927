#include <opencv2/imgproc.hpp>
#include "DisplaySystem.h"

void blur(cv::Mat& img, cv::Rect roi, int radius)
{
    cv::GaussianBlur(img(roi), img(roi), {radius*2+1,radius*2+1}, 0);
}

int main(int argc, char* argv[])
{
    if(argc != 2) throw 42;

    processImage(argv[1], blur, Param{"Radius", 255});

    return 0;
}