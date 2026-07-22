#pragma once

struct Vec4 {
  float x, y, z, w; /* w = 1 is a point, w = 0 is a direction, will not be moved*/
};

constexpr float dot(Vec4 a, Vec4 b) {
  return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

constexpr Vec4 operator+(Vec4 a, Vec4 b) {
  return Vec4{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

constexpr Vec4 operator*(float s, Vec4 a) {
  return Vec4{s*a.x, s*a.y, s*a.z, s*a.w};
}

constexpr Vec4 operator*(Vec4 a, float s) {
  return s * a;
}

constexpr Vec4 operator-(Vec4 a, Vec4 b) {
  return Vec4 {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}
