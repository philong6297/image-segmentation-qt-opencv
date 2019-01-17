/*
 * This is a personal academic project. Dear PVS-Studio, please check it.
 * PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
 */
#include "main_window.h"
#include "clustering_widget.h"
#include "threshold_widget.h"
MainWindow::MainWindow() : QMainWindow{nullptr} {
	auto* central = new QTabWidget{this};
	central->addTab(new ThresholdWidget{central}, "Thresholding");
	central->addTab(new ClusteringWidget{central}, "Clustering");
	setCentralWidget(central);
	setFixedSize(QGuiApplication::primaryScreen()->geometry().size() * 0.8);
}
