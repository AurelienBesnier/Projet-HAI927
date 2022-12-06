#ifndef OBSCURATION_H
#define OBSCURATION_H

#include <opencv2/core/types.hpp>


void blackhead(cv::Mat& img, cv::Rect roi);

void blur(cv::Mat& img, cv::Rect roi, int radius);

void pixel(cv::Mat& img, cv::Rect roi, int size);

inline void noop(cv::Mat& img, cv::Rect roi) {}

#endif
