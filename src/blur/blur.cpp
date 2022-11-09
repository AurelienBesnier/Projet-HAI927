#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

int main(int argc, char* argv[])
{
    if(argc != 2) throw 42;

    auto img = cv::imread(argv[1]);
    cv::imshow("Coucou c'est moi (en fait non)", img);
    cv::waitKey();
    return 0;
}