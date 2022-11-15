#include <opencv2/highgui.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <filesystem>
#include <fstream>
#include <string>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <optional>

namespace
{
    cv::Mat img;
    cv::Point p1, p2;
    bool selected;
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
            selected = true;
            cv::destroyWindow("Select Face");
        break;
    }
}

bool validateFace(cv::Rect face)
{
    cv::imshow("Img", img(face));
    while(true)
    {
        int input = cv::waitKey();
        if(input == 'y') return true;
        if(input == 'n') return false;
    }
}

cv::CascadeClassifier classifier;

void processImage(std::filesystem::path path)
{
    std::filesystem::path faceloc_path = path;
    faceloc_path += ".faceloc";
    if(std::filesystem::exists(faceloc_path)) return;

    if(!std::filesystem::exists(path)) throw std::runtime_error("Listed file doesn't exist : " + path.string());
    img = cv::imread(path);
    cv::Mat img_gray; cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(img_gray, img_gray);

    std::vector<cv::Rect> faces;
    classifier.detectMultiScale(img_gray, faces);
    
    std::optional<cv::Rect> rect;
    if(faces.size() == 1) rect = faces.front();
    while(!rect || !validateFace(*rect))
    {
        selected = false;
        cv::namedWindow("Select Face");
        while(cv::pollKey(), !selected)
        {
            cv::imshow("Select Face", img);
            cv::setMouseCallback("Select Face", mouse_click);
        }
        rect = cv::Rect(p1, p2);
    }
    std::ofstream faceloc(faceloc_path);
    faceloc << rect->x << ' ' << rect->y << ' ' << rect->width << ' ' << rect->height << std::endl;
}

void processFile(std::filesystem::path folder, std::istream& listing)
{
    std::string last_name;
    std::string token;
    int count;
    listing >> count;
    listing >> last_name;
    while(listing >> token)
    {
        if(!std::isdigit(token.front()))
        {
            last_name = std::move(token);
        }
        else
        {
            token.insert(0, 4-token.length(), '0');
            std::filesystem::path img_path = folder / last_name / (last_name + '_' + token + ".jpg");
            std::cout << img_path << '\n';
            processImage(img_path);
        }
    }
}

int main(int argc, char* argv[])
{
    if(argc != 3) return -1;
    classifier.load("haarcascade_frontalface_alt.xml");
    std::ifstream file(argv[2]);
    processFile(argv[1], file);
    return 0;
}
