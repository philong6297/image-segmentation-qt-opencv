/*
 * This is a personal academic project. Dear PVS-Studio, please check it.
 * PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
 */
#include "otsu.h"
#include <array>
#include <cassert>
#include <vector>
using image_segmentation::GrayScaleHistogram;
using image_segmentation::GrayScaleImage;
using image_segmentation::GrayScaleThresholdList;
using image_segmentation::OtsuThresholdResult;
using std::vector;
namespace {
	GrayScaleHistogram calculate_gray_scale_histogram(
			const GrayScaleImage& image) {
		static const vector<int> channels{0};
		static const cv::Mat mask{};
		static const vector<int> histogram_size{256};
		static const vector<float> ranges{0, 255};
		static constexpr auto accumulate = false;
		GrayScaleHistogram result;
		calcHist(std::vector<cv::Mat>{image},
						 channels,
						 mask,
						 result,
						 histogram_size,
						 ranges,
						 accumulate);
		return result;
	}

	std::tuple<GrayScaleThresholdList, GrayScaleImage> single_otsu(
			const GrayScaleImage& image,
			const GrayScaleHistogram& histogram) {
		auto result_threshold = 0;
		auto result_image = image.clone();
		// total number of pixels
		const auto total = result_image.total();

		// compute total sum = i * histogram(i) + ...
		auto total_sum = 0.0f;
		for (auto row = 0; row < histogram.rows; ++row) {
			// hist is [256 x 1]
			total_sum += row * histogram.at<float>(row);
		}

		std::array<float, 2> sum{.0f, total_sum};
		std::array<float, 2> weight{.0f, static_cast<float>(total)};
		std::array<float, 2> mean{.0f, .0f};
		auto max_variance_between = .0f;
		for (auto row = 0; row < histogram.rows; ++row) {
			const auto histogram_data = histogram.at<float>(row);
			weight.at(0) += histogram_data;
			if (weight.at(0) == 0) {
				continue;
			}

			weight.at(1) = total - weight.at(0);
			if (weight.at(1) == 0) {
				break;
			}

			sum.at(0) += row * histogram_data;
			sum.at(1) = total_sum - sum.at(0);
			mean.at(0) = sum.at(0) / weight.at(0);
			mean.at(1) = sum.at(1) / weight.at(1);
			if (const auto variance_between = weight.at(0) * weight.at(1) *
																				std::powf(mean.at(0) - mean.at(1), 2);
					variance_between > max_variance_between) {
				max_variance_between = variance_between;
				result_threshold = row;
			}
		}

		result_image.forEach<uchar>(
				[result_threshold](uchar& intensity,
													 [[maybe_unused]] const int* position) {
					if (intensity < result_threshold) {
						intensity = 0;
					}
					else {
						intensity = result_threshold;
					}
				});

		return std::make_tuple(GrayScaleThresholdList{result_threshold},
													 result_image);
	}

	std::tuple<GrayScaleThresholdList, GrayScaleImage> double_otsu(
			const GrayScaleImage& image,
			const GrayScaleHistogram& histogram) {
		auto result_threshold = 0;
		auto result_image = image.clone();
		// total number of pixels
		int N = image.total();

		double W0K, W1K, W2K, M0, M1, M2, currVarB, maxBetweenVar, M0K, M1K, M2K,
				MT;

		int optimalThresh1 = 0;
		int optimalThresh2 = 0;

		W0K = 0;
		W1K = 0;

		M0K = 0;
		M1K = 0;

		MT = 0;
		maxBetweenVar = 0;
		for (int k = 0; k <= 255; k++) {
			MT += k * (histogram.at<float>(k) / (double)N);
		}

		for (int t1 = 0; t1 <= 255; t1++) {
			W0K += histogram.at<float>(t1) / (double)N;         // Pi
			M0K += t1 * (histogram.at<float>(t1) / (double)N);  // i * Pi
			M0 = M0K / W0K;                                     //(i * Pi)/Pi

			W1K = 0;
			M1K = 0;

			for (int t2 = t1 + 1; t2 <= 255; t2++) {
				W1K += histogram.at<float>(t2) / (double)N;         // Pi
				M1K += t2 * (histogram.at<float>(t2) / (double)N);  // i * Pi
				M1 = M1K / W1K;                                     //(i * Pi)/Pi

				W2K = 1 - (W0K + W1K);
				M2K = MT - (M0K + M1K);

				if (W2K <= 0)
					break;

				M2 = M2K / W2K;

				currVarB = W0K * (M0 - MT) * (M0 - MT) + W1K * (M1 - MT) * (M1 - MT) +
									 W2K * (M2 - MT) * (M2 - MT);

				if (maxBetweenVar < currVarB) {
					maxBetweenVar = currVarB;
					optimalThresh1 = t1;
					optimalThresh2 = t2;
				}
			}
		}

		result_image.forEach<uchar>(
				[optimalThresh1, optimalThresh2](uchar& intensity,
																				 [[maybe_unused]] const int* position) {
					if (intensity < optimalThresh1) {
						intensity = 0;
					}
					else if (intensity < optimalThresh2) {
						intensity = 128;
					}
					else {
						intensity = 255;
					}
				});

		return std::make_tuple(
				GrayScaleThresholdList{optimalThresh1, optimalThresh2}, result_image);
	}
}  // namespace

OtsuThresholdResult image_segmentation::otsu(const GrayScaleImage& image,
																						 const int thresholds) {
	assert(image.data != nullptr && image.channels() == 1);

	// Compute histogram
	const auto histogram = calculate_gray_scale_histogram(image);

	const auto& [threshold_list, output_image] = double_otsu(image, histogram);
	return {histogram, threshold_list, output_image};
}

OtsuThresholdResult image_segmentation::otsu(std::string_view path,
																						 const int thresholds) {
	const auto gray_scale_img = cv::imread(path.data(), cv::IMREAD_GRAYSCALE);
	return otsu(gray_scale_img, thresholds);
}
