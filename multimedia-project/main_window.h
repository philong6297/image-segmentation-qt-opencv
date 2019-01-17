#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#pragma once

class MainWindow final : public QMainWindow {
	Q_OBJECT

 public:
	explicit MainWindow();
	~MainWindow() override = default;
	MAKE_NOT_COPYABLE_NOR_MOVEABLE(MainWindow)
};

#endif  // !MAIN_WINDOW_H
