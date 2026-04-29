#include <cassert>
#include <iostream>
#include <chrono>
#include <random>

#include "array.hpp"

void randomize(auto& ...a) noexcept
{
  (
    [&]() noexcept
    {
      a.resize(std::rand() % a.capacity() + 1);
      a.clear();
    }(),
    ...
  );
}

int main()
{
  constexpr std::size_t N(1000000);

  dq::array<int, N, dq::NEW> a;

  std::srand(std::random_device{}());
  randomize(a);

  decltype(std::chrono::high_resolution_clock::now()) start, end;
  a.resize(N);
  
  //
  std::iota(a.rbegin(), a.rend(), 0);
  assert(!std::is_sorted(a.cbegin(), a.cend()));

  start = std::chrono::high_resolution_clock::now();
  std::sort(std::execution::unseq, a.begin(), a.end());
  end = std::chrono::high_resolution_clock::now();
  assert(std::is_sorted(a.cbegin(), a.cend()));

  std::chrono::duration<double> const d0(end - start);

  //
  std::iota(a.rbegin(), a.rend(), 0);
  assert(!std::is_sorted(a.cbegin(), a.cend()));

  start = std::chrono::high_resolution_clock::now();
  a.sort(a.begin(), a.end());
  end = std::chrono::high_resolution_clock::now();
  assert(std::is_sorted(a.cbegin(), a.cend()));

  std::chrono::duration<double> const d1(end - start);

  //
  std::cout << "std::sort time: " << d0.count() << " seconds" << std::endl;
  std::cout << "dq::sort time: " << d1.count() << " seconds" << std::endl;

  //
  return 0;
}
