#ifndef IMAGE_VIEW_H
#define IMAGE_VIEW_H
#pragma once

#include <optional>
/* TODO: Create a real image view
 * this still depends on QGroupBox to make an image view with title
 */
class ImageView final : public QGroupBox {
	Q_OBJECT
 public:
	explicit ImageView(std::string_view title, QWidget* parent = nullptr);
	~ImageView() override;
	MAKE_NOT_COPYABLE_NOR_MOVEABLE(ImageView)
	void SetupLayout();
	std::optional<QPixmap> GetPixmap() const;
 signals:
	void LoadedPixmap(std::optional<QPixmap> pixmap) const;

 public slots:
	void UpdateImage(std::optional<QPixmap> pixmap);
	void UpdateImage(std::optional<QImage> image);

 private:
	const gsl::not_null<QLabel*> label_image_;
	const gsl::not_null<QHBoxLayout*> layout_manager_;
	std::optional<QPixmap> pixmap_{};
};

#endif  // !IMAGE_VIEW_H
