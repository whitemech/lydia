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

#include <lydia/logic/ldlf/only_test.hpp>

namespace whitemech::lydia {

void OnlyTestVisitor::visit(const PropositionalRegExp& r) { result = false; }
void OnlyTestVisitor::visit(const TestRegExp& r) { result = true; }
void OnlyTestVisitor::visit(const UnionRegExp& r) {
  result = std::all_of(r.get_container().cbegin(), r.get_container().cend(),
                       [this](regex_ptr p) { return apply(*p); });
}
void OnlyTestVisitor::visit(const SequenceRegExp& r) {
  result = std::all_of(r.get_container().cbegin(), r.get_container().cend(),
                       [this](regex_ptr p) { return apply(*p); });
}
void OnlyTestVisitor::visit(const StarRegExp& r) {
  result = apply(*r.get_arg());
}

bool OnlyTestVisitor::apply(const RegExp& r) {
  r.accept(*this);
  return result;
}

bool is_test_only(const RegExp& r) {
  auto v = OnlyTestVisitor();
  return v.apply(r);
}

} // namespace whitemech::lydia
