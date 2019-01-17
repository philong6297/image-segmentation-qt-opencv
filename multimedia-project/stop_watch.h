#ifndef STOP_WATCH_H
#define STOP_WATCH_H
#pragma once
class StopWatch : public QLabel {
	Q_OBJECT
 public:
	explicit StopWatch(QWidget *parent = 0)
			: QLabel("0:0:0:0"), mRunning(false), mStartTime() {
		startTimer(0);
	}

 public slots:
	void start(void) {
		mStartTime = QDateTime::currentDateTime();
		mRunning = true;
	}

	void stop(void) { mRunning = false; }

 protected:
	void timerEvent(QTimerEvent *) {
		if (mRunning) {
			qint64 ms = mStartTime.msecsTo(QDateTime::currentDateTime());
			int h = ms / 1000 / 60 / 60;
			int m = (ms / 1000 / 60) - (h * 60);
			int s = (ms / 1000) - (m * 60);
			ms = ms - (s * 1000);
			QString diff = QString("%1:%2:%3:%4").arg(h).arg(m).arg(s).arg(ms);
			setText(diff);
		}
	}

 private:
	bool mRunning;
	QDateTime mStartTime;
};
#endif  // !STOP_WATCH_H
