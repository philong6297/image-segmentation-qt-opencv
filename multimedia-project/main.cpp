/*
* This is a personal academic project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/

#include "main_window.h"

int main(int argc, char *argv[]) {
	[[maybe_unused]] QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return QApplication::exec();
}
