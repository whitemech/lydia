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
    "<(<a>tt?)*><b>tt",
    // sequence of stars
    "<true*; true*>tt",
    "[true*; true*]ff",
    "<a*; b*; c*>tt",
    "[a*; b*; c*]ff",
    // LTLf formulas: Next and Until
    "<true>(<a>tt & ~end)",
    "<(<a>tt?; true)*>(<b>tt & ~end)",
    // other misc formulas
    "<((a & (!b)))>tt",
    "[true*](([true]ff) || (<!a>tt) || (<true*>(<b>tt)))",
    "<(((<a && b>tt)?)*)*>([(<((c*) + (d || a))*><b>tt)?]<d>tt)",
    "<true>tt && (<a>tt -> <true><b>tt)",
    "([(true*) ; ((!(<<b>tt?>(tt)))?)](ff))",
    "[(true*) ; ((<(<!b>tt)?>tt)?)]ff",
    "(<(((<b>tt?);true)*) ; ((<a & b>tt)?)>tt) || ([(true*);((<!b>tt)?)]ff)",
    "(<(((<b>tt?);true)*)>(<a & b>tt) || ([true*]<b>tt))",
    "[(((((<!a>tt)?) ; (true))*) ; ((<!b>tt)?))]ff",
    "(<true> tt) & ([a] <b>tt)",
    "<a>tt & !(<true>(<true>tt))",
    "!(<true*>!((<a>tt) -> (<true>(<b>tt))) )",
    "[( ((<!a>tt)? ; true)* ; ((<!b>tt)?))]ff",
    "< (!(a | b | c ))* ; (a | c) ; (!(a | b | c))* ; (b | c) >(<true>tt)",
    "<a*>([(tt)?]ff)",
    "<a;b;c>([true]ff)",
    "<(a || b) ; (!c)*>(tt)",
    //    // from ltlf
    "[(((!(<a>(tt)))? ; true))*]((<b>(tt) | end))",
    "[(true)*](((!(<a>(tt)) | <(true)*>((<b>(tt) & !(end)))) | end))",
    "[(((!(<(true)*>((<d>(tt) & !(end)))))? ; true))*]((<((((<g>(tt) | "
    "<i>(tt)))? ; true))*>((<l>(tt) & !(end))) | end))",
    "[(((!(<(true)*>((<d>(tt) & !(end)))))? ; true))*](<l>(tt) | end)",
    "(!(<(true)*>(([(((!(<(((<a>(tt))? ; true))*>(((<b>(tt) | <c>(tt)) & "
    "!(end)))))? ; true))*](((<true>((<e>(tt) & !(end))) | ([true]((<f>(tt) | "
    "end)) & [(true)*]((<h>(tt) | end)))) | end)) & !(end)))) | "
    "[(((!(<(true)*>((<d>(tt) & !(end)))))? ; true))*]((<((((<g>(tt) | "
    "<i>(tt)))? ; true))*>((<l>(tt) & !(end))) | end)))",
    "([(true)*](((!(<rl>(tt)) | <(true)*>((<aa>(tt) & !(end)))) | end)) & "
    "[(true)*](((!(<aa>(tt)) | <(true)*>((<dl>(tt) & !(end)))) | end)) & "
    "[(true)*](((!(<aa>(tt)) | <true>((<dl>(tt) & !(end)))) | end)))",
    "([(true)*](((!(<rl>(tt)) | <(true)*>((<aa>(tt) & !(end)))) | end)) & "
    "[(true)*](((!(<aa>(tt)) | <true>((<dl>(tt) & !(end)))) | end)))",
    "([(true)*](((<a>(tt) | <b>(tt)) | end)) | [(true)*]((<c>(tt) | end)))",
    "[(true)*](((<a>(tt) | <b>(tt) | <c>(tt)) | end))",
    "((!(([(true)*](((<a>(tt) | <b>(tt)) | end)) | [(true)*]((<c>(tt) | "
    "end)))) | [(true)*](((<a>(tt) | <b>(tt) | <c>(tt)) | end))) & "
    "(([(true)*](((<a>(tt) | <b>(tt)) | end)) | [(true)*]((<c>(tt) | end))) | "
    "!([(true)*](((<a>(tt) | <b>(tt) | <c>(tt)) | end)))))",
    "!([(true)*](((<a>(tt) | <b>(tt) | <c>(tt)) | end)))",
    "(!(<(true)*>((<a>(tt) & !(end)))) | !(<(true)*>((<b>(tt) & !(end)))))",
    "(((<a>(tt) & !(<b>(tt))) | (<b>(tt) & !(<a>(tt)))) & [true]((<false>(tt) "
    "| end)))",
    "(<true>(tt) & [true](((!(<a>(tt)) | [true]((<b>(tt) | end))) | end)))",
    "<true>((<true>(tt) & !(end)))",
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
        {"(<({0}?)*>({1})) <-> ({1})", 2},
        {"([({0}?)*]({1})) <-> ({1})", 2},
        {"(<((<(({0}?);true)*>({1}))?;true)*>({1})) <-> "
         "(<(({0}?);true)*>({1}))",
         2},
    };

} // namespace whitemech::lydia