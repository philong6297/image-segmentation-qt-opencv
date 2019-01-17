#ifndef OTSU_H
#define OTSU_H
#pragma once
#include <tuple>
#include <vector>
namespace image_segmentation {
	using GrayScaleHistogram = cv::Mat;
	using GrayScaleThresholdList = std::vector<int>;
	using GrayScaleImage = cv::Mat;
	using OtsuThresholdResult =
			std::tuple<GrayScaleHistogram, GrayScaleThresholdList, GrayScaleImage>;
	OtsuThresholdResult otsu(const GrayScaleImage& image, int thresholds = 1);
	OtsuThresholdResult otsu(std::string_view path, int thresholds = 1);
}  // namespace image_segmentation
#endif  // !OTSU_H
