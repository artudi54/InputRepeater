#include "stdafx.h"
#include "ReplayTabWidget.hpp"
#include "time_util.hpp"
namespace chrono = std::chrono;
using namespace std::literals;

ReplayTabWidget::ReplayTabWidget(QWidget *parent)
	: QWidget(parent)
	, options(nullptr)
	, player()
	, notifierTimer(this)
	, start()
	, repeatCount(0)
	, speed(1.0) {
	ui.setupUi(this);
	this->connect_signals();
}

ReplayTabWidget::~ReplayTabWidget() {}






void ReplayTabWidget::set_options(Options & options) noexcept {
	this->options = &options;
}

void ReplayTabWidget::set_record(const InputRecord & record) {
	this->stop_playing();
	player.set_record(record);
	this->set_time(
		0us,
		chrono::duration_cast<chrono::microseconds>(record.total_time() / speed)
	);
	ui.buttonReplay->setEnabled(!record.empty());
}

bool ReplayTabWidget::has_record() const noexcept {
	return !player.get_record().empty();
}

void ReplayTabWidget::set_title(const QString & title) {
	if (title.isEmpty())
		ui.labelTitle->setText(QStringLiteral("Title: Unsaved"));
	else
		ui.labelTitle->setText("Title: " + title);
}









void ReplayTabWidget::start_playing() {
	this->stop_playing();
	if (this->has_record()) {
		ui.buttonReplay->setEnabled(false);
		ui.buttonStop->setEnabled(true);
		if (ui.checkRepeatInfinitely->isChecked())
			repeatCount = -1;
		else
			repeatCount = ui.spinRepeatCount->value();

		speed = ui.spinSpeed->value();
		player.start_playing(speed);
		start = chrono::steady_clock::now();
		notifierTimer.start((20ms).count());
		emit replay_started();
		emit replay_time_changed(
			0us,
			chrono::duration_cast<chrono::microseconds>(player.get_record().total_time() / speed)
		);
	}
}

void ReplayTabWidget::stop_playing() {
	if (notifierTimer.isActive()) { // is playing
		ui.buttonReplay->setEnabled(true);
		ui.buttonStop->setEnabled(false);
		notifierTimer.stop();
		player.stop_playing();
		emit replay_time_changed(
			chrono::duration_cast<chrono::microseconds>(player.get_record().total_time() / speed),
			chrono::duration_cast<chrono::microseconds>(player.get_record().total_time() / speed)
		);
		emit replay_stopped();
	}
}




void ReplayTabWidget::notifier_procedure() {
	chrono::steady_clock::time_point current = chrono::steady_clock::now();

	if (current - start > player.get_record().total_time() / speed) {
		if (!--repeatCount) {
			player.wait_until_finished();
			this->stop_playing();
		} else {
			player.start_playing(speed);
			start = chrono::steady_clock::now();
			emit replay_time_changed(
				0us,
				chrono::duration_cast<chrono::microseconds>(player.get_record().total_time() / speed)
			);
		}
		return;
	}

	emit replay_time_changed(
		chrono::duration_cast<chrono::microseconds>(current - start),
		chrono::duration_cast<chrono::microseconds>(player.get_record().total_time() / speed)
	);
}





void ReplayTabWidget::connect_signals() {
	QObject::connect(ui.buttonReplay, &QPushButton::clicked, this, &ReplayTabWidget::start_playing);
	QObject::connect(ui.buttonStop, &QPushButton::clicked, this, &ReplayTabWidget::stop_playing);
	
	QObject::connect(ui.checkRepeatInfinitely, &QCheckBox::toggled, ui.spinRepeatCount, &QSpinBox::setDisabled);

	QObject::connect(this, &ReplayTabWidget::replay_time_changed, this, &ReplayTabWidget::set_time);

	QObject::connect(&notifierTimer, &QTimer::timeout, this, &ReplayTabWidget::notifier_procedure);
}


void ReplayTabWidget::set_time(std::chrono::microseconds currentTime, std::chrono::microseconds totalTime) {
	ui.labelTime->setText(time_util::double_printable_time(
		chrono::duration_cast<chrono::microseconds>(currentTime * speed),
		chrono::duration_cast<chrono::microseconds>(totalTime * speed)
	));
}
