#include "stdafx.h"
#include "InputRepeaterStatusBar.hpp"
#include "time_util.hpp"
namespace chrono = std::chrono;
using namespace std::literals;

InputRepeaterStatusBar::InputRepeaterStatusBar(QWidget *parent)
	: QStatusBar(parent)
	, labelStatus(QStringLiteral("Status: None\t"))
	, labelTime(this) {
	this->addWidget(&labelStatus);
	this->addPermanentWidget(&labelTime);
	this->reset_time();
}

InputRepeaterStatusBar::~InputRepeaterStatusBar() {}






void InputRepeaterStatusBar::set_status(const QString & status, const QString & color) {
	labelStatus.setText(
		" Status: <font color=\"" + color + "\">" + status + "</font>\t"
	);
}

void InputRepeaterStatusBar::reset_time() {
	labelTime.setText(QStringLiteral("Time: None "));
}

void InputRepeaterStatusBar::set_time(chrono::microseconds time) {
	labelTime.setText("Time: " + time_util::single_printable_time(time) + ' ');
}

void InputRepeaterStatusBar::set_time(chrono::microseconds currentTime, chrono::microseconds totalTime) {
	labelTime.setText("Time: " + time_util::double_printable_time(currentTime, totalTime) + ' ');
}
