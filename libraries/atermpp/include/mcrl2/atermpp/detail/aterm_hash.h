// Author(s): Maurice Laveaux.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_HASH_H_
#define MCRL2_ATERMPP_DETAIL_ATERM_HASH_H_

#include "mcrl2/atermpp/detail/aterm.h"
#include "mcrl2/atermpp/detail/aterm_appl.h"
#include "mcrl2/atermpp/detail/aterm_int.h"
#include "mcrl2/atermpp/function_symbol.h"
#include "mcrl2/utilities/detail/memory_utility.h"

#include <array>
#include <functional>
#include <limits>
#include <tuple>


namespace std
{

/// \brief specialization of the standard std::hash function.
template<>
struct hash<atermpp::detail::_aterm*>
{
  // Default constructor, required for const qualified hash functions.
  hash()
  {}

  std::size_t operator()(const atermpp::detail::_aterm* term) const
  {
    // All terms are 8 bytes aligned which means that the three lowest significant
    // bits of their pointers are always 0. However, their smallest size is 16 bytes so
    // the lowest 4 bits do not carry much information.
    return reinterpret_cast<std::uintptr_t>(term) >> 4;
  }
};

/// \brief specialization of the standard std::hash function.
template<>
struct hash<atermpp::unprotected_aterm>
{
  // Default constructor, required for const qualified hash functions.
  hash()
  {}

  std::size_t operator()(const atermpp::unprotected_aterm& term) const
  {
    std::hash<atermpp::detail::_aterm*> hasher;
    return hasher(atermpp::detail::address(term));
  }
};

/// \brief specialization of the standard std::hash function.
template<>
struct hash<atermpp::aterm>
{
  // Default constructor, required for const qualified hash functions.
  hash()
  {}

  std::size_t operator()(const atermpp::aterm& t) const
  {
    std::hash<atermpp::detail::_aterm*> aterm_hasher;
    return aterm_hasher(atermpp::detail::address(t));
  }
};

} // namespace std

namespace atermpp
{
namespace detail
{

/// \brief Indicates that the number of arguments is not known at compile time.
constexpr std::size_t DynamicNumberOfArguments = std::numeric_limits<std::size_t>::max();

/// \brief Construct the proxy where its arguments are given by applying the converter
///        to each element in the iterator.
template<std::size_t N,
         typename InputIterator,
         typename TermConverter,
         typename std::enable_if<is_iterator<InputIterator>::value, void>::type* = nullptr>
static std::array<unprotected_aterm, N> construct_arguments(InputIterator it, TermConverter converter)
{
  std::array<unprotected_aterm, N> arg;
  // Copy the arguments.
  for (size_t i = 0; i < N; ++i)
  {
    // Use placement new as the arguments are not default constructed.
    arg[i] = std::forward<TermConverter>(converter)(*it);

    // Increment the iterator
    ++it;
  }
  return arg;
}

/// \brief Computes the hash of the given term.
/// \details Can be optimized with loop unrolling when template parameter N is provided.
///          However, this assumes that every passed to term has arity equal to N.
template<std::size_t N = DynamicNumberOfArguments>
struct aterm_hasher
{
  std::size_t operator()(const _aterm& term) const noexcept;
  std::size_t operator()(const function_symbol& symbol) const noexcept;
  std::size_t operator()(const function_symbol& symbol, unprotected_aterm arguments[]) const noexcept;

  template<typename ForwardIterator,
           typename std::enable_if<is_iterator<ForwardIterator>::value, void>::type* = nullptr>
  std::size_t operator()(const function_symbol& symbol, ForwardIterator begin) const noexcept;
};

/// \brief Computes the hash of the given term.
/// \details This version only works whenever N is a compile time constant.
template<std::size_t N = 0>
struct aterm_hasher_finite : public aterm_hasher<N>
{
  using aterm_hasher<N>::operator();
  std::size_t operator()(const function_symbol& symbol, std::array<unprotected_aterm, N> key) const noexcept;

  template<typename ...Args>
  std::size_t operator()(const function_symbol& symbol, const Args&... args) const noexcept;
};

/// \brief Computes the hash of the integral term arguments.
struct aterm_int_hasher
{
  inline std::size_t operator()(const _aterm_int& term) const noexcept;
  inline std::size_t operator()(std::size_t value) const noexcept;
};

/// \brief Returns true iff first and second are value-equivalent.
/// \details Can be optimized with loop unrolling when template parameter N is provided.
///          However, this assumes that every passed to term has arity equal to N.
template<std::size_t N = DynamicNumberOfArguments>
struct aterm_equals
{
  using is_transparent = void;

  bool operator()(const _aterm& first, const _aterm& second) const noexcept;
  bool operator()(const _aterm& term, const function_symbol& symbol) const noexcept;
  bool operator()(const _aterm& term, const function_symbol& symbol, unprotected_aterm arguments[]) const noexcept;

