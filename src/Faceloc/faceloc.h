#ifndef FACELOC_H
#define FACELOC_H

#include <filesystem>
#include <utility>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

//Loads an image with the cached face location. If the cache doesn't exist, create it and ask user to confirm
std::pair<cv::Mat, cv::Rect> loadFaceWithLoc(std::filesystem::path path);

#endif
