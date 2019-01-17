#ifndef HISTOGRAM_VIEW_H
#define HISTOGRAM_VIEW_H
#pragma once
#include "otsu.h"
class HistogramView final : public QGroupBox {
	Q_OBJECT

 public:
	explicit HistogramView(QWidget *parent = nullptr);
	~HistogramView() override;
	MAKE_NOT_COPYABLE_NOR_MOVEABLE(HistogramView)
	void SetupLayout();
 public slots:
	void UpdateHistogram(
			const image_segmentation::GrayScaleHistogram &histogram,
			const image_segmentation::GrayScaleThresholdList &threshold_list);
	void ShowHistogram(const bool show) const {
		if (show) {
			plot_->show();
		}
		else {
			plot_->hide();
		}
	}

 private:
	const gsl::not_null<QwtPlot *> plot_;
	const gsl::not_null<QHBoxLayout *> layout_manager_;
};

#endif  // !HISTOGRAM_VIEW_H
