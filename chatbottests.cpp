#include <cassert>
#include <deque>
#include <forward_list>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <random>
#include <thread>
#include <vector>

#include "array.hpp"

void test() {
  // Basic construction, element access, and modifiers.
  {
    // Construct a container using the multi-argument tag.
    dq::array<int, 20> dq(dq::multi, 1, 2, 3);

    // Verify the initial size.
    assert(dq.size() == 3);

    // Verify read access by index.
    assert(dq[0] == 1);
    assert(dq[1] == 2);
    assert(dq[2] == 3);

    // Verify write access by index.
    dq[1] = 42;
    assert(dq[1] == 42);

    // Remove the back element.
    dq.pop_back();
    assert(dq.size() == 2);

    // Verify forward iterator traversal.
    auto it = dq.begin();
    assert(*it == 1);
    ++it;
    assert(*it == 42);
    ++it;
    assert(it == dq.end());

    // Clear all elements.
    dq.clear();
    assert(dq.empty());
    assert(dq.size() == 0);

    // Insert elements at a specific position.
    dq.push_back(4);
    dq.insert(dq.begin() + 1, 5);
    assert(dq.size() == 2);
    assert(dq[0] == 4);
    assert(dq[1] == 5);

    // Erase the first element.
    dq.erase(dq.begin());
    assert(dq.size() == 1);
    assert(dq[0] == 5);

    // Resize to a larger size.
    dq.resize(5);
    assert(dq.size() == 5);

    // Swap contents with another container.
    decltype(dq) otherContainer;
    otherContainer.push_back(6);
    otherContainer.push_back(7);
    dq.swap(otherContainer);
    assert(dq.size() == 2);
    assert(otherContainer.size() == 5);

    // Copy construct a new container.
    decltype(dq) copiedContainer = dq;
    assert(copiedContainer.size() == 2);
    assert(copiedContainer[0] == 6);

    // Verify range-based iteration.
    int sum = 0;
    for (const auto& item : dq) {
        sum += item;
    }
    assert(sum == 13);
  }

  // Front, back, and middle insertion and erasure.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};

    // Access the first element via an iterator.
    auto it = dq.begin();
    assert(*it == 1);

    // Modify through the iterator.
    *it = 10;
    assert(*it == 10);

    // Insert at the front and the back.
    dq.push_front(0);
    dq.push_back(6);
    assert(dq.front() == 0);
    assert(dq.back() == 6);

    // Remove from the front and the back.
    dq.pop_front();
    dq.pop_back();
    assert(dq.front() == 10);
    assert(dq.back() == 5);

    // Insert into the middle.
    it = dq.insert(std::next(dq.begin(), 2), 99);
    assert(*it == 99);
    assert(dq[2] == 99);

    // Erase from the middle.
    it = dq.erase(std::next(dq.begin(), 3));
    assert(*it == 4);
    assert(dq[3] == 4);
  }

  // Push, pop, resize, clear, and swap.
  {
    dq::array<int, 10> d;

    // Insert at both ends.
    d.push_front(1);
    d.push_back(2);
    assert(d.front() == 1 && d.back() == 2);

    // Delete from both ends.
    d.pop_front();
    d.pop_back();
    assert(d.empty());

    // Re-insert and verify size.
    d.push_front(3);
    d.push_back(4);
    assert(d.front() == 3 && d.back() == 4);
    assert(d.size() == 2);

    // Resize to a smaller size.
    d.resize(1);
    assert(d.size() == 1 && d.front() == 3);

    // Clear the container.
    d.clear();
    assert(d.empty());

    // Swap contents of two containers.
    d.push_back(5);
    dq::array<int, 10> d2;
    d2.push_back(6);
    d.swap(d2);
    assert(d.front() == 6 && d2.front() == 5);
  }

  // Mixed push_front and push_back with erasure and insertion.
  {
    dq::array<int, 20> dq;

    // Push elements to the back.
    for (int i = 1; i <= 5; ++i) {
        dq.push_back(i);
    }

    // Push elements to the front.
    for (int i = 10; i <= 15; ++i) {
        dq.push_front(i);
    }

    // Verify the logical order.
    assert(dq.size() == 11);
    assert(dq[0] == 15);
    assert(dq[1] == 14);
    assert(dq[2] == 13);
    assert(dq[3] == 12);
    assert(dq[4] == 11);
    assert(dq[5] == 10);
    assert(dq[6] == 1);
    assert(dq[7] == 2);
    assert(dq[8] == 3);
    assert(dq[9] == 4);
    assert(dq[10] == 5);

    // Erase elements from arbitrary positions.
    dq.erase(dq.begin() + 2);
    dq.erase(dq.end() - 3);

    // Verify the modified deque.
    assert(dq.size() == 9);
    assert(dq[0] == 15);
    assert(dq[1] == 14);
    assert(dq[2] == 12);
    assert(dq[3] == 11);
    assert(dq[4] == 10);
    assert(dq[5] == 1);
    assert(dq[6] == 2);
    assert(dq[7] == 4);
    assert(dq[8] == 5);

    // Insert new elements.
    dq.insert(dq.begin() + 3, 100);
    dq.insert(dq.end() - 1, 200);

    // Verify the final state.
    assert(dq.size() == 11);
    assert(dq[0] == 15);
    assert(dq[1] == 14);
    assert(dq[2] == 12);
    assert(dq[3] == 100);
    assert(dq[4] == 11);
    assert(dq[5] == 10);
    assert(dq[6] == 1);
    assert(dq[7] == 2);
    assert(dq[8] == 4);
    assert(dq[9] == 200);
    assert(dq[10] == 5);
  }

  // Copy and move construction and assignment.
  {
    dq::array<int, 20> dq1; // Default construction.
    assert(dq1.empty());

    dq::array<int, 20> dq2 = {1, 2, 3, 4, 5}; // Initializer list.
    assert(!dq2.empty());
    assert(dq2.size() == 5);

    dq::array<int, 20> dq3(dq2); // Copy construction.
    assert(dq3 == dq2);

    dq::array<int, 20> dq4(std::move(dq2)); // Move construction.
    assert(dq4 == dq3);
    assert(dq2.empty());

    // Copy assignment.
    dq1 = dq3;
    assert(dq1 == dq3);

    // Move assignment.
    dq2 = std::move(dq3);
    assert(dq2 == dq4);
    assert(dq3.empty());
  }

  // Element access functions.
  {
    dq::array<int, 20> dq = {1, 2, 3, 4, 5};

    assert(dq.at(0) == 1);
    assert(dq[1] == 2);
    assert(dq.front() == 1);
    assert(dq.back() == 5);

    assert(!dq.empty());
    assert(dq.size() == 5);
  }

  // Emplace operations with the NEW allocator tag.
  {
    dq::array<int, 20, dq::NEW> dq;

    dq.push_back(2);
    dq.push_front(1);
    dq.push_back(3);
    assert(dq.front() == 1);
    assert(dq.back() == 3);

    dq.emplace_back(4);
    dq.emplace_front(0);
    assert(dq.front() == 0);
    assert(dq.back() == 4);

    dq.pop_back();
    assert(dq.back() == 3);
    dq.pop_front();
    assert(dq.front() == 1);

    dq.resize(5);
    assert(dq.size() == 5);

    dq::array<int, 20, dq::NEW> dq2 = {6, 7, 8, 9, 10};
    dq.swap(dq2);
    assert(dq.front() == 6);
  }

  // Iterator and reverse iterator access.
  {
    dq::array<int, 20> dq = {1, 2, 3, 4, 5};

    assert(*dq.begin() == 1);
    assert(*(dq.end() - 1) == 5);
    assert(*dq.rbegin() == 5);
    assert(*(dq.rend() - 1) == 1);

    const dq::array<int, 20> cdq = dq;
    assert(*cdq.cbegin() == 1);
    assert(*(cdq.cend() - 1) == 5);
    assert(*cdq.crbegin() == 5);
    assert(*(cdq.crend() - 1) == 1);
  }

  // Comparison operators.
  {
    dq::array<int, 20> dq1 = {1, 2, 3, 4, 5};
    dq::array<int, 20> dq2 = {1, 2, 3, 4, 5};
    dq::array<int, 20> dq3 = {1, 2, 3, 4, 6};

    assert(dq1 == dq2);
    assert(dq1 != dq3);
    assert(dq1 < dq3);
    assert(dq1 <= dq2);
    assert(dq3 > dq1);
    assert(dq2 >= dq1);
  }

  // Single insert, emplace, range erase, and clear.
  {
    dq::array<int, 20> dq = {1, 2, 3, 4, 5};

    dq.insert(dq.begin() + 2, 6);
    assert((dq == std::array{1, 2, 6, 3, 4, 5}));

    dq.emplace(dq.begin() + 3, 7);
    assert((dq == std::array{1, 2, 6, 7, 3, 4, 5}));

    dq.erase(dq.begin() + 2, dq.begin() + 4);
    assert((dq == std::array{1, 2, 3, 4, 5}));

    dq.clear();
    assert(dq.empty());
  }

  // Push and pop on an initially empty container.
  {
    dq::array<int, 10> dq;

    dq.push_front(1);
    assert(dq.front() == 1);
    dq.push_back(2);
    assert(dq.back() == 2);

    dq.pop_front();
    assert(dq.front() == 2);
    dq.pop_back();
    assert(dq.empty());
  }

  // Copy and move with non-trivial types.
  {
    dq::array<std::string, 10> dq1; // Default construction.
    assert(dq1.empty());

    dq::array<std::string, 10> dq2 = {"one", "two", "three"}; // Initializer list.
    assert(!dq2.empty());
    assert(dq2.size() == 3);

    dq::array<std::string, 10> dq3(dq2); // Copy construction.
    assert(dq3 == dq2);

    dq::array<std::string, 10> dq4(std::move(dq2)); // Move construction.
    assert(dq4 == dq3);
    assert(dq2.empty());

    // Copy assignment.
    dq1 = dq3;
    assert(dq1 == dq3);

    // Move assignment.
    dq2 = std::move(dq3);
    assert(dq2 == dq4);
    assert(dq3.empty());
  }

  // Copy assignment operator.
  {
    dq::array<int, 10> dq1 = {1, 2, 3};
    dq::array<int, 10> dq2 = {4, 5, 6, 7};

    dq1 = dq2;
    assert(dq1 == dq2);
  }

  // Subscript operator.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};

    assert(dq[0] == 1);
    assert(dq[2] == 3);
    assert(dq[4] == 5);
  }

  // std::find integration and the dq::find helper.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto it = std::find(dq.cbegin(), dq.cend(), 3);
    assert(it && *it == 3);
    it = dq::find(dq, 3);
    assert(it && *it == 3);
    it = std::find(dq.begin(), dq.end(), 6);
    assert(it && *it == 6);
    assert(3 == dq::count(dq, 1, 3, 10, 11));
  }

  // Member swap.
  {
    dq::array<int, 10> dq1 = {1, 2, 3};
    dq::array<int, 10> dq2 = {4, 5, 6, 7};

    dq1.swap(dq2);
    assert((dq1 == std::array{4, 5, 6, 7}));
    assert((dq2 == std::array{1, 2, 3}));
  }

  // Comparison operators with empty containers.
  {
    dq::array<int, 10> dq1 = {1, 2, 3};
    dq::array<int, 10> dq2 = {1, 2, 3};
    dq::array<int, 10> dq3 = {1, 2, 3, 4};

    assert(dq1 == dq2);
    assert(dq1 != dq3);
    assert(dq1 < dq3);
    assert(dq1 <= dq2);
    assert(dq3 > dq1);
    assert(dq2 >= dq1);
  }

  // Resize with and without a fill value.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};

    dq.resize(3);
    assert(dq.size() == 3);
    assert((dq == std::array{1, 2, 3}));

    dq.resize(5);
    assert(dq.size() == 5);
  }

  // Emplace at front and back.
  {
    dq::array<int, 10> dq;

    dq.emplace_front(1);
    assert(dq.front() == 1);
    dq.emplace_back(2);
    assert(dq.back() == 2);
  }

  // Single-element insert.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};

    dq.insert(dq.begin() + 2, 6);
    assert((dq == std::array{1, 2, 6, 3, 4, 5}));
  }

  // Single-element emplace.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};

    dq.emplace(dq.begin() + 2, 6);
    assert((dq == std::array{1, 2, 6, 3, 4, 5}));
  }

  // Range erase.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};

    dq.erase(dq.begin() + 2, dq.begin() + 4);
    assert((dq == std::array{1, 2, 5}));
  }

  // Insert a sub-range from another container.
  {
    dq::array<int, 10> dq = {1, 2, 4, 5};
    dq::array<int, 10> to_insert = {3};

    dq.insert(dq.begin() + 2, to_insert.begin(), to_insert.end());
    assert((dq == std::array{1, 2, 3, 4, 5}));
  }

  // Erase from front, middle, and back with iterators.
  {
    dq::array<int, 10> dq = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    dq.erase(dq.begin());
    assert((dq == std::array{1, 2, 3, 4, 5, 6, 7, 8, 9}));

    dq.erase(dq.begin() + 2, dq.begin() + 5);
    assert((dq == std::array{1, 2, 6, 7, 8, 9}));

    // Erase all even numbers using a loop.
    for (auto it = dq.begin(); it != dq.end();) {
      if (*it % 2 == 0)
          it = dq.erase(it);
      else
          ++it;
    }
    assert((dq == dq::array<int, 10>{1, 7, 9}));

    // Erase all remaining elements.
    dq.erase(dq.begin(), dq.end());
    assert(dq.empty());
  }

  // Single-element erase at various positions.
  {
    dq::array dq = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // dq::array<int, 10>
    assert(10 == dq.capacity());
    assert(10 == dq.size());
    assert(dq.full());

    dq.erase(dq.begin() + 3);
    assert((dq == dq::array{0, 1, 2, 4, 5, 6, 7, 8, 9}));

    dq.erase(dq.end() - 1);
    assert((dq == std::array{0, 1, 2, 4, 5, 6, 7, 8}));
  }

  // Swap verifies deep content exchange.
  {
    dq::array<int, 10> d1, d2;

    for (int i = 1; i <= 5; ++i) {
        d1.push_back(i);
    }
    for (int i = 6; i <= 10; ++i) {
        d2.push_back(i);
    }

    d1.swap(d2);

    for (int i = 1; i <= 5; ++i) {
        assert(d2[i-1] == i);
    }
    for (int i = 6; i <= 10; ++i) {
        assert(d1[i-6] == i);
    }
  }

  // Circular overwrite when capacity is reached.
  {
    dq::array<int, 5> stack(dq::multi, 1, 2, 3, 4, 5);

    assert(stack.size() == 5);

    stack.push_back(6);

    assert(stack.size() == 5);
    assert(stack.front() == 2);

    stack.pop_front();
    stack.pop_front();
    assert(stack.front() == 4);

    stack.clear();
    assert(stack.empty());
  }

  // Pop front and back, then restore original size.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};

    assert(dq.size() == 5);

    dq.pop_front();
    dq.pop_back();

    assert(dq.size() == 3);
    assert(dq.front() == 2);
    assert(dq.back() == 4);

    dq.push_front(1);
    dq.push_back(5);

    assert(dq.size() == 5);
    assert(dq.front() == 1);
    assert(dq.back() == 5);

    dq.clear();
    assert(dq.empty());
  }

  // Palindrome verification using front and back.
  {
    dq::array<char, 10> palindrome(dq::from_range, std::string_view("racecar"));

    while(palindrome.size() > 1) {
      assert(palindrome.front() == palindrome.back());
      palindrome.pop_front();
      palindrome.pop_back();
    }

    assert(1 == palindrome.size());
  }

  // Simulate a deck of cards.
  {
    dq::array<std::string, 52> deck;

    std::string suits[4] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    std::string ranks[13] = {"Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King"};

    for(auto &suit : suits) {
        for(auto &rank : ranks) {
            deck.push_back(rank + " of " + suit);
        }
    }

    assert(deck.size() == 52);

    std::string topCard = deck.front();
    deck.pop_front();
    assert(deck.size() == 51);

    std::string bottomCard = deck.back();
    deck.pop_back();
    assert(deck.size() == 50);

    assert(topCard != bottomCard);
  }

  // Erase from the middle and verify neighbors.
  {
    dq::array<int, 10> deque;

    deque.push_back(1);
    deque.push_front(2);
    deque.push_back(3);

    assert(deque.size() == 3);
    assert(deque[0] == 2);
    assert(deque[1] == 1);
    assert(deque[2] == 3);

    deque.erase(deque.begin() + 1);

    assert(deque.size() == 2);
    assert(deque[0] == 2);
    assert(deque[1] == 3);
  }

  // Simulate a queue with VIP front insertion.
  {
    dq::array<std::string, 10> line;

    line.push_back("Alice");
    line.push_back("Bob");
    line.push_back("Charlie");

    assert(line.front() == "Alice");

    line.push_front("Dave");
    assert(line.front() == "Dave");

    line.pop_front();
    assert(line.front() == "Alice");

    line.push_front("Eve");
    assert(line.front() == "Eve");

    while(!line.empty()) {
        line.pop_front();
    }

    assert(line.empty());
  }

  // Front and back symmetry.
  {
    dq::array<std::string, 10> magicBox;

    magicBox.push_front("Gold");
    assert(magicBox.back() == "Gold");

    magicBox.push_back("Diamond");
    assert(magicBox.back() == "Diamond");

    assert(magicBox.size() == 2);

    magicBox.pop_front();
    assert(magicBox.front() == "Diamond");

    magicBox.pop_back();
    assert(magicBox.empty());
  }

  // Iterator arithmetic with std::advance.
  {
    dq::array<int, 10> d;

    for(int i = 0; i < 10; ++i) d.push_back(i);

    auto it = d.begin();

    std::advance(it, 5);
    assert(*it == 5);

    std::advance(it, 2);
    assert(*it == 7);

    std::advance(it, -3);
    assert(*it == 4);
  }

  // std::remove_if integration.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    d.erase(std::remove_if(d.begin(), d.end(), [](int n){ return n % 2 == 0; }), d.end());
    for (int n : d) assert(n % 2 != 0);
  }

  // dq::erase_if helper.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    assert(2 == dq::erase_if(d, [](int n){ return n % 2 == 0; }));
    for (int n : d) assert(n % 2 != 0);
  }

  // std::unique integration.
  {
    dq::array<int, 20> dq{1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
    dq.erase(std::unique(dq.begin(), dq.end()), dq.end());
    assert((dq == std::array{1, 2, 3, 4}));
  }

  // Insert a range from a different container type.
  {
    dq::array<int, 10> d = {1, 2, 3, 4};
    dq::array<int, 10> l = {-1, -2, -3};
    auto pos = std::next(d.begin(), 2);
    d.insert(pos, l.begin(), l.end());
    assert((d == std::array{1, 2, -1, -2, -3, 3, 4}));
  }

  // std::merge and std::reverse integration.
  {
    dq::array<int, 10> dq1{1, 2, 3, 4, 5};
    dq::array<int, 10> dq2{6, 7, 8, 9, 10};

    dq::array<int, 10> mdq(10);

    std::merge(dq1.begin(), dq1.end(), dq2.begin(), dq2.end(), mdq.begin());

    assert((mdq == dq::array<int, 10>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));

    std::reverse(mdq.begin(), mdq.end());

    assert((mdq == std::array{10, 9, 8, 7, 6, 5, 4, 3, 2, 1}));
  }

  // Shuffle, rotate, reverse, and sort round-trip.
  {
    dq::array<int, 10> originalDeque{1, 2, 3, 4, 5};
    auto deque{originalDeque};

    std::shuffle(deque.begin(), deque.end(), std::mt19937{std::random_device{}()});

    std::rotate(deque.begin(), deque.begin() + 1 + std::rand() % (deque.size() - 1), deque.end());

    auto mid = deque.begin() + deque.size() / 2;

    std::reverse(mid, deque.end());
    std::sort(deque.begin(), deque.end());

    assert(std::equal(deque.begin(), deque.end(), originalDeque.begin()));
  }

  // Fill constructor with count and value.
  {
    dq::array<int, 10> a(5, 10);

    assert(a.size() == 5);

    for (const auto& e: a) assert(e == 10);
  }

  // Insert at beginning, middle, and end.
  {
    dq::array<int, 20> myDeque = {1, 2, 3, 4, 5};

    auto it = myDeque.insert(myDeque.begin(), 0);
    assert(myDeque.size() == 6);
    assert(*it == 0);
    assert(myDeque.front() == 0);

    it = myDeque.insert(myDeque.begin() + 3, 6);
    assert(myDeque.size() == 7);
    assert(*it == 6);
    assert(myDeque[3] == 6);

    it = myDeque.insert(myDeque.end(), 7);
    assert(myDeque.size() == 8);
    assert(*it == 7);
    assert(myDeque.back() == 7);

    dq::array<int, 20> otherDeque = {8, 9, 10};
    myDeque.insert(myDeque.begin() + 2, otherDeque.begin(), otherDeque.end());
    assert(myDeque.size() == 11);
    assert(myDeque[2] == 8);
    assert(myDeque[3] == 9);
    assert(myDeque[4] == 10);
  }

  // Erase with move-only types.
  {
    dq::array<std::unique_ptr<int>, 10> myDeque;
    for (int i = 0; i < 10; i++) {
      myDeque.push_back(std::make_unique<int>(i));
    }

    auto it = myDeque.begin() + 5;

    it = myDeque.erase(it);

    assert(myDeque.size() == 9);
    assert(*myDeque[5] == 6);
    assert(**it == 6);
  }

  // Erase single element and ranges.
  {
    dq::array<int, 10> deque = {1, 2, 3, 4, 5};
    deque.erase(deque.begin() + 2);
    assert((deque == std::array{1, 2, 4, 5}));

    deque = {1, 2, 3, 4, 5};
    deque.erase(deque.begin() + 1, deque.begin() + 3);
    assert((deque == std::array{1, 4, 5}));

    deque = {1, 2, 3, 4, 5};
    deque.erase(deque.begin(), deque.begin() + 2);
    assert((deque == std::array{3, 4, 5}));

    deque = {1, 2, 3, 4, 5};
    deque.erase(deque.begin() + 3, deque.end());
    assert((deque == std::array{1, 2, 3}));

    deque = {1, 2, 3, 4, 5};
    deque.erase(deque.begin(), deque.end());
    assert(deque.empty());

    deque = {};
    deque.erase(deque.begin(), deque.end());
    assert(deque.empty());
  }

  // Swap with various container states.
  {
    dq::array<int, 10, dq::NEW> deque1 = {1, 2, 3};
    dq::array<int, 10, dq::NEW> deque2 = {4, 5, 6};
    deque1.swap(deque2);
    assert((deque1 == std::array{4, 5, 6}));
    assert((deque2 == std::array{1, 2, 3}));

    deque1 = {1, 2, 3};
    deque2 = {};
    deque1.swap(deque2);
    assert(deque1.empty());
    assert((deque2 == std::array{1, 2, 3}));

    deque1 = {};
    deque2 = {};
    deque1.swap(deque2);
    assert(deque1.empty());
    assert(deque2.empty());

    deque1 = {1, 2, 3};
    deque2 = {1, 2, 3, 4, 5};
    deque1.swap(deque2);
    assert((deque1 == std::array{1, 2, 3, 4, 5}));
    assert((deque2 == std::array{1, 2, 3}));
  }

  // Insert initializer lists at various positions.
  {
    dq::array<int, 20, dq::NEW> deque = {1, 2, 3, 4, 5};
    deque.insert(deque.begin() + 2, 6);
    assert((deque == std::array{1, 2, 6, 3, 4, 5}));

    deque = {1, 2, 3, 4, 5};
    deque.insert(deque.begin() + 2, {6, 7, 8});
    assert((deque == std::array{1, 2, 6, 7, 8, 3, 4, 5}));

    deque = {1, 2, 3, 4, 5};
    deque.insert(deque.begin(), {6, 7, 8});
    assert((deque == std::array{6, 7, 8, 1, 2, 3, 4, 5}));

    deque = {1, 2, 3, 4, 5};
    deque.insert(deque.end(), {6, 7, 8});
    assert((deque == std::array{1, 2, 3, 4, 5, 6, 7, 8}));

    deque = {1, 2, 3, 4, 5};
    deque.insert(deque.begin(), {6, 7, 8});
    deque.insert(deque.end(), {9, 10, 11});
    assert((deque == std::array{6, 7, 8, 1, 2, 3, 4, 5, 9, 10, 11}));

    deque = {};
    deque.insert(deque.begin(), {6, 7, 8});
    deque.insert(deque.end(), {9, 10, 11});
    assert((deque == std::array{6, 7, 8, 9, 10, 11}));
  }

  // append_range, prepend_range, and insert_range.
  {
    dq::array<int, 30> arr{1, 2, 3};

    std::vector<int> vec{4, 5, 6};

    arr.append_range(vec);
    assert((arr == std::array{1, 2, 3, 4, 5, 6}));

    arr.prepend_range(vec);
    assert((arr == std::array{4, 5, 6, 1, 2, 3, 4, 5, 6}));

    auto it = arr.begin();
    std::advance(it, 3);
    arr.insert_range(it, vec);
    assert((arr == std::array{4, 5, 6, 4, 5, 6, 1, 2, 3, 4, 5, 6}));
  }

  // Resize with explicit fill value.
  {
    dq::array<int, 20> dq = {1, 2, 3, 4, 5};

    dq.resize(3);
    assert(dq.size() == 3);
    assert(dq[0] == 1);
    assert(dq[1] == 2);
    assert(dq[2] == 3);

    dq.resize(5, 100);
    assert(dq.size() == 5);
    assert(dq[0] == 1);
    assert(dq[1] == 2);
    assert(dq[2] == 3);
    assert(dq[3] == 100);
    assert(dq[4] == 100);
  }

  // General usage with iterators and modifiers.
  {
    dq::array<int, 20> myDeque;

    myDeque.push_back(10);
    myDeque.push_front(5);
    myDeque.push_back(20);

    assert(myDeque.size() == 3);

    auto it = myDeque.begin();
    assert(*it == 5);

    *it = 15;
    assert(myDeque.front() == 15);

    myDeque.pop_front();
    assert(myDeque.front() == 10);

    myDeque.clear();
    assert(myDeque.empty());
  }

  // Comprehensive functional test case.
  {
    dq::array<int, 20> deque1 = {1, 2, 3};
    assert(deque1.size() == 3);
    assert(deque1[0] == 1);
    assert(deque1[1] == 2);
    assert(deque1[2] == 3);

    deque1.push_front(0);
    deque1.push_back(4);
    assert(deque1.size() == 5);
    assert(deque1[0] == 0);
    assert(deque1[4] == 4);

    int sum = 0;
    for (const auto& elem : deque1) {
        sum += elem;
    }
    assert(sum == 10);

    deque1.erase(deque1.begin() + 2);
    assert(deque1.size() == 4);
    assert(deque1[2] == 3);

    deque1.clear();
    assert(deque1.empty());

    deque1.emplace_back(10);
    deque1.emplace_front(5);
    assert(deque1.size() == 2);
    assert(deque1[0] == 5);
    assert(deque1[1] == 10);
    assert(dq::find(deque1, 10));
    assert(dq::erase(deque1, 5, 10));
    assert(deque1.empty());
    assert(!dq::find(deque1, 10));
  }

  // dq::erase and dq::erase_if with std::find validation.
  {
    dq::array<int, 10> d{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    dq::erase(d, 5);
    assert(std::find(d.begin(), d.end(), 5) == d.end());
    assert(!dq::find(d, 5));

    dq::erase_if(d, [](int n){ return n % 2 == 0; });
    assert(std::none_of(d.begin(), d.end(), [](int n){ return n % 2 == 0; }));
    assert(!dq::find_if(d, [](int n){ return n % 2 == 0; }));
    assert(!dq::count_if(d, [](int n){ return n % 2 == 0; }));
  }

  // erase_if and erase helpers with expected results.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    dq::erase_if(d, [](int i){ return i % 2 == 0; });
    assert(d.size() == 5);
    assert((d == dq::array<int, 10>{1, 3, 5, 7, 9}));

    d = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    dq::erase(d, 5);
    assert(d.size() == 9);
    assert((d == std::array{1, 2, 3, 4, 6, 7, 8, 9, 10}));
  }

  // Range erase with various boundaries.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    dq.erase(dq.begin(), dq.begin() + 5);
    assert(dq.size() == 5);
    assert((dq == decltype(dq){6, 7, 8, 9, 10}));

    dq.erase(dq.begin() + 1, dq.end());
    assert(dq.size() == 1);
    assert(dq.front() == 6);

    dq.erase(dq.begin(), dq.end());
    assert(dq.empty());

    dq.erase(dq.begin(), dq.end());
    assert(dq.empty());

    dq.push_back(1);
    dq.erase(dq.begin(), dq.begin() + 1);
    assert(dq.empty());

    dq.push_back(1, 2);
    dq.erase(dq.begin(), dq.end());
    assert(dq.empty());

    dq.push_back(1);
    dq.erase(dq.begin());
    assert(dq.empty());

    dq.push_back(1, 2, 3);
    dq.erase(dq.begin(), dq.end());
    assert(dq.empty());

    dq.push_back(1, 2, 3, 4);
    dq.erase(dq.begin(), dq.end());
    assert(dq.empty());

    dq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    dq.erase(dq.begin() + 2, dq.begin() + 5);
    assert(dq.size() == 7);
  }

  // Range erase returning correct iterators.
  {
    dq::array<int, 10> dq{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    dq.erase(dq.begin() + 2, dq.begin() + 5);
    assert((dq == std::array{1, 2, 6, 7, 8, 9, 10}));

    dq.erase(dq.begin(), dq.end());
    assert(dq.empty());

    dq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    dq.erase(dq.begin() + 4);
    assert((dq == std::array{1, 2, 3, 4, 6, 7, 8, 9, 10}));

    auto it = dq.begin() + 3;
    dq.erase(it, it);
    assert((dq == std::array{1, 2, 3, 4, 6, 7, 8, 9, 10}));

    dq.erase(dq.begin(), dq.end());
    assert(dq.empty());

    dq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    dq.erase(dq.end(), dq.end());
    assert((dq == std::array{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
  }

  // Range construction from a C array.
  {
    int const a[10]{};
    assert(std::ranges::equal(a, dq::array<int, 10>(a)));
  }

  // std::ranges algorithms integration.
  {
    dq::array<int, 20> dq = {3, 1, 4, 1, 5};

    std::ranges::sort(dq);
    assert(std::ranges::is_sorted(dq));
    assert(std::ranges::equal(dq, std::array{1, 1, 3, 4, 5}));

    assert(std::ranges::find(dq, 3) != dq.end());
    assert(std::ranges::count(dq, 1) == 2);
  }

  // Move-only types.
  {
    dq::array<std::unique_ptr<int>, 10> dq;
    dq.emplace_back(std::make_unique<int>(7));
    dq.emplace_front(std::make_unique<int>(9));

    assert(*dq.front() == 9);
    assert(*dq.back()  == 7);

    dq.erase(dq.begin());
    assert(dq.size() == 1);
    assert(*dq.front() == 7);
  }

  // Capacity edge cases with circular overwrite.
  {
    dq::array<int, 4> dq{0, 1, 2, 3};

    assert(dq.full());
    assert(dq.size() == 4);

    dq.push_back(42);
    assert(dq.full());
    assert(dq.size() == 4);
    assert(dq.back()  == 42);
    assert(dq.front() == 1);

    dq.push_front(99);
    assert(dq.full());
    assert(dq.front() == 99);
    assert(dq.back()  == 42);
  }

  // Heterogeneous lookup with std::string_view.
  {
    dq::array<std::string, 10> dq = {"apple", "banana", "pear"};
    assert(std::ranges::find(dq, std::string_view("banana")) != dq.end());
    assert(std::ranges::find(dq, std::string_view("grape")) == dq.end());
  }

  // Iterator stability after insert and erase.
  {
    dq::array<int, 20> dq = {0,1,2,3,4,5};
    auto it = dq.begin() + 3;
    auto dist = std::distance(dq.begin(), it);

    dq.insert(it, 42);
    it = dq.begin() + dist;
    assert(*it == 42);

    it = dq.erase(it);
    assert(*it == 3);
  }

  // Randomized stress test.
  {
    constexpr int N = 1'000;
    dq::array<int, N> dq;
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> op(0, 3);

    for (int i = 0; i < 10 * N; ++i)
    {
      switch (op(rng))
      {
        case 0: dq.push_back(i); break;
        case 1: dq.push_front(i); break;
        case 2: if (!dq.empty()) dq.pop_back(); break;
        case 3: if (!dq.empty()) dq.pop_front(); break;
      }
      assert(dq.size() <= N);
    }
  }

  // Ranges projection with custom struct.
  {
    struct Point { int x, y; };
    dq::array<Point, 10> dq = {{5,6}, {1,2}, {3,4}};

    std::ranges::sort(dq, std::less<>{}, &Point::y);
    assert(dq[0].y == 2 && dq[2].y == 6);
  }

  // Const iterators yield const references.
  {
    const dq::array<int, 5> dq = {10,20,30};
    static_assert(std::same_as<decltype(dq.begin()), dq::array<int,5>::const_iterator>);
    assert(std::accumulate(dq.begin(), dq.end(), 0) == 60);
  }

  // Resize with value initialization.
  {
    dq::array<int, 10> dq = {1,2,3};
    dq.resize(6, 42);
    assert(dq.size() == 6);
    assert(dq[3] == 42 && dq[5] == 42);

    dq.resize(2);
    assert(dq.size() == 2);
  }

  // Self-swap is a no-op.
  {
    dq::array<int, 5> dq = {1,2,3};
    dq.swap(dq);
    assert(dq.size() == 3 && dq[0] == 1);
  }

  // Reverse iterator ranges.
  {
    dq::array<int, 10> dq = {1,2,3,2,1};
    auto rfirst = dq.rbegin(), rlast = dq.rend();
    assert(std::ranges::count(rfirst, rlast, 2) == 2);
  }

  // erase_if with lambda capture.
  {
    dq::array<int, 10> dq = {1,2,3,4,5,6};
    int threshold = 4;
    dq::erase_if(dq, [threshold](int x){ return x > threshold; });
    assert((dq == std::array{1,2,3,4}));
  }

  // Single-producer single-consumer threading.
  {
    dq::array<int, 10> buffer;
    std::atomic<bool> done{};
    std::mutex m;
    int consumed{};

    std::thread producer([&]{
      for (int i = 0; i < 1000;)
      {
        if (std::lock_guard l(m); !buffer.full())
          buffer.push_back(i++);
      }
      done = true;
    });

    std::thread consumer([&]{
      for (;;)
      {
        if (std::lock_guard l(m); !buffer.empty())
          consumed += buffer.front(), buffer.pop_front();
        else if (done) break;
      }
    });

    producer.join();
    consumer.join();

    assert(consumed == 999*1000/2);
  }

  // Insert from a raw C array.
  {
    int raw[] = {9,8,7};
    dq::array<int, 10> dq = {1,2,3};
    dq.insert(dq.begin()+1, std::begin(raw), std::end(raw));
    assert((dq == std::array{1,9,8,7,2,3}));
  }

  // assign with an initializer list.
  {
    dq::array<int, 10> dq;
    dq.assign({10,20,30});
    assert((dq == std::array{10,20,30}));
    assert(dq.size() == 3);
  }

  // std::swap overload.
  {
    dq::array<std::string, 5> a = {"a","b"};
    dq::array<std::string, 5> b = {"x","y","z"};
    std::swap(a,b);
    assert((a == std::initializer_list<std::string_view>{"x","y","z"}));
    assert((b == std::initializer_list<std::string_view>{"a","b"}));
  }

  // std::inner_product integration.
  {
    dq::array<int, 5> a = {1,2,3};
    dq::array<int, 5> b = {4,5,6};
    int dot = std::inner_product(a.begin(), a.end(), b.begin(), 0);
    assert(dot == 32);
  }

  // Exact reverse iteration.
  {
    dq::array<int, 6> dq = {0,1,2,3,4,5};
    std::vector<int> expected = {5,4,3,2,1,0};
    std::vector<int> actual;
    std::copy(dq.rbegin(), dq.rend(), std::back_inserter(actual));
    assert(actual == expected);
  }

  // Lexicographical comparison via ranges.
  {
    dq::array<int, 20> dq = {1,2,3};
    std::vector<int> v = {1,2,3,4};

    assert(std::ranges::lexicographical_compare(dq, v));
    assert(!std::ranges::lexicographical_compare(v, dq));
    assert(!std::ranges::lexicographical_compare(dq, dq));
  }

  // Fill construction and subsequent modification.
  {
    dq::array<int, 10> dq(7, 42);
    assert(dq.size() == 7);
    assert(std::all_of(dq.begin(), dq.end(), [](int x){ return x==42; }));

    dq[3] = 99;
    assert(dq[3] == 99);
    assert(std::count(dq.begin(), dq.end(), 42) == 6);
  }

  // Front element tracking through cyclic overwrites.
  {
    dq::array<char, 4> dq;
    for (char c = 'a'; c <= 'z'; ++c)
    {
      dq.push_back(c);
      if (dq.size() > 1)
        assert(dq.front() == char('a' + (c - 'a') - (dq.size() - 1)));
    }
  }

  // Shrink via resize.
  {
    dq::array<int, 100> dq(std::ranges::iota_view{0, 50});
    dq.resize(10);
    assert(dq.size() == 10);
    assert(dq.front() == 0 && dq.back() == 9);
  }

  // Parallel unsequenced reduce.
  {
    dq::array<int, 10> dq = {1,2,3,4,5};
    int sum = std::reduce(std::execution::unseq, dq.begin(), dq.end(), 0);
    assert(sum == 15);
  }

  // Rotational equivalence of push_front and pop_back.
  {
    dq::array<int, 8> a = {1,2,3,4,5,6};
    auto b = a;

    for (unsigned i = 0; i < 6; ++i)
    {
      auto const t(b.back());
      b.pop_back();
      b.push_front(t);
    }
    assert(a == b);
  }

  // std::rotate and reverse rotate.
  {
    dq::array<int, 10> dq = {1,2,3,4,5};
    std::rotate(dq.begin(), dq.begin()+2, dq.end());
    assert((dq == std::array{3,4,5,1,2}));

    std::rotate(dq.rbegin(), dq.rbegin()+2, dq.rend());
    assert((dq == std::array{1,2,3,4,5}));
  }

  // Non-trivial type with full capacity overwrites.
  {
    dq::array<std::string, 5> dq;
    dq.push_back("hello");
    dq.push_back("world");
    dq.push_front("start");
    assert(dq.size() == 3);
    assert(dq[0] == "start");
    assert(dq[1] == "hello");
    assert(dq[2] == "world");

    dq.pop_front();
    assert(dq.front() == "hello");

    dq = {"a", "b", "c", "d", "e"};
    assert(dq.full());
    dq.push_back("f");
    assert(dq.size() == 5);
    assert(dq[0] == "b");
    assert(dq[4] == "f");

    dq = {"a", "b", "c", "d", "e"};
    dq.push_front("f");
    assert(dq.size() == 5);
    assert(dq[0] == "f");
    assert(dq[4] == "e");
  }

  // Move-only type lifecycle.
  {
    dq::array<std::unique_ptr<int>, 3> dq;
    dq.push_back(std::make_unique<int>(1));
    dq.push_back(std::make_unique<int>(2));
    dq.push_front(std::make_unique<int>(3));

    assert(*dq[0] == 3);
    assert(*dq[1] == 1);
    assert(*dq[2] == 2);

    auto p = std::move(dq[1]);
    assert(*p == 1);
    assert(!dq[1]);
    dq.pop_back();
    assert(dq.size() == 2);
    dq.pop_back();
    assert(dq.size() == 1);
    assert(*dq[0] == 3);
  }

  // Self-assignment through a reference.
  {
    dq::array<int, 5> dq = {1, 2, 3};
    auto& dq_ref = dq;
    dq = dq_ref;
    assert(dq.size() == 3);
    assert(dq[0] == 1);
  }

  // Self-swap with the NEW allocator.
  {
    dq::array<int, 5, dq::NEW> dq = {1, 2, 3};
    dq.swap(dq);
    assert(dq.size() == 3);
    assert(dq[0] == 1);
  }

  // Range assignment from a vector.
  {
    std::vector<int> v = {10, 20, 30, 40};
    dq::array<int, 10> dq;
    dq.assign(v.begin(), v.end());
    assert(dq.size() == 4);
    assert(dq[0] == 10);
    assert(dq[3] == 40);
  }

  // Destructor correctness via a counting type.
  {
    static int count;
    struct Counter {
      int x;
      Counter() { ++count; }
      Counter(int x_) : x(x_) { ++count; }
      ~Counter() { --count; }
      Counter(const Counter& other) : x(other.x) { ++count; }
      Counter(Counter&& other) : x(other.x) { ++count; }
      Counter& operator=(const Counter&) = default;
      Counter& operator=(Counter&&) = default;
    };

    {
      dq::array<Counter, 10> dq;
      dq.push_back(1);
      dq.push_back(2);
      assert(count == 11);
      dq.pop_back();
      assert(count == 11);
      dq.clear();
      assert(count == 11);
    }
    assert(count == 0);
  }

  // Stress test with mixed operations.
  {
    dq::array<int, 100> dq;
    for (int i = 0; i < 10000; ++i) {
      switch (i % 5) {
        case 0: dq.push_back(i); break;
        case 1: dq.push_front(i); break;
        case 2: if (!dq.empty()) dq.pop_back(); break;
        case 3: if (!dq.empty()) dq.pop_front(); break;
        case 4: if (!dq.empty()) dq.erase(dq.begin() + (i % dq.size())); break;
      }
    }
    assert(dq.size() <= 100);
  }

  // Heterogeneous lookup and multi-key erase.
  {
    dq::array<std::string, 5> dq = {"apple", "banana", "cherry"};
    auto it = dq::find(dq, "banana");
    assert(it && *it == "banana");
    it = dq::find(dq, std::string_view("cherry"));
    assert(it && *it == "cherry");
    dq::erase(dq, "apple", "banana", "cherry");
    assert(dq.empty());
  }

  // Iterator navigation with std::advance, next, and prev.
  {
    dq::array<int, 20> dq = { 10, 20, 30, 40, 50, 60, 70 };

    auto it = dq.begin();

    std::advance(it, 3);
    assert(*it == 40);

    std::advance(it, -2);
    assert(*it == 20);

    std::advance(it, 0);
    assert(*it == 20);

    assert(*std::next(it, 4)  == 60);
    assert(*std::next(it, -1) == 10);
    assert(*std::next(it, 0)  == 20);

    it = dq.end();
    assert(*std::prev(it, 1)  == 70);
    assert(*std::prev(it, 3)  == 50);
    assert(*std::next(it, -2) == 60);
    assert(*std::next(it, -1)  == 70);

    it = dq.begin();
    std::advance(it, static_cast<int>(dq.size()));
    assert(it == dq.end());

    std::advance(it, -static_cast<int>(dq.size()));
    assert(it == dq.begin());

    assert(*std::next(dq.rbegin(), 2) == 50);
    assert(*std::prev(dq.rend(), 3)   == 30);
  }

  // Static capacity and max_size accessors.
  {
    dq::array<int, 7> dq = {1, 2, 3};
    static_assert(dq.capacity() == 7);
    assert(dq.capacity() == 7);
    assert(dq.max_size() == PTRDIFF_MAX);
    assert(dq.size() == 3);
    assert(!dq.full());
  }

  // Raw pointer accessors data(), first(), and last().
  {
    dq::array<int, 5> dq = {10, 20, 30};
    assert(dq.data() != nullptr);
    assert(dq.first() == &dq.front());
    assert(dq.last()  != nullptr);
    assert(dq.last() - dq.first() == static_cast<std::ptrdiff_t>(dq.size()));
  }

  // Bulk pop_front and pop_back.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5, 6, 7, 8};
    dq.pop_back(3);
    assert(dq.size() == 5);
    assert(dq.back()  == 5);
    assert(dq.front() == 1);

    dq.pop_front(2);
    assert(dq.size() == 3);
    assert(dq.front() == 3);
    assert(dq.back()  == 5);

    dq.pop_back(dq.size());
    assert(dq.empty());
  }

  // assign with count and value.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};
    dq.assign(4, 99);
    assert(dq.size() == 4);
    assert(std::all_of(dq.begin(), dq.end(), [](int x){ return x == 99; }));

    dq::array<int, 10> dq2;
    dq2.assign(3, 7);
    assert(dq2.size() == 3);
    assert(dq2.front() == 7 && dq2.back() == 7);

    dq.assign(0, 42);
    assert(dq.empty());
  }

  // dq::copy to a raw buffer.
  {
    dq::array<int, 5> dq = {10, 20, 30, 40, 50};
    int buf[5]{};
    dq::copy(dq, buf);
    assert(dq == buf);

    int buf2[3]{};
    dq::copy(dq, buf2, 3);
    assert(buf2[0] == 10 && buf2[1] == 20 && buf2[2] == 30);
  }

  // append from a raw pointer region.
  {
    int src[] = {4, 5, 6};
    dq::array<int, 10> dq = {1, 2, 3};
    auto added = dq.append(src, 3);
    assert(added == 3);
    assert(dq.size() == 6);
    assert((dq == std::array{1, 2, 3, 4, 5, 6}));

    int big[20]{};
    dq::array<int, 4> small = {1, 2};
    auto capped = small.append(big, 20);
    assert(capped == 2);
    assert(small.size() == 4);
    assert(small.full());
  }

  // split() and csplit() on unwrapped and wrapped states.
  {
    dq::array<int, 6> dq = {1, 2, 3, 4};
    auto segs = dq.split();
    assert(segs[0][1] - segs[0][0] == 4);
    assert(segs[1][0] == segs[1][1]);

    dq::array<int, 4> circ;
    circ.push_back(1);
    circ.push_back(2);
    circ.push_back(3);
    circ.push_back(4);
    circ.pop_front();
    circ.push_back(5);
    auto s = circ.split();
    std::size_t total = (s[0][1] - s[0][0]) + (s[1][1] - s[1][0]);
    assert(total == 4);

    int rebuilt[4]{};
    int* p = rebuilt;
    for (auto [i, j] : circ.csplit())
      for (auto k = i; k != j; ++k) *p++ = *k;
    assert((rebuilt[0] == 2 && rebuilt[1] == 3 &&
            rebuilt[2] == 4 && rebuilt[3] == 5));
  }

  // dq::copy on a wrapped buffer.
  {
    dq::array<int, 4> circ;
    circ.push_back(10); circ.push_back(20);
    circ.push_back(30); circ.push_back(40);
    circ.pop_front(); circ.push_back(50);
    int out[4]{};
    dq::copy(circ, out);
    assert(out[0]==20 && out[1]==30 && out[2]==40 && out[3]==50);
  }

  // Iterator explicit bool conversion.
  {
    dq::array<int, 5> dq = {1, 2, 3};
    auto it  = dq.begin();
    auto end = dq.end();

    while (it != end) {
      assert(static_cast<bool>(it));
      ++it;
    }
    assert(!static_cast<bool>(end));

    const auto& cdq = dq;
    auto cit = cdq.begin();
    assert(static_cast<bool>(cit));
    std::advance(cit, static_cast<int>(cdq.size()));
    assert(!static_cast<bool>(cit));
  }

  // Variadic insert, push_back, and push_front.
  {
    dq::array<int, 20> dq = {1, 5};
    auto it = dq.insert(dq::multi, dq.cbegin() + 1, 2, 3, 4);
    assert(dq.size() == 5);
    assert((dq == std::array{1, 2, 3, 4, 5}));
    assert(*it == 2);

    dq::array<int, 10> dq2;
    dq2.push_back(10, 20, 30);
    assert(dq2.size() == 3);
    assert(dq2[0]==10 && dq2[1]==20 && dq2[2]==30);

    dq::array<int, 10> dq3;
    dq3.push_front(10, 20, 30);
    assert(dq3.size() == 3);
  }

  // Member sort() with custom comparators.
  {
    dq::array<int, 10> dq = {5, 3, 8, 1, 9, 2};
    dq.sort(dq.begin(), dq.end());
    assert(std::is_sorted(dq.begin(), dq.end()));

    dq.sort(dq.begin(), dq.end(), std::greater<int>{});
    assert(std::is_sorted(dq.begin(), dq.end(), std::greater<int>{}));

    dq::array<int, 10> dq2 = {5, 3, 8, 1, 9, 2};
    dq2.sort(dq2.begin() + 1, dq2.begin() + 5);
    assert(std::is_sorted(dq2.begin() + 1, dq2.begin() + 5));
    assert(dq2[0] == 5);
    assert(dq2[5] == 2);
  }

  // Member sort() on a custom struct.
  {
    struct Item { int key, val; };
    dq::array<Item, 10> dq;
    dq.push_back({3, 30}); dq.push_back({1, 10});
    dq.push_back({4, 40}); dq.push_back({2, 20});

    dq.sort(dq.begin(), dq.end(),
      [](Item const& a, Item const& b){ return a.key < b.key; });

    assert(dq[0].key == 1 && dq[1].key == 2 &&
           dq[2].key == 3 && dq[3].key == 4);
  }

  // Range construction from views.
  {
    auto doubled = std::views::iota(1, 6)
                 | std::views::transform([](int x){ return x * 2; });
    dq::array<int, 10> dq(doubled);
    assert(dq.size() == 5);
    assert((dq == std::array{2, 4, 6, 8, 10}));

    auto evens = std::views::iota(1, 11)
               | std::views::filter([](int x){ return x % 2 == 0; });
    dq::array<int, 10> dq2(dq::from_range, evens);
    assert(dq2.size() == 5);
    assert((dq2 == std::array{2, 4, 6, 8, 10}));

    dq::array<int, 5> dq3(dq::from_range, std::views::iota(0, 5));
    assert((dq3 == std::array{0, 1, 2, 3, 4}));
  }

  // Assignment from a range.
  {
    dq::array<int, 10> dq = {99, 99, 99};
    dq = std::views::iota(1, 6);
    assert(dq.size() == 5);
    assert((dq == std::array{1, 2, 3, 4, 5}));
  }

  // dq::find_if with various predicates.
  {
    dq::array<int, 10> dq = {1, 3, 5, 7, 8, 9};

    auto it = dq::find_if(dq, [](int x){ return x % 2 == 0; });
    assert(it && *it == 8);

    it = dq::find_if(dq, [](int x){ return x > 100; });
    assert(!it);
    assert(!dq::count_if(dq, [](int x){ return x > 100; }));

    const auto& cdq = dq;
    auto cit = dq::find_if(cdq, [](int x){ return x == 5; });
    assert(cit && *cit == 5);
  }

  // dq::find with multiple keys.
  {
    dq::array<int, 10> dq = {10, 20, 30, 40, 50};

    auto it = dq::find(dq, 30, 40);
    assert(it != dq.end() && (*it == 30 || *it == 40));

    it = dq::find(dq, 99, 100);
    assert(it == dq.end());
    assert(!static_cast<bool>(it));
  }

  // Circular overwrite and dq::copy consistency.
  {
    dq::array<int, 3> ring;
    ring.push_back(1); ring.push_back(2); ring.push_back(3);
    ring.push_back(4);
    ring.push_back(5);

    assert(ring.size() == 3);
    assert(ring.front() == 3 && ring.back() == 5);

    int out[3]{};
    dq::copy(ring, out);
    assert(out[0]==3 && out[1]==4 && out[2]==5);
  }

  // insert(pos, count, value).
  {
    dq::array<int, 20> dq = {1, 2, 5};
    auto it = dq.insert(dq.begin() + 2, 3, 99);
    assert(dq.size() == 6);
    assert(*it == 99);
    assert(dq[2] == 99 && dq[3] == 99 && dq[4] == 99);
    assert(dq[5] == 5);

    auto it2 = dq.insert(dq.begin() + 1, 0, 42);
    assert(dq.size() == 6);
    (void)it2;
  }

  // Iterator distance across a wrap boundary.
  {
    dq::array<int, 5> circ;
    circ.push_back(1); circ.push_back(2); circ.push_back(3);
    circ.push_back(4); circ.push_back(5);
    circ.pop_front(); circ.push_back(6);

    auto b = circ.begin();
    auto e = circ.end();
    assert(std::distance(b, e) == 5);

    auto mid = b + 2;
    assert(*mid == 4);
    assert(mid - b == 2);
    assert(e - mid == 3);
  }

  // Reverse iteration on a wrapped buffer.
  {
    dq::array<int, 4> circ = {1, 2, 3, 4};
    circ.pop_front(); circ.push_back(5);

    std::vector<int> fwd(circ.begin(), circ.end());
    std::vector<int> rev(circ.rbegin(), circ.rend());
    std::reverse(rev.begin(), rev.end());
    assert(fwd == rev);
  }

  // Self-range assign is a no-op.
  {
    dq::array<int, 5> dq = {1, 2, 3};
    dq.assign_range(dq);
    assert(dq.size() == 3 && dq[0] == 1 && dq[2] == 3);
  }

  // Static capacity is compile-time constant.
  {
    using A = dq::array<double, 16>;
    static_assert(A::capacity() == 16);
    A dq;
    assert(dq.capacity() == 16);
    assert(!dq.full());
  }

  // stable_sort() preserves relative order.
  {
    struct Item { int key, idx; };
    dq::array<Item, 10> dq;
    dq.push_back({2, 0}); dq.push_back({1, 1});
    dq.push_back({2, 2}); dq.push_back({1, 3});
    dq.push_back({3, 4});

    dq.stable_sort(dq.begin(), dq.end(),
      [](Item const& a, Item const& b){ return a.key < b.key; });

    assert(std::is_sorted(dq.begin(), dq.end(),
      [](Item const& a, Item const& b){ return a.key < b.key; }));
    assert(dq[0].idx == 1 && dq[1].idx == 3);
    assert(dq[2].idx == 0 && dq[3].idx == 2);
  }

  // stable_sort() on a wrapped buffer.
  {
    dq::array<int, 6> circ;
    for (int v : {9, 3, 7, 1, 5, 4}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(2); circ.push_back(8);
    circ.stable_sort(circ.begin(), circ.end());
    assert(std::is_sorted(circ.begin(), circ.end()));
    assert((circ == std::array{1, 2, 4, 5, 7, 8}));
  }

  // stable_sort() with descending comparator.
  {
    dq::array<int, 8> dq = {3, 1, 4, 1, 5, 9, 2, 6};
    dq.stable_sort(dq.begin(), dq.end(), std::greater<int>{});
    assert(std::is_sorted(dq.begin(), dq.end(), std::greater<int>{}));
  }

  // Zero-count bulk pop is a no-op.
  {
    dq::array<int, 6> dq = {1, 2, 3};
    dq.pop_front(0);
    dq.pop_back(0);
    assert(dq.size() == 3 && dq[0] == 1 && dq[2] == 3);
  }

  // NEW allocator move constructor transfers ownership.
  {
    dq::array<int, 8, dq::NEW> a = {10, 20, 30};
    auto* raw = a.data();

    dq::array<int, 8, dq::NEW> b(std::move(a));
    assert(b.data() == raw);
    assert(b.size() == 3 && b[1] == 20);
    assert(a.empty());
    assert(a.data() != raw);
  }

  // NEW allocator move assignment swaps ownership.
  {
    dq::array<int, 8, dq::NEW> a = {1, 2, 3};
    dq::array<int, 8, dq::NEW> b = {4, 5, 6, 7};
    auto* raw_a = a.data();
    auto* raw_b = b.data();

    b = std::move(a);
    assert(b.data() == raw_a);
    assert(a.data() == raw_b);
    assert(a.empty());
    assert(b.size() == 3 && b[0] == 1);
  }

  // erase_if returns the number of removed elements.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto n = dq::erase_if(dq, [](int x){ return x % 3 == 0; });
    assert(n == 3);
    assert(dq.size() == 7);
    for (int x : dq) assert(x % 3 != 0);
  }

  // erase_if on a wrapped buffer.
  {
    dq::array<int, 6> circ;
    for (int v : {1, 2, 3, 4, 5, 6}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(7); circ.push_back(8);
    auto n = dq::erase_if(circ, [](int x){ return x % 2 == 0; });
    assert(n == 3);
    assert(circ.size() == 3);
    for (int x : circ) assert(x % 2 != 0);
  }

  // insert_range returns iterator to first inserted element.
  {
    std::vector<int> src = {10, 20, 30};
    dq::array<int, 10> dq = {1, 2, 3};
    auto it = dq.insert_range(dq.begin() + 1, src);
    assert(*it == 10);
    assert((dq == std::array{1, 10, 20, 30, 2, 3}));
  }

  // first() pointer arithmetic matches operator[].
  {
    dq::array<int, 8> dq = {5, 10, 15, 20};
    for (std::size_t i = 0; i < dq.size(); ++i)
      assert(dq.first()[i] == dq[i]);
  }

  // Constexpr size and capacity queries.
  {
    using A = dq::array<int, 5>;
    static_assert(A::capacity() == 5);
    static_assert(A::max_size() == PTRDIFF_MAX);

    dq::array<int, 5> d = {10, 20, 30};
    d[1] = 99;
    assert(d[0] == 10 && d[1] == 99 && d[2] == 30);
  }

  // sort() on a wrapped buffer.
  {
    dq::array<int, 6> circ;
    for (int v : {5, 3, 1, 4, 2, 6}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(9); circ.push_back(7);
    circ.sort(circ.begin(), circ.end());
    assert(std::is_sorted(circ.begin(), circ.end()));
  }

  // std::partition via random-access iterator.
  {
    dq::array<int, 10> dq = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    auto mid = std::partition(dq.begin(), dq.end(), [](int x){ return x < 5; });
    assert(std::all_of(dq.begin(), mid, [](int x){ return x < 5; }));
    assert(std::all_of(mid, dq.end(), [](int x){ return x >= 5; }));
  }

  // std::nth_element via random-access iterator.
  {
    dq::array<int, 8> dq = {7, 2, 5, 4, 1, 8, 3, 6};
    auto mid = dq.begin() + 3;
    std::nth_element(dq.begin(), mid, dq.end());
    assert(*mid == 4);
    assert(std::all_of(dq.begin(), mid, [](int x){ return x <= 4; }));
    assert(std::all_of(mid + 1, dq.end(), [](int x){ return x >= 4; }));
  }

  // assign(count, value) overload.
  {
    dq::array<int, 10> dq = {1, 2, 3};
    dq.assign(5, 7);
    assert(dq.size() == 5);
    assert(std::all_of(dq.begin(), dq.end(), [](int x){ return x == 7; }));
    dq.assign(0, 99);
    assert(dq.empty());
  }

  // Emplace with multiple constructor arguments.
  {
    struct MultiArg {
      int a, b, c;
      MultiArg() = default;
      MultiArg(int x, int y, int z) : a(x), b(y), c(z) {}
      bool operator==(const MultiArg& other) const {
        return a == other.a && b == other.b && c == other.c;
      }
    };
    dq::array<MultiArg, 10> dq;
    dq.emplace_back(1, 2, 3);
    dq.emplace_front(4, 5, 6);
    dq.emplace(dq.begin() + 1, 7, 8, 9);
    assert(dq.size() == 3);
    assert(dq.front() == MultiArg(4, 5, 6));
    assert(dq[1] == MultiArg(7, 8, 9));
    assert(dq.back() == MultiArg(1, 2, 3));
  }

  // Sort edge cases — empty, single element, all equal.
  {
    dq::array<int, 5> empty;
    empty.sort(empty.begin(), empty.end());
    assert(empty.empty());

    dq::array<int, 5> single = {42};
    single.sort(single.begin(), single.end());
    assert(single.size() == 1 && single[0] == 42);

    dq::array<int, 5> equal = {5, 5, 5, 5};
    equal.sort(equal.begin(), equal.end());
    assert(std::all_of(equal.begin(), equal.end(), [](int x){ return x == 5; }));
  }

  // Stable sort preserves order for all-equal keys.
  {
    struct Track { int val, seq; };
    dq::array<Track, 10> dq;
    for (int i = 0; i < 5; ++i) dq.push_back({1, i});
    dq.stable_sort(dq.begin(), dq.end(),
      [](Track const& a, Track const& b){ return a.val < b.val; });
    for (int i = 0; i < 5; ++i) assert(dq[i].seq == i);
  }

  // split() and csplit() on an empty container.
  {
    dq::array<int, 5> empty;
    auto segs = empty.split();
    assert(segs[0][0] == segs[0][1]);
    assert(segs[1][0] == segs[1][1]);

    int sum = 0;
    for (auto [i, j] : empty.csplit()) {
      for (auto k = i; k != j; ++k) sum += *k;
    }
    assert(sum == 0);
  }

  // assign from an empty range.
  {
    dq::array<int, 10> dq = {1, 2, 3};
    std::vector<int> empty;
    dq.assign(empty.begin(), empty.end());
    assert(dq.empty());

    dq.assign({});
    assert(dq.empty());
  }

  // Insert at capacity boundary behavior.
  {
    dq::array<int, 4> dq = {1, 2, 3};
    dq.insert(dq.begin(), 0); // should succeed, size becomes 4
    assert(dq.size() == 4);
    assert(dq.full());
    assert(dq[0] == 0);
    assert(dq[3] == 3);
  }

  // Erase of the last element returns end().
  {
    dq::array<int, 5> dq = {1, 2, 3};
    auto it = dq.erase(dq.end() - 1);
    assert(it == dq.end());
    assert(dq.size() == 2);
    assert(dq.back() == 2);
  }

  // find and find_if on empty containers.
  {
    dq::array<int, 5> empty;
    assert(dq::find(empty, 1) == empty.end());
    assert(!dq::find(empty, 1));
    assert(!dq::count(empty, 1));
    assert(dq::find_if(empty, [](int){ return true; }) == empty.end());
  }

  // erase_if returning zero on empty or non-matching ranges.
  {
    dq::array<int, 5> empty;
    assert(dq::erase_if(empty, [](int){ return true; }) == 0);

    dq::array<int, 5> dq = {1, 3, 5};
    assert(dq::erase_if(dq, [](int x){ return x % 2 == 0; }) == 0);
    assert(dq.size() == 3);
  }

  // Iterator operator-> for struct types.
  {
    struct Node { int value; };
    dq::array<Node, 5> dq;
    dq.push_back({10});
    dq.push_back({20});
    auto it = dq.begin();
    assert(it->value == 10);
    ++it;
    assert(it->value == 20);
  }

  // first() / last() consistency after cyclic operations.
  {
    dq::array<int, 4> dq;
    dq.push_back(1); dq.push_back(2);
    dq.push_back(3); dq.push_back(4);
    dq.pop_front();
    dq.push_back(5);
    // Logical contents: [2,3,4,5]
    assert((dq == std::initializer_list{2, 3, 4, 5}));
    assert(dq.first() - dq.last() == 1);
    assert(dq.full());
  }

  // pop_front(0) and pop_back(0) on an empty container.
  {
    dq::array<int, 5> empty;
    empty.pop_front(0);
    empty.pop_back(0);
    assert(empty.empty());
  }

  // insert_range at begin() and end().
  {
    std::vector<int> src = {0, 9};
    dq::array<int, 10> dq = {1, 2, 3};
    auto it = dq.insert_range(dq.begin(), src);
    assert(*it == 0);
    assert(dq[0] == 0 && dq[1] == 9 && dq[2] == 1);

    it = dq.insert_range(dq.end(), src);
    assert(*it == 0);
    assert(dq.back() == 9);
  }

  // append_range and prepend_range with empty ranges.
  {
    dq::array<int, 10> dq = {1, 2, 3};
    std::vector<int> empty;
    dq.append_range(empty);
    assert(dq.size() == 3);
    dq.prepend_range(empty);
    assert(dq.size() == 3);
  }

  // Comparison operators with empty containers.
  {
    dq::array<int, 5> empty1, empty2;
    assert(empty1 == empty2);
    assert(!(empty1 != empty2));
    assert(!(empty1 < empty2));
    assert(empty1 <= empty2);
    assert(!(empty1 > empty2));
    assert(empty1 >= empty2);

    dq::array<int, 5> non_empty = {1};
    assert(empty1 != non_empty);
    assert(empty1 < non_empty);
    assert(non_empty > empty1);
  }

  // push_back / push_front cyclic overwrite with string tracking.
  {
    dq::array<std::string, 3> dq;
    dq.push_back("A");
    dq.push_back("B");
    dq.push_back("C");
    dq.push_back("D"); // overwrites A
    assert(dq.size() == 3);
    assert((dq == std::array{"B", "C", "D"}));
    dq.push_front("E"); // overwrites B
    assert(dq.size() == 3);
    assert((dq == std::array{"E", "C", "D"}));
  }

  // std::distance with reverse iterators across wrap boundary.
  {
    dq::array<int, 5> circ;
    for (int v : {1, 2, 3, 4, 5}) circ.push_back(v);
    circ.pop_front(); circ.push_back(6); // wrap
    assert(std::distance(circ.rbegin(), circ.rend()) == 5);
    assert(std::distance(circ.crbegin(), circ.crend()) == 5);
  }

  // resize(0) and regrowth.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};
    dq.resize(0);
    assert(dq.empty());
    dq.push_back(10);
    dq.push_front(20);
    assert(dq.size() == 2);
    assert(dq.front() == 20);
    assert(dq.back() == 10);
  }

  // data() pointer is non-null for NEW allocator after move.
  {
    dq::array<int, 5, dq::NEW> a;
    assert(a.data() != nullptr); // NEW should always have an allocation
    dq::array<int, 5, dq::NEW> b(std::move(a));
    assert(b.data() != nullptr);
    assert(a.data() != nullptr); // moved-from still has an allocation
  }

  // dq::copy with empty source.
  {
    dq::array<int, 5> empty;
    int buf[1] = {99};
    dq::copy(empty, buf, 0);
    assert(buf[0] == 99); // should not write anything
  }

  // Full container insert behavior.
  {
    dq::array<int, 3> dq = {1, 2, 3};
    assert(dq.full());
    dq.insert(dq.begin() + 1, 99);
    // After insert into full container, front (oldest) element is dropped.
    assert(dq.size() == 3);
    assert((dq == std::array{99, 2, 3}));
  }

  // Post-increment returns a copy of the pre-advance iterator.
  {
    dq::array<int, 5> dq = {10, 20, 30};
    auto it = dq.begin();

    auto old = it++;              // post-increment
    assert(*old == 10);           // copy points at first element
    assert(*it  == 20);           // it has advanced

    auto old2 = it--;             // post-decrement
    assert(*old2 == 20);          // copy points at second element
    assert(*it   == 10);          // it has retreated
  }

  // Iterator operator[] random access.
  {
    dq::array<int, 8> dq = {0, 1, 2, 3, 4, 5, 6, 7};
    auto it = dq.begin();

    for (int i = 0; i < 8; ++i)
      assert(it[i] == i);

    // Wrap the buffer so storage is non-contiguous.
    dq::array<int, 4> circ;
    circ.push_back(1); circ.push_back(2);
    circ.push_back(3); circ.push_back(4);
    circ.pop_front();
    circ.push_back(5);               // logical: [2,3,4,5], wraps

    auto cit = circ.begin();
    assert(cit[0] == 2);
    assert(cit[1] == 3);
    assert(cit[2] == 4);
    assert(cit[3] == 5);
  }

  // Iterator three-way comparison operator<=>.
  {
    dq::array<int, 6> dq = {1, 2, 3, 4, 5};
    auto a = dq.begin();
    auto b = dq.begin() + 2;
    auto e = dq.end();

    assert((a <=> b) < 0);
    assert((b <=> a) > 0);
    assert((a <=> a) == 0);
    assert((b <=> e) < 0);
    assert((e <=> e) == 0);

    // Ordering must survive a wrap boundary.
    dq::array<int, 4> circ;
    for (int v : {10, 20, 30, 40}) circ.push_back(v);
    circ.pop_front();
    circ.push_back(50);              // logical: [20,30,40,50]

    auto ci = circ.begin();
    auto cj = circ.begin() + 3;
    assert((ci <=> cj) < 0);
    assert((cj <=> ci) > 0);
    assert((ci <=> ci) == 0);
  }

  // Non-member n + iterator operator.
  {
    dq::array<int, 6> dq = {0, 1, 2, 3, 4};
    auto it = dq.begin();

    assert(*(2 + it) == 2);
    assert(*(0 + it) == 0);
    assert(*(4 + it) == 4);
    assert((2 + it) == (it + 2));
  }

  // Iterator operator+= and operator-=.
  {
    dq::array<int, 8> dq = {10, 20, 30, 40, 50};
    auto it = dq.begin();

    it += 2;
    assert(*it == 30);

    it += 2;
    assert(*it == 50);

    it -= 4;
    assert(*it == 10);

    it -= 0;
    assert(*it == 10);

    it += static_cast<int>(dq.size()) - 1;
    assert(*it == 50);
  }

  // Iterator difference operator-(iterator).
  {
    dq::array<int, 6> dq = {1, 2, 3, 4, 5};
    auto b = dq.begin();
    auto e = dq.end();

    assert(e - b == 5);
    assert(b - e == -5);
    assert(b - b ==  0);

    auto m = b + 2;
    assert(m - b ==  2);
    assert(e - m ==  3);

    // Across a wrap boundary.
    dq::array<int, 4> circ;
    for (int v : {1, 2, 3, 4}) circ.push_back(v);
    circ.pop_front();
    circ.push_back(5);               // logical: [2,3,4,5]

    auto cb = circ.begin();
    auto ce = circ.end();
    assert(ce - cb == 4);
    assert(cb - ce == -4);
    assert((cb + 1) - cb == 1);
  }

  // Implicit iterator to const_iterator conversion.
  {
    dq::array<int, 5> dq = {1, 2, 3};
    using iter_t       = decltype(dq)::iterator;
    using const_iter_t = decltype(dq)::const_iterator;

    iter_t it = dq.begin() + 1;
    const_iter_t cit = it;           // implicit conversion must compile
    assert(*cit == *it);
    assert(cit == it);               // cross-type equality

    // Verify that const_iterator advances independently.
    ++cit;
    assert(*cit == 3);
    assert(*it  == 2);               // original iterator unchanged
  }

  // Const container yields const_iterator from begin()/end().
  {
    const dq::array<int, 5> cdq = {5, 10, 15};
    static_assert(std::same_as<decltype(cdq.begin()),
                               dq::array<int,5>::const_iterator>);
    static_assert(std::same_as<decltype(cdq.end()),
                               dq::array<int,5>::const_iterator>);

    int sum = 0;
    for (auto it = cdq.begin(); it != cdq.end(); ++it)
      sum += *it;
    assert(sum == 30);
  }

  // Iterator operator* and operator-> on const_iterator.
  {
    struct Pair { int first, second; };
    dq::array<Pair, 4> dq;
    dq.push_back({1, 10});
    dq.push_back({2, 20});
    dq.push_back({3, 30});

    const auto& cdq = dq;
    auto it = cdq.begin();

    assert((*it).first  == 1);
    assert(it++->second == 10);
    assert((*it).first  == 2);
    assert(it->second   == 20);
  }

  // Iterator bool conversion on a wrapped buffer.
  {
    dq::array<int, 3> circ;
    circ.push_back(1); circ.push_back(2); circ.push_back(3);
    circ.pop_front();
    circ.push_back(4);               // logical: [2,3,4], wraps

    int count = 0;
    for (auto it = circ.begin(); it; ++it)
      ++count;
    assert(count == 3);

    // end() iterator must evaluate to false.
    assert(!circ.end());
  }

  // Basic Construction and Access
  {
    dq::array<int, 20> dq{1, 2, 3};
    assert(dq.size() == 3);
    assert(dq[0] == 1 && dq[2] == 3);

    dq[1] = 42;
    dq.pop_back();
    assert(dq.size() == 2);

    dq.clear();
    assert(dq.empty());

    dq.push_back(4);
    dq.insert(dq.begin() + 1, 5);
    assert(dq.size() == 2 && dq[1] == 5);
  }

  // Circular Logic & Wrap-around ---
  {
    dq::array<int, 5> dq = {1, 2, 3, 4, 5};
    assert(dq.full());

    // Trigger wrap-around by popping front and pushing back
    dq.pop_front(); // {2, 3, 4, 5}
    dq.push_back(6);  // {2, 3, 4, 5, 6}
    assert(dq.front() == 2 && dq.back() == 6);

    // Verify split() - since it wrapped, it should have two segments
    auto segments = dq.split();
    assert(segments[0][0] != nullptr);
    // Note: Depending on internal pointer state, it might or might not be split
    // But we can verify the sum of elements through split segments
    int sum = 0;
    for (auto const seg : segments) {
      if (seg[0] == seg[1]) break;
      sum = std::accumulate(seg[0], seg[1], sum);
    }
    assert(sum == 20); // 2+3+4+5+6
  }

  // Sorting Across Wrap-around ---
  {
    dq::array<int, 6> dq;
    dq.push_back(10); // Start somewhere in the middle
    dq.pop_front();

    // Fill so it wraps: [empty, empty, 5, 4, 3, 2] -> [1, empty, 5, 4, 3, 2]
    dq.push_back(5, 4, 3, 2, 1);
    assert(!dq.full());

    // Member sort handles the wrap-around internally
    dq.sort(dq.begin(), dq.end());
    assert(std::is_sorted(dq.begin(), dq.end()));
    assert(dq.front() == 1 && dq.back() == 5);
  }

  // Append from Memory Region
  {
    dq::array<int, 10> dq = {1, 2, 3};
    int raw_data[] = {4, 5, 6};
    dq.append(raw_data, 3);
    assert(dq.size() == 6);
    assert(dq.back() == 6);
  }

  // Multi-argument Helpers (dq::find / dq::count)
  {
    dq::array<int, 10> dq = {1, 2, 3, 1, 2, 3, 4};
    // Count occurrences of 1 OR 3
    assert(dq::count(dq, 1, 3) == 4);

    // Find first occurrence of 4 or 5
    auto it = dq::find(dq, 4, 5);
    assert(it && *it == 4);
  }

  // Non-Trivial Types & Move Semantics
  {
    dq::array<std::unique_ptr<int>, 5> dq;
    dq.push_back(std::make_unique<int>(10));
    dq.push_back(std::make_unique<int>(20));

    auto p = std::move(dq.front());
    assert(*p == 10);
    dq.pop_front();
    assert(dq.size() == 1);
    assert(*dq.front() == 20);
  }

  // Comparison Operators
  {
    dq::array<int, 5> dq1 = {1, 2, 3};
    dq::array<int, 10> dq2 = {1, 2, 3}; // Different capacity

    // Comparison works via ranges/iterators
    assert(dq1 == dq2);
    dq2.push_back(4);
    assert(dq1 < dq2);
  }

  // Range Insertion & Erase_if
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};
    std::vector<int> v = {10, 11};
    dq.insert(dq.begin() + 2, v.begin(), v.end());
    // Result: {1, 2, 10, 11, 3, 4, 5}
    assert(dq[2] == 10 && dq[4] == 3);

    dq::erase_if(dq, [](int x) { return x % 2 == 0; });
    // Result: {1, 11, 3, 5}
    assert(dq.size() == 4);
    for(int n : dq) assert(n % 2 != 0);
  }

  // Reverse Iterators
  {
    dq::array<int, 5> dq = {1, 2, 3};
    auto rit = dq.rbegin();
    assert(*rit == 3);
    ++rit;
    assert(*rit == 2);
    assert(std::distance(dq.rbegin(), dq.rend()) == 3);
  }

  // std::bool support and heterogeneous lookup
  {
    dq::array<bool, 8> bools{true, false, true, false, true};
    assert(bools.size() == 5);
    assert(dq::count(bools, true) == 3);
    assert(dq::count(bools, false) == 2);
    auto it = dq::find(bools, true);
    assert(it && *it == true);
  }

  // append() capacity clamping behavior
  {
    dq::array<int, 5> arr{1, 2, 3}; // size=3, space=2
    int overflow_data[] = {4, 5, 6, 7, 8};
    auto added = arr.append(overflow_data, 5);
    assert(added == 2); // Only 2 slots available
    assert(arr.size() == 5);
    assert(arr.full());
    assert((arr == std::array{1, 2, 3, 4, 5}));
  }

  // insert with dq::multi on a full container (circular overwrite per element)
  {
    dq::array<int, 4> full{1, 2, 3, 4};
    assert(full.full());
    // Inserting 2 elements into full drops 2 from front
    full.insert(dq::multi, full.begin() + 1, 99, 88);
    assert(full.size() == 4);
    assert((full == std::array{88, 2, 3, 4}));
  }

  // resize() on a wrapped buffer (shrink & grow)
  {
    dq::array<int, 6> wrap;
    for (int i = 0; i < 6; ++i) wrap.push_back(i);
    wrap.pop_front(); wrap.push_back(6); // Wrapped state: [1,2,3,4,5,6]

    wrap.resize(3); // Shrink
    assert(wrap.size() == 3);
    assert((wrap == std::array{1, 2, 3}));

    wrap.resize(5, 99); // Grow with fill
    assert(wrap.size() == 5);
    assert(wrap[0] == 1);
    assert(wrap[3] == 99);
    assert(wrap[4] == 99);
  }

  // dq::erase_if on a fully wrapped buffer
  {
    dq::array<int, 8> wrap;
    for (int i = 0; i < 8; ++i) wrap.push_back(i);
    wrap.pop_front(); wrap.push_back(8); // [1,2,3,4,5,6,7,8] wrapped
    auto removed = dq::erase_if(wrap, [](int x) { return x % 3 == 0; });
    assert(removed == 2); // 3 and 6 removed
    assert(wrap.size() == 6);
    assert((wrap == std::array{1, 2, 4, 5, 7, 8}));
  }

  // split() / csplit() exhaustive segment validation
  {
    dq::array<int, 5> empty;
    auto s1 = empty.split();
    assert(s1[0][0] == s1[0][1] && s1[1][0] == s1[1][1]);

    dq::array<int, 4> contiguous{1, 2, 3};
    auto s2 = contiguous.split();
    assert(s2[0][1] - s2[0][0] == 3);
    assert(s2[1][0] == s2[1][1]);

    dq::array<int, 4> wrapped;
    wrapped.push_back(1); wrapped.push_back(2); wrapped.push_back(3); wrapped.push_back(4);
    wrapped.pop_front(); wrapped.push_back(5); // Wrapped: [2,3,4,5]
    auto s3 = wrapped.split();
    assert((s3[0][1] - s3[0][0]) + (s3[1][1] - s3[1][0]) == 4);
    // Verify reconstruction from split segments
    int rebuilt[4]{};
    int* p = rebuilt;
    for (auto [beg, end] : wrapped.csplit())
        while (beg != end) *p++ = *beg++;
    assert(rebuilt[0] == 2 && rebuilt[1] == 3 && rebuilt[2] == 4 && rebuilt[3] == 5);
  }

  // dq::copy to raw buffer with size limit on wrapped state
  {
    dq::array<int, 6> src;
    for (int i = 0; i < 6; ++i) src.push_back(i * 10); // [0, 10, 20, 30, 40, 50]
    src.pop_front(); src.push_back(70); // [10, 20, 30, 40, 50, 70]
    int dest[6]{};
    dq::copy(src, dest, 6);
    assert((src == std::array{10, 20, 30, 40, 50, 70}));
    assert(dest == src);
  }

  // stable_sort correctness on heavily wrapped buffer
  {
    dq::array<int, 7> wrap;
    for (int i = 0; i < 7; ++i) wrap.push_back(i);
    wrap.pop_front(); wrap.pop_front(); wrap.push_back(7); wrap.push_back(8);
    // State: [2,3,4,5,6,7,8] wrapped
    wrap.stable_sort(wrap.begin(), wrap.end());
    assert(std::is_sorted(wrap.begin(), wrap.end()));
    assert(wrap.front() == 2 && wrap.back() == 8);
  }

  // Reverse iterator arithmetic and base conversion on wrapped state
  {
    dq::array<int, 5> wrap{1, 2, 3, 4, 5};
    wrap.pop_front(); wrap.push_back(6); // [2,3,4,5,6] wrapped
    auto r = wrap.rbegin();
    assert(*r == 6);
    r += 2;
    assert(*r == 4);
    r -= 1;
    assert(*r == 5);
    // Verify base() points to correct forward iterator
    assert(std::distance(wrap.begin(), r.base() - 1) == 3);
  }

  // dq::count_if with complex predicate on wrapped buffer
  {
    dq::array<int, 6> wrap{10, 20, 30, 40, 50, 60};
    wrap.pop_front(); wrap.push_back(70); // [20..70] wrapped
    auto cnt = dq::count_if(wrap, [](int x) { return x >= 40; });
    assert(cnt == 4); // 40, 50, 60, 70
  }

  // assign() with initializer list clearing existing wrapped state
  {
    dq::array<int, 10> wrap{1, 2, 3, 4, 5};
    wrap.pop_front(); wrap.push_back(6); // wrapped
    wrap.assign({100, 200});
    assert(wrap.size() == 2);
    assert(wrap[0] == 100 && wrap[1] == 200);
    assert(!wrap.full());
    wrap.append_range({300, 400});
    assert((wrap == std::array{100, 200, 300, 400}));
  }

  // Operator <=> lexicographical comparison with different lengths
  {
    dq::array<int, 10> a{1, 2, 3};
    dq::array<int, 10> b{1, 2, 3, 4};
    assert((a <=> b) < 0);
    assert((b <=> a) > 0);
    assert((a <=> a) == 0);
  }

  // dq::from_range with filtered view
  {
    auto odds = std::views::iota(1, 10) | std::views::filter([](int x) { return x % 2 != 0; });
    dq::array<int, 5> arr(dq::from_range, odds);
    assert(arr.size() == 5);
    assert((arr == std::array{1, 3, 5, 7, 9}));
  }

  // Swap between empty and full containers
  {
    dq::array<int, 4> full{1, 2, 3, 4};
    dq::array<int, 4> empty;
    full.swap(empty);
    assert(full.empty() && empty.full());
    assert((empty == std::array{1, 2, 3, 4}));
  }

  // dq::find with heterogeneous string_view lookup
  {
    dq::array<std::string, 5> strs{"apple", "banana", "cherry", "date", "elderberry"};
    auto it = dq::find(strs, std::string_view("cherry"));
    assert(it && *it == "cherry");
    it = dq::find(strs, std::string_view("grape"));
    assert(!it);
  }

  // insert_range returning correct iterator to first inserted element
  {
    std::vector<int> src{10, 20, 30};
    dq::array<int, 10> dq{1, 2, 3};
    auto it = dq.insert_range(dq.begin() + 1, src);
    assert(*it == 10);
    assert(dq[1] == 10 && dq[2] == 20 && dq[3] == 30);
    assert(dq[4] == 2);
  }

  // Bulk pop with count exactly matching size
  {
    dq::array<int, 5> dq{1, 2, 3, 4, 5};
    dq.pop_front(2);
    dq.pop_back(3);
    assert(dq.empty());
    assert(dq.first() == dq.last());
  }

  // Emplace with multiple constructor arguments on wrapped buffer
  {
    struct Triplet { int a, b, c; Triplet() = default; Triplet(int x, int y, int z) : a(x), b(y), c(z) {} bool operator==(const Triplet&) const = default; };
    dq::array<Triplet, 3> dq;
    dq.emplace_back(1, 2, 3);
    dq.emplace_front(4, 5, 6);
    dq.emplace(dq.begin() + 1, 7, 8, 9); // Triggers wrap/overwrite if needed, but size=3 so it fits
    assert(dq.size() == 3);
    assert(dq[0] == Triplet(4, 5, 6));
    assert(dq[1] == Triplet(7, 8, 9));
    assert(dq[2] == Triplet(1, 2, 3));
  }

  // Empty range — must not crash or modify anything.
  {
    std::vector<int> v;
    nicesort::sort(v.begin(), v.end());
    assert(v.empty());
  }

  // Single element — already sorted by definition.
  {
    std::vector<int> v{42};
    nicesort::sort(v.begin(), v.end());
    assert(v[0] == 42);
  }

  // Two elements, already ordered.
  {
    std::vector<int> v{1, 2};
    nicesort::sort(v.begin(), v.end());
    assert((v == std::vector{1, 2}));
  }

  // Two elements, reversed.
  {
    std::vector<int> v{2, 1};
    nicesort::sort(v.begin(), v.end());
    assert((v == std::vector{1, 2}));
  }

  // Already sorted range — must remain sorted.
  {
    std::vector<int> v(64);
    std::iota(v.begin(), v.end(), 0);
    nicesort::sort(v.begin(), v.end());
    assert(std::is_sorted(v.begin(), v.end()));
    assert(v[0] == 0 && v[63] == 63);
  }

  // Reverse-sorted range.
  {
    std::vector<int> v(64);
    std::iota(v.begin(), v.end(), 0);
    std::reverse(v.begin(), v.end());
    nicesort::sort(v.begin(), v.end());
    assert(std::is_sorted(v.begin(), v.end()));
  }

  // All elements equal.
  {
    std::vector v(50, 7);
    nicesort::sort(v.begin(), v.end());
    assert(v.size() == 50);
    for (int x : v) assert(x == 7);
  }

  // Duplicate-heavy — many equal groups.
  {
    std::vector<int> v;
    for (int k = 0; k < 10; ++k)
      for (int j = 0; j < 10; ++j) v.push_back(k);
    std::shuffle(v.begin(), v.end(), std::mt19937{42});
    nicesort::sort(v.begin(), v.end());
    assert(std::is_sorted(v.begin(), v.end()));
    assert(v.front() == 0 && v.back() == 9);
  }

  // Large random permutation.
  {
    std::vector<int> v(1024);
    std::iota(v.begin(), v.end(), -512);
    std::shuffle(v.begin(), v.end(), std::mt19937{123});
    nicesort::sort(v.begin(), v.end());
    assert(std::is_sorted(v.begin(), v.end()));
    assert(v[0] == -512 && v[1023] == 511);
  }

  // Custom descending comparator.
  {
    std::vector<int> v{3, 1, 4, 1, 5, 9, 2, 6, 5};
    nicesort::sort(v.begin(), v.end(), std::greater<int>{});
    assert(std::is_sorted(v.begin(), v.end(), std::greater<int>{}));
  }

  // nicesort preserves relative order of equal keys (stability guarantee).
  {
    struct Keyed { int key, seq; };
    std::vector<Keyed> v;
    for (int i = 0; i < 20; ++i) v.push_back({i % 4, i});
    nicesort::sort(v.begin(), v.end(), [](Keyed const& a, Keyed const& b){ return a.key < b.key; });
    assert(std::is_sorted(v.begin(), v.end(), [](Keyed const& a, Keyed const& b){ return a.key < b.key; }));
    // Within each key group the original insertion order (seq) must increase.
    for (std::size_t i = 1; i < v.size(); ++i)
      if (v[i].key == v[i-1].key) assert(v[i].seq > v[i-1].seq);
  }

  // nicesort on a plain array (pointer iterators).
  {
    int arr[] = {5, 3, 8, 1, 9, 2, 7, 4, 6};
    nicesort::sort(arr, arr + 9);
    assert(std::is_sorted(arr, arr + 9));
  }

  // random_access_iterator requirement.
  {
    using A = dq::array<int, 8>;
    static_assert(std::random_access_iterator<A::iterator>);
    // const_iterator has the correct iterator_category tag.
    static_assert(std::same_as<
      A::const_iterator::iterator_category,
      std::random_access_iterator_tag>);
  }

  // Negative offset arithmetic: it + (-n) == it - n.
  {
    dq::array<int, 8> dq = {10, 20, 30, 40, 50};
    auto e = dq.end();
    assert(*(e + (-1)) == 50);
    assert(*(e + (-3)) == 30);
    assert(*(e - 1) == *(e + (-1)));
  }

  // Reverse-iterator subscript operator[].
  {
    dq::array<int, 6> dq = {0, 1, 2, 3, 4};
    auto r = dq.rbegin();
    assert(r[0] == 4);
    assert(r[1] == 3);
    assert(r[4] == 0);

    // Same test on a wrapped buffer.
    dq::array<int, 4> circ;
    for (int v : {1, 2, 3, 4}) circ.push_back(v);
    circ.pop_front(); circ.push_back(5); // [2,3,4,5]
    auto cr = circ.rbegin();
    assert(cr[0] == 5);
    assert(cr[1] == 4);
    assert(cr[3] == 2);
  }

  // Cross-type iterator comparison (iterator vs const_iterator).
  {
    dq::array<int, 6> dq = {1, 2, 3, 4, 5};
    auto  it  = dq.begin() + 2;
    auto  cit = dq.cbegin() + 2;

    assert(it == cit);          // iterator == const_iterator
    assert(cit == it);          // symmetric
    assert(!(it != cit));

    auto it2 = dq.begin() + 3;
    assert(it2 != cit);
    assert((it2 <=> cit) > 0);
    assert((cit <=> it2) < 0);
  }

  // Iterator difference between iterators at different offsets.
  {
    dq::array<int, 8> dq = {0, 1, 2, 3, 4, 5, 6, 7};
    auto   it  = dq.begin();
    auto   it2 = dq.begin() + 5;
    assert(it2 - it == 5);
    assert(it  - it2 == -5);
  }

  // emplace_back / emplace_front return reference to emplaced element.
  {
    dq::array<int, 5> dq;
    int& r1 = dq.emplace_back(42);
    assert(r1 == 42 && &r1 == &dq.back());

    int& r2 = dq.emplace_front(99);
    assert(r2 == 99 && &r2 == &dq.front());
  }

  // prepend_range on a fresh container.
  {
    dq::array<int, 10> dq = {4, 5, 6};
    dq.prepend_range({1, 2, 3});
    assert((dq == std::array{1, 2, 3, 4, 5, 6}));
    assert(dq.size() == 6);
  }

  // prepend_range on a wrapped buffer.
  {
    dq::array<int, 8> circ;
    for (int v : {10, 20, 30, 40, 50, 60}) circ.push_back(v);
    circ.pop_front(); circ.pop_front(); // remove 10, 20 → logical [30,40,50,60]
    circ.prepend_range({1, 2});
    assert((circ == std::array{1, 2, 30, 40, 50, 60}));
  }

  // prepend_range with empty range — must be a no-op.
  {
    dq::array<int, 6> dq = {7, 8, 9};
    dq.prepend_range({});
    assert((dq == std::array{7, 8, 9}));
    assert(dq.size() == 3);
  }

  // prepend_range using an initializer_list overload.
  {
    dq::array<int, 8> dq = {4, 5};
    dq.prepend_range({1, 2, 3});
    assert(dq.size() == 5);
    assert((dq == std::array{1, 2, 3, 4, 5}));
  }

  // insert(pos, initializer_list) on wrapped buffer
  {
    dq::array<int, 10> circ;
    for (int v : {10, 20, 30, 40, 50}) circ.push_back(v);
    circ.pop_front(); circ.push_back(60); // [20,30,40,50,60] wrapped
    circ.insert(circ.begin() + 2, {100, 200});
    assert(circ.size() == 7);
    assert(circ[0] == 20 && circ[1] == 30);
    assert(circ[2] == 100 && circ[3] == 200);
    assert(circ[4] == 40 && circ[5] == 50 && circ[6] == 60);
  }

  // sort on a sub-range of a wrapped buffer
  {
    dq::array<int, 8> circ;
    for (int v : {5, 3, 9, 1, 7, 4, 2, 8}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(6); circ.push_back(0); // wrapped: [9,1,7,4,2,8,6,0] (logical, wrapped in storage)

    // Sort only the middle portion [begin+1, begin+5).
    auto lo = circ.begin() + 1;
    auto hi = circ.begin() + 5;
    circ.sort(lo, hi);
    assert(std::is_sorted(circ.begin() + 1, circ.begin() + 5));
    // Unsorted extremes are untouched.
    assert(*circ.begin() == 9);
    assert(*(circ.begin() + 5) == 8);
  }

  // stable_sort on a sub-range of a wrapped buffer — stability preserved.
  {
    struct S { int key, seq; };
    dq::array<S, 8> circ;
    int idx = 0;
    for (int k : {4, 2, 4, 1, 3, 2, 3, 1}) circ.push_back({k, idx++});
    circ.pop_front(); circ.pop_front();
    circ.push_back({5, idx++}); circ.push_back({0, idx++}); // wrapped

    circ.stable_sort(circ.begin(), circ.end(),
      [](S const& a, S const& b){ return a.key < b.key; });
    assert(std::is_sorted(circ.begin(), circ.end(),
      [](S const& a, S const& b){ return a.key < b.key; }));
    for (std::size_t i = 1; i < circ.size(); ++i)
      if (circ[i].key == circ[i-1].key)
        assert(circ[i].seq > circ[i-1].seq);
  }

  // pop on a single-element container.
  {
    dq::array<int, 4> dq = {99};
    dq.pop_back();
    assert(dq.empty());

    dq.push_back(42);
    dq.pop_front();
    assert(dq.empty());
  }

  // Alternating push_back / pop_front on a 1-slot container.
  {
    dq::array<int, 1> tiny;
    for (int i = 0; i < 100; ++i) {
      tiny.push_back(i);
      assert(tiny.size() == 1);
      assert(tiny.front() == i);
      tiny.pop_front();
      assert(tiny.empty());
    }
  }

  // Push and pop in lock-step, verifying FIFO order across many wrap cycles.
  {
    constexpr int CAP = 7;
    dq::array<int, CAP> dq;
    int next_push = 0, next_pop = 0;

    // Fill to capacity.
    while (!dq.full()) dq.push_back(next_push++);

    // For each subsequent push, pop one from front first; verify value.
    for (int round = 0; round < 3 * CAP; ++round) {
      assert(dq.front() == next_pop++);
      dq.pop_front();
      dq.push_back(next_push++);
      assert(std::ptrdiff_t(dq.size()) == CAP);
    }

    // Drain and verify FIFO order.
    while (!dq.empty()) {
      assert(dq.front() == next_pop++);
      dq.pop_front();
    }
    assert(next_pop == next_push);
  }

  // assign_range with a transformed view
  {
    dq::array<int, 10> dq = {100, 200, 300};
    auto sq = std::views::iota(1, 6) | std::views::transform([](int x){ return x * x; });
    dq.assign_range(sq);
    assert(dq.size() == 5);
    assert((dq == std::array{1, 4, 9, 16, 25}));
  }

  // assign_range from another dq::array (self-assign guards against corruption).
  {
    dq::array<int, 6> dq = {1, 2, 3};
    dq::array<int, 6> src = {7, 8, 9, 10};
    dq.assign_range(src);
    assert(dq.size() == 4);
    assert((dq == std::array{7, 8, 9, 10}));
  }

  // Erase single occurrence.
  {
    dq::array<int, 8> dq = {1, 2, 3, 2, 4};
    auto removed = dq::erase(dq, 2);
    assert(removed == 2); // both 2s are removed
    assert(dq.size() == 3);
    assert((dq == std::array{1, 3, 4}));
  }

  // Erase value not present — count must be zero.
  {
    dq::array<int, 6> dq = {1, 2, 3};
    auto removed = dq::erase(dq, 99);
    assert(removed == 0);
    assert(dq.size() == 3);
  }

  // Erase multiple keys in a single call.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5, 6};
    auto removed = dq::erase(dq, 2, 4, 6);
    assert(removed == 3);
    assert((dq == std::array{1, 3, 5}));
  }

  // Erase all elements by value.
  {
    dq::array<int, 5> dq = {7, 7, 7, 7};
    auto removed = dq::erase(dq, 7);
    assert(removed == 4);
    assert(dq.empty());
  }

  // count_if — all match.
  {
    dq::array<int, 6> dq = {2, 4, 6, 8, 10};
    auto n = dq::count_if(dq, [](int x){ return x % 2 == 0; });
    assert(n == 5);
  }

  // count_if — none match.
  {
    dq::array<int, 6> dq = {1, 3, 5, 7};
    auto n = dq::count_if(dq, [](int x){ return x % 2 == 0; });
    assert(n == 0);
  }

  // count_if on an empty container.
  {
    dq::array<int, 4> empty;
    assert(dq::count_if(empty, [](int){ return true; }) == 0);
  }

  // count_if across a wrap boundary.
  {
    dq::array<int, 6> circ;
    for (int v : {1, 2, 3, 4, 5, 6}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(7); circ.push_back(8); // logical [3,4,5,6,7,8], wrapped
    auto odd = dq::count_if(circ, [](int x){ return x % 2 != 0; });
    auto evn = dq::count_if(circ, [](int x){ return x % 2 == 0; });
    assert(odd == 3 && evn == 3); // 3,5,7 odd; 4,6,8 even
  }

  // find_if — first element matches.
  {
    dq::array<int, 5> dq = {10, 20, 30};
    auto it = dq::find_if(dq, [](int x){ return x == 10; });
    assert(it == dq.begin() && *it == 10);
  }

  // find_if — last element matches.
  {
    dq::array<int, 5> dq = {1, 3, 5, 7, 9};
    auto it = dq::find_if(dq, [](int x){ return x == 9; });
    assert(it != dq.end() && *it == 9);
  }

  // find_if on a wrapped buffer — finds element in second physical segment.
  {
    dq::array<int, 4> circ;
    for (int v : {10, 20, 30, 40}) circ.push_back(v);
    circ.pop_front(); circ.push_back(50); // logical [20,30,40,50], wrapped
    auto it = dq::find_if(circ, [](int x){ return x == 50; });
    assert(it != circ.end() && *it == 50);
  }

  // dq::swap free function
  {
    dq::array<int, 6> a = {1, 2, 3};
    dq::array<int, 6> b = {4, 5, 6, 7};
    dq::swap(a, b);
    assert((a == std::array{4, 5, 6, 7}));
    assert((b == std::array{1, 2, 3}));
  }

  // Reverse-iterator arithmetic on a wrapped buffer
  {
    dq::array<int, 5> circ;
    for (int v : {1, 2, 3, 4, 5}) circ.push_back(v);
    circ.pop_front(); circ.push_back(6); // [2,3,4,5,6], wrapped
    auto rb = circ.rbegin();
    auto re = circ.rend();

    // rbegin points at last logical element.
    assert(*rb == 6);
    // Advance by 3.
    auto rm = rb + 3;
    assert(*rm == 3);
    // Retreat by 1.
    auto rm2 = rm - 1;
    assert(*rm2 == 4);
    // Distance.
    assert(re - rb == 5);
    assert(rb - re == -5);
    // += / -=
    rb += 2;
    assert(*rb == 4);
    rb -= 1;
    assert(*rb == 5);
  }

  // insert(pos, count, value) across wrap boundary
  {
    dq::array<int, 12> circ;
    for (int v : {10, 20, 30, 40, 50}) circ.push_back(v);
    circ.pop_front(); circ.push_back(60); // [20,30,40,50,60], wrapped
    auto it = circ.insert(circ.begin() + 2, std::size_t(3), 99);
    assert(circ.size() == 8);
    assert(*it == 99);
    assert(circ[2] == 99 && circ[3] == 99 && circ[4] == 99);
    assert(circ[0] == 20 && circ[1] == 30 && circ[5] == 40);
  }

  // Range erase that returns exactly end().
  {
    dq::array<int, 6> dq = {1, 2, 3, 4};
    auto it = dq.erase(dq.begin() + 2, dq.end());
    assert(it == dq.end());
    assert(dq.size() == 2);
    assert((dq == std::array{1, 2}));
  }

  // Range erase on a wrapped buffer.
  {
    dq::array<int, 6> circ;
    for (int v : {10, 20, 30, 40, 50, 60}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(70); circ.push_back(80); // [30,40,50,60,70,80] wrapped
    auto it = circ.erase(circ.begin() + 1, circ.begin() + 3); // erase 40,50
    assert(*it == 60);
    assert(circ.size() == 4);
    assert((circ == std::array{30, 60, 70, 80}));
  }

  // NEW allocator — copy construction
  {
    dq::array<int, 6, dq::NEW> src = {1, 2, 3, 4};
    dq::array<int, 6, dq::NEW> dst(src); // copy ctor
    assert(dst.size() == 4);
    assert((dst == src));
    // Mutation of dst must not affect src.
    dst[0] = 999;
    assert(src[0] == 1);
  }

  // Initializer-list assignment operator
  {
    dq::array<int, 8> dq = {1, 2, 3, 4, 5};
    dq = {10, 20, 30};
    assert(dq.size() == 3);
    assert((dq == std::array{10, 20, 30}));
  }

  // Range-assignment operator= from a range
  {
    dq::array<int, 8> dq = {0, 0, 0};
    dq = std::views::iota(1, 7);
    assert(dq.size() == 6);
    assert((dq == std::array{1, 2, 3, 4, 5, 6}));
  }

  // Interleave push_front and push_back, verify final order is correct.
  {
    dq::array<int, 10> dq;
    // Each push_front goes before the existing front; each push_back goes after.
    dq.push_back(3);
    dq.push_front(2);
    dq.push_back(4);
    dq.push_front(1);
    dq.push_back(5);
    assert(dq.size() == 5);
    assert((dq == std::array{1, 2, 3, 4, 5}));
  }

  // sort() with descending comparator across a wrap boundary
  {
    dq::array<int, 8> circ;
    for (int v : {4, 2, 7, 1, 5, 3, 6}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(9); circ.push_back(8); // wrapped state
    circ.sort(circ.begin(), circ.end(), std::greater<int>{});
    assert(std::is_sorted(circ.begin(), circ.end(), std::greater<int>{}));
  }

  // std::ranges concept satisfaction
  {
    using A = dq::array<int, 8>;
    static_assert(std::ranges::range<A>);
    static_assert(std::ranges::sized_range<A>);
    static_assert(std::ranges::random_access_range<A>);
    static_assert(std::ranges::common_range<A>);

    A dq = {5, 3, 1, 4, 2};
    assert(std::ranges::distance(dq) == 5);
    assert(*std::ranges::min_element(dq) == 1);
    assert(*std::ranges::max_element(dq) == 5);
  }

  // Accumulate / transform / copy_if via std algorithms
  {
    dq::array<int, 10> src = {1, 2, 3, 4, 5, 6};
    // transform into another dq
    dq::array<int, 10> dst;
    dst.resize(src.size());
    std::transform(src.begin(), src.end(), dst.begin(), [](int x){ return x * x; });
    assert((dst == std::array{1, 4, 9, 16, 25, 36}));

    // copy_if to a vector
    std::vector<int> evens;
    std::copy_if(src.begin(), src.end(), std::back_inserter(evens), [](int x){ return x % 2 == 0; });
    assert((evens == std::vector{2, 4, 6}));
  }

  // Repeated resize doesn't corrupt existing elements
  {
    dq::array<int, 20> dq;
    for (int i = 0; i < 10; ++i) dq.push_back(i);
    dq.resize(5);
    assert(dq.size() == 5 && dq[4] == 4);
    dq.resize(8, 42);
    assert(dq.size() == 8 && dq[5] == 42 && dq[7] == 42);
    dq.resize(3);
    assert(dq.size() == 3 && dq[0] == 0 && dq[2] == 2);
    dq.resize(0);
    assert(dq.empty());
  }

  // Mixed operations with std::string elements on wrapped buffer
  {
    dq::array<std::string, 5> dq;
    dq.push_back("cat");
    dq.push_back("dog");
    dq.push_back("fox");
    dq.push_back("owl");
    dq.push_back("ant"); // full
    dq.push_back("bee"); // overwrites "cat" → [dog,fox,owl,ant,bee]
    assert(dq.front() == "dog" && dq.back() == "bee");
    assert(dq.size() == 5);

    dq.sort(dq.begin(), dq.end()); // sort lexicographically
    assert(std::is_sorted(dq.begin(), dq.end()));
    assert(dq.front() == "ant" && dq.back() == "owl");
  }

  // Capacity static_assert — CAP template param
  {
    static_assert(dq::array<int, 1>::capacity() == 1);
    static_assert(dq::array<int, 100>::capacity() == 100);
    static_assert(dq::array<int, 1000>::capacity() == 1000);
  }

  // Full-container erase followed by reinsertion
  {
    dq::array<int, 5> dq = {1, 2, 3, 4, 5};
    assert(dq.full());
    dq.erase(dq.begin(), dq.end());
    assert(dq.empty());
    for (int i = 10; i <= 14; ++i) dq.push_back(i);
    assert(dq.full());
    assert((dq == std::array{10, 11, 12, 13, 14}));
  }

  // max_size static assertions
  {
    static_assert(dq::array<int, 8>::max_size() == PTRDIFF_MAX);
    static_assert(dq::array<char, 4>::max_size() == PTRDIFF_MAX);
  }

  // insert_range from a lazy view
  {
    dq::array<int, 12> dq = {1, 5};
    auto mid = std::views::iota(2, 5); // 2, 3, 4
    auto it = dq.insert_range(dq.begin() + 1, mid);
    assert(*it == 2);
    assert((dq == std::array{1, 2, 3, 4, 5}));
  }

  // Verify that CTAD picks the right value_type and CAP
  {
    dq::array arr = {1.0, 2.0, 3.0}; // should deduce dq::array<double, 3>
    static_assert(std::same_as<decltype(arr)::value_type, double>);
    static_assert(decltype(arr)::capacity() == 3);
    assert(arr.size() == 3 && arr[2] == 3.0);
  }

  // Heavy random-operation stress test with value verification
  {
    constexpr int CAP = 16;
    dq::array<int, CAP> dq;
    std::mt19937 rng{9999};
    std::uniform_int_distribution<int> opd(0, 5);
    std::deque<int> ref; // reference container
    int  val = 0;

    for (int iter = 0; iter < 2000; ++iter) {
      switch (opd(rng)) {
        case 0:
          if (!dq.full()) { dq.push_back(val); ref.push_back(val++); }
          break;
        case 1:
          if (!dq.full()) { dq.push_front(val); ref.push_front(val++); }
          break;
        case 2:
          if (!dq.empty()) { dq.pop_back();  ref.pop_back(); }
          break;
        case 3:
          if (!dq.empty()) { dq.pop_front(); ref.pop_front(); }
          break;
        case 4:
          if (!dq.empty()) {
            std::size_t idx = std::uniform_int_distribution<std::size_t>(0, dq.size()-1)(rng);
            dq.erase(dq.begin() + static_cast<std::ptrdiff_t>(idx));
            ref.erase(ref.begin() + static_cast<std::ptrdiff_t>(idx));
          }
          break;
        case 5:
          if (!dq.full()) {
            std::size_t idx = std::uniform_int_distribution<std::size_t>(0, dq.size())(rng);
            dq.insert(dq.begin() + static_cast<std::ptrdiff_t>(idx), val);
            ref.insert(ref.begin() + static_cast<std::ptrdiff_t>(idx), val++);
          }
          break;
      }

      // Invariants.
      assert(dq.size() == ref.size());
      assert(dq.size() <= std::size_t(CAP));
      for (std::size_t i = 0; i < dq.size(); ++i)
        assert(dq[i] == ref[i]);
    }
  }

  // range moving
  {
    std::string a[]{"A", "B", "C"};
    dq::array<std::string, std::size(a)> b;

    assert(std::ranges::none_of(a, [](auto const& s){ return s.empty(); }));
    assert(std::ranges::all_of(b, [](auto const& s){ return s.empty(); }));
    b = std::move(a);
    assert(std::ranges::all_of(a, [](auto const& s){ return s.empty(); }));
    assert(std::ranges::none_of(b, [](auto const& s){ return s.empty(); }));

    b.assign_range({"a", {"b"}, {"c"}});
    assert(3 == b.size());
  }

  // Cross-container comparison (dq::array vs std::vector/deque)
  {
    dq::array<int, 10> da = {1, 2, 3, 4, 5};
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::deque<int> deq = {1, 2, 3, 4, 5};
    assert(da == vec);
    assert(da == deq);
    assert(!(da < vec));
    assert((da <=> vec) == 0);
    vec.push_back(6);
    assert(da < vec);
    assert((da <=> vec) < 0);
  }

  // c++20 Ranges Customization Points interoperability
  {
    dq::array<int, 5> ra = {10, 20, 30};
    assert(std::ranges::data(ra) == ra.data());
    assert(std::ranges::size(ra) == ra.size());
    assert(std::ranges::empty(ra) == ra.empty());
    assert(*std::ranges::begin(ra) == 10);
    assert(std::ranges::distance(ra) == 3);
    // Verify contiguous_range is false (circular buffer is not physically contiguous)
    static_assert(!std::ranges::contiguous_range<dq::array<int, 5>>);
  }

  // insert with count == 0 (explicit no-op verification)
  {
    dq::array<int, 10> nz = {1, 2, 3};
    auto original = nz;
    auto ret = nz.insert(nz.begin() + 1, 0, 99);
    assert(nz == original);
    assert(ret == nz.begin() + 1);
  }

  // assign with empty initializer list on full container
  {
    dq::array<int, 4> full_arr{1, 2, 3, 4};
    assert(full_arr.full());
    full_arr.assign({});
    assert(full_arr.empty());
    assert(full_arr.first() == full_arr.last());
  }

  // dq::erase with multiple keys on a fully wrapped buffer
  {
    dq::array<int, 6> wrap;
    for(int i=0; i<6; ++i) wrap.push_back(i);
    wrap.pop_front(); wrap.push_back(6); // wrapped state: [1,2,3,4,5,6]
    auto removed = dq::erase(wrap, 2, 5, 99);
    assert(removed == 2);
    assert(wrap.size() == 4);
    assert((wrap == std::array{1,3,4,6}));
  }

  // explicit std::move_iterator usage in assign/insert
  {
    std::vector<std::string> src = {"A", "B", "C"};
    dq::array<std::string, 10> move_arr;
    move_arr.assign(std::make_move_iterator(src.begin()), std::make_move_iterator(src.end()));
    assert(move_arr.size() == 3);
    assert(move_arr[0] == "A");
    // Verify source was moved from
    assert(src[0].empty());
  }

  // move Semantics via std::move_iterator
  {
    std::vector<std::string> src = {"A", "B", "C"};
    dq::array<std::string, 10> dst;

    // Assign from move iterators
    dst.assign(std::make_move_iterator(src.begin()),
               std::make_move_iterator(src.end()));

    assert(dst.size() == 3);
    assert(dst[0] == "A" && dst[1] == "B" && dst[2] == "C");
    // Source elements should be moved-from
    assert(src[0].empty() && src[1].empty() && src[2].empty());

    // Insert with move iterators
    std::vector<std::string> src2 = {"X", "Y"};
    dst.insert(dst.begin(), std::make_move_iterator(src2.begin()),
                               std::make_move_iterator(src2.end()));
    assert(dst.size() == 5 && dst[0] == "X" && dst[1] == "Y");
    assert(src2[0].empty());
  }

  // alignment & Memory Layout
  {
    struct alignas(64) AlignedInt { int val; };
    dq::array<AlignedInt, 4> dq;
    dq.push_back({1}); dq.push_back({2});

    // Verify element alignment
    assert(reinterpret_cast<uintptr_t>(&dq.front()) % 64 == 0);
    assert(reinterpret_cast<uintptr_t>(&dq[1]) % 64 == 0);

    // Verify data pointer alignment
    assert(reinterpret_cast<uintptr_t>(dq.data()) % 64 == 0);
  }

  // concept checks
  {
    // dq::array is a circular buffer; elements are not physically contiguous
    static_assert(std::ranges::random_access_range<dq::array<int, 5>>);
    static_assert(std::ranges::sized_range<dq::array<int, 5>>);
    static_assert(!std::ranges::contiguous_range<dq::array<int, 5>>);
  }

  // dq::copy with Size Limits on Wrapped Buffer
  {
    dq::array<int, 5> dq;
    for (int i = 0; i < 5; ++i) dq.push_back(i);
    dq.pop_front(); dq.push_back(5); // Wrapped: [1, 2, 3, 4, 5]

    int buf_small[3];
    dq::copy(dq, buf_small, 3); // Copy first 3 elements
    assert(buf_small[0] == 1 && buf_small[1] == 2 && buf_small[2] == 3);

    int buf_large[10];
    dq::copy(dq, buf_large, 10); // Copy all, buffer larger
    assert(buf_large[0] == 1 && buf_large[4] == 5);
  }

  // dq::erase with Multiple Keys on Wrapped Buffer
  {
    dq::array<int, 5> dq;
    for (int i = 0; i < 5; ++i) dq.push_back(i);
    dq.push_back(5); // Wrapped: [1, 2, 3, 4, 5]

    auto removed = dq::erase(dq, 2, 4);
    assert(removed == 2);
    assert(dq.size() == 3);
    assert(dq[0] == 1 && dq[1] == 3 && dq[2] == 5);
  }

  // dq::multi Insert on Full Container
  {
    dq::array full{1, 2};
    assert(full.full());
    full.insert(dq::multi, full.begin(), 10, 20);
    assert((full == std::array{1, 2}));
  }

  // std::inplace_merge trigger across Wrap Boundary
  {
    dq::array<int, 6> dq;
    // Create a wrapped state where sorting requires a merge across the physical split
    dq.push_back(10); dq.push_back(20); dq.push_back(5);
    dq.push_back(6); dq.push_back(7); dq.push_back(8);
    dq.pop_front(); dq.push_back(30); // Wrapped state

    dq.sort(dq.begin(), dq.end());
    assert(std::is_sorted(dq.begin(), dq.end()));
    assert(dq.front() == 5 && dq.back() == 30);
  }

  // dq::erase_if on Wrapped Buffer with Complex Type
  {
    dq::array<std::string, 4> dq;
    dq.push_back("hello"); dq.push_back("world");
    dq.push_back("foo");   dq.push_back("bar");
    dq.pop_front(); dq.push_back("baz"); // Wrapped: [world, foo, bar, baz]

    auto removed = dq::erase_if(dq, [](const std::string& s){
        return s == "foo" || s == "baz";
    });

    assert(removed == 2);
    assert(dq.size() == 2);
    assert(dq[0] == "world" && dq[1] == "bar");
  }

  // nicesort boundary conditions — sizes around bsize0 (default 16).
  // Each size tests a different code path inside the merge logic.
  {
    for (std::size_t n : {15u, 16u, 17u, 31u, 32u, 33u, 63u, 64u, 65u, 127u, 128u, 129u}) {
      std::vector<int> v(n);
      std::iota(v.begin(), v.end(), 0);
      std::shuffle(v.begin(), v.end(), std::mt19937{static_cast<unsigned>(n * 31 + 7)});
      auto ref = v;
      nicesort::sort(v.begin(), v.end());
      std::sort(ref.begin(), ref.end());
      assert(v == ref);
    }
  }

  // nicesort with floating-point and negative values.
  {
    std::vector<double> v{3.14, -2.71, 0.0, 1.41, -1.0, 2.72, -0.5, 100.0, -100.0};
    nicesort::sort(v.begin(), v.end());
    assert(std::is_sorted(v.begin(), v.end()));
    assert(v.front() == -100.0 && v.back() == 100.0);

    // All negative.
    std::vector<int> neg{-5, -3, -9, -1, -7, -2, -8, -4, -6};
    nicesort::sort(neg.begin(), neg.end());
    assert(std::is_sorted(neg.begin(), neg.end()));
    assert(neg.front() == -9 && neg.back() == -1);
  }

  // nicesort::insertion_sort direct invocation.
  {
    std::vector<int> v{5, 3, 8, 1, 9, 2, 7, 4, 6};
    nicesort::insertion_sort(v.begin(), v.end(), std::less<int>{});
    assert(std::is_sorted(v.begin(), v.end()));

    // Already sorted — must remain sorted.
    auto sorted = v;
    nicesort::insertion_sort(v.begin(), v.end(), std::less<int>{});
    assert(v == sorted);

    // Descending comparator.
    nicesort::insertion_sort(v.begin(), v.end(), std::greater<int>{});
    assert(std::is_sorted(v.begin(), v.end(), std::greater<int>{}));

    // Two elements, reversed.
    std::vector<int> two{2, 1};
    nicesort::insertion_sort(two.begin(), two.end(), std::less<int>{});
    assert((two == std::vector{1, 2}));

    // Single element — must not crash.
    std::vector<int> one{42};
    nicesort::insertion_sort(one.begin(), one.end(), std::less<int>{});
    assert(one[0] == 42);
  }

  // nicesort stability is preserved for sizes that span multiple block merges.
  {
    struct KV { int key, seq; };
    for (int n : {8, 16, 17, 32, 64, 128}) {
      std::vector<KV> v;
      // Interleave keys so equal-key elements are non-adjacent (stress merges),
      // but each key group's seqs are still in ascending order within the group
      // before sorting — a stable sort must keep them ascending.
      for (int i = 0; i < n; ++i) v.push_back({i % 5, i});
      // Do NOT shuffle: seqs within each key group are already ascending.
      // A stable sort must preserve that order.
      nicesort::sort(v.begin(), v.end(),
        [](KV const& a, KV const& b){ return a.key < b.key; });

      assert(std::is_sorted(v.begin(), v.end(),
        [](KV const& a, KV const& b){ return a.key < b.key; }));

      // Within each key group, seqs must remain in ascending order.
      for (std::size_t i = 1; i < v.size(); ++i)
        if (v[i].key == v[i-1].key) assert(v[i].seq > v[i-1].seq);
    }
  }

  // nicesort correctness vs std::sort on 2000-element permutation.
  {
    std::vector<int> v(2000);
    std::iota(v.begin(), v.end(), -1000);
    std::shuffle(v.begin(), v.end(), std::mt19937{77777u});
    auto ref = v;
    nicesort::sort(v.begin(), v.end());
    std::sort(ref.begin(), ref.end());
    assert(v == ref);
  }

  // nicesort on a sub-range of a larger vector.
  {
    std::vector<int> v{9, 5, 3, 7, 1, 8, 2, 6, 4};
    nicesort::sort(v.begin() + 2, v.begin() + 7); // sort only [3,7,1,8,2]
    assert(std::is_sorted(v.begin() + 2, v.begin() + 7));
    assert(v.front() == 9 && v[1] == 5); // outside range untouched
    assert(v.back() == 4 && v[7] == 6);  // outside range untouched
  }

  // prepend_range with rvalue move semantics.
  {
    std::vector<std::string> src = {"X", "Y", "Z"};
    dq::array<std::string, 10> dq = {"A", "B"};
    dq.prepend_range(std::move(src));
    assert(dq.size() == 5);
    assert(dq[0] == "X" && dq[1] == "Y" && dq[2] == "Z");
    assert(dq[3] == "A" && dq[4] == "B");
    // Source elements must be moved-from.
    assert(src[0].empty() && src[1].empty() && src[2].empty());
  }

  // insert_range with rvalue move semantics.
  {
    std::vector<std::string> src = {"M", "N"};
    dq::array<std::string, 10> dq = {"A", "D"};
    dq.insert_range(dq.begin() + 1, std::move(src));
    assert(dq.size() == 4);
    assert(dq[0] == "A" && dq[1] == "M" && dq[2] == "N" && dq[3] == "D");
    assert(src[0].empty() && src[1].empty());
  }

  // append_range with rvalue move semantics.
  {
    std::vector<std::string> src = {"P", "Q"};
    dq::array<std::string, 10> dq = {"A", "B"};
    dq.append_range(std::move(src));
    assert(dq.size() == 4);
    assert(dq[2] == "P" && dq[3] == "Q");
    assert(src[0].empty() && src[1].empty());
  }

  // allocator — default-constructed data() is always non-null.
  {
    dq::array<int, 8, dq::NEW> a;
    assert(a.data() != nullptr);
    assert(a.empty());
    a.push_back(1);
    assert(a.data() != nullptr);
    assert(a.front() == 1);
  }

  // allocator — push stress to full capacity.
  {
    dq::array<int, 100, dq::NEW> a;
    for (int i = 0; i < 100; ++i) a.push_back(i);
    assert(a.full());
    assert(a.size() == 100);
    for (std::size_t i = 0; i < 100; ++i) assert(a[i] == static_cast<int>(i));
  }

  // count multi-key returns zero when none present (wrapped buffer).
  {
    dq::array<int, 6> circ;
    for (int v : {1, 3, 5, 7, 9, 11}) circ.push_back(v);
    circ.push_back(13); // [3,5,7,9,11,13], wrapped
    auto n = dq::count(circ, 2, 4, 6);
    assert(n == 0);
  }

  // count multi-key finds each value exactly once (wrapped buffer).
  {
    dq::array<int, 6> circ;
    for (int v : {1, 2, 3, 4, 5, 6}) circ.push_back(v);
    circ.push_back(7); // [2,3,4,5,6,7], wrapped
    auto n = dq::count(circ, 3, 6, 99); // 3 found once, 6 found once, 99 not found
    assert(n == 2);
  }

  // Element value correctness across multiple sequential cyclic overwrites.
  // push_back on full: drops front, adds to back.
  // push_front on full: overwrites front in place.
  {
    dq::array<int, 4> ring;
    ring.push_back(1); ring.push_back(2); ring.push_back(3); ring.push_back(4);

    ring.push_back(5); // drops 1 → [2,3,4,5]
    assert((ring == std::array{2, 3, 4, 5}));

    ring.push_back(6); // drops 2 → [3,4,5,6]
    assert((ring == std::array{3, 4, 5, 6}));

    ring.push_front(10); // full: overwrites front (3) → [10,4,5,6]
    assert(ring.front() == 10);
    assert(ring.back()  == 6);
    assert(ring.size()  == 4);

    ring.push_front(20); // full: overwrites front (10) → [20,4,5,6]
    assert(ring.front() == 20);
    assert(ring.back()  == 6);
  }

  // append() returns zero when container is already full.
  {
    dq::array<int, 3> full{1, 2, 3};
    assert(full.full());
    int src[] = {4, 5};
    auto added = full.append(src, 2);
    assert(added == 0);
    assert((full == std::array{1, 2, 3})); // unchanged
  }

  // erase() returns the correct successor on a wrapped buffer.
  {
    dq::array<int, 6> circ;
    for (int v : {10, 20, 30, 40, 50, 60}) circ.push_back(v);
    circ.push_back(70); // [20,30,40,50,60,70], wrapped

    auto it = circ.erase(circ.begin() + 2); // erase 40
    assert(*it == 50);
    assert(circ.size() == 5);
    assert((circ == std::array{20, 30, 50, 60, 70}));

    it = circ.erase(circ.begin()); // erase front
    assert(*it == 30);
    assert(circ.size() == 4);
  }

  // find_if returns end() for no match in non-empty wrapped container.
  {
    dq::array<int, 6> circ;
    for (int v : {1, 3, 5, 7, 9, 11}) circ.push_back(v);
    circ.push_back(13); // [3,5,7,9,11,13], wrapped; all odd
    auto it = dq::find_if(circ, [](int x){ return x % 2 == 0; });
    assert(it == circ.end());
    assert(!it);
  }

  // pop_back(n) and pop_front(n) across a wrap boundary.
  {
    dq::array<int, 8> circ;
    for (int v : {1,2,3,4,5,6,7,8}) circ.push_back(v);
    circ.push_back(9); // [2,3,4,5,6,7,8,9], wrapped

    circ.pop_back(3);
    assert(circ.size() == 5);
    assert(circ.back() == 6);

    circ.pop_front(2);
    assert(circ.size() == 3);
    assert(circ.front() == 4);
    assert((circ == std::array{4, 5, 6}));
  }

  // CTAD deduction from different value types.
  {
    dq::array arr_int    = {1, 2, 3};
    dq::array arr_double = {1.0, 2.0, 3.0};
    dq::array arr_char   = {'a', 'b', 'c'};
    static_assert(std::same_as<decltype(arr_int)::value_type,    int>);
    static_assert(std::same_as<decltype(arr_double)::value_type, double>);
    static_assert(std::same_as<decltype(arr_char)::value_type,   char>);
    static_assert(decltype(arr_int)::capacity()    == 3);
    static_assert(decltype(arr_double)::capacity() == 3);
    static_assert(decltype(arr_char)::capacity()   == 3);
  }

  // emplace_back / emplace_front return references that remain valid for non-trivial types.
  {
    dq::array<std::string, 5> dq;
    std::string& r1 = dq.emplace_back("hello");
    assert(r1 == "hello");
    assert(&r1 == &dq.back());

    std::string& r2 = dq.emplace_front("world");
    assert(r2 == "world");
    assert(&r2 == &dq.front());
  }

  // Verify FIFO semantics: every value pushed is eventually popped in order.
  {
    constexpr int CAP = 5;
    dq::array<int, CAP> dq;
    std::deque<int> ref;

    std::mt19937 rng{12345u};
    std::uniform_int_distribution<int> opd(0, 1);
    int next_push = 0;

    for (int iter = 0; iter < 200; ++iter) {
      if (opd(rng) == 0 && !dq.full()) {
        dq.push_back(next_push);
        ref.push_back(next_push++);
      } else if (!dq.empty()) {
        assert(dq.front() == ref.front());
        dq.pop_front();
        ref.pop_front();
      }
      assert(dq.size() == ref.size());
    }
    // Drain remaining.
    while (!dq.empty()) {
      assert(dq.front() == ref.front());
      dq.pop_front();
      ref.pop_front();
    }
  }

  // std::lower_bound / upper_bound on a sorted dq::array.
  {
    dq::array<int, 10> dq = {1, 3, 5, 7, 9, 11};
    auto lo = std::lower_bound(dq.begin(), dq.end(), 5);
    assert(*lo == 5 && std::distance(dq.begin(), lo) == 2);

    auto hi = std::upper_bound(dq.begin(), dq.end(), 5);
    assert(*hi == 7 && std::distance(dq.begin(), hi) == 3);

    // Value not present.
    auto it = std::lower_bound(dq.begin(), dq.end(), 6);
    assert(*it == 7);

    // Value past end.
    it = std::lower_bound(dq.begin(), dq.end(), 99);
    assert(it == dq.end());
  }

  // std::binary_search on a sorted dq::array.
  {
    dq::array<int, 10> dq = {2, 4, 6, 8, 10};
    assert(std::binary_search(dq.begin(), dq.end(), 4));
    assert(std::binary_search(dq.begin(), dq.end(), 10));
    assert(!std::binary_search(dq.begin(), dq.end(), 5));
    assert(!std::binary_search(dq.begin(), dq.end(), 0));
  }

  // std::equal_range on a sorted dq::array with duplicates.
  {
    dq::array<int, 10> dq = {1, 2, 2, 2, 3, 4};
    auto [lo, hi] = std::equal_range(dq.begin(), dq.end(), 2);
    assert(std::distance(lo, hi) == 3);
    assert(*lo == 2);
    assert(*hi == 3);
  }

  // std::transform_reduce (C++17) integration.
  {
    dq::array<int, 8> a = {1, 2, 3, 4};
    dq::array<int, 8> b = {10, 20, 30, 40};
    int dot = std::transform_reduce(a.begin(), a.end(), b.begin(), 0);
    assert(dot == 1*10 + 2*20 + 3*30 + 4*40); // 300
  }

  // std::for_each accumulates correctly (even vs odd sums).
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5, 6, 7, 8};
    int even_sum = 0, odd_sum = 0;
    std::for_each(dq.begin(), dq.end(), [&](int x){
      if (x % 2 == 0) even_sum += x; else odd_sum += x;
    });
    assert(even_sum == 2+4+6+8);  // 20
    assert(odd_sum  == 1+3+5+7);  // 16
  }

  // Interleaved erase + insert keeps correct element count and values.
  {
    dq::array<int, 10> dq = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    // Erase all even indices, inserting a sentinel after each erasure.
    for (auto it = dq.begin(); it != dq.end(); ) {
      if (*it % 2 == 0) {
        it = dq.erase(it);
        it = dq.insert(it, -1);
        ++it; // skip the -1 we just inserted
      } else {
        ++it;
      }
    }
    // Result: [-1,1,-1,3,-1,5,-1,7,-1,9]
    assert(dq.size() == 10);
    for (std::size_t i = 0; i < dq.size(); i += 2) assert(dq[i] == -1);
    for (std::size_t i = 1; i < dq.size(); i += 2) assert(dq[i] % 2 == 1);
  }

  // Iterator comparison across const / non-const variants (cross-type <=>).
  {
    dq::array<int, 8> dq = {10, 20, 30, 40, 50};
    auto  it  = dq.begin() + 1;
    auto  cit = dq.cbegin() + 3;

    assert((it  <=> cit) < 0);
    assert((cit <=> it)  > 0);
    assert(it != cit);
    assert(!(it == cit));
  }

  // Verify resize shrinks, grows, then shrinks again without corruption.
  {
    dq::array<int, 20> dq;
    for (int i = 0; i < 15; ++i) dq.push_back(i);
    dq.resize(8);
    assert(dq.size() == 8 && dq[7] == 7);
    dq.resize(12, 99);
    assert(dq.size() == 12 && dq[8] == 99 && dq[11] == 99);
    dq.resize(4);
    assert(dq.size() == 4 && dq[3] == 3);
    dq.resize(0);
    assert(dq.empty());
  }

  // push_front variadic inserts elements in correct left-to-right order.
  {
    dq::array<int, 10> dq = {4, 5};
    dq.push_front(1, 2, 3); // should prepend 1, 2, 3 in that order
    assert(dq.size() == 5);
    assert((dq == std::array{1, 2, 3, 4, 5}));
  }

  // push_back variadic appends elements in left-to-right order.
  {
    dq::array<int, 10> dq = {1, 2};
    dq.push_back(3, 4, 5);
    assert(dq.size() == 5);
    assert((dq == std::array{1, 2, 3, 4, 5}));
  }

  // assign_range from an rvalue view — elements are moved.
  {
    std::vector<std::string> src = {"alpha", "beta", "gamma"};
    dq::array<std::string, 10> dq;
    dq.assign_range(std::move(src));
    assert(dq.size() == 3);
    assert(dq[0] == "alpha" && dq[2] == "gamma");
    // Source elements moved-from.
    assert(src[0].empty() && src[1].empty() && src[2].empty());
  }

  // stable_sort on an all-equal-key container preserves insertion sequence.
  {
    struct Item { int key, id; };
    dq::array<Item, 8> dq;
    for (int i = 0; i < 8; ++i) dq.push_back({42, i});
    dq.stable_sort(dq.begin(), dq.end(),
      [](Item const& a, Item const& b){ return a.key < b.key; });
    for (int i = 0; i < 8; ++i) assert(dq[i].id == i); // order preserved
  }

  // nicesort::sort with a custom strict-weak-order on strings.
  {
    std::vector<std::string> v{"banana", "apple", "cherry", "date", "elderberry"};
    nicesort::sort(v.begin(), v.end());
    assert(std::is_sorted(v.begin(), v.end()));
    assert(v.front() == "apple" && v.back() == "elderberry");

    // Reverse.
    nicesort::sort(v.begin(), v.end(), std::greater<std::string>{});
    assert(std::is_sorted(v.begin(), v.end(), std::greater<std::string>{}));
  }

  // dq::find_if returns const_iterator when called on a const container.
  {
    const dq::array<int, 6> cdq = {2, 4, 6, 8, 10};
    // Predicate that only the first element satisfies.
    auto cit = dq::find_if(cdq, [](int x){ return x == 2; });
    static_assert(std::same_as<decltype(cit),
                               dq::array<int,6>::const_iterator>);
    assert(cit != cdq.end() && *cit == 2);

    // Predicate that no element satisfies → end().
    auto none = dq::find_if(cdq, [](int x){ return x > 100; });
    assert(none == cdq.end());
  }

  // dq::count_if returns correct count on const container.
  {
    const dq::array<int, 8> cdq = {1, 2, 3, 4, 5, 6};
    auto n = dq::count_if(cdq, [](int x){ return x > 3; });
    assert(n == 3);
  }

  // Erase-remove idiom with std::ranges::remove.
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5, 6, 7, 8};
    auto rem = std::ranges::remove(dq, 4);
    dq.erase(rem.begin(), rem.end());
    assert(dq.size() == 7);
    assert(std::find(dq.begin(), dq.end(), 4) == dq.end());
  }

  // std::ranges::copy_if into a back_inserter on dq::array.
  {
    dq::array<int, 10> src = {1, 2, 3, 4, 5, 6};
    dq::array<int, 10> dst;
    std::ranges::copy_if(src, std::back_inserter(dst), [](int x){ return x % 2 == 0; });
    assert(dst.size() == 3);
    assert((dst == std::array{2, 4, 6}));
  }

  // std::ranges::transform writes into another dq::array via begin().
  {
    dq::array<int, 6> src = {1, 2, 3, 4, 5};
    dq::array<int, 6> dst(src.size());
    std::ranges::transform(src, dst.begin(), [](int x){ return x * x; });
    assert((dst == std::array{1, 4, 9, 16, 25}));
  }

  // pop_back(n) on a 1-capacity container.
  {
    dq::array<int, 1> tiny;
    tiny.push_back(7);
    assert(tiny.size() == 1 && tiny.front() == 7);
    tiny.pop_back(1);
    assert(tiny.empty());
  }

  // Wrap condition detection via split() segment count.
  {
    dq::array<int, 4> dq = {1, 2, 3, 4};
    {
      auto segs = dq.split();
      // No wrap: first segment covers all, second empty.
      assert(segs[0][1] - segs[0][0] == 4);
      assert(segs[1][0] == segs[1][1]);
    }

    // Two cycles of pop-front/push-back force a non-empty second physical segment.
    dq.pop_front(); dq.push_back(5); // f_ advances once, tail wraps
    dq.pop_front(); dq.push_back(6); // now second segment has data

    // Logical contents: [3,4,5,6], both segments non-empty.
    assert((dq == std::array{3, 4, 5, 6}));
    auto segs = dq.split();
    // Total elements across both segments must equal size.
    auto total = (segs[0][1]-segs[0][0]) + (segs[1][1]-segs[1][0]);
    assert(total == 4);
    // With two wrap cycles the second segment is non-empty.
    assert(segs[1][1] - segs[1][0] > 0);
  }

  // std::min_element and std::max_element on a wrapped buffer.
  {
    dq::array<int, 6> circ;
    for (int v : {5, 1, 9, 3, 7, 2}) circ.push_back(v);
    circ.pop_front(); circ.push_back(8); // [1,9,3,7,2,8], wrapped

    assert(*std::min_element(circ.begin(), circ.end()) == 1);
    assert(*std::max_element(circ.begin(), circ.end()) == 9);
    assert(*std::ranges::min_element(circ) == 1);
    assert(*std::ranges::max_element(circ) == 9);
  }

  // Comparison: empty dq::array < non-empty of the same capacity.
  {
    dq::array<int, 5> empty;
    dq::array<int, 5> one = {1};

    assert(empty < one);
    assert(!(one < empty));
    assert(empty != one);
    assert((empty <=> one) < 0);
    assert((one <=> empty) > 0);
  }

  // Assign from a forward_list (single-pass input range) via assign().
  {
    std::forward_list fl{10, 20, 30, 40};
    dq::array<int, 10> dq;
    dq.assign(fl.begin(), fl.end());
    assert(dq.size() == 4);
    assert(dq[0] == 10 && dq[3] == 40);
  }

  // Large stress: sorted state verified after interleaved sort + erase_if.
  {
    dq::array<int, 200> dq;
    for (int i = 0; i < 200; ++i) dq.push_back(200 - i); // reverse order
    dq.sort(dq.begin(), dq.end());
    assert(std::is_sorted(dq.begin(), dq.end()));

    auto removed = dq::erase_if(dq, [](int x){ return x % 3 == 0; });
    assert(removed > 0);
    assert(std::is_sorted(dq.begin(), dq.end()));
    for (int x : dq) assert(x % 3 != 0);
  }

  // self-assign move
  {
    dq::array a{1, 2, 3, 4};
    a = std::move(a);
    assert((a == std::array{1, 2, 3, 4}));
  }
}

int main() {
  test();
  std::cout << "All tests passed!" << std::endl;
  return 0;
}
