#include <climits>
#include <algorithm>
#include <bit>
#include <execution>
#include <iterator>

namespace nice
{

enum { bsize0 = 16 };

template <typename It>
void insertion_sort(It const i, It const j, auto cmp)
{
  for (auto m(i + 1); j != m; ++m)
    if (cmp(*m, *(m - 1)))
    {
      auto tmp(std::move(*m));

      auto n(m);

      while (*n = std::move(*(n - 1)), (i != --n) && cmp(tmp, *(n - 1)));

      *n = std::move(tmp);
    }
}

template <auto E, typename It>
void merge(It& a, It& b, It& c, It& d, auto& cmp)
{ // merge runs [a, b) and [c, d)
  // assert(b == c);
  if (cmp(*b, *(b - 1)))
    std::inplace_merge(E, a, b, d, cmp);

  b = d; c = a;
}

template <auto E = std::execution::unseq,
  typename It,
  class Cmp = std::less<typename std::iterator_traits<It>::value_type>>
void sort(It i, It const e, Cmp&& cmp = Cmp())
{
  if (e == i) return;

  std::pair<It, It> runs[sizeof(unsigned) * CHAR_BIT];

  unsigned mask{}; // occupancy mask

  {
    auto j(i);
    std::size_t sz(e - i);

    do
    {
      { // advance j
        auto const d(std::min(std::size_t(bsize0), sz));
        j += d; sz -= d;
      }

      insertion_sort(i, j, cmp); // sort run [i, j)

      // merge run [i, j) with valid stored runs
      auto r(runs);
      ++mask;

      for (auto n(~mask & (mask - 1)); n; n >>= 1)
      { // ~(x + 1) & x - isolate trailing ones
        auto& [c, d](*r++);
        merge<E>(c, d, i, j, cmp);
      }

      *r = {i, j};

      //
      i = j;
    }
    while (sz);
  }

  auto& [c, d](runs[std::countr_zero(mask)]); // first valid stored run

  while (mask &= mask - 1) // x &= x - 1 - clear the least significant (rightmost) set bit
  { // merge remaining valid stored runs
    auto& [a, b](runs[std::countr_zero(mask)]);
    merge<E>(a, b, c, d, cmp);
  }
}

}
