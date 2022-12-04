#include <string_view>
#include <filesystem>
#include <vector>
#include <fstream>
#include <iostream>
#include <random>
#include <algorithm>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "faceloc.h"

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

void noop(cv::Mat& img, cv::Rect roi)
{
}

bool isNumber(std::string_view s)
{
    for(char c : s) if(!isdigit(c)) return false;
    return !s.empty();
}

namespace fs = std::filesystem;
using MethodSet = std::vector<std::pair<std::function<void(cv::Mat&, cv::Rect)>, std::string>>;

void applyRandomMethod(fs::path from, fs::path to, const MethodSet& methods)
{
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution dist;
    auto [img, loc] = loadFaceWithLoc(from);
    auto& [method, name] = methods[dist(gen, std::uniform_int_distribution<int>::param_type{0, methods.size()-1})];
    method(img, loc);
    std::cout << name << '\n';
    cv::imwrite(to, img);
}

void generateObscurations(fs::path filename, fs::path from_folder, fs::path to_folder, const MethodSet& methods)
{
    std::ifstream file(filename);
    std::string count;
    file >> count; file.ignore(std::numeric_limits<std::streamsize>::max(), file.widen('\n'));
    std::cout << count << '\n';
    int id = 0;
    std::string val;
    while(getline(file, val) && !val.empty())
    {
        std::string local_id = std::to_string(id++);
        local_id.insert(0, count.size() - local_id.size(), '0');

        std::stringstream line(std::move(val)); line.exceptions(std::ios::badbit | std::ios::failbit);
        std::string last_name;
        line >> last_name;
        std::string index;
        line >> index;
        index.insert(0, 4 - index.size(), '0');
        fs::copy_file(from_folder / last_name / (last_name + '_' + index + ".jpg"), to_folder / (local_id + "_1.jpg"), fs::copy_options::overwrite_existing);

        bool same = true;
        line >> val;
        if(!isNumber(val))
        {
            same = val == last_name;
            last_name = std::move(val);
            line >> val;
        }
        index = std::move(val);
        index.insert(0, 4 - index.size(), '0');
        std::cout << same << ' ';
        applyRandomMethod(from_folder / last_name / (last_name + '_' + index + ".jpg"), to_folder / (local_id + "_2.jpg"), methods);
    }
}

int main(int argc, char* argv[])
{
    if(argc<5)
    {
        std::cerr << "Usage : " << argv[0] << " dataset.txt from_folder to_folder method1 [param] method2 [param]...\n";
        return EXIT_FAILURE;
    }
    std::unordered_map<std::string, int> method_names{
        {"noop", 1},
        {"blur", 2},
        {"pixel",3},
        {"blackhead",4}
    };
    MethodSet methods;
    int argi = 4;
    auto nextarg = [&]()
    {
        if(argi == argc) throw std::invalid_argument("Wrong number of arguments");
        return argv[argi++];
    };
    while(argi < argc)
    {
        std::string arg = nextarg();
        std::string temp;
        switch(method_names[arg])
        {
            using namespace std::placeholders;
        case 1:
            methods.emplace_back(noop, arg);
            break;
        case 2:
            temp = nextarg();
            methods.emplace_back(std::bind(blur,_1,_2,std::stoi(temp)), arg + ' ' + temp);
            break;
        case 3:
            temp = nextarg();
            methods.emplace_back(std::bind(pixel,_1,_2,std::stoi(temp)), arg + ' ' + temp);
            break;
        case 4:
            methods.emplace_back(blackhead, arg);
            break;
        default:
            throw std::invalid_argument("Unknown method name");
        }
    }
    generateObscurations(argv[1], argv[2], argv[3], methods);
  
    return EXIT_SUCCESS;
}
