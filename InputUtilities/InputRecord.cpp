#include "InputRecord.hpp"
#include <algorithm>
namespace chrono = std::chrono;
using namespace std::literals;


InputRecord::InputAction::InputAction(chrono::microseconds time, const INPUT & input) noexcept
	: time(time)
	, input(input) {}

InputRecord::InputAction::~InputAction() noexcept {}










InputRecord::InputRecord(RecordType recordType) noexcept
	: recordType(recordType)
	, actions()
	, totalTime() {}


InputRecord::~InputRecord() {}





InputRecord::RecordType InputRecord::get_record_type() const noexcept {
	return recordType;
}

void InputRecord::set_record_type(RecordType recordType) noexcept {
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




void InputRecord::clear() noexcept {
	actions.clear();
	totalTime.reset();
}

bool InputRecord::empty() const noexcept {
	return actions.empty();
}

const std::vector<InputRecord::InputAction>& InputRecord::get_actions() const noexcept {
	return actions;
}

void InputRecord::reserve(std::size_t capacity) {
	actions.reserve(capacity);
}

std::size_t InputRecord::size() const noexcept {
	return actions.size();
}






chrono::microseconds InputRecord::total_time() const noexcept {
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
		actions.begin(), actions.end(),
		[](const InputAction &action) {
			return action.input.type != INPUT_MOUSE;
	});
	if (it != actions.end())
		actions.erase(it, actions.end());
	totalTime.reset();
}



void InputRecord::make_keyboard_only() {
	if (recordType != RecordType::MouseAndKeyboard)
		return;
	recordType = RecordType::Keyboard;
	auto it = std::remove_if(
		actions.begin(), actions.end(),
		[](const InputAction &action) {
			return action.input.type != INPUT_KEYBOARD;
	});
	if (it != actions.end())
		actions.erase(it, actions.end());
	totalTime.reset();
}



void InputRecord::remove_first_wait_time() noexcept {
	if (!actions.empty()) {
		actions[0].time = 1us;
		totalTime.reset();
	}
}



void InputRecord::change_speed(double speed) noexcept {
	for (InputAction &action : actions)
		action.time = chrono::duration_cast<chrono::microseconds>(action.time / speed);
	totalTime.reset();
}
