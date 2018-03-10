#pragma once

#include "ui_ReplayTabWidget.h"
#include "Options.hpp"
#include <InputPlayer.hpp>

class ReplayTabWidget : public QWidget {
	Q_OBJECT
public:
	explicit ReplayTabWidget(QWidget *parent = nullptr);
	virtual ~ReplayTabWidget();
	void set_options(Options &options);
	void set_record(const InputRecord &record);
	void set_title(const QString &title);
signals:
	void replay_started();
	void replay_time_changed(std::chrono::microseconds currentTime, std::chrono::microseconds totalTime);
	void replay_stopped();
public slots:
	void start_playing();
	void stop_playing();
private:
	void notifier_procedure();
	void connect_signals();
	void set_time(std::chrono::microseconds currentTime, std::chrono::microseconds totalTime);

	Ui::ReplayTabWidget ui;
	Options *options;
	InputPlayer player;

	QTimer notifierTimer;
	std::chrono::steady_clock::time_point start;
	unsigned repeatCount;
	double speed;
};
