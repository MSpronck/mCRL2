// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/identifier_string.h
/// \brief Add your file description here.

#ifndef MCRL2_BASIC_IDENTIFIER_STRING_H
#define MCRL2_BASIC_IDENTIFIER_STRING_H

#include "mcrl2/atermpp/aterm_string.h"

namespace lps {

  /// \brief String type of the LPS library.
  /// Identifier strings are represented internally as ATerms.
  typedef atermpp::aterm_string identifier_string;
  
  /// Test if a term is an identifier string.
  inline
  bool is_identifier_string(atermpp::aterm t)
  {
    return t.type() == AT_APPL && atermpp::aterm_appl(t).size() == 0;
  }

} // namespace lps

#endif // MCRL2_BASIC_IDENTIFIER_STRING_H
