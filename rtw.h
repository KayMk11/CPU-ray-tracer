#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <cstdlib>

using std::fabs;
using std::make_shared;
using std::shared_ptr;
using std::sqrt;

constexpr float infinity = std::numeric_limits<float>::infinity();
constexpr float PI = 3.1415926535897932385;

inline float to_radians(float degrees) { return degrees * PI / 180.0; }

inline float random_float() { return rand() / (RAND_MAX + 1.0); }
inline float random_float(float min, float max) { return min + (max - min) * random_float(); }

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"
