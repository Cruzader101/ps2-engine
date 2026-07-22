#pragma once

struct Vec3 {
  float x, y, z;
};

constexpr float dot(Vec3 a, Vec3 b) { // free function instead of member function because we want function to be symmetric, ie instead of a.add(b) kind of weird//
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

constexpr Vec3 operator+(Vec3 a, Vec3 b) {
  return Vec3{a.x + b.x, a.y + b.y, a.z + b.z};
}

constexpr Vec3 operator*(float s, Vec3 a) {
  return Vec3{s*a.x, s*a.y, s*a.z};
}

constexpr Vec3 operator*(Vec3 a, float s) {
  return s * a;
}

constexpr Vec3 operator-(Vec3 a, Vec3 b) {
  return Vec3 {a.x - b.x, a.y - b.y, a.z - b.z};
}
