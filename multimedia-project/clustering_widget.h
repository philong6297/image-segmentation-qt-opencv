#ifndef CLUSTERING_WIDGET_H
#define CLUSTERING_WIDGET_H
#pragma once
class ImageView;
class StopWatch;
class ClusteringWidget final : public QWidget {
	Q_OBJECT

 public:
	explicit ClusteringWidget(QWidget* parent = nullptr);
	~ClusteringWidget() override;
	MAKE_NOT_COPYABLE_NOR_MOVEABLE(ClusteringWidget)

 private:
	void SetupLayout();
	void SetupSignalsAndSlots();
	void OnBrowseButtonClicked();
	void OnStartButtonClicked();
	bool LoadFile(const QString& fileName);
	ImageView* input_;
	ImageView* output_;
	QSpinBox* clusters_;
	QSpinBox* max_iteration_;
	QDoubleSpinBox* desired_accuracy_;
	QPushButton* browse_;
	QPushButton* start_;
	StopWatch* stopwatch_;
	QFutureWatcher<QImage>* finish_clustering_watcher_;
};

#endif  // !CLUSTERING_WIDGET_H
