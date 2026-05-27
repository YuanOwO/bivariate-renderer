#pragma once

#include <algorithm>

using uint8_t = unsigned char;

struct RGBColor {
    double r, g, b;
};

template <typename T>
inline T clamp(T value, T min_value, T max_value) {
    return std::max(min_value, std::min(max_value, value));
}

template <typename T>
inline double normalize(T value, T min_value, T max_value) {
    if (max_value - min_value == 0) return 0.0;  // 防止除以零
    return 1.0 * (value - min_value) / (max_value - min_value);
}

RGBColor getRGBGradientJet(double ratio);
RGBColor getRGBGradientViridis(double ratio);
