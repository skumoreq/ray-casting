#include "game_log.h"

std::string escape_codes::CursorUp(int num_cells) {
  return kCSI + std::to_string(num_cells) + "A";
}

std::string escape_codes::SelectGraphicRendition(DisplayMode display_mode) {
  return kCSI + std::to_string(static_cast<int>(display_mode)) + "m";
}

std::string game_log::FloatToString(float number) {
  std::ostringstream number_buffer;

  number_buffer << std::fixed << std::setprecision(kDecimalPlaces)
                << std::setw(kNumberFieldWidth) << std::internal
                << number;

  return number_buffer.str();
}

std::string game_log::VectorToString(const Vector& vector) {
  return "(" + FloatToString(vector.x) + ", " + FloatToString(vector.y) + ")";
}

std::string game_log::AccelStateToString(motion::AccelState accel_state) {
  switch (accel_state) {
   case motion::AccelState::kAccelerate: return "accelerate";
   case motion::AccelState::kDeaccelerate: return "deaccelerate";
   default: return "none";
  }
}

std::string game_log::AccelDirectionToString(
    motion::AccelDirection accel_direction) {
  switch (accel_direction) {
   case motion::AccelDirection::kForward: return "forward";
   case motion::AccelDirection::kBackward: return "backward";
   default: return "none";
  }
}

std::string game_log::GenerateLogEntry(
    escape_codes::DisplayMode header_color_fg,
    const game_log::LogEntry& log_entry) {
  using escape_codes::DisplayMode;
  using escape_codes::SelectGraphicRendition;

  std::ostringstream header_buffer;

  header_buffer << std::setw(kHeaderFieldWidth) << std::right
                << log_entry.header;

  return SelectGraphicRendition(DisplayMode::kBold) +
         SelectGraphicRendition(header_color_fg) +
         header_buffer.str() +
         kLogEntrySeperator + " " +
         SelectGraphicRendition(DisplayMode::kNotBold) +
         SelectGraphicRendition(DisplayMode::kBrightWhiteFg) +
         log_entry.value;
}

void game_log::OutputGameLog(float frame_time, const Camera& camera) {
  using escape_codes::DisplayMode;
  using escape_codes::kEraseInLine;
  using escape_codes::CursorUp;
  using escape_codes::SelectGraphicRendition;

  const LogEntry log_entries[] =
  {
    { "FrameRate", FloatToString(1.0f / frame_time) + " FPS" },
    { "FrameTime", FloatToString(frame_time * 1000.0f) + " ms" },
    { "Position", VectorToString(camera.Position()) },
    { "Direction", VectorToString(camera.Direction()) },
    { "Plane", VectorToString(camera.Plane()) },
    { "AccelState", AccelStateToString(camera.AccelState()) },
    { "AccelDirection", AccelDirectionToString(camera.AccelDirection()) },
    { "MovementSpeed", FloatToString(camera.MovementSpeed()) },
    { "RotationSpeed", FloatToString(camera.RotationSpeed()) }
  };
  const int num_log_entries = std::size(log_entries);

  DisplayMode header_color_fg;

  std::cout << SelectGraphicRendition(DisplayMode::kBlackBg)
            << kEraseInLine << '\n';

  for (int i = 0; i < num_log_entries; ++i) {
    // Determines the header color based on the index of the log entry.
    switch (i) {
     case 0:
     case 1:
      header_color_fg = DisplayMode::kBrightRedFg;
      break;

     case 2:
     case 3:
     case 4:
      header_color_fg = DisplayMode::kBrightGreenFg;
      break;

     case 5:
     case 6:
      header_color_fg = DisplayMode::kBrightYellowFg;
      break;

     default:
      header_color_fg = DisplayMode::kBrightBlueFg;
      break;
    }

    // Outputs the formatted log entry
    std::cout << GenerateLogEntry(header_color_fg, log_entries[i])
              << kEraseInLine << '\n';
  }

  // Resets the terminal display mode and moves the cursor back to the start of
  // the log output area.
  std::cout << kEraseInLine
            << SelectGraphicRendition(DisplayMode::kReset)
            << CursorUp(num_log_entries + 1)
            << std::flush;
}
