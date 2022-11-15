#include "faceloc.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <fstream>
#include <string>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <optional>

namespace
{
    cv::Point p1, p2;
    bool selected;

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

    bool validateFace(const cv::Mat& img, cv::Rect face)
    {
        cv::imshow("Confirm this is a face (y/n)", img(face));
        while(true)
        {
            int input = cv::waitKey();
            if(input == 'y') return true;
            if(input == 'n') return false;
        }
    }

    cv::CascadeClassifier buildClassifier()
    {
        cv::CascadeClassifier classifier;
        classifier.load("haarcascade_frontalface_alt.xml");
        return classifier;
    }

    cv::CascadeClassifier& getClassifier()
    {
        static cv::CascadeClassifier classifier = buildClassifier();
        return classifier;
    }

    cv::Rect generateFaceLoc(const cv::Mat& img)
    {
        cv::Mat img_gray; cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(img_gray, img_gray);

        std::vector<cv::Rect> faces;
        getClassifier().detectMultiScale(img_gray, faces);
        
        std::optional<cv::Rect> rect;
        if(faces.size() == 1) rect = faces.front();
        while(!rect || !validateFace(img, *rect))
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
        cv::destroyWindow("Confirm this is a face (y/n)");

        return *rect;
    }
}

std::pair<cv::Mat, cv::Rect> loadFaceWithLoc(std::filesystem::path path)
{
    std::pair<cv::Mat, cv::Rect> result;
    auto& [img, loc] = result;
    img = cv::imread(path);
    path += ".faceloc";
    if(std::filesystem::exists(path))
    {
        std::ifstream faceloc(path);
        faceloc >> loc.x >> loc.y >> loc.width >> loc.height;
    }
    else
    {
        loc = generateFaceLoc(img);
        std::ofstream faceloc(path);
        faceloc << loc.x << ' ' << loc.y << ' ' << loc.width << ' ' << loc.height << '\n';
    }

    return result;
}
