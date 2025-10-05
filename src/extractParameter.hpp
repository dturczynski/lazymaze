#pragma once
#include "result.hpp"
#include <ranges>
#include <regex>

template <typename T>
int ExtractParameterValue(int argc, char **argv, std::string regex, T *dst) {
  const std::regex query{regex};

  for (int i = 0; i < argc; ++i) {
    auto kvp = std::ranges::views::split(std::string{argv[i]}, '=');
    std::vector<std::string> seq{};
    for (auto it = kvp.begin(); it != kvp.end(); ++it)
      seq.push_back(std::string{(*it).begin(), (*it).end()});

    if (std::regex_match(seq.front(), query)) {
      std::string target{seq.back()};
      if (seq.size() == 1) {
        if (i + 1 == argc)
          return Result::SyntaxError;
        target = argv[++i];
      }

      if constexpr (std::same_as<T, std::string>)
        *dst = target;
      else
        try {
          *dst = static_cast<T>(std::stold(target));
        } catch (...) {
          return Result::ConversionError;
        }

      return Result::Success;
    }
  }

  return Result::NotFound;
}
