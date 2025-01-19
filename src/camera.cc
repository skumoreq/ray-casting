#include "camera.h"

raycasting::DDAData::DDAData(float position, float ray_direction) {
  tile = static_cast<int>(position);

  if (ray_direction == 0.0f) {
    step = 0;
    delta_dist = init_dist = std::numeric_limits<float>::infinity();
  } else {
    delta_dist = 1.0f / std::abs(ray_direction);

    if (ray_direction > 0.0f) {
      step = 1;
      init_dist = tile + 1.0f - position;
    } else {
      step = -1;
      init_dist = position - tile;
    }

    init_dist *= delta_dist;
  }
}

Camera::Camera(float x, float y, float angle, float fov)
    : plane_length_(std::tan(fov / 2.0f)),
      position_(x, y),
      direction_(std::cos(angle), std::sin(angle)) {
  UpdatePlane();
}

void Camera::UpdatePlane() {
  plane_.x = direction_.y;
  plane_.y = -direction_.x;

  plane_ *= plane_length_;
}

Vector Camera::Position() const {
  return position_;
}

Vector Camera::Direction() const {
  return direction_;
}

Vector Camera::Plane() const {
  return plane_;
}

motion::AccelState Camera::AccelState() const {
  return accel_state_;
}

motion::AccelDirection Camera::AccelDirection() const {
  return accel_direction_;
}

float Camera::MovementSpeed() const {
  return movement_speed_;
}

float Camera::RotationSpeed() const {
  return rotation_speed_;
}

void Camera::SetAcceleration(motion::AccelState accel_state,
                             motion::AccelDirection accel_direction) {
  accel_state_ = accel_state;
  accel_direction_ = accel_direction;
}

void Camera::SetMovementSpeed(float frame_time) {
  // Casts the acceleration direction to an integer to simplify its usage in
  // conditional statements and improve code readability.
  const int accel_direction = static_cast<int>(accel_direction_);

  if (accel_state_ == motion::AccelState::kAccelerate) {
    // Gradually increases speed until it reaches the maximum value.
    // Once the maximum speed is reached, the acceleration is reset.
    if (movement_speed_ * accel_direction < kMaxMovementSpeed) {
      movement_speed_ += kMaxMovementSpeed * accel_direction * frame_time;
    } else {
      movement_speed_ = kMaxMovementSpeed * accel_direction;
      SetAcceleration(motion::AccelState::kNone,
                      motion::AccelDirection::kNone);
    }
  } else if (accel_state_ == motion::AccelState::kDeaccelerate) {
    // Gradually decreases speed until it reaches zero.
    // Once speed reaches zero, it is reset, and the acceleration is reset.
    if (movement_speed_ * accel_direction > 0.0f) {
      movement_speed_ -= kMaxMovementSpeed * accel_direction * frame_time;
    } else {
      movement_speed_ = 0.0f;
      SetAcceleration(motion::AccelState::kNone,
                      motion::AccelDirection::kNone);
    }
  }
}

void Camera::SetRotationSpeed(motion::RotationDirection rotation_direction) {
  rotation_speed_ = kMaxRotationSpeed * static_cast<int>(rotation_direction);
}

void Camera::HandleMotion(float frame_time) {
  if (movement_speed_ != 0.0f) {
    // Calculates the position offset by scaling the direction with the movement
    // speed.
    const Vector position_offset = direction_ * (movement_speed_ * frame_time);
    const Vector new_position = position_ + position_offset;

    // Identifies the current and new tiles based on the camera's position.
    const int tile_x = static_cast<int>(position_.x);
    const int tile_y = static_cast<int>(position_.y);

    const int tile_new_x = static_cast<int>(new_position.x);
    const int tile_new_y = static_cast<int>(new_position.y);

    // Checks for collisions independently along each axis, allowing movement
    // along one axis even if the other collides with a wall.
    if (level::kLevelData[tile_new_x][tile_y] == 0) {
      position_.x = new_position.x;
    }
    if (level::kLevelData[tile_x][tile_new_y] == 0) {
      position_.y = new_position.y;
    }
  }

  if (rotation_speed_ != 0.0f) {
    direction_.Rotate(rotation_speed_ * frame_time);
    UpdatePlane();
  }
}

raycasting::RayData Camera::CalculateRay(float plane_scalar) const {
  // Calculates the ray direction by scaling and adding the camera plane to the
  // direction vector.
  const Vector ray_direction = direction_ + plane_ * plane_scalar;

  // Initializes DDA data for the X and Y axes.
  raycasting::DDAData dda_data_x(position_.x, ray_direction.x);
  raycasting::DDAData dda_data_y(position_.y, ray_direction.y);

  int wall_id = 0;
  raycasting::WallSide wall_side;

  // Performs the DDA algorithm until a wall is hit.
  while (wall_id == 0) {
    // Selects the shorter distance to the next tile side.
    if (dda_data_x.init_dist < dda_data_y.init_dist) {
      dda_data_x.tile += dda_data_x.step;
      dda_data_x.init_dist += dda_data_x.delta_dist;

      wall_side = raycasting::WallSide::kXSide;
    } else {
      dda_data_y.tile += dda_data_y.step;
      dda_data_y.init_dist += dda_data_y.delta_dist;

      wall_side = raycasting::WallSide::kYSide;
    }

    wall_id = level::kLevelData[dda_data_x.tile][dda_data_y.tile];
  }

  float distance = 0;

  // Selects the last hit distance and compensates for overshooting by one tile
  // during the DDA.
  if (wall_side == raycasting::WallSide::kXSide) {
    distance = dda_data_x.init_dist - dda_data_x.delta_dist;
  } else if (wall_side == raycasting::WallSide::kYSide) {
    distance = dda_data_y.init_dist - dda_data_y.delta_dist;
  }

  return raycasting::RayData{ distance, wall_id, wall_side };
}
