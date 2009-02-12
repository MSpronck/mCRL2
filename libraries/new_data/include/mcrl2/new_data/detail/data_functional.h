// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/detail/data_functional.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DETAIL_DATA_FUNCTIONAL_H
#define MCRL2_DATA_DETAIL_DATA_FUNCTIONAL_H

#include <algorithm>
#include <functional>
#include <iterator>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "boost/format.hpp"
#include "mcrl2/new_data/data.h"
#include "mcrl2/new_data/data_specification.h"
#include "mcrl2/new_data/basic_sort.h"
#include "mcrl2/new_data/detail/data_utility.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/utility.h"

namespace mcrl2 {

namespace new_data {

namespace detail {

template <typename Term>
struct compare_term: public std::unary_function<atermpp::aterm_appl, bool>
{
  const Term& term;

  compare_term(const Term& t)
   : term(t)
  {}

  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  template <typename Term2>
  bool operator()(Term2 t) const
  {
    return term == t;
  }
};

/// \brief Function object that determines if a term is equal to a given data variable.
struct compare_variable: public compare_term<variable>
{
  compare_variable(const variable& v)
   : compare_term<variable>(v)
  {}
};

/// Function object that returns true if the expressions x and y have the same sort.
struct equal_data_expression_sort: public std::binary_function<data_expression, data_expression, bool>
{
  /// \brief Function call operator
  /// \param x A data expression
  /// \param y A data expression
  /// \return The function result
  bool operator()(const data_expression& x, const data_expression& y) const
  {
    return x.sort() == y.sort();
  }
};

/// \brief Function object that returns the name of a data variable
struct variable_name: public std::unary_function<variable, core::identifier_string>
{
  /// \brief Function call operator
  /// \param v A data variable
  /// \return The function result
  core::identifier_string operator()(const variable& v) const
  {
    return v.name();
  }
};

/// \brief Function object that returns the sort of a data variable
struct variable_sort: public std::unary_function<variable, sort_expression>
{
  /// \brief Function call operator
  /// \param v A data variable
  /// \return The function result
  sort_expression operator()(const variable& v) const
  {
    return v.sort();
  }
};

struct sort_has_name
{
  std::string m_name;

  sort_has_name(std::string name)
    : m_name(name)
  {}

  /// \brief Function call operator
  /// \param s A sort expression
  /// \return The function result
  bool operator()(sort_expression s) const
  {
    return s.is_basic_sort() && std::string(basic_sort(s).name()) == m_name;
  }
};

struct function_symbol_has_name
{
  std::string m_name;

  function_symbol_has_name(std::string name)
    : m_name(name)
  {}

  /// \brief Function call operator
  /// \param c A data operation
  /// \return The function result
  bool operator()(function_symbol c) const
  {
    return std::string(c.name()) == m_name;
  }
};

/// \brief Finds a mapping in a data specification.
/// \param data A data specification
/// \param s A string
/// \return The found mapping
inline
function_symbol find_mapping(data_specification data, std::string s)
{
  boost::iterator_range< function_symbol_list::const_iterator > r(data.mappings());

  function_symbol_list::const_iterator i = std::find_if(r.begin(), r.end(), function_symbol_has_name(s));
  if (i == data.mappings().end())
  {
    return function_symbol();
  }
  return *i;
}

/// \brief Finds a constructor in a data specification.
/// \param data A data specification
/// \param s A string
/// \return The found constructor
inline
function_symbol find_constructor(data_specification data, std::string s)
{
  function_symbol_list::const_iterator i = std::find_if(data.constructors().begin(), data.constructors().end(), function_symbol_has_name(s));
  if (i == data.constructors().end())
  {
    return function_symbol();
  }
  return *i;
}

/// \brief Finds a sort in a data specification.
/// \param data A data specification
/// \param s A string
/// \return The found sort
inline
sort_expression find_sort(data_specification data, std::string s)
{
  boost::iterator_range< sort_expression_list::const_iterator > r(data.sorts());

  sort_expression_list::const_iterator i = std::find_if(r.begin(), r.end(), sort_has_name(s));
  if (i == data.sorts().end())
  {
    return sort_expression();
  }
  return *i;
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_DATA_FUNCTIONAL_H
