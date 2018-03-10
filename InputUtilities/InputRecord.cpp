#include "InputRecord.hpp"
#include <algorithm>

namespace chrono = std::chrono;
using namespace std::literals;

InputRecord::InputAction::InputAction(chrono::microseconds time, const INPUT & input) noexcept
	: time(time)
	, input (input) {}

InputRecord::InputAction::~InputAction() noexcept {}







InputRecord::InputRecord(RecordType recordType)
	: recordType(recordType) {}


InputRecord::~InputRecord() {}

InputRecord::RecordType InputRecord::get_record_type() const {
	return recordType;
}

void InputRecord::set_record_type(RecordType recordType) {
	this->recordType = recordType;
}

InputRecord::InputAction& InputRecord::add(const InputAction & action) {
	actions.push_back(action);
	totalTime.reset();
	return actions.back();
}

InputRecord::InputAction& InputRecord::add(chrono::microseconds time, const INPUT & input) {
	actions.emplace_back(time, input);
	totalTime.reset();
	return actions.back();
}

void InputRecord::clear() {
	actions.clear();
	totalTime.reset();
}

bool InputRecord::empty() const {
	return actions.empty();
}

const std::vector<InputRecord::InputAction>& InputRecord::get_actions() const {
	return actions;
}

void InputRecord::reserve(std::size_t size) {
	actions.reserve(size);
}

std::size_t InputRecord::size() const {
	return actions.size();
}

chrono::microseconds InputRecord::total_time() const {
	if (totalTime.has_value())
		return totalTime.value();
	totalTime = 0us;
	for (auto &x : actions)
		totalTime.value() += x.time;
	return totalTime.value();
}










void InputRecord::make_mouse_only() {
	if (recordType != RecordType::MouseAndKeyboard)
		return;
	recordType = RecordType::Mouse;
	auto it = std::remove_if(
		actions.begin(),
		actions.end(),
		[](const InputAction &action) {
		return action.input.type != INPUT_MOUSE;
	});
	if (it != actions.end())
		actions.erase(
			it,
			actions.end()
		);
	totalTime.reset();
}

void InputRecord::make_keyboard_only() {
	if (recordType != RecordType::MouseAndKeyboard)
		return;
	recordType = RecordType::Keyboard;
	auto it = std::remove_if(
		actions.begin(),
		actions.end(),
		[](const InputAction &action) {
		return action.input.type != INPUT_KEYBOARD;
	});
	if (it != actions.end())
		actions.erase(
			it,
			actions.end()
		);
	totalTime.reset();
}

void InputRecord::remove_wait_time_before_first_action() {
	if (!actions.empty()) {
		actions[0].time = 0us;
		totalTime.reset();
	}
}

void InputRecord::change_speed(double speed) {
	for (InputAction &action : actions) {
		double newTime = action.time.count() * speed;
		chrono::microseconds::rep newTimeInt = static_cast<chrono::microseconds::rep>(newTime);
		action.time = chrono::microseconds(newTimeInt);
	}
	totalTime.reset();
}
