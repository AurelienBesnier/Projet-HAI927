#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "DisplaySystem.h"

namespace
{
    cv::Mat img;
    cv::Point p1, p2;
}

void impl::trackbar_callback(int val, void* param)
{
    if(param) *(int*)param = val;
}

void mouse_click(int event, int x, int y, int flags, void *param)
{
    using namespace cv;
    switch(event)
    {
        case EVENT_LBUTTONDOWN:
            p1.x = x;
            p1.y = y;
        break;
        case EVENT_LBUTTONUP:
            p2.x = x;
            p2.y = y;
            if(p1 == p2) return;
            Rect r(p1, p2);
            if(param)
            {
                std::function<void(Mat&, Rect)>* f = static_cast<std::function<void(Mat&, Rect)>*>(param);
                (*f)(img, r);
            }
            cv::imshow("Img", img);
        break;
    }
}

void impl::doProcess(std::string_view file, std::function<void(cv::Mat&, cv::Rect)>& func)
{
    img = cv::imread(std::string(file));
    
    cv::imshow("Img", img);
    cv::setMouseCallback("Img", mouse_click, &func);
    while(cv::waitKey() != 27/* && cv::getWindowProperty("Img", cv::WND_PROP_VISIBLE) >= 0*/)
    {
    }
}
