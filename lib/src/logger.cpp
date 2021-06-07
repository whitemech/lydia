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
#include <lydia/logger.hpp>
#include <spdlog/sinks/dist_sink.h>

#ifdef _WIN32
#include <spdlog/sinks/wincolor_sink.h>
#else
#include <spdlog/sinks/ansicolor_sink.h>
#endif

#if defined(_DEBUG) && defined(_MSC_VER)
#include <spdlog/sinks/msvc_sink.h>
#endif // _DEBUG && _MSC_VER

namespace whitemech::lydia {

auto create_spdlog(const std::string& logger_name) {
#ifdef _WIN32
  auto color_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
  auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif
  auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_st>();
  dist_sink->add_sink(color_sink);
#if defined(_DEBUG) && defined(_MSC_VER)
  auto debug_sink = std::make_shared<spdlog::sinks::msvc_sink_st>();
  dist_sink->add_sink(debug_sink);
#endif // _DEBUG && _MSC_VER
  auto logger_ = std::make_shared<spdlog::logger>(logger_name, dist_sink);
  return logger_;
}

Logger::Logger(std::string section) : section_{std::move(section)} {
  std::string log_name{whitemech::lydia::Logger::logger_name};
  internal_logger_ = spdlog::get(log_name);

  if (internal_logger_ == nullptr) {
    internal_logger_ = create_spdlog(logger_name);
    spdlog::register_logger(internal_logger_);
  }
}

} // namespace whitemech::lydia
