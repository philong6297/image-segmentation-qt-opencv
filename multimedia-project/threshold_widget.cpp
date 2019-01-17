/*
 * This is a personal academic project. Dear PVS-Studio, please check it.
 * PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
 */
#include "threshold_widget.h"
#include "histogram_view.h"
#include "image_view.h"
#include "otsu.h"
#include "utilities.h"
using gsl::make_not_null;
using gsl::not_null;
using gsl::owner;
using image_segmentation::GrayScaleHistogram;
using image_segmentation::GrayScaleThresholdList;

ThresholdWidget::ThresholdWidget(QWidget *parent)
		: QWidget{parent},
			control_{owner<QGroupBox *>{new QGroupBox{"Control", this}}},
			original_input_{
					owner<ImageView *>{new ImageView{"Original Input", this}}},
			grayscale_input_{
					owner<ImageView *>{new ImageView{"Grayscale Input", this}}},
			// histogram_{owner<ImageView *>{new ImageView{"Histogram", this}}},
			histogram_{owner<HistogramView *>{new HistogramView{this}}},
			output_{owner<ImageView *>{new ImageView{"Otsu Thresholding", this}}},
			browse_button_{owner<QPushButton *>{new QPushButton{"Browse"}}},
			start_button_{owner<QPushButton *>{new QPushButton{"Start"}}},
			layout_manager_{owner<QGridLayout *>{new QGridLayout{this}}} {
	SetupLayout();
	SetupSignalsAndSlots();
}

ThresholdWidget::~ThresholdWidget() = default;

void ThresholdWidget::SetupLayout() const {
	layout_manager_->setSpacing(6);
	layout_manager_->setContentsMargins(12, 12, 12, 12);
	layout_manager_->setRowStretch(0, 1);
	layout_manager_->setRowStretch(1, 4);
	layout_manager_->setRowStretch(2, 4);
	layout_manager_->setColumnStretch(0, 1);
	layout_manager_->setColumnStretch(1, 1);
	layout_manager_->addWidget(control_, 0, 0, 1, 1);
	layout_manager_->addWidget(original_input_, 1, 0, 1, 1);
	layout_manager_->addWidget(grayscale_input_, 1, 1, 1, 1);
	layout_manager_->addWidget(histogram_, 2, 0, 1, 1);
	layout_manager_->addWidget(output_, 2, 1, 1, 1);

	owner<QHBoxLayout *> control_layout_manager{new QHBoxLayout{control_}};
	control_layout_manager->setSpacing(6);
	control_layout_manager->setContentsMargins(12, 12, 12, 12);
	control_layout_manager->addWidget(browse_button_);
	control_layout_manager->addWidget(start_button_);
	const owner<QSpacerItem *> control_spacer{
			new QSpacerItem{236, 20, QSizePolicy::Expanding, QSizePolicy::Minimum}};
	control_layout_manager->addItem(control_spacer);

	original_input_->SetupLayout();
	grayscale_input_->SetupLayout();
	histogram_->SetupLayout();
	output_->SetupLayout();
}

void ThresholdWidget::SetupSignalsAndSlots() {
	connect(start_button_,
					&QPushButton::clicked,
					this,
					&ThresholdWidget::OnStartButtonClicked);
	connect(browse_button_,
					&QPushButton::clicked,
					this,
					&ThresholdWidget::OnBrowseButtonClicked);
	connect(original_input_,
					&ImageView::LoadedPixmap,
					[this](const std::optional<QPixmap> pixmap) {
						start_button_->setEnabled(pixmap.has_value());
						grayscale_input_->UpdateImage(QImage{});
						output_->UpdateImage(QImage{});
						histogram_->ShowHistogram(false);
					});

	start_button_->setEnabled(false);
}

bool ThresholdWidget::LoadFile(const QString &fileName) {
	QImageReader reader(fileName);
	reader.setAutoTransform(true);
	const QImage newImage = reader.read();
	if (newImage.isNull()) {
		QMessageBox::information(
				this,
				QGuiApplication::applicationDisplayName(),
				tr("Cannot load %1: %2")
						.arg(QDir::toNativeSeparators(fileName), reader.errorString()));
		return false;
	}

	original_input_->UpdateImage(newImage);

	setWindowFilePath(fileName);

	return true;
}

void ThresholdWidget::OnStartButtonClicked() {
	if (const auto pixmap = original_input_->GetPixmap(); pixmap.has_value()) {
		const auto grayscale_image = pixmap.value().toImage().convertToFormat(
				QImage::Format::Format_Grayscale8);
		grayscale_input_->UpdateImage(grayscale_image);
		const auto &[histogram, threshold_list, output_mat_image] =
				image_segmentation::otsu(QImageToCvMat(grayscale_image));
		output_->UpdateImage(CvMatToQImage(output_mat_image));
		histogram_->UpdateHistogram(histogram, threshold_list);
	}
}

void ThresholdWidget::OnBrowseButtonClicked() {
	QFileDialog dialog{nullptr, "Open File"};
	dialog.setDirectory(QDir::currentPath().append("/input/"));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	QString filter = "Image files (";
	for (const auto &type : QImageReader::supportedImageFormats()) {
		filter += "*." + type + " ";
	}
	filter += ")";
	dialog.setNameFilter(filter);
	dialog.setFileMode(QFileDialog::ExistingFile);
	while (dialog.exec() == QDialog::Accepted &&
				 !LoadFile(dialog.selectedFiles().first())) {
	}
}
