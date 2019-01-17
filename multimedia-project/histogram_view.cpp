/*
 * This is a personal academic project. Dear PVS-Studio, please check it.
 * PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
 */

#include "histogram_view.h"
using gsl::make_not_null;
using gsl::not_null;
using gsl::owner;
HistogramView::HistogramView(QWidget *parent)
		: QGroupBox{"Histogram", parent},
			plot_{owner<QwtPlot *>{new QwtPlot{this}}},
			layout_manager_{owner<QHBoxLayout *>{new QHBoxLayout{this}}} {
	// SetupLayout();
}

HistogramView::~HistogramView() = default;

void HistogramView::UpdateHistogram(
		const image_segmentation::GrayScaleHistogram &histogram,
		const image_segmentation::GrayScaleThresholdList &threshold_list) {
	plot_->detachItems();
	double min_y{}, max_y{};
	cv::minMaxLoc(histogram, &min_y, &max_y);
	plot_->setCanvasBackground(Qt::white);  // Set the Background colour
	plot_->setAxisScale(QwtPlot::yLeft, min_y, max_y);         // Scale the y-axis
	plot_->setAxisScale(QwtPlot::xBottom, 0, 255);             // Scale the x-axis
	plot_->insertLegend(owner<QwtLegend *>{new QwtLegend()});  // Insert a legend

	// plot the bins curve
	auto *bins_curve =
			owner<QwtPlotCurve *>{new QwtPlotCurve()};  // Create a curve
	bins_curve->setTitle("Count");                  // Name the curve
	bins_curve->setPen(Qt::blue,
										 1);  // Set colour and thickness for drawing the curve
	// Use Antialiasing to improve plot render quality
	bins_curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
	/*Insert the points that should be plotted on the graph in a
	Vector of QPoints or a QPolgonF */
	QPolygonF points;
	for (auto row = 0; row < histogram.rows; ++row) {
		points << QPointF(row, histogram.at<float>(row));
	}
	bins_curve->setSamples(points);  // pass points to be drawn on the curve
	bins_curve->attach(plot_);       // Attach curve to the plot

	// plot threshold
	std::for_each(threshold_list.begin(),
								threshold_list.end(),
								[this, min_y](const int threshold) {
									auto marker = owner<QwtPlotMarker *>{new QwtPlotMarker{}};
									// Use Antialiasing to improve plot render quality
									marker->setRenderHint(QwtPlotItem::RenderAntialiased, true);
									marker->setLinePen(Qt::red, 1);
									marker->setYValue(min_y);
									marker->setXValue(threshold);
									marker->setLineStyle(QwtPlotMarker::LineStyle::VLine);
									marker->attach(plot_);
								});
	if (plot_->isHidden()) {
		plot_->show();
	}
	if (plot_->width() == 0 || plot_->height() == 0) {
		plot_->setFixedSize(width() - 36, height() - 36);
	}
	plot_->adjustSize();
}

void HistogramView::SetupLayout() {
	setAlignment(Qt::AlignCenter);
	layout_manager_->setSpacing(6);
	layout_manager_->setContentsMargins(12, 12, 12, 12);
	layout_manager_->addWidget(plot_);

	setLayout(layout_manager_);
	plot_->setAutoReplot(true);
	plot_->setFixedSize(0, 0);
}
