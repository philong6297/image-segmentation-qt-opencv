/*
 * This is a personal academic project. Dear PVS-Studio, please check it.
 * PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
 */
#include "kmean.h"
using cv::Mat;
using cv::Mat1f;
using std::vector;
#include <future>
#include <vector>
namespace {
	void image_from_kmean(Mat* output,
												const vector<int>& labels,
												const Mat1f& centers,
												const int start,
												const int end,
												const size_t max_iteration_in_thread) {
		const auto length = static_cast<size_t>(end - start);
		if (length <= max_iteration_in_thread) {
			for (auto i = start; i < end; ++i) {
				centers.row(labels.at(static_cast<size_t>(i))).copyTo(output->row(i));
			}
		}
		else {
			const auto middle = start + static_cast<int>(length >> 1U);
			auto handle = std::async(std::launch::async,
															 image_from_kmean,
															 output,
															 labels,
															 centers,
															 middle,
															 end,
															 max_iteration_in_thread);
			image_from_kmean(
					output, labels, centers, start, middle, max_iteration_in_thread);
			handle.wait();
		}
	}
}  // namespace
cv::Mat image_segmentation::kmean(const cv::Mat& source,
																	const int cluster_count,
																	const CriteriaInfo criteria_info) {
	const auto total_elems = source.total();
	auto data_set = source.clone().reshape(1, static_cast<int>(total_elems));
	data_set.convertTo(data_set, CV_32F);
	vector<int32_t> labels{};
	Mat1f centers;
	cv::kmeans(data_set,
						 static_cast<int>(cluster_count),
						 labels,
						 {cv::TermCriteria::EPS + cv::TermCriteria::COUNT,
							criteria_info.max_iteration,
							criteria_info.desired_accuracy},
						 1,
						 cv::KmeansFlags::KMEANS_PP_CENTERS,
						 centers);
	image_from_kmean(
			&data_set, labels, centers, 0, static_cast<int>(total_elems), 10000);

	data_set.reshape(3, source.rows).convertTo(data_set, CV_8U);
	return data_set;
}