  template<typename ForwardIterator,
           typename std::enable_if<is_iterator<ForwardIterator>::value>::type* = nullptr>
  bool operator()(const _aterm& term, const function_symbol& symbol, ForwardIterator begin) const noexcept;
};

template<std::size_t N>
struct aterm_equals_finite : public aterm_equals<N>
{
  using aterm_equals<N>::operator();
  bool operator()(const _aterm& term, const function_symbol& symbol, std::array<unprotected_aterm, N> key) const noexcept;

  template<typename ...Args>
  bool operator()(const _aterm& term, const function_symbol& symbol, const Args&... args) const noexcept;
};

/// \brief Returns true iff the given term(s) or value are equivalent.
struct aterm_int_equals
{
  using is_transparent = void;

  inline bool operator()(const _aterm_int& first, const _aterm_int& second) const noexcept;
  inline bool operator()(const _aterm_int& term, std::size_t value) const noexcept;
};

/// \brief Auxiliary function to combine seed hnr with value w.
inline
std::size_t combine(const std::size_t hnr, const std::size_t w)
{
  // Addition works better than xor, because xor maps equal inputs to 0 which
  // can lead to many collisions. However, with addition we also want to prevent
  // symmetry, i.e a + b equals b + a, so a relative cheap solution is to multiply
  // one side by a number. For this purpose we chose w + floor(2.5 * hnr);
  return w + (hnr<<1) + (hnr>>1);
}

/// \brief Auxiliary function to combine hnr with aterms.
inline
std::size_t combine(const std::size_t hnr, const unprotected_aterm& w)
{
  std::hash<unprotected_aterm> hasher;
  return combine(hnr,hasher(w));
}

/// \brief Auxiliary function to combine hnr with _aterm*.
inline
std::size_t combine(const std::size_t hnr, const _aterm* w)
{
  std::hash<_aterm*> hasher;
  return combine(hnr,hasher(w));
}

/// Implementation

template<std::size_t N>
std::size_t aterm_hasher<N>::operator()(const _aterm& term) const noexcept
{
  const function_symbol& f = term.function();
  std::size_t hnr = operator()(f);

  // The arity is defined by the function symbol iff N is unchanged and the arity is N otherwise.
  const std::size_t arity = (N == DynamicNumberOfArguments) ? f.arity() : N;

  // This is a function application with arguments, hash each argument and combine the result.
  const _aterm_appl<aterm>& term_appl = reinterpret_cast<const _aterm_appl<aterm>&>(term);
  for (std::size_t i = 0; i < arity; ++i)
  {
    hnr = combine(hnr, term_appl.arg(i));
  }

  return hnr;
}
template<std::size_t N>
std::size_t aterm_hasher<N>::operator()(const function_symbol& symbol) const noexcept
{
  std::hash<function_symbol> function_hasher;
  return function_hasher(symbol);
}

template<std::size_t N>
std::size_t aterm_hasher<N>::operator()(const function_symbol& symbol, unprotected_aterm arguments[]) const noexcept
{
  // The arity is defined by the function symbol iff N is unchanged and the arity is N otherwise.
  const std::size_t arity = (N == DynamicNumberOfArguments) ? symbol.arity() : N;

  // This is a function application with arguments, hash each argument and combine the result.
  std::size_t hnr = operator()(symbol);
  for (std::size_t i = 0; i < arity; ++i)
  {
    hnr = combine(hnr, arguments[i]);
  }

  return hnr;
}

template<std::size_t N>
template<typename ForwardIterator,
         typename std::enable_if<is_iterator<ForwardIterator>::value, void>::type*>
inline std::size_t aterm_hasher<N>::operator()(const function_symbol& symbol, ForwardIterator begin) const noexcept
{
  // The arity is defined by the function symbol iff N is unchanged and the arity is N otherwise.
  const std::size_t arity = (N == DynamicNumberOfArguments) ? symbol.arity() : N;

  // This is a function application with arguments, hash each argument and combine the result.
  std::size_t hnr = operator()(symbol);
  for (std::size_t i = 0; i < arity; ++i)
  {
    hnr = combine(hnr, *begin);
    ++begin;
  }

  return hnr;
}

template<std::size_t N>
std::size_t aterm_hasher_finite<N>::operator()(const function_symbol& symbol, std::array<unprotected_aterm, N> arguments) const noexcept
{
  std::size_t hnr = operator()(symbol);

  // This is a function application with arguments, hash each argument and combine the result.
  for (std::size_t i = 0; i < N; ++i)
  {
    hnr = combine(hnr, arguments[i]);
  }

  return hnr;
}

template<std::size_t I = 0, typename... Tp,
         typename std::enable_if<I == sizeof...(Tp) - 1, void>::type* = nullptr>
std::size_t combine_args(std::size_t hnr, const Tp&... t)
{
  return combine(hnr, std::get<I>(std::forward_as_tuple(t...)));
}

template<std::size_t I = 0, typename... Tp,
         typename std::enable_if<I < sizeof...(Tp) - 1, void>::type* = nullptr>
std::size_t combine_args(std::size_t hnr, const Tp&... t)
{
  return combine_args<I+1>(combine(hnr, std::get<I>(std::forward_as_tuple(t...))), t...);
}

template<std::size_t N>
template<typename ...Args>
std::size_t aterm_hasher_finite<N>::operator()(const function_symbol& symbol, const Args&... args) const noexcept
{
  std::size_t hnr = operator()(symbol);

  // This is a function application with arguments, hash each argument and combine the result.
  return combine_args(hnr, args...);
}

std::size_t aterm_int_hasher::operator()(const _aterm_int& term) const noexcept
{
  return term.value();
}

std::size_t aterm_int_hasher::operator()(std::size_t value) const noexcept
{
  return value;
}

template<std::size_t N>
bool aterm_equals<N>::operator()(const _aterm& first, const _aterm& second) const noexcept
{
  if (&first == &second)
  {
    // If the pointers are equal they match by definition
    return true;
  }

  if (first.function() == second.function())
  {
    // The arity is defined by the function symbol iff N is unchanged and the arity is N otherwise.
    const std::size_t arity = (N == DynamicNumberOfArguments) ? first.function().arity() : N;

    // Check whether the remaining arguments match
    for (std::size_t i = 0; i < arity; ++i)
    {
      if (reinterpret_cast<const _aterm_appl<aterm>&>(first).arg(i)
            != reinterpret_cast<const _aterm_appl<aterm>&>(second).arg(i))
      {
        return false;
      }
    }

    return true; // The function symbol and all arguments match.
  }

  return false; // Function symbols are different.
}

template<std::size_t N>
bool aterm_equals<N>::operator()(const _aterm& term, const function_symbol& symbol) const noexcept
{
  return term.function() == symbol;
}

template<std::size_t N>
bool aterm_equals<N>::operator()(const _aterm& term, const function_symbol& symbol, unprotected_aterm arguments[]) const noexcept
{
  if (term.function() == symbol)
  {
    // Each argument should be equal.
    for (std::size_t i = 0; i < symbol.arity(); ++i)
    {
      if (reinterpret_cast<const _aterm_appl<aterm>&>(term).arg(i) != arguments[i])
      {
        return false;
      }
    }

    return true;
  }

  return false;
}

template<std::size_t N>
template<typename ForwardIterator,
         typename std::enable_if<is_iterator<ForwardIterator>::value>::type*>
inline bool aterm_equals<N>::operator()(const _aterm& term, const function_symbol& symbol, ForwardIterator begin) const noexcept
{
  if (term.function() == symbol)
  {
    const std::size_t arity = (N == DynamicNumberOfArguments) ? symbol.arity() : N;

    // Each argument should be equal.
    for (std::size_t i = 0; i < arity; ++i)
    {
      if (reinterpret_cast<const _aterm_appl<aterm>&>(term).arg(i) != (*begin))
      {
        return false;
      }
      ++begin;
    }

    return true;
  }

  return false;
}

template<std::size_t N>
bool aterm_equals_finite<N>::operator()(const _aterm& term, const function_symbol& symbol, std::array<unprotected_aterm, N> arguments) const noexcept
{
  if (term.function() == symbol)
  {
    // Each argument should be equal.
    for (std::size_t i = 0; i < N; ++i)
    {
      if (reinterpret_cast<const _aterm_appl<aterm, N>&>(term).arg(i) != arguments[i])
      {
        return false;
      }
    }

    return true;
  }

  return false;
}

template<std::size_t I = 0,
         typename... Tp,
         typename std::enable_if<I == sizeof...(Tp) - 1, void>::type* = nullptr>
bool equal_args(const _aterm_appl<aterm, 8>& term, const Tp&... t)
{
  return term.arg(I) == std::get<I>(std::forward_as_tuple(t...));
}

template<std::size_t I = 0,
         typename... Tp,
         typename std::enable_if<I < sizeof...(Tp) - 1, void>::type* = nullptr>
bool equal_args(const _aterm_appl<aterm, 8>& term, const Tp&... t)
{
  return term.arg(I) == std::get<I>(std::forward_as_tuple(t...)) && equal_args<I+1>(term, t...);
}

template<std::size_t N>
template<typename ...Args>
bool aterm_equals_finite<N>::operator()(const _aterm& term, const function_symbol& symbol, const Args&... args) const noexcept
{
  return (term.function() == symbol && equal_args(reinterpret_cast<const _aterm_appl<aterm, 8>&>(term), args...));
}

bool aterm_int_equals::operator()(const _aterm_int& first, const _aterm_int& second) const noexcept
{
  return (first.value() == second.value());
}

bool aterm_int_equals::operator()(const _aterm_int& term, std::size_t value) const noexcept
{
  return (term.value() == value);
}

} // namespace detail
} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_HASH_H_