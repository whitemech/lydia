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

namespace whitemech::lydia {
class AbstractDriver {
public:
  std::shared_ptr<AstManager> context = nullptr;
  AbstractDriver() : context{std::make_shared<AstManager>()} {}
  AbstractDriver(std::shared_ptr<AstManager> c) : context{std::move(c)} {}
  virtual void parse(const char* const filename) = 0;
  virtual void parse(std::istream& iss) = 0;
  virtual ldlf_ptr get_result() = 0;
};
} // namespace whitemech::lydia
