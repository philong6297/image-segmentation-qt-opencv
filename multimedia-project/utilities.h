#ifndef UTILITIES_H
#define UTILITIES_H
#pragma once
inline cv::Mat QImageToCvMat(const QImage &input,
														 const bool should_clone_data = true) {
	switch (input.format()) {
		// 8-bit, 4 channel
		case QImage::Format_ARGB32:
		case QImage::Format_ARGB32_Premultiplied: {
			cv::Mat mat(input.height(),
									input.width(),
									CV_8UC4,
									const_cast<uchar *>(input.bits()),
									static_cast<size_t>(input.bytesPerLine()));

			return (should_clone_data ? mat.clone() : mat);
		}

		// 8-bit, 3 channel
		case QImage::Format_RGB32: {
			if (!should_clone_data) {
				qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning so "
											"we don't modify the original QImage data";
			}

			cv::Mat mat(input.height(),
									input.width(),
									CV_8UC4,
									const_cast<uchar *>(input.bits()),
									static_cast<size_t>(input.bytesPerLine()));

			cv::Mat matNoAlpha;

			cv::cvtColor(mat,
									 matNoAlpha,
									 cv::COLOR_BGRA2BGR);  // drop the all-white alpha channel

			return matNoAlpha;
		}

		// 8-bit, 3 channel
		case QImage::Format_RGB888: {
			if (!should_clone_data) {
				qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning so "
											"we don't modify the original QImage data";
			}

			QImage swapped = input.rgbSwapped();

			return cv::Mat(swapped.height(),
										 swapped.width(),
										 CV_8UC3,
										 const_cast<uchar *>(swapped.bits()),
										 static_cast<size_t>(swapped.bytesPerLine()))
					.clone();
		}

		// 8-bit, 1 channel
		case QImage::Format_Indexed8:
		case QImage::Format_Grayscale8: {
			cv::Mat mat(input.height(),
									input.width(),
									CV_8UC1,
									const_cast<uchar *>(input.bits()),
									static_cast<size_t>(input.bytesPerLine()));

			return (should_clone_data ? mat.clone() : mat);
		}

		default:
			qWarning()
					<< "ASM::QImageToCvMat() - QImage format not handled in switch:"
					<< input.format();
			break;
	}

	return cv::Mat();
}

// NOTE: This does not cover all cases - it should be easy to add new ones as
// required.
inline QImage CvMatToQImage(const cv::Mat &inMat) {
	switch (inMat.type()) {
		// 8-bit, 4 channel
		case CV_8UC4: {
			QImage image(inMat.data,
									 inMat.cols,
									 inMat.rows,
									 static_cast<int>(inMat.step),
									 QImage::Format_ARGB32);

			return image;
		}

		// 8-bit, 3 channel
		case CV_8UC3: {
			QImage image(inMat.data,
									 inMat.cols,
									 inMat.rows,
									 static_cast<int>(inMat.step),
									 QImage::Format_RGB888);

			return image.rgbSwapped();
		}

		// 8-bit, 1 channel
		case CV_8UC1: {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
			QImage image(inMat.data,
									 inMat.cols,
									 inMat.rows,
									 static_cast<int>(inMat.step),
									 QImage::Format_Grayscale8);
#else
			static QVector<QRgb> sColorTable;

			// only create our color table the first time
			if (sColorTable.isEmpty()) {
				sColorTable.resize(256);

				for (int i = 0; i < 256; ++i) {
					sColorTable[i] = qRgb(i, i, i);
				}
			}

			QImage image(inMat.data,
									 inMat.cols,
									 inMat.rows,
									 static_cast<int>(inMat.step),
									 QImage::Format_Indexed8);

			image.setColorTable(sColorTable);
#endif

			return image;
		}

		default:
			qWarning()
					<< "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:"
					<< inMat.type();
			break;
	}

	return QImage();
}
#endif  // !UTILITIES_H
