// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/replace_subterm.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REPLACE_SUBTERM_H
#define MCRL2_PBES_REPLACE_SUBTERM_H

#include <utility>

#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/detail/position_count_traverser.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct find_subterm_traverser: public pbes_expression_traverser<find_subterm_traverser>
{
  typedef pbes_expression_traverser<find_subterm_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  utilities::detail::position_counter counter;
  std::size_t xpos;
  std::size_t ypos; // depth
  pbes_expression result;

  find_subterm_traverser(std::size_t xpos_, std::size_t ypos_)
    : xpos(xpos_), ypos(ypos_)
  {}

  template <typename T>
  void visit(const T& x)
  {
    counter.increase();
    if (counter.at(xpos, ypos))
    {
      result = x;
    }
    super::apply(x);
    counter.decrease();
  }
  void apply(const and_& x)
  {
    visit(x);
  }

  void apply(const or_& x)
  {
    visit(x);
  }

  void apply(const imp& x)
  {
    visit(x);
  }

  void apply(const exists& x)
  {
    visit(x);
  }

  void apply(const forall& x)
  {
    visit(x);
  }

  void apply(const propositional_variable_instantiation& x)
  {
    visit(x);
  }

  void apply(const data::data_expression& x)
  {
    visit(x);
  }
};

struct replace_subterm_builder: public pbes_expression_builder<replace_subterm_builder>
{
  typedef pbes_expression_builder<replace_subterm_builder> super;
  using super::apply;
  using super::update;

  std::size_t xpos;
  std::size_t ypos; // depth
  pbes_expression replacement;
  bool ready = false;
  utilities::detail::position_counter counter;

  replace_subterm_builder(std::size_t xpos_, std::size_t ypos_, const pbes_expression& replacement_)
    : xpos(xpos_), ypos(ypos_), replacement(replacement_)
  {}

  template <typename T>
  pbes_expression visit(const T& x)
  {
    counter.increase();
    bool arrived = counter.at(xpos, ypos);
    pbes_expression result;
    if (counter.y > ypos)
    {
      result = x;
    }
    else if (arrived)
    {
      result = replacement;
    }
    else
    {
      result = super::apply(x);
    }
    counter.decrease();
    return result;
  }

  pbes_expression apply(const data::data_expression& x)
  {
    return visit(x);
  }

  pbes_expression apply(const propositional_variable_instantiation& x)
  {
    return visit(x);
  }

  pbes_system::pbes_expression apply(const pbes_system::forall& x)
  {
    return visit(x);
  }

  pbes_system::pbes_expression apply(const pbes_system::exists& x)
  {
    return visit(x);
  }

  pbes_system::pbes_expression apply(const pbes_system::and_& x)
  {
    return visit(x);
  }

  pbes_system::pbes_expression apply(const pbes_system::or_& x)
  {
    return visit(x);
  }

  pbes_system::pbes_expression apply(const pbes_system::imp& x)
  {
    return visit(x);
  }
};

} // namespace detail

/// \brief Replace the subterm at position (x, y) with a given term
inline
pbes_expression replace_subterm(const pbes_expression& expr, std::size_t x, std::size_t y, const pbes_expression& replacement)
{
  detail::replace_subterm_builder f(x, y, replacement);
  return f.apply(expr);
}

/// \brief Replace the subterm at position (x, y) with a given term
inline
pbes replace_subterm(const pbes& p, std::size_t x, std::size_t y, const pbes_expression& replacement)
{
  pbes result = p;
  detail::replace_subterm_builder f(x, y, replacement);
  f.update(result);
  return result;
}

inline
pbes_expression find_subterm(const pbes& pbesspec, std::size_t x, std::size_t y)
{
  detail::find_subterm_traverser f(x, y);
  f.apply(pbesspec);
  return f.result;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REPLACE_SUBTERM_H
