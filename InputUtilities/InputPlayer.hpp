#pragma once

#ifdef RECORD_DLL
#define RECORD_EXPORT __declspec(dllexport)
#else
#define RECORD_EXPORT __declspec(dllimport)
#endif

#include "InputRecord.hpp"
#include <thread>
#include <atomic>
#include <future>

class InputPlayer {
public:
	RECORD_EXPORT explicit InputPlayer(const InputRecord &record = InputRecord());
	RECORD_EXPORT ~InputPlayer();
	RECORD_EXPORT void set_record(const InputRecord &record);
	RECORD_EXPORT void set_record(InputRecord &&record) noexcept;
	RECORD_EXPORT const InputRecord& get_record() const noexcept;

	RECORD_EXPORT void start_playing(double speed);
	RECORD_EXPORT void stop_playing();
	RECORD_EXPORT void wait_until_finished();
	RECORD_EXPORT bool is_playing() const;
private:
	RECORD_EXPORT void playback_procedure(double speed);

	InputRecord record;
	std::promise<void> playStartEvent;
	std::future<void> playingStopEventFuture;
	std::atomic_bool stopFlag, isPlaying;
};

