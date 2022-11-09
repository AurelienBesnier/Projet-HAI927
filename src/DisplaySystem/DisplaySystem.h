#ifndef DISPLAYSYSTEM_H
#define DISPLAYSYSTEM_H

#include <utility>
#include <functional>
#include <string_view>
#include <opencv2/highgui.hpp>
#include <opencv2/core/types.hpp>


namespace impl
{
    void trackbar_callback(int val, void* param);

    template<typename Tuple, size_t... Indexes, typename... Params>
    void buildTrackbars(Tuple&& tuple, std::index_sequence<Indexes...> s, Params&&... params)
    {
        (cv::createTrackbar(std::string(params.name), "Img", nullptr, params.max, trackbar_callback, &std::get<Indexes>(tuple)), ...);
    }

    void doProcess(std::string_view file, std::function<void(cv::Mat&, cv::Rect)>& func);
}

struct Param
{
    std::string_view name;
    int max;
};

template<typename F, typename... Params>
void processImage(std::string_view file, F&& processor, Params... params)
{
    using namespace cv;
    using namespace impl;

    namedWindow("Img");
    auto pvals = std::make_tuple((params,0)...);
    buildTrackbars(pvals, std::index_sequence_for<Params...>{}, params...);
    std::function<void(Mat&, Rect)> func = [&](Mat& mat, Rect rect)
    {
        std::apply(processor, std::tuple_cat(std::tie(mat, rect), pvals));
    };

    doProcess(file, func);
}

#endif
