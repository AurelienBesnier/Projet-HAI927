#include "tools.h"
#include <fstream>
#include <random>
#include <algorithm>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "faceloc.h"
#include "obscuration.h"


namespace
{
    bool isNumber(std::string_view s)
    {
        for(char c : s) if(!isdigit(c)) return false;
        return !s.empty();
    }

    namespace fs = std::filesystem;
    using MethodSet = std::vector<std::pair<std::function<void(cv::Mat&, cv::Rect)>, std::string>>;

    cv::Mat applyRandomMethod(std::ostream& output, fs::path from, const MethodSet& methods)
    {
        static std::mt19937 gen(std::random_device{}());
        static std::uniform_int_distribution dist;
        auto [img, loc] = loadFaceWithLoc(from);
        auto& [method, name] = methods[dist(gen, std::uniform_int_distribution<int>::param_type{0, methods.size()-1})];
        method(img, loc);
        output << name << '\n';
        return img;
    }

    void generateObscurations(std::ostream& output, fs::path filename, fs::path from_folder, fs::path to_folder, const MethodSet& methods)
    {
        std::ifstream file(filename);
        std::string count;
        file >> count; file.ignore(std::numeric_limits<std::streamsize>::max(), file.widen('\n'));
        output << count << '\n';
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
            cv::Mat left = cv::imread(from_folder / last_name / (last_name + '_' + index + ".jpg"));

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
            output << same << ' ';
            cv::Mat right = applyRandomMethod(output, from_folder / last_name / (last_name + '_' + index + ".jpg"), methods);
            cv::Mat img;
            cv::hconcat(left, right, img);
            cv::imwrite(to_folder / (local_id + ".jpg"), img);
        }
    }
}

void obscureDataSet(std::ostream& output, std::filesystem::path dataset, std::filesystem::path from_folder, std::filesystem::path to_folder,
    const std::vector<std::string>& method_descriptor)
{
    std::unordered_map<std::string, int> method_names{
        {"noop", 1},
        {"blur", 2},
        {"pixel",3},
        {"blackhead",4}
    };
    MethodSet methods;
    int argi = 0;
    auto nextarg = [&]()
    {
        if(argi == method_descriptor.size()) throw std::invalid_argument("Wrong number of arguments");
        return method_descriptor[argi++];
    };
    while(argi < method_descriptor.size())
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
    generateObscurations(output, dataset, from_folder, to_folder, methods);
}
