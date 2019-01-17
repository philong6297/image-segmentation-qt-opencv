#ifndef KMEAN_H
#define KMEAN_H
#pragma once
namespace image_segmentation {
	using KMeanClusteringResult = cv::Mat;
	struct CriteriaInfo final {
		double desired_accuracy;
		int max_iteration;
	};
	cv::Mat kmean(const cv::Mat& source,
								int cluster_count,
								CriteriaInfo criteria_info);
}  // namespace image_segmentation
#endif  // !KMEAN_H
