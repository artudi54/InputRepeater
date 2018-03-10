#pragma once

namespace time_util {
	std::pair<unsigned, unsigned> minute_second_time(std::chrono::microseconds time);
	QString single_printable_time(std::chrono::microseconds time);
	QString double_printable_time(std::chrono::microseconds currentTime, std::chrono::microseconds totalTime);
}