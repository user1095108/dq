#include <cassert>
#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <ranges>
#include <vector>

#include "nicesort.hpp"

constexpr inline std::size_t N(1000000);

bool is_stable_sort()
{
  auto const v(
    std::views::iota(0, 1000) |
    std::views::transform(
      [](int const i) noexcept
      {
        return std::pair{i % 3, i};
      }
    )
  );

  std::vector<std::pair<int, int>> c(v.begin(), v.end());

  auto const cmp([](auto const& a, auto const& b) noexcept
    { return std::get<0>(a) < std::get<0>(b); });

  nice::sort(c.begin(), c.end());

  return std::ranges::is_sorted(c, cmp) &&
    (c.cend() == std::ranges::adjacent_find(std::as_const(c),
      [](auto const& l, auto const& r) noexcept
      { // equal keys but wrong order - not stable
        return l.first == r.first && l.second > r.second;
      }));
}

void test_run(std::string_view const& title, auto& a)
{
  std::cout << "=== " << title << " ===" << std::endl;

  std::vector<int> b, c;
  c = b = a;
  assert(b == a);
  assert(c == a);

  //
  decltype(std::chrono::high_resolution_clock::now()) start, end;

  //
  start = std::chrono::high_resolution_clock::now();
  std::sort(a.begin(), a.end());
  end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> const d0(end - start);

  //
  start = std::chrono::high_resolution_clock::now();
  std::stable_sort(b.begin(), b.end());
  end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> const d1(end - start);

  //
  start = std::chrono::high_resolution_clock::now();
  nice::sort(c.begin(), c.end());
  end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> const d2(end - start);

  //
  assert(std::is_sorted(a.cbegin(), a.cend()));
  assert(std::is_sorted(b.cbegin(), b.cend()));
  assert(std::is_sorted(c.cbegin(), c.cend()));

  //
  std::cout << "std::sort time: " << d0.count() << " seconds" << std::endl;
  std::cout << "std::stable_sort time: " << d1.count() << " seconds" << std::endl;
  std::cout << "nice::sort time: " << d2.count() << " seconds" << std::endl;
}

