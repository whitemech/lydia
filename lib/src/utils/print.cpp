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

#include <lydia/utils/print.hpp>
#include <sstream>
#include <stdexcept>

namespace whitemech::lydia {

void StrPrinter::visit(const Symbol& x) { result = x.get_name(); }

void StrPrinter::visit(const LDLfTrue& x) { result = "tt"; }
void StrPrinter::visit(const LDLfFalse& x) { result = "ff"; }

void StrPrinter::visit(const LDLfAnd& x) {
  std::ostringstream s;
  auto container = x.get_container();
  s << "(";
  s << apply(**container.begin());
  for (auto it = ++(container.begin()); it != container.end(); ++it) {
    s << " & " << apply(**it);
  }
  s << ")";
  result = s.str();
}

void StrPrinter::visit(const LDLfOr& x) {
  std::ostringstream s;
  auto container = x.get_container();
  s << "(";
  s << apply(**container.begin());
  for (auto it = ++(container.begin()); it != container.end(); ++it) {
    s << " | " << apply(**it);
  }
  s << ")";
  result = s.str();
}

void StrPrinter::visit(const LDLfNot& x) {
  std::ostringstream s;
  s << "!(" << apply(*x.get_arg()) << ")";
  result = s.str();
}

void StrPrinter::visit(const LDLfDiamond& x) {
  std::ostringstream s;
  s << "<" << apply(*x.get_regex()) << ">(" << apply(*x.get_formula()) << ")";
  result = s.str();
}

void StrPrinter::visit(const LDLfBox& x) {
  std::ostringstream s;
  s << "[" << apply(*x.get_regex()) << "](" << apply(*x.get_formula()) << ")";
  result = s.str();
}

void StrPrinter::visit(const PropositionalRegExp& x) {
  std::ostringstream s;
  s << apply(*x.get_arg());
  result = s.str();
}

void StrPrinter::visit(const TestRegExp& x) {
  std::ostringstream s;
  s << "(" << apply(*x.get_arg()) << ")?";
  result = s.str();
}

void StrPrinter::visit(const UnionRegExp& x) {
  std::ostringstream s;
  auto container = x.get_container();
  s << "(";
  s << apply(**container.begin());
  for (auto it = ++(container.begin()); it != container.end(); ++it) {
    s << " + " << apply(**it);
  }
  s << ")";
  result = s.str();
}

void StrPrinter::visit(const SequenceRegExp& x) {
  std::ostringstream s;
  auto container = x.get_container();
  s << "(";
  s << apply(**container.begin());
  for (auto it = ++(container.begin()); it != container.end(); ++it) {
    s << " ; " << apply(**it);
  }
  s << ")";
  result = s.str();
}

void StrPrinter::visit(const StarRegExp& x) {
  std::ostringstream s;
  s << "(" << apply(*x.get_arg()) << ")*";
  result = s.str();
}

void StrPrinter::visit(const PropositionalTrue&) { result = "true"; }

void StrPrinter::visit(const PropositionalFalse&) { result = "false"; }

void StrPrinter::visit(const PropositionalAtom& x) {
  result = apply(*x.symbol);
}

void StrPrinter::visit(const PropositionalAnd& x) {
  std::ostringstream s;
  auto container = x.get_container();
  s << "(";
  s << apply(**container.begin());
  for (auto it = ++(container.begin()); it != container.end(); ++it) {
    s << " & " << apply(**it);
  }
  s << ")";
  result = s.str();
}

void StrPrinter::visit(const PropositionalOr& x) {
  std::ostringstream s;
  auto container = x.get_container();
  s << "(";
  s << apply(**container.begin());
  for (auto it = ++(container.begin()); it != container.end(); ++it) {
    s << " | " << apply(**it);
  }
  s << ")";
  result = s.str();
}

void StrPrinter::visit(const PropositionalNot& x) {
  std::ostringstream s;
  s << "!(" << apply(*x.get_arg()) << ")";
  result = s.str();
}

std::string StrPrinter::apply(const Basic& b) {
  b.accept(*this);
  return result;
}

std::string StrPrinter::apply(const vec_basic& v) {
  throw not_implemented_error();
}

std::string StrPrinter::apply(const set_formulas& v) {
  throw not_implemented_error();
}

std::string to_string(const Basic& x) {
  StrPrinter strPrinter;
  return strPrinter.apply(x);
}

} // namespace whitemech::lydia