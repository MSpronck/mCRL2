// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file aterm_test.cpp
/// \brief Add your file description here.

// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : test/aterm_test.cpp
// date          : 04/25/05
// version       : 0.3
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/utility.h"

using namespace std;
using namespace atermpp;

void test_aterm()
{
  aterm a = make_term("f(x)");
  aterm label = make_term("label");
  aterm annotation = make_term("annotation");
  aterm b = set_annotation(a, label, annotation);
  BOOST_CHECK(a != b);
  aterm c = remove_annotation(b, label);
  BOOST_CHECK(a == c);
  
  aterm d = aterm_int(10);
  BOOST_CHECK(aterm_int(d).value() == 10);
  
  aterm e = aterm();
  BOOST_CHECK(!e);
}

int test_main( int, char*[] )
{
  ATERM_LIBRARY_INIT()
  test_aterm();
  return 0;
}
