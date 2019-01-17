#ifndef THRESHOLD_WIDGET_H
#define THRESHOLD_WIDGET_H
#pragma once
class ImageView;
class HistogramView;
class ThresholdWidget final : public QWidget {
	Q_OBJECT

 public:
	explicit ThresholdWidget(QWidget *parent = nullptr);
	~ThresholdWidget() override;
	MAKE_NOT_COPYABLE_NOR_MOVEABLE(ThresholdWidget)

 private:
	void SetupLayout() const;
	void SetupSignalsAndSlots();
	bool LoadFile(const QString &file);

	gsl::not_null<QGroupBox *> control_;
	gsl::not_null<ImageView *> original_input_;
	gsl::not_null<ImageView *> grayscale_input_;
	// gsl::not_null<ImageView *> histogram_;
	gsl::not_null<HistogramView *> histogram_;
	gsl::not_null<ImageView *> output_;
	gsl::not_null<QPushButton *> browse_button_;
	gsl::not_null<QPushButton *> start_button_;
	gsl::not_null<QGridLayout *> layout_manager_;

 private slots:
	void OnStartButtonClicked();
	void OnBrowseButtonClicked();
};

#endif  // !THRESHOLD_WIDGET_H
