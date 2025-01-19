#ifndef CAMERA_H_
#define CAMERA_H_

#include <algorithm>
#include <cmath>
#include <limits>

#include "level_data.h"
#include "vector.h"

/*
 * camera.h
 *
 * This header file defines the Camera class, which encapsulates the logic for
 * managing the camera's position, direction, and movement within a 2D
 * environment.
 *
 * It includes functionalities for handling acceleration, rotation, and movement
 * as well as performing raycasting using the DDA (Digital Differential
 * Analysis) algorithm.
 *
 * The file also defines related enums and structs necessary for camera and
 * raycasting operations.
 */

namespace motion {

enum class AccelState {
  kNone = 0,
  kAccelerate = 1,
  kDeaccelerate = -1
};

enum class AccelDirection {
  kNone = 0,
  kForward = 1,
  kBackward = -1
};

enum class RotationDirection {
  kNone = 0,
  kClockwise = 1,
  kCounterclockwise = -1
};

}  // namespace motion

namespace raycasting {

enum class WallSide {
  kXSide = 0,
  kYSide = 1
};

struct RayData {
  float distance;
  int wall_id;
  WallSide wall_side;
};

// Data required for the DDA algorithm is separated for the X and Y axes.
// This data is used to calculate distances to tile sides during the algorithm's
// execution.
struct DDAData {
  int tile;
  int step;
  float delta_dist;
  float init_dist;

  DDAData(float position, float ray_direction);
};

}  // namespace ray

class Camera {
 private:
  static constexpr float kMaxMovementSpeed = 2.5f;
  static constexpr float kMaxRotationSpeed = 1.5f;

  float plane_length_;

  Vector position_;
  Vector direction_;
  Vector plane_;

  motion::AccelState accel_state_ = motion::AccelState::kNone;
  motion::AccelDirection accel_direction_ = motion::AccelDirection::kNone;

  float movement_speed_ = 0.0f;
  float rotation_speed_ = 0.0f;

 public:
  Camera(float x, float y, float angle, float fov);

  // Updates the camera plane to be a vector perpendicular to the camera's
  // direction and ensures it has the specified plane length.
  void UpdatePlane();

  // Basic getters that simply return the current value.
  Vector Position() const;
  Vector Direction() const;
  Vector Plane() const;
  motion::AccelState AccelState() const;
  motion::AccelDirection AccelDirection() const;
  float MovementSpeed() const;
  float RotationSpeed() const;

  // Sets the camera's acceleration state and direction.
  void SetAcceleration(motion::AccelState accel_state,
                       motion::AccelDirection accel_direction);

  // Updates the movement speed based on the current acceleration.
  // The speed will gradually increase or decrease, and is capped at a maximum
  // value.
  // The changes are scaled by frame time to ensure consistent acceleration.
  void SetMovementSpeed(float frame_time);

  // Sets the rotation speed to the maximum value in the specified rotation
  // direction.
  void SetRotationSpeed(motion::RotationDirection rotation_direction);

  // Updates the camera's position, direction, and plane based on the current
  // movement and rotation speeds, scaled by frame time to maintain consistent
  // behavior.
  void HandleMotion(float frame_time);

  // Performs the DDA algorithm and returns ray information, including distance
  // to the wall, the wall ID, and the side (X or Y) that was hit.
  raycasting::RayData CalculateRay(float plane_scalar) const;
};

#endif  // CAMERA_H_
