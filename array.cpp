#include <iostream>
#include <memory>

#include "array.hpp"

int main()
{
  dq::array<int, 3> ca;

  *ca.end() = {}; // end() is not constant and is always dereferencable

  ca.push_front(1ul);
  ca.push_back(2ll, 3l); // push multiple values in one go
  ca.push_front(4u);

  std::sort(ca.begin(), ca.end());

  std::cout << "size: " << ca.size() << std::endl;
  std::cout << "full: " << ca.full() << std::endl;

  std::copy(ca.cbegin(), ca.cend(), std::ostream_iterator<int>(std::cout, "\n"));

  ca.push_back(5, 10);

  std::cout << "size: " << ca.size() << std::endl;
  std::cout << "full: " << ca.full() << std::endl;

  for (auto&& [b, e]: ca.split())
    std::copy(b, e, std::ostream_iterator<int>(std::cout, "\n"));

  ca.erase(std::next(ca.cbegin()));

  std::cout << "size: " << ca.size() << std::endl;
  std::cout << "full: " << ca.full() << std::endl;

  std::for_each(
    ca.cbegin(),
    ca.cend(),
    [](auto&& v) { std::cout << v << std::endl; }
  );

  std::cout << std::distance(ca.cbegin(), ca.cend()) << " : " << ca.size() << " " << ca[0] << std::endl;

  erase(ca, 10, 9, 8); // erase as much as you like

  dq::array<std::unique_ptr<int>, 4> m(dq::multi, std::make_unique<int>(2));

  m.push_back(std::make_unique<int>(1));
  m.insert(dq::multi, m.begin(), std::make_unique<int>(3), std::make_unique<int>(4));
  m.insert(dq::multi, m.end(), std::make_unique<int>(3), std::make_unique<int>(4));

  return 0;
}
