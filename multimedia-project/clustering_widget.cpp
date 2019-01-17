/*
 * This is a personal academic project. Dear PVS-Studio, please check it.
 * PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
 */
#include "clustering_widget.h"
#include "image_view.h"
#include "kmean.h"
#include "stop_watch.h"
#include "utilities.h"
ClusteringWidget::ClusteringWidget(QWidget* parent)
		: QWidget{parent},
			input_{new ImageView{"Original Input", this}},
			output_{new ImageView{"KMean Clustering", this}},
			clusters_{new QSpinBox{this}},
			max_iteration_{new QSpinBox{this}},
			desired_accuracy_{new QDoubleSpinBox{this}},
			browse_{new QPushButton{"Browse", this}},
			start_{new QPushButton{"Start", this}},
			stopwatch_{new StopWatch{this}},
			finish_clustering_watcher_{new QFutureWatcher<QImage>{}} {
	SetupLayout();
	SetupSignalsAndSlots();
}

ClusteringWidget::~ClusteringWidget() = default;

void ClusteringWidget::SetupLayout() {
	auto* clustering_widget_layout = new QGridLayout{this};
	clustering_widget_layout->setSpacing(6);
	clustering_widget_layout->setContentsMargins(12, 12, 12, 12);
	clustering_widget_layout->addWidget(input_, 0, 0, 1, 1);

	auto* control = new QGroupBox{"Control", this};
	control->setAlignment(Qt::AlignCenter);
	auto* control_layout = new QGridLayout{control};
	control_layout->setSpacing(6);
	control_layout->setContentsMargins(12, 12, 12, 12);

	auto* control_config = new QFrame{control};
	control_config->setFrameShape(QFrame::StyledPanel);
	control_config->setFrameShadow(QFrame::Raised);

	auto* control_config_layout = new QFormLayout{control_config};
	control_config_layout->setSpacing(6);
	control_config_layout->setContentsMargins(12, 12, 12, 12);
	control_config_layout->setFieldGrowthPolicy(
			QFormLayout::AllNonFixedFieldsGrow);

	auto* clusters_label = new QLabel{"Clusters", control_config};
	control_config_layout->setWidget(0, QFormLayout::LabelRole, clusters_label);
	control_config_layout->setWidget(0, QFormLayout::FieldRole, clusters_);

	auto* max_iteration_label = new QLabel{"Max Iteration", control_config};
	control_config_layout->setWidget(
			1, QFormLayout::LabelRole, max_iteration_label);
	control_config_layout->setWidget(1, QFormLayout::FieldRole, max_iteration_);

	auto* desired_accuracy_label = new QLabel{"Desired Accuracy", control_config};
	control_config_layout->setWidget(
			2, QFormLayout::LabelRole, desired_accuracy_label);
	control_config_layout->setWidget(
			2, QFormLayout::FieldRole, desired_accuracy_);

	auto* elapsed_time = new QLabel{"Elapsed Time", control_config};
	control_config_layout->setWidget(3, QFormLayout::LabelRole, elapsed_time);
	control_config_layout->setWidget(3, QFormLayout::FieldRole, stopwatch_);

	control_layout->addWidget(
			control_config, 0, 0, 1, 1, Qt::AlignHCenter | Qt::AlignVCenter);

	auto* control_buttons = new QFrame{control};
	control_buttons->setFrameShape(QFrame::StyledPanel);
	control_buttons->setFrameShadow(QFrame::Raised);
	auto* control_buttons_layout = new QHBoxLayout(control_buttons);
	control_buttons_layout->setSpacing(6);
	control_buttons_layout->setContentsMargins(12, 12, 12, 12);
	control_buttons_layout->addWidget(browse_);
	control_buttons_layout->addWidget(start_);

	control_layout->addWidget(
			control_buttons, 1, 0, 1, 1, Qt::AlignHCenter | Qt::AlignVCenter);

	control_layout->setRowStretch(0, 3);
	control_layout->setRowStretch(1, 1);

	clustering_widget_layout->addWidget(control, 0, 1, 1, 1);

	clustering_widget_layout->addWidget(output_, 1, 0, 1, 1);

	clustering_widget_layout->setRowStretch(0, 1);
	clustering_widget_layout->setRowStretch(1, 1);
	clustering_widget_layout->setColumnStretch(0, 1);
	clustering_widget_layout->setColumnStretch(1, 1);

	input_->SetupLayout();
	output_->SetupLayout();
}

void ClusteringWidget::SetupSignalsAndSlots() {
	connect(browse_,
					&QPushButton::clicked,
					this,
					&ClusteringWidget::OnBrowseButtonClicked);
	connect(start_,
					&QPushButton::clicked,
					this,
					&ClusteringWidget::OnStartButtonClicked);
	connect(input_,
					&ImageView::LoadedPixmap,
					[this](const std::optional<QPixmap> pixmap) {
						output_->UpdateImage(QImage{});
						start_->setEnabled(true);
					});

	// setup signal when image finish processed
	connect(
			finish_clustering_watcher_, &QFutureWatcher<QImage>::finished, [this]() {
				output_->UpdateImage(finish_clustering_watcher_->future().result());
				stopwatch_->stop();
				start_->setEnabled(true);
				browse_->setEnabled(true);
			});
	start_->setEnabled(false);
	stopwatch_->stop();
	clusters_->setMinimum(2);
	clusters_->setMaximum(2000);
	max_iteration_->setMinimum(1000);
	max_iteration_->setMaximum(100000);
	desired_accuracy_->setMaximum(0.1);
	desired_accuracy_->setMinimum(0.00001);
	desired_accuracy_->setDecimals(5);
	desired_accuracy_->setSingleStep(0.00005);
}

void ClusteringWidget::OnBrowseButtonClicked() {
	QFileDialog dialog{nullptr, "Open File"};
	dialog.setDirectory(QDir::currentPath().append("/input/"));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	QString filter = "Image files (";
	for (const auto& type : QImageReader::supportedImageFormats()) {
		filter += "*." + type + " ";
	}
	filter += ")";
	dialog.setNameFilter(filter);
	dialog.setFileMode(QFileDialog::ExistingFile);
	while (dialog.exec() == QDialog::Accepted &&
				 !LoadFile(dialog.selectedFiles().first())) {
	}
}

bool ClusteringWidget::LoadFile(const QString& fileName) {
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

	input_->UpdateImage(newImage);

	setWindowFilePath(fileName);

	return true;
}

void ClusteringWidget::OnStartButtonClicked() {
	if (const auto& pixmap = input_->GetPixmap(); pixmap.has_value()) {
		stopwatch_->start();
		output_->UpdateImage(QImage{});
		start_->setEnabled(false);
		browse_->setEnabled(false);

		// create a new thread to image processing
		finish_clustering_watcher_->setFuture(
				QtConcurrent::run([this, pixmap]() -> QImage {
					const auto& image = pixmap.value().toImage().convertToFormat(
							QImage::Format::Format_RGB32);

					const auto& output_mat_image = image_segmentation::kmean(
							QImageToCvMat(image),
							clusters_->value(),
							{desired_accuracy_->value(), max_iteration_->value()});
					return CvMatToQImage(output_mat_image);
				}));
	}
}
