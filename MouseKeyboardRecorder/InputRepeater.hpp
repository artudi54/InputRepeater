#pragma once

#include "ui_InputRepeater.h"
#include <QHotkey>



class InputRepeater : public QMainWindow {
	Q_OBJECT
public:
	InputRepeater(QWidget *parent = nullptr);
	virtual void closeEvent(QCloseEvent *event) override;
private slots:
	void set_hotkeys();
	void record_started();
	void record_stopped(RecordTabWidget::RecordResult result);
	void record_updated();
	void record_unloaded();
	void replay_started();
	void replay_stopped();

	void auto_disable_hotkey();
private:
	void single_play(const QUrl &media);
	void load_options();
	void connect_signals();
	Ui::InputRepeater ui;
	QSystemTrayIcon trayIcon;
	Options options;
	QHotkey recordStartHotkey, recordStopHotkey;
	QHotkey replayStartHotkey, replayStopHotkey;
	static const QString OPTIONS_FILE;
};
