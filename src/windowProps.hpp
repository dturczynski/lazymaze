#pragma once
#include "extractParameter.hpp"
#include <iostream>

namespace lm {
struct WindowSize {
	unsigned width;
	unsigned height;
};

int initWindowSize(int argc, char** argv, WindowSize* size);
}
