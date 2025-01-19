#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <limits>

#include <SDL2/SDL.h>

#include "level_data.h"
#include "vector.h"
#include "camera.h"
#include "game_log.h"

std::string GenerateSDLErrorMessage(const std::string error_context);

float DegreesToRadians(float degrees);
float CalculateFrameTime();

void LogGameActivity(float frame_time, const Camera& camera);
void HandleKeyboardEvent(
    const SDL_KeyboardEvent& keyboard_event,
    Camera* camera);

void RenderBackground(SDL_Renderer* renderer);
void RenderWallSegment(
    SDL_Renderer* renderer,
    const raycasting::RayData& ray_data,
    int x);

// Constants for window dimensions.
constexpr int kWindowWidth = 1920;
constexpr int kWindowHeight = 1080;
constexpr int kMaxWindowY = kWindowHeight - 1;

int main() {
  // Initialize SDL create window and renderer.
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cout << GenerateSDLErrorMessage("SDL could not initialize!")
              << std::endl;
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow(
      "ray-casting",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      kWindowWidth,
      kWindowHeight,
      0);

  if (window == nullptr) {
    std::cout << GenerateSDLErrorMessage("Renderer could not be created!")
              << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(
      window,
      -1,
      SDL_RENDERER_ACCELERATED);

  if (renderer == nullptr) {
    std::cout << GenerateSDLErrorMessage("Renderer could not be created!")
              << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Game variables.
  bool running = true;
  float frame_time = 0.0f;

  // Initialize camera.
  Camera camera(
      22.0f, 12.0f,
      DegreesToRadians(180.0f),
      DegreesToRadians(90.0f));

  std::cout << escape_codes::kHideTheCursor;

  while (running) {
    // Calculate frame time and log game activity.
    frame_time = CalculateFrameTime();
    LogGameActivity(frame_time, camera);

    // Poll for SDL events.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
       case SDL_QUIT:
        running = false;
        break;

       case SDL_KEYDOWN:
       case SDL_KEYUP:
        HandleKeyboardEvent(event.key, &camera);
        break;
      }
    }

    // Update camera movement based on frame time.
    camera.SetMovementSpeed(frame_time);
    camera.HandleMotion(frame_time);

    // Render background (floor and ceiling).
    RenderBackground(renderer);

    // Loop through all screen width pixels and render wall segments.
    for (int x = 0; x < kWindowWidth; x++) {
      float plane_scalar = (2.0f * x) / (kWindowWidth - 1.0f) - 1.0f;

      raycasting::RayData ray_data = camera.CalculateRay(plane_scalar);

      RenderWallSegment(renderer, ray_data, x);
    }

    SDL_RenderPresent(renderer);
  }

  std::cout << escape_codes::kEraseInDisplay
            << escape_codes::kShowTheCursor
            << std::flush;

  // Clean up SDL and resources before exiting.
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

float DegreesToRadians(float degrees) {
  static constexpr float kPi = 2.0f * std::acos(0.0f);

  return std::fmod(degrees, 360.0f) * (kPi / 180.0f);
}

float CalculateFrameTime() {
  static Uint32 last_time = SDL_GetTicks();

  const float frame_time = (SDL_GetTicks() - last_time) / 1000.0f;

  last_time = SDL_GetTicks();

  return frame_time;
}

std::string GenerateSDLErrorMessage(const std::string error_context) {
  using escape_codes::DisplayMode;
  using escape_codes::SelectGraphicRendition;

  return error_context + '\n' +
         SelectGraphicRendition(DisplayMode::kBold) +
         SelectGraphicRendition(DisplayMode::kRedFg) +
         "SDL_Error: " +
         SelectGraphicRendition(DisplayMode::kReset) +
         SDL_GetError();
}

void LogGameActivity(float frame_time, const Camera& camera) {
  static Uint32 last_time = SDL_GetTicks();
  static float sum_frame_time = 0;
  static int frame_count = 0;

  const Uint32 elapsed_time = SDL_GetTicks() - last_time;

  sum_frame_time += frame_time;
  frame_count++;

  if (elapsed_time > 100) {
    game_log::OutputGameLog(sum_frame_time / frame_count, camera);

    last_time = SDL_GetTicks();
    sum_frame_time = 0.0f;
    frame_count = 0;
  }
}

void HandleKeyboardEvent(
    const SDL_KeyboardEvent& keyboard_event,
    Camera* camera) {
  if (keyboard_event.repeat) return; // Ignore repeated key events.

  motion::AccelState accel_state = motion::AccelState::kNone;
  motion::RotationDirection rotation_direction = motion::RotationDirection::kNone;

  if (keyboard_event.state == SDL_PRESSED) {
    accel_state = motion::AccelState::kAccelerate;
  } else if (keyboard_event.state == SDL_RELEASED) {
    accel_state = motion::AccelState::kDeaccelerate;
  }

  switch (keyboard_event.keysym.sym) {
   case SDLK_w:
    camera->SetAcceleration(accel_state, motion::AccelDirection::kForward);
    break;

   case SDLK_s:
    camera->SetAcceleration(accel_state, motion::AccelDirection::kBackward);
    break;

   case SDLK_a:
    if (keyboard_event.state == SDL_PRESSED) {
      rotation_direction = motion::RotationDirection::kCounterclockwise;
    }
    camera->SetRotationSpeed(rotation_direction);
    break;

   case SDLK_d:
    if (keyboard_event.state == SDL_PRESSED) {
      rotation_direction = motion::RotationDirection::kClockwise;
    }
    camera->SetRotationSpeed(rotation_direction);
    break;
  }
}

void RenderBackground(SDL_Renderer* renderer) {
  static const SDL_Color floor_color = { 0x1c, 0x1c, 0x1c, SDL_ALPHA_OPAQUE };
  static const SDL_Color ceil_color = { 0x12, 0x12, 0x12, SDL_ALPHA_OPAQUE };
  static const SDL_Rect ceil_rect = { 0, 0, kWindowWidth, kWindowHeight / 2 };

  // Render the floor.
  SDL_SetRenderDrawColor(
      renderer,
      floor_color.r,
      floor_color.g,
      floor_color.b,
      floor_color.a);
  SDL_RenderClear(renderer);

  // Render the ceiling.
  SDL_SetRenderDrawColor(
      renderer,
      ceil_color.r,
      ceil_color.g,
      ceil_color.b,
      ceil_color.a);
  SDL_RenderFillRect(renderer, &ceil_rect);
}

void RenderWallSegment(
    SDL_Renderer* renderer,
    const raycasting::RayData& ray_data,
    int x) {
  int wall_height = static_cast<int>(kMaxWindowY / ray_data.distance);

  // Clamp wall height to maximum window Y.
  if (wall_height > kMaxWindowY) {
    wall_height = kMaxWindowY;
  }

  const int draw_start = (kMaxWindowY - wall_height) / 2;
  const int draw_end = draw_start + wall_height;

  SDL_Color wall_color = { 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE };

  switch (ray_data.wall_id) {
   case 1:
    wall_color.r = 0xff;
    break;

   case 2:
    wall_color.g = 0xff;
    break;

   case 3:
    wall_color.b = 0xff;
    break;

   case 4:
    wall_color.r = wall_color.g = wall_color.b = 0xff;
    break;

   default:
    wall_color.r = wall_color.g = 0xff;
    break;
  }

  // Adjust wall color if the wall is on the Y side.
  if (ray_data.wall_side == raycasting::WallSide::kYSide) {
    wall_color.r /= 2;
    wall_color.g /= 2;
    wall_color.b /= 2;
  }

  SDL_SetRenderDrawColor(
      renderer,
      wall_color.r,
      wall_color.g,
      wall_color.b,
      wall_color.a);
  SDL_RenderDrawLine(renderer, x, draw_start, x, draw_end);
}
