/*
 * This is a personal academic project. Dear PVS-Studio, please check it.
 * PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
 */
#include "image_view.h"
using gsl::make_not_null;
using gsl::owner;
ImageView::ImageView(std::string_view title, QWidget* parent)
		: QGroupBox{title.data(), parent},
			label_image_{make_not_null(owner<QLabel*>{new QLabel{this}})},
			layout_manager_{
					make_not_null(owner<QHBoxLayout*>{new QHBoxLayout{this}})},
			pixmap_{std::nullopt} {}

ImageView::~ImageView() = default;

void ImageView::UpdateImage(std::optional<QPixmap> pixmap) {
	pixmap_ = pixmap;
	if (pixmap.has_value()) {
		label_image_->setPixmap(pixmap.value());
	}
	else {
		label_image_->setText("");
	}
	emit LoadedPixmap(pixmap_);
}

void ImageView::UpdateImage(std::optional<QImage> image) {
	std::optional<QPixmap> pixmap{};
	if (image.has_value()) {
		const auto scaled_image =
				image.value().scaled(label_image_->size(),
														 Qt::AspectRatioMode::KeepAspectRatio,
														 Qt::TransformationMode::SmoothTransformation);
		pixmap = QPixmap::fromImage(scaled_image);
	}
	UpdateImage(pixmap);
}

void ImageView::SetupLayout() {
	setAlignment(Qt::AlignCenter);
	layout_manager_->setSpacing(6);
	layout_manager_->setContentsMargins(12, 12, 12, 12);
	layout_manager_->addWidget(label_image_);
	label_image_->setAlignment(Qt::AlignCenter);
	setLayout(layout_manager_);
}

std::optional<QPixmap> ImageView::GetPixmap() const {
	return pixmap_;
}
