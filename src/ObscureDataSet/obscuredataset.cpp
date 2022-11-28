#include <string_view>
#include <filesystem>
#include <vector>
#include <fstream>
#include <iostream>
#include <random>
#include <algorithm>

#include <opencv2/imgproc.hpp>

void blur(cv::Mat& img, cv::Rect roi, int radius)
{
    cv::GaussianBlur(img(roi), img(roi), {radius*2+1,radius*2+1}, 0);
}

void pixel(cv::Mat& img, cv::Rect roi, int size)
{
    roi.height = (roi.height + size) / (size + 1) * (size + 1);
    roi.width = (roi.width + size) / (size + 1) * (size + 1);
    if(roi.height + roi.y > img.rows || roi.width + roi.x > img.cols)
    {
        return;
    }
    cv::Mat area = img(roi);
    cv::Mat smol(roi.height / (size + 1), roi.width / (size + 1), area.type());
    cv::resize(area, smol, smol.size(), 0, 0, cv::INTER_AREA);
    cv::resize(smol, area, area.size(), 0, 0, cv::INTER_NEAREST);
}

void blackhead(cv::Mat& img, cv::Rect roi)
{
    img(roi) *= 0;
}

cv::Rect getRoiFromFaceloc(std::filesystem::path file, std::filesystem::path faceloc_file, int obscuration_method)
{
    // TODO: Obscure the file with the corresponding faceloc and method
    // if method = 1 - > blur
    // if method = 2 - > pixel
    // if method = 3 - > blackhead
  
}

int main(int argc, char* argv[])
{

  if(argc!=3) return EXIT_FAILURE;


  return EXIT_SUCCESS;
}
