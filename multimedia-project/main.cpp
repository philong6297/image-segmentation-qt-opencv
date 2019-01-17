/*
 * This is a personal academic project. Dear PVS-Studio, please check it.
 * PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
 */
#include "main_window.h"
int main() {
	auto argc = 0;
	const gsl::owner<char **> argv{nullptr};
	[[maybe_unused]] auto app = std::make_unique<QApplication>(argc, argv);
	// Read input image
	MainWindow window{};
	window.show();
	return QApplication::exec();
}
