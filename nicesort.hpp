#include <climits>
#include <algorithm>
#include <execution>
#include <iterator>

namespace nice
{

enum { bsize0 = 16 };

constexpr auto assign(auto& ...a) noexcept
{
  return [&](auto const ...b) noexcept { assign((a = b)...); };
}

template <typename It>
void insertion_sort(It const i, It const j, auto cmp)
  noexcept(noexcept(cmp(*i, *i)))
{
  for (It m(i + 1); j != m; ++m)
  {
    if (cmp(*m, *(m - 1)))
    {
      auto tmp(std::move(*m));

      auto n(m);

      do
        *n = std::move(*(n - 1));
      while ((i != --n) && cmp(tmp, *(n - 1)));

      *n = std::move(tmp);
    }
  }
}

template <auto E, typename It>
void merge(It& a, It& b, It& c, It& d, auto&& cmp)
  noexcept(noexcept(cmp(*a, *a)))
{ // merge runs [a, b) and [c, d)
  // assert(b == c);
  if (cmp(*b, *(b - 1)))
    std::inplace_merge(E, a, b, d, cmp);

  assign(b, c)(d, a);
}

template <auto E = std::execution::unseq,
  typename It,
  class Cmp = std::less<typename std::iterator_traits<It>::value_type>>
void sort(It i, It const e, Cmp&& cmp = Cmp())
  noexcept(noexcept(cmp(*i, *i)))
{
  std::pair<It, It> runs[sizeof(unsigned) * CHAR_BIT];

  unsigned mask{}; // occupancy mask

  for (auto sz(std::distance(i, e)); e != i;)
  {
    auto j(i);

    // advance j
    std::advance(j, sz >= int(bsize0) ? sz -= int(bsize0), int(bsize0) : sz);

    insertion_sort(i, j, cmp); // sort run [i, j)

    // merge run [i, j) with valid stored runs
    auto r(runs);
    ++mask;

    for (auto n(~mask & (mask - 1)); n; n >>= 1)
    { // ~(x + 1) & x - isolate trailing ones
      auto& [c, d](*r++);
      merge<E>(c, d, i, j, cmp);
    }

    assign(r->first, r->second, i)(i, j, j); // *r = {i, j}, i = j
  }

  auto& [m, n](runs[std::countr_zero(mask)]); // first valid stored run

  while (mask &= mask - 1) // x &= x - 1 - clear the least significant (rightmost) set bit
  { // merge remaining valid stored runs
    auto& [c, d](runs[std::countr_zero(mask)]);
    merge<E>(c, d, m, n, cmp);
  }
}

}
