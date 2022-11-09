#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

cv::Mat img;
cv::Point p1, p2;

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
            GaussianBlur(img(r), img(r), {51,51}, 0);
            cv::imshow("Img", img);
        break;
    }
}

int main(int argc, char* argv[])
{
    if(argc != 2) throw 42;

    img = cv::imread(argv[1]);
    cv::namedWindow("Img");
    cv::imshow("Img", img);
    cv::setMouseCallback("Img", mouse_click);
    while(cv::waitKey() != 27/* && cv::getWindowProperty("Img", cv::WND_PROP_VISIBLE) >= 0*/)
    {
    }

    return 0;
}