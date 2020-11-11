#pragma once
/*
 * This file is part of Lydia.
 *
 * Lydia is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Lydia is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Lydia.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <string>
#include <vector>

namespace whitemech::lydia {

// incomplete list of LDLf formulas for testing purposes.
static const std::vector<std::string> FORMULAS{
    // only booleans
    "tt",
    "ff",
    "tt & ff",
    "tt | ff",
    // single diamond/box formulas with propositional
    "<true>tt",
    "[true]tt",
    "<true>ff",
    "[true]ff",
    "<a>tt",
    "<a>ff",
    "[a]tt",
    "[a]ff",
    "<!a>tt",
    "<!a>ff",
    "[!a]tt",
    "[!a]ff",
    "<a & b>tt",
    "<a & b>ff",
    "[a & b]tt",
    "[a & b]ff",
    "<a | b>tt",
    "<a | b>ff",
    "[a | b]tt",
    "[a | b]ff",
    "<!(a & b)>tt",
    "<!(a & b)>ff",
    "[!(a & b)]tt",
    "[!(a & b)]ff",
    "<!(a | b)>tt",
    "<!(a | b)>ff",
    "[!(a | b)]tt",
    "[!(a | b)]ff",
    // simple test regex
    "<ff?>ff",
    "[ff?]ff",
    "<ff?>tt",
    "[ff?]tt",
    "<tt?>ff",
    "[tt?]ff",
    "<tt?>tt",
    "[tt?]tt",
    "<<true>tt?>tt",
    "<[true]ff?>tt",
    "<<a>tt?><b>tt",
    "<[a]ff?><b>tt",
    // simple sequence regex
    "<a ; b; c>tt",
    "[a ; b; c]ff",
    "<a & b; c & d>tt",
    "[a & b; c & d]ff",
    // simple sequence regex
    "<a ; b; c>tt",
    "[a ; b; c]ff",
    "<a & b; c & d>tt",
    "[a & b; c & d]ff",
    // simple union regex
    "<a + b + c>tt",
    "[a + b + c]ff",
    "<a & b + c & d>tt",
    "[a & b + c & d]ff",
    // simple star regexes
    "<true*>tt",
    "[true*]ff",
    "<true*>ff",
    "[true*]tt",
    "<a*>tt",
    "[a*]ff",
    "<(a & b)*>tt",
    "[(a & b)*]ff",
    // sequence of stars
    "<true*; true*>tt",
    "[true*; true*]ff",
    "<a*; b*; c*>tt",
    "[a*; b*; c*]ff",
    // LTLf formulas: Next and Until
    "<true>(<a>tt & ~end)",
    "<(<a>tt?; true)*>(<b>tt & ~end)",
};

static const std::vector<std::string> SIMPLE_THEOREMS{
    "<a>tt <-> <a>tt",
    "(~(<a>tt)) <-> [a]ff",
    "<a & b>tt <-> <a & b>tt",
    "<a | b>tt <-> <a | b>tt",
    "(~(<a | b>tt)) <-> [a | b]ff",
    "(<a>tt & <b>tt) <-> (<a & b>tt)",
    "(<a>tt | <b>tt) <-> (<a | b>tt)",
    "([a]tt & [b]tt) <-> ([a & b]tt)",
    "([a]tt | [b]tt) <-> ([a | b]tt)",
};

static const std::vector<std::pair<const std::string, const unsigned>>
    ADVANCED_THEOREMS{
        {"(<a>({0}) & <a>({1})) <-> (<a>(({0}) & ({1})))", 2},
        {"(<a>({0}) | <a>({1})) <-> (<a>(({0}) | ({1})))", 2},
        {"([a]({0}) & [a]({1})) <-> ([a](({0}) & ({1})))", 2},
        {"([a]({0}) | [a]({1})) <-> ([a](({0}) | ({1})))", 2},
        {"(<a>({0}) & <b>({1})) <-> (<a & b>(({0}) & ({1})))", 2},
        {"([a]({0}) | [b]({1})) <-> ([a & b](({0}) | ({1})))", 2},
        {"(<a>({0}) & [b]({1})) <-> (<a & !b>({0}) | <a>(({0}) & ({1})))", 2},
        {"([a]({0}) | <b>({1})) <-> ([a & !b]({0}) & [a](({0}) | ({1})))", 2},
        {"(<a>({0}) | <b>({0})) <-> (<a | b>({0}))", 1},
        {"([a]({0}) & [b]({0})) <-> ([a | b]({0}))", 1},
        {"<a>({0}) <-> ([!a]ff & <a>({0}))", 1},
        {"[a]({0}) <-> (<!a>tt | [a]({0}))", 1},
        {"(<({0}?)>({1})) <-> (({0}) & ({1}))", 2},
        {"([({0}?)]({1})) <-> (!({0}) | ({1}))", 2},
    };

} // namespace whitemech::lydia