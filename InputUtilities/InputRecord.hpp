#pragma once

#ifdef RECORD_DLL
#define RECORD_EXPORT __declspec(dllexport)
#else
#define RECORD_EXPORT __declspec(dllimport)
#endif

#include <chrono>
#include <vector>
#include <optional>
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

class InputRecord {
public:
	enum class RecordType : std::uint8_t {
		None,
		Mouse,
		Keyboard,
		MouseAndKeyboard
	};

	struct InputAction {
		RECORD_EXPORT InputAction() noexcept = default;
		RECORD_EXPORT InputAction(const InputAction&) noexcept = default;
		RECORD_EXPORT InputAction(InputAction&&) noexcept = default;
		RECORD_EXPORT InputAction& operator=(const InputAction&) noexcept = default;
		RECORD_EXPORT InputAction& operator=(InputAction&&) noexcept = default;

		RECORD_EXPORT InputAction(std::chrono::microseconds time, const INPUT &input) noexcept;
		RECORD_EXPORT ~InputAction() noexcept;

		std::chrono::microseconds time;
		INPUT input;
	};
	
	RECORD_EXPORT InputRecord() noexcept = default;
	RECORD_EXPORT InputRecord(const InputRecord&) = default;
	RECORD_EXPORT InputRecord(InputRecord&&) noexcept = default;
	RECORD_EXPORT InputRecord& operator=(const InputRecord&) = default;
	RECORD_EXPORT InputRecord& operator=(InputRecord&&) noexcept = default;

	RECORD_EXPORT InputRecord(RecordType recordType) noexcept;
	RECORD_EXPORT ~InputRecord();

	RECORD_EXPORT RecordType get_record_type() const noexcept;
	RECORD_EXPORT void set_record_type(RecordType recordType) noexcept;
	
	RECORD_EXPORT InputAction& add(const InputAction &action);
	RECORD_EXPORT InputAction& add(std::chrono::microseconds time, const INPUT &input);

	RECORD_EXPORT void clear() noexcept;
	RECORD_EXPORT bool empty() const noexcept;
	RECORD_EXPORT const std::vector<InputAction>& get_actions() const noexcept;
	RECORD_EXPORT void reserve(std::size_t capacity);
	RECORD_EXPORT std::size_t size() const noexcept;
	
	RECORD_EXPORT std::chrono::microseconds total_time() const noexcept;

	RECORD_EXPORT void make_mouse_only();
	RECORD_EXPORT void make_keyboard_only();
	RECORD_EXPORT void remove_first_wait_time() noexcept;
	RECORD_EXPORT void change_speed(double speed) noexcept;
private:
	RecordType recordType;
	std::vector<InputAction> actions;
	mutable std::optional<std::chrono::microseconds> totalTime;
};

