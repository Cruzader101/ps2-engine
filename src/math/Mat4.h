#pragma once
#include <math/vec3.h>
#include <cmath>

struct Mat4{
  float m[16];

   static constexpr Mat4 identity() {
     Mat4 result{};
     for (int i = 0; i < 4; ++i) {
       result.m[i*4+i] = 1.0f; // c*4 + c are diagonal entries from a matrix in the array //
     }
     return result;
   }

   static constexpr Mat4 translate(Vec3 t) {
     Mat4 result = identity();
     result.m[3*4 + 0] = t.x; // column 3 and then rows 0, 1, and 2 //
     result.m[3*4 + 1] = t.y;
     result.m[3*4 + 2] = t.z;
     return result;
   }
};

constexpr Mat4 operator*(const Mat4& a, const Mat4& b) {
  Mat4 result{};
  for (int c = 0; c < 4; ++c) {
    for (int r = 0; r < 4; ++r) {
       float sum = 0.0f;
       for (int k = 0; k < 4; ++k) {
          sum += a.m[k*4 + r] * b.m[c*4 + k];
       }
      result.m[c*4 + r] = sum;
    }
  }
  return result;
}

inline bool approx_equal(const Mat4& a, const Mat4& b, float eps = 1e-5f) {
   for (int i = 0; i < 16; ++i) {
      if (std::fabs(a.m[i] - b.m[i]) > eps) {
        return false;
      }
   }
   return true;
}

