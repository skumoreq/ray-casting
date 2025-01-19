#ifndef GAME_LOG_H_
#define GAME_LOG_H_

#include <iostream>
#include <iomanip>
#include <string>

#include "vector.h"
#include "camera.h"

/*
 * game_log.h
 *
 * This header defines logging utilities for the game, including formatting
 * and outputting game-related data such as frame timing, camera state.
 *
 * It supports various data types like floats, vectors, and enumerations,
 * using escape codes for terminal formatting to enhance readability.
 */

namespace escape_codes {

// Enum representing different display modes for text styling.
enum class DisplayMode {
  kReset = 0,

  kBold = 1,
  kNotBold = 22,

  kRedFg = 31,
  kBrightRedFg = 91,
  kBrightGreenFg = 92,
  kBrightYellowFg = 93,
  kBrightBlueFg = 94,
  kBrightWhiteFg = 97,

  kBlackBg = 40
};

// Control Sequence Introducer ESC [
const std::string kCSI = "\033[";

// ANSI escape sequences for clearing display or line content.
const std::string kEraseInDisplay = kCSI + 'J';
const std::string kEraseInLine = kCSI + 'K';

// ANSI escape sequences for showing and hiding the cursor.
const std::string kShowTheCursor = kCSI + "?25h";
const std::string kHideTheCursor = kCSI + "?25l";

// Moves the cursor up by a specified number of cells.
std::string CursorUp(int num_cells);

// Returns an ANSI escape sequence to set text graphic rendition based on the
// specified display mode.
std::string SelectGraphicRendition(DisplayMode display_mode);

}  // namespace escape_codes

namespace game_log {

// Right-Pointing Double Angle Quotation Mark »
const std::string kLogEntrySeperator = u8"\u00BB";

constexpr int kDecimalPlaces = 2;
constexpr int kNumberFieldWidth = 5;
constexpr int kHeaderFieldWidth = 15;

struct LogEntry {
  std::string header;
  std::string value;
};

// Returns a formatted string representation of a float value.
// The number is formatted in fixed-point notation with a specified number of
// decimal places, justified within a defined field width.
std::string FloatToString(float number);

// Returns a formatted string representation of a vector in the format:
// ([x], [y]).
std::string VectorToString(const Vector& vector);

std::string AccelStateToString(motion::AccelState accel_state);
std::string AccelDirectionToString(motion::AccelDirection accel_direction);

// Returns a formatted string representation of a log entry.
// The format includes a header styled with bold and a specified color, followed
// by a separator and a value styled in bright white.
std::string GenerateLogEntry(
    escape_codes::DisplayMode header_color_fg,
    const LogEntry& log_entry);

// Outputs game log entries to the standard output stream.
// This function retrieves various game-related data (such as frame time and
// camera state), formats it appropriately, and displays it in a readable format
// using ANSI escape sequences.
void OutputGameLog(float frame_time, const Camera& camera);

}  // namespace game_log

#endif
