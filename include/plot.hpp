#pragma once
#include <initializer_list>
#include <string>
#include <vector>

#include "types.hpp"

bool renderPlot(const String& filename, const std::vector<Value>& data, int resolution = 800);
