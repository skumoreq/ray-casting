#include "vector.h"

Vector::Vector(float x, float y) : x(x), y(y) {}

Vector Vector::operator+(const Vector& other) const {
  return Vector(x + other.x, y + other.y);
}

Vector& Vector::operator+=(const Vector& other) {
  x += other.x;
  y += other.y;

  return *this;
}

Vector Vector::operator*(float scalar) const {
  return Vector(x * scalar, y * scalar);
}

Vector& Vector::operator*=(float scalar) {
  x *= scalar;
  y *= scalar;

  return *this;
}

Vector& Vector::Rotate(float angle) {
  // Cache the sine and cosine of the angle to avoid redundant calculations.
  const float sin_angle = std::sin(angle);
  const float cos_angle = std::cos(angle);

  // Compute the new coordinates using the rotation matrix.
  // [ cos(angle)   sin(angle) ] [ x ]
  // [-sin(angle)   cos(angle) ] [ y ]
  const float new_x = x * cos_angle + y * sin_angle;
  const float new_y = x * -sin_angle + y * cos_angle;

  x = new_x;
  y = new_y;

  return *this;
}
