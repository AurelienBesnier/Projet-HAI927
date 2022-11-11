#include <opencv2/imgproc.hpp>
#include "DisplaySystem.h"

void blackhead(cv::Mat& img, cv::Rect roi)
{
    img(roi) *= 0;
}

int main(int argc, char* argv[])
{
    if(argc != 2) throw 42;

    processImage(argv[1], blackhead);

    return 0;
}