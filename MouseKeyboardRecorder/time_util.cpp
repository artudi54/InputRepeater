#include "stdafx.h"
#include "time_util.hpp"
namespace chrono = std::chrono;
using namespace std::literals;

namespace time_util {
	std::pair<unsigned, unsigned> minute_second_time(std::chrono::microseconds time) {
		return std::make_pair(
			static_cast<unsigned>(time / 1min),
			static_cast<unsigned>(
				chrono::duration_cast<chrono::seconds>(time % 1min).count()
				)
		);
	}

	QString single_printable_time(std::chrono::microseconds time) {
		std::pair<unsigned, unsigned> printTime = minute_second_time(time);
		return QString("%1:%2")
				.arg(printTime.first, 2, 10, QChar('0'))
				.arg(printTime.second, 2, 10, QChar('0'));

	}

	QString double_printable_time(std::chrono::microseconds currentTime, std::chrono::microseconds totalTime) {
		std::pair<unsigned, unsigned> printTime = minute_second_time(currentTime);
		std::pair<unsigned, unsigned> printTotal = minute_second_time(totalTime);
		return QString("%1:%2/%3:%4")
				.arg(printTime.first, 2, 10, QChar('0'))
				.arg(printTime.second, 2, 10, QChar('0'))
				.arg(printTotal.first, 2, 10, QChar('0'))
				.arg(printTotal.second, 2, 10, QChar('0'));
	}
}