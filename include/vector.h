#ifndef VECTOR_H_
#define VECTOR_H_

#include <cmath>

/*
 * vector.h
 *
 * This header file defines the Vector class, which represents a two-dimensional
 * vector and provides operations such as addition, scalar multiplication, and
 * rotation.
 */

class Vector {
 public:
  float x, y;

  explicit Vector(float x = 0.0f, float y = 0.0f);

  Vector operator+(const Vector& other) const;
  Vector& operator+=(const Vector& other);
  Vector operator*(float scalar) const;
  Vector& operator*=(float scalar);

  // Rotates the vector counterclockwise by a specified angle (in radians) using
  // a rotation matrix.
  Vector& Rotate(float angle);
};

#endif  // VECTOR_H_