int main()
{
  std::cout << "nice::sort is stable? " << is_stable_sort() << std::endl;

  std::vector<int> a;

  a.resize(N);

  //
  std::iota(a.rbegin(), a.rend(), 0);
  assert(!std::is_sorted(a.cbegin(), a.cend()));
  test_run("reverse-sorted", a);

  //
  std::mt19937 gen(std::random_device{}());
  std::uniform_int_distribution<int> dist(
    std::numeric_limits<int>::min(),
    std::numeric_limits<int>::max()
  );

  std::generate(a.begin(), a.end(), [&]{ return dist(gen); });
  assert(!std::is_sorted(a.cbegin(), a.cend()));
  test_run("random", a);

  //
  std::iota(a.begin(), a.end(), 0);
  test_run("already sorted", a);

  //
  for (std::size_t i = 0; i < N / 1000; ++i)
    *std::next(a.begin(), gen() % N) = dist(gen);
  test_run("mostly-sorted", a);

  //
  std::fill(a.begin(), a.end(), dist(gen));
  test_run("constant-value", a);

  //
  {
    auto it = a.begin();
    for (std::size_t i = 0; i != N / 2; ++i) *it++ = i;
    for (std::size_t i = N / 2; i != N;  ++i) *it++ = N - i;
  }
  test_run("organ-pipe", a);

  // Sawtooth
  a.clear();
  for (std::size_t i = 0; i < N; ++i)
    a.push_back(i % 1000);
  test_run("sawtooth", a);

  // Checkerboard (alternating high-low)
  a.clear();
  for (std::size_t i = 0; i < N; ++i)
    a.push_back((i % 2 == 0) ? i : N - i);
  test_run("checkerboard", a);

  // Two-valued (binary)
  a.clear();
  for (std::size_t i = 0; i < N; ++i)
    a.push_back(i % 2);
  test_run("two-valued", a);

  // Ascending blocks
  a.clear();
  for (std::size_t i = 0; i < N; ++i)
    a.push_back((i / 1000) * 1000 + (i % 1000));
  test_run("ascending-blocks", a);

  // Descending blocks
  a.clear();
  for (std::size_t i = 0; i < N; ++i)
    a.push_back((i / 1000) * 1000 + (999 - (i % 1000)));
  test_run("descending-blocks", a);

  // Gaussian distribution
  {
    std::normal_distribution<> normal_dist(N / 2, N / 10);
    a.clear();
    for (std::size_t i = 0; i < N; ++i)
      a.push_back(static_cast<int>(normal_dist(gen)));
    test_run("gaussian", a);
  }

  // V-shape
  a.clear();
  for (std::size_t i = 0; i < N / 2; ++i) a.push_back(N/2 - i);
  for (std::size_t i = 0; i < N / 2; ++i) a.push_back(i);
  test_run("v-shape", a);

  // Duplicate-heavy
  a.clear();
  for (std::size_t i = 0; i < N; ++i)
    a.push_back((i % 100 == 0) ? dist(gen) : 42);
  test_run("duplicate-heavy", a);

  // Single outlier
  std::iota(a.begin(), a.end(), 0);
  *std::next(a.begin(), N / 2) = N * 10;
  test_run("single-outlier", a);

  // Reversed tail
  std::iota(a.begin(), a.end(), 0);
  std::reverse(std::next(a.begin(), N - N/10), a.end());
  test_run("reversed-tail", a);

  // Sorted with random noise
  std::iota(a.begin(), a.end(), 0);
  for (std::size_t i = 0; i < N; i += 100)
    *std::next(a.begin(), i) = dist(gen);
  test_run("sorted-with-noise", a);

  // Small range (many duplicates)
  a.clear();
  for (std::size_t i = 0; i < N; ++i)
    a.push_back(dist(gen) % 100);
  test_run("small-range", a);

  // Large gaps
  a.clear();
  for (std::size_t i = 0; i < N; ++i)
    a.push_back(i * 1000);
  test_run("large-gaps", a);

  // Bit-reversed order
  {
    auto bit_reverse = [](unsigned x, unsigned bits) {
      unsigned r = 0;
      for (unsigned i = 0; i < bits; ++i) {
        r = (r << 1) | (x & 1);
        x >>= 1;
      }
      return r;
    };
    a.clear();
    unsigned bits = std::ceil(std::log2(N));
    for (unsigned i = 0; i < N; ++i)
      a.push_back(bit_reverse(i, bits));
    test_run("bit-reversed", a);
  }

  // Fibonacci mod N
  a.clear();
  {
    int aa = 0, bb = 1;
    for (std::size_t i = 0; i < N; ++i) {
      a.push_back(aa % N);
      int next = aa + bb;
      aa = bb; bb = next;
    }
  }
  test_run("fibonacci-mod", a);

  // Plateau (half constant, half increasing)
  a.clear();
  for (std::size_t i = 0; i < N/2; ++i) a.push_back(42);
  for (std::size_t i = N/2; i < N; ++i) a.push_back(i);
  test_run("plateau", a);

  // Zig-zag
  a.clear();
  for (std::size_t i = 0; i < N; ++i)
    a.push_back((i % 2 == 0) ? i : N - i);
  test_run("zig-zag", a);

  // Geometric growth
  a.clear();
  {
    long long v = 1;
    for (std::size_t i = 0; i < N; ++i) {
      a.push_back(v % 1000000);
      v *= 2;
    }
  }
  test_run("geometric", a);

  // Primes mod N
  {
    auto is_prime = [](int x) {
      if (x < 2) return false;
      for (int i = 2; i*i <= x; ++i)
        if (x % i == 0) return false;
      return true;
    };
    a.clear();
    for (std::size_t i = 0, count = 0; count < N; ++i) {
      if (is_prime(i)) {
        a.push_back(i % N);
        ++count;
      }
    }
    test_run("primes", a);
  }

  // Interleaved sorted runs
  a.clear();
  for (std::size_t i = 0; i < N; ++i)
    a.push_back((i % 2 == 0) ? i/2 : (N/2 + i/2));
  test_run("interleaved-runs",  a);

  //
  return 0;
}
