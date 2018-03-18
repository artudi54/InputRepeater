#include "InputPlayer.hpp"
namespace chrono = std::chrono;
using namespace std::literals;

InputPlayer::InputPlayer(const InputRecord &record)
	: record(record)
	, playStartEvent()
	, playingStopEventFuture()
	, stopFlag(false)
	, isPlaying(false) {}

InputPlayer::~InputPlayer() {
	this->stop_playing();
}




void InputPlayer::set_record(const InputRecord& record) {
	this->record = record;
}

void InputPlayer::set_record(InputRecord&& record) noexcept {
	this->record = std::move(record);
}


const InputRecord & InputPlayer::get_record() const noexcept {
	return record;
}





void InputPlayer::start_playing(double speed) {
	if (isPlaying)
		this->stop_playing();
	playingStopEventFuture = std::async(&InputPlayer::playback_procedure, this, speed);
	playStartEvent.get_future().get(); //sync with start
	playStartEvent = std::promise<void>();
}

void InputPlayer::stop_playing() {
	stopFlag = true;
	this->wait_until_finished();
}

void InputPlayer::wait_until_finished() {
	if (playingStopEventFuture.valid()) {
		playingStopEventFuture.get();
		playingStopEventFuture = std::future<void>();
	}
}

bool InputPlayer::is_playing() const {
	return isPlaying;
}




void InputPlayer::playback_procedure(double speed) {
	stopFlag = false;
	isPlaying = true;
	playStartEvent.set_value();

	const std::vector<InputRecord::InputAction> &actions = record.get_actions();
	chrono::steady_clock::time_point pt = chrono::steady_clock::now();
	std::size_t playIndex = 0;
	chrono::steady_clock::time_point awaitedTime =
		pt + chrono::duration_cast<chrono::microseconds>(actions[0].time / speed);

	while (playIndex < actions.size() && !stopFlag) {
		pt = chrono::steady_clock::now();
		if (pt >= awaitedTime) {
			INPUT inputs[] = { actions[playIndex].input };
			::SendInput(1, inputs, sizeof(INPUT));
			++playIndex;
			if (playIndex < actions.size()) {
				awaitedTime +=
					chrono::duration_cast<chrono::microseconds>(actions[playIndex].time / speed);
				pt = chrono::steady_clock::now();
			}
		}
		std::this_thread::sleep_for(
			std::clamp(
				awaitedTime - pt,
				chrono::steady_clock::duration(0ms),
				chrono::steady_clock::duration(1ms)
			)
		);
	}
	isPlaying = false;
}
