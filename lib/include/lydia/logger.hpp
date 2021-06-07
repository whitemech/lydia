#pragma once
/*
 * This file is part of Lydia.
 *
 * Lydia is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Lydia is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lydia.  If not, see <https://www.gnu.org/licenses/>.
 */

// clang-format off
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
// clang-format on
#include <string>

namespace whitemech::lydia {

enum class LogLevel {
  info = spdlog::level::level_enum::info,
  trace = spdlog::level::level_enum::trace,
  debug = spdlog::level::level_enum::debug,
  error = spdlog::level::level_enum::err
};

class Logger {
public:
  explicit Logger(std::string section);
  static constexpr const char* const logger_name = "lydia";
  auto section() const noexcept { return section_; }

  template <typename Arg1, typename... Args>
  void log(const LogLevel level, const char* fmt, const Arg1& arg1,
           const Args&... args) {
    auto new_fmt = "[{}] " + std::string(fmt);

    internal_logger_->log(static_cast<spdlog::level::level_enum>(level),
                          new_fmt.c_str(), section(), arg1, args...);
  }

  template <typename Arg1, typename... Args>
  void info(const char* fmt, const Arg1& arg1, const Args&... args) {
    log(LogLevel::info, fmt, arg1, args...);
  }

  template <typename Arg1> void info(const Arg1& arg1) { info("{}", arg1); }

  template <typename Arg1, typename... Args>
  void debug(const char* fmt, const Arg1& arg1, const Args&... args) {
    log(LogLevel::debug, fmt, arg1, args...);
  }

  template <typename Arg1> void debug(const Arg1& arg1) { debug("{}", arg1); }

  template <typename Arg1, typename... Args>
  void trace(const char* fmt, const Arg1& arg1, const Args&... args) {
    log(LogLevel::trace, fmt, arg1, args...);
  }

  template <typename Arg1> void trace(const Arg1& arg1) { trace("{}", arg1); }

  template <typename Arg1, typename... Args>
  void error(const char* fmt, const Arg1& arg1, const Args&... args) {
    log(LogLevel::error, fmt, arg1, args...);
  }

  template <typename Arg1> void error(const Arg1& arg1) { error("{}", arg1); }

  static void level(const LogLevel level) noexcept {
    spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
  }

private:
  std::string section_{""};
  std::shared_ptr<spdlog::logger> internal_logger_{nullptr};
};
} // namespace whitemech::lydia
