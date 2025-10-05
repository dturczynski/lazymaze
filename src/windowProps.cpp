#include "windowProps.hpp"
#include "result.hpp"

namespace {
std::vector<std::pair<unsigned, unsigned>> supportedSizes() { 
	return {
		{1920, 1080},
		{1000, 1000},
		{640, 480}
	};
}
}

namespace lm {
int initWindowSize(int argc, char** argv, WindowSize* size) {
	if (!size) {
		std::cerr << "Size is nullptr!" << std::endl;
		return Result::DomainError;
	}

	size->width = 1920;
	size->height = 1080;

	auto readNum = [argc, argv]<typename T>(const std::string param, T* val) {
		T value{};
		auto ret = ExtractParameterValue(argc, argv, param, &value);
		if (ret == Result::Success) {
			*val = value;
			std::cout << "Matched regex '" << param << "' with '" << *val << "'\n";
		}
	};

	readNum("--width|-w", &size->width);
	readNum("--height|-h", &size->height);

	const auto supported = supportedSizes();
	bool isOk = false;
	for (auto okSize : supported) {
		if (okSize.first == size->width && okSize.second == size->height) {
			isOk = true;
			break;
		}
	}

	if (!isOk) {
		std::cerr << "The specified resolution is not supported!" << std::endl;
		return Result::DomainError;
	}

	return Result::Success;
}
}
