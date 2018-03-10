#pragma once
#include <chrono>

class InputRepeaterStatusBar : public QStatusBar {
	Q_OBJECT
public:
	explicit InputRepeaterStatusBar(QWidget *parent);
	virtual ~InputRepeaterStatusBar();

	void set_status(const QString &status, const QString &color = QStringLiteral("black"));
	void reset_time();
	void set_time(std::chrono::microseconds time);
	void set_time(std::chrono::microseconds currentTime, std::chrono::microseconds totalTime);
private:
	QLabel labelStatus, labelTime;
};
