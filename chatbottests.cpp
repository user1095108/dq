#include <cassert>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <thread>
#include <vector>

#include "array.hpp"

void test() {
  // ================================================================
  // Test: Basic construction, element access, and modifiers.
  // ================================================================
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

  // ================================================================
  // Test: Front, back, and middle insertion and erasure.
  // ================================================================
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

  // ================================================================
  // Test: Push, pop, resize, clear, and swap.
  // ================================================================
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

  // ================================================================
  // Test: Mixed push_front and push_back with erasure and insertion.
  // ================================================================
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

  // ================================================================
  // Test: Copy and move construction and assignment.
  // ================================================================
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

  // ================================================================
  // Test: Element access functions.
  // ================================================================
  {
    dq::array<int, 20> dq = {1, 2, 3, 4, 5};

    assert(dq.at(0) == 1);
    assert(dq[1] == 2);
    assert(dq.front() == 1);
    assert(dq.back() == 5);

    assert(!dq.empty());
    assert(dq.size() == 5);
  }

  // ================================================================
  // Test: Emplace operations with the NEW allocator tag.
  // ================================================================
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

  // ================================================================
  // Test: Iterator and reverse iterator access.
  // ================================================================
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

  // ================================================================
  // Test: Comparison operators.
  // ================================================================
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

  // ================================================================
  // Test: Single insert, emplace, range erase, and clear.
  // ================================================================
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

  // ================================================================
  // Test: Push and pop on an initially empty container.
  // ================================================================
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

  // ================================================================
  // Test: Copy and move with non-trivial types.
  // ================================================================
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

  // ================================================================
  // Test: Copy assignment operator.
  // ================================================================
  {
    dq::array<int, 10> dq1 = {1, 2, 3};
    dq::array<int, 10> dq2 = {4, 5, 6, 7};

    dq1 = dq2;
    assert(dq1 == dq2);
  }

  // ================================================================
  // Test: Subscript operator.
  // ================================================================
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};

    assert(dq[0] == 1);
    assert(dq[2] == 3);
    assert(dq[4] == 5);
  }

  // ================================================================
  // Test: std::find integration and the dq::find helper.
  // ================================================================
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto it = std::find(dq.cbegin(), dq.cend(), 3);
    assert(it != dq.end() && *it == 3);
    it = dq::find(dq, 3);
    assert(it && *it == 3);
    it = std::find(dq.begin(), dq.end(), 6);
    assert(it && *it == 6);
  }

  // ================================================================
  // Test: Member swap.
  // ================================================================
  {
    dq::array<int, 10> dq1 = {1, 2, 3};
    dq::array<int, 10> dq2 = {4, 5, 6, 7};

    dq1.swap(dq2);
    assert((dq1 == std::array{4, 5, 6, 7}));
    assert((dq2 == std::array{1, 2, 3}));
  }

  // ================================================================
  // Test: Comparison operators with empty containers.
  // ================================================================
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

  // ================================================================
  // Test: Resize with and without a fill value.
  // ================================================================
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};

    dq.resize(3);
    assert(dq.size() == 3);
    assert((dq == std::array{1, 2, 3}));

    dq.resize(5);
    assert(dq.size() == 5);
  }

  // ================================================================
  // Test: Emplace at front and back.
  // ================================================================
  {
    dq::array<int, 10> dq;

    dq.emplace_front(1);
    assert(dq.front() == 1);
    dq.emplace_back(2);
    assert(dq.back() == 2);
  }

  // ================================================================
  // Test: Single-element insert.
  // ================================================================
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};

    dq.insert(dq.begin() + 2, 6);
    assert((dq == std::array{1, 2, 6, 3, 4, 5}));
  }

  // ================================================================
  // Test: Single-element emplace.
  // ================================================================
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};

    dq.emplace(dq.begin() + 2, 6);
    assert((dq == std::array{1, 2, 6, 3, 4, 5}));
  }

  // ================================================================
  // Test: Range erase.
  // ================================================================
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5};

    dq.erase(dq.begin() + 2, dq.begin() + 4);
    assert((dq == std::array{1, 2, 5}));
  }

  // ================================================================
  // Test: Insert a sub-range from another container.
  // ================================================================
  {
    dq::array<int, 10> dq = {1, 2, 4, 5};
    dq::array<int, 10> to_insert = {3};

    dq.insert(dq.begin() + 2, to_insert.begin(), to_insert.end());
    assert((dq == std::array{1, 2, 3, 4, 5}));
  }

  // ================================================================
  // Test: Erase from front, middle, and back with iterators.
  // ================================================================
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

  // ================================================================
  // Test: Single-element erase at various positions.
  // ================================================================
  {
    dq::array<int, 10> dq = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    dq.erase(dq.begin() + 3);
    assert((dq == std::array{0, 1, 2, 4, 5, 6, 7, 8, 9}));

    dq.erase(dq.end() - 1);
    assert((dq == std::array{0, 1, 2, 4, 5, 6, 7, 8}));
  }

  // ================================================================
  // Test: Swap verifies deep content exchange.
  // ================================================================
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

  // ================================================================
  // Test: Circular overwrite when capacity is reached.
  // ================================================================
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

  // ================================================================
  // Test: Pop front and back, then restore original size.
  // ================================================================
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

  // ================================================================
  // Test: Palindrome verification using front and back.
  // ================================================================
  {
    dq::array<char, 10> palindrome(dq::from_range, std::string_view("racecar"));

    while(palindrome.size() > 1) {
      assert(palindrome.front() == palindrome.back());
      palindrome.pop_front();
      palindrome.pop_back();
    }

    assert(1 == palindrome.size());
  }

  // ================================================================
  // Test: Simulate a deck of cards.
  // ================================================================
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

  // ================================================================
  // Test: Erase from the middle and verify neighbors.
  // ================================================================
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

  // ================================================================
  // Test: Simulate a queue with VIP front insertion.
  // ================================================================
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

  // ================================================================
  // Test: Front and back symmetry.
  // ================================================================
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

  // ================================================================
  // Test: Iterator arithmetic with std::advance.
  // ================================================================
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

  // ================================================================
  // Test: std::remove_if integration.
  // ================================================================
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    d.erase(std::remove_if(d.begin(), d.end(), [](int n){ return n % 2 == 0; }), d.end());
    for (int n : d) assert(n % 2 != 0);
  }

  // ================================================================
  // Test: dq::erase_if helper.
  // ================================================================
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    assert(2 == dq::erase_if(d, [](int n){ return n % 2 == 0; }));
    for (int n : d) assert(n % 2 != 0);
  }

  // ================================================================
  // Test: std::unique integration.
  // ================================================================
  {
    dq::array<int, 20> dq{1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
    dq.erase(std::unique(dq.begin(), dq.end()), dq.end());
    assert((dq == std::array{1, 2, 3, 4}));
  }

  // ================================================================
  // Test: Insert a range from a different container type.
  // ================================================================
  {
    dq::array<int, 10> d = {1, 2, 3, 4};
    dq::array<int, 10> l = {-1, -2, -3};
    auto pos = std::next(d.begin(), 2);
    d.insert(pos, l.begin(), l.end());
    assert((d == std::array{1, 2, -1, -2, -3, 3, 4}));
  }

  // ================================================================
  // Test: std::merge and std::reverse integration.
  // ================================================================
  {
    dq::array<int, 10> dq1{1, 2, 3, 4, 5};
    dq::array<int, 10> dq2{6, 7, 8, 9, 10};

    dq::array<int, 10> mdq(10);

    std::merge(dq1.begin(), dq1.end(), dq2.begin(), dq2.end(), mdq.begin());

    assert((mdq == dq::array<int, 10>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));

    std::reverse(mdq.begin(), mdq.end());

    assert((mdq == std::array{10, 9, 8, 7, 6, 5, 4, 3, 2, 1}));
  }

  // ================================================================
  // Test: Shuffle, rotate, reverse, and sort round-trip.
  // ================================================================
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

  // ================================================================
  // Test: Fill constructor with count and value.
  // ================================================================
  {
    dq::array<int, 10> a(5, 10);

    assert(a.size() == 5);

    for (const auto& e: a) assert(e == 10);
  }

  // ================================================================
  // Test: Insert at beginning, middle, and end.
  // ================================================================
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

  // ================================================================
  // Test: Erase with move-only types.
  // ================================================================
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

  // ================================================================
  // Test: Erase single element and ranges.
  // ================================================================
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

  // ================================================================
  // Test: Swap with various container states.
  // ================================================================
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

  // ================================================================
  // Test: Insert initializer lists at various positions.
  // ================================================================
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

  // ================================================================
  // Test: append_range, prepend_range, and insert_range.
  // ================================================================
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

  // ================================================================
  // Test: Resize with explicit fill value.
  // ================================================================
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

  // ================================================================
  // Test: General usage with iterators and modifiers.
  // ================================================================
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

  // ================================================================
  // Test: Comprehensive functional test case.
  // ================================================================
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

  // ================================================================
  // Test: dq::erase and dq::erase_if with std::find validation.
  // ================================================================
  {
    dq::array<int, 10> d{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    dq::erase(d, 5);
    assert(std::find(d.begin(), d.end(), 5) == d.end());
    assert(!dq::find(d, 5));

    dq::erase_if(d, [](int n){ return n % 2 == 0; });
    assert(std::none_of(d.begin(), d.end(), [](int n){ return n % 2 == 0; }));
    assert(!dq::find_if(d, [](int n){ return n % 2 == 0; }));
  }

  // ================================================================
  // Test: erase_if and erase helpers with expected results.
  // ================================================================
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

  // ================================================================
  // Test: Range erase with various boundaries.
  // ================================================================
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

  // ================================================================
  // Test: Range erase returning correct iterators.
  // ================================================================
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

  // ================================================================
  // Test: Range construction from a C array.
  // ================================================================
  {
    int const a[10]{};
    assert(std::ranges::equal(a, dq::array<int, 10>(a)));
  }

  // ================================================================
  // Test: std::ranges algorithms integration.
  // ================================================================
  {
    dq::array<int, 20> dq = {3, 1, 4, 1, 5};

    std::ranges::sort(dq);
    assert(std::ranges::is_sorted(dq));
    assert(std::ranges::equal(dq, std::array{1, 1, 3, 4, 5}));

    assert(std::ranges::find(dq, 3) != dq.end());
    assert(std::ranges::count(dq, 1) == 2);
  }

  // ================================================================
  // Test: Move-only types.
  // ================================================================
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

  // ================================================================
  // Test: Capacity edge cases with circular overwrite.
  // ================================================================
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

  // ================================================================
  // Test: Heterogeneous lookup with std::string_view.
  // ================================================================
  {
    dq::array<std::string, 10> dq = {"apple", "banana", "pear"};
    assert(std::ranges::find(dq, std::string_view("banana")) != dq.end());
    assert(std::ranges::find(dq, std::string_view("grape")) == dq.end());
  }

  // ================================================================
  // Test: Iterator stability after insert and erase.
  // ================================================================
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

  // ================================================================
  // Test: Randomized stress test.
  // ================================================================
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

  // ================================================================
  // Test: Ranges projection with custom struct.
  // ================================================================
  {
    struct Point { int x, y; };
    dq::array<Point, 10> dq = {{5,6}, {1,2}, {3,4}};

    std::ranges::sort(dq, std::less<>{}, &Point::y);
    assert(dq[0].y == 2 && dq[2].y == 6);
  }

  // ================================================================
  // Test: Const iterators yield const references.
  // ================================================================
  {
    const dq::array<int, 5> dq = {10,20,30};
    static_assert(std::same_as<decltype(dq.begin()), dq::array<int,5>::const_iterator>);
    assert(std::accumulate(dq.begin(), dq.end(), 0) == 60);
  }

  // ================================================================
  // Test: Resize with value initialization.
  // ================================================================
  {
    dq::array<int, 10> dq = {1,2,3};
    dq.resize(6, 42);
    assert(dq.size() == 6);
    assert(dq[3] == 42 && dq[5] == 42);

    dq.resize(2);
    assert(dq.size() == 2);
  }

  // ================================================================
  // Test: Self-swap is a no-op.
  // ================================================================
  {
    dq::array<int, 5> dq = {1,2,3};
    dq.swap(dq);
    assert(dq.size() == 3 && dq[0] == 1);
  }

  // ================================================================
  // Test: Reverse iterator ranges.
  // ================================================================
  {
    dq::array<int, 10> dq = {1,2,3,2,1};
    auto rfirst = dq.rbegin(), rlast = dq.rend();
    assert(std::ranges::count(rfirst, rlast, 2) == 2);
  }

  // ================================================================
  // Test: erase_if with lambda capture.
  // ================================================================
  {
    dq::array<int, 10> dq = {1,2,3,4,5,6};
    int threshold = 4;
    dq::erase_if(dq, [threshold](int x){ return x > threshold; });
    assert((dq == std::array{1,2,3,4}));
  }

  // ================================================================
  // Test: Single-producer single-consumer threading.
  // ================================================================
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

  // ================================================================
  // Test: Insert from a raw C array.
  // ================================================================
  {
    int raw[] = {9,8,7};
    dq::array<int, 10> dq = {1,2,3};
    dq.insert(dq.begin()+1, std::begin(raw), std::end(raw));
    assert((dq == std::array{1,9,8,7,2,3}));
  }

  // ================================================================
  // Test: assign with an initializer list.
  // ================================================================
  {
    dq::array<int, 10> dq;
    dq.assign({10,20,30});
    assert((dq == std::array{10,20,30}));
    assert(dq.size() == 3);
  }

  // ================================================================
  // Test: std::swap overload.
  // ================================================================
  {
    dq::array<std::string, 5> a = {"a","b"};
    dq::array<std::string, 5> b = {"x","y","z"};
    std::swap(a,b);
    assert((a == std::initializer_list<std::string_view>{"x","y","z"}));
    assert((b == std::initializer_list<std::string_view>{"a","b"}));
  }

  // ================================================================
  // Test: std::inner_product integration.
  // ================================================================
  {
    dq::array<int, 5> a = {1,2,3};
    dq::array<int, 5> b = {4,5,6};
    int dot = std::inner_product(a.begin(), a.end(), b.begin(), 0);
    assert(dot == 32);
  }

  // ================================================================
  // Test: Exact reverse iteration.
  // ================================================================
  {
    dq::array<int, 6> dq = {0,1,2,3,4,5};
    std::vector<int> expected = {5,4,3,2,1,0};
    std::vector<int> actual;
    std::copy(dq.rbegin(), dq.rend(), std::back_inserter(actual));
    assert(actual == expected);
  }

  // ================================================================
  // Test: Lexicographical comparison via ranges.
  // ================================================================
  {
    dq::array<int, 20> dq = {1,2,3};
    std::vector<int> v = {1,2,3,4};

    assert(std::ranges::lexicographical_compare(dq, v));
    assert(!std::ranges::lexicographical_compare(v, dq));
    assert(!std::ranges::lexicographical_compare(dq, dq));
  }

  // ================================================================
  // Test: Fill construction and subsequent modification.
  // ================================================================
  {
    dq::array<int, 10> dq(7, 42);
    assert(dq.size() == 7);
    assert(std::all_of(dq.begin(), dq.end(), [](int x){ return x==42; }));

    dq[3] = 99;
    assert(dq[3] == 99);
    assert(std::count(dq.begin(), dq.end(), 42) == 6);
  }

  // ================================================================
  // Test: Front element tracking through cyclic overwrites.
  // ================================================================
  {
    dq::array<char, 4> dq;
    for (char c = 'a'; c <= 'z'; ++c)
    {
      dq.push_back(c);
      if (dq.size() > 1)
        assert(dq.front() == char('a' + (c - 'a') - (dq.size() - 1)));
    }
  }

  // ================================================================
  // Test: Shrink via resize.
  // ================================================================
  {
    dq::array<int, 100> dq(std::ranges::iota_view{0, 50});
    dq.resize(10);
    assert(dq.size() == 10);
    assert(dq.front() == 0 && dq.back() == 9);
  }

  // ================================================================
  // Test: Parallel unsequenced reduce.
  // ================================================================
  {
    dq::array<int, 10> dq = {1,2,3,4,5};
    int sum = std::reduce(std::execution::unseq, dq.begin(), dq.end(), 0);
    assert(sum == 15);
  }

  // ================================================================
  // Test: Rotational equivalence of push_front and pop_back.
  // ================================================================
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

  // ================================================================
  // Test: std::rotate and reverse rotate.
  // ================================================================
  {
    dq::array<int, 10> dq = {1,2,3,4,5};
    std::rotate(dq.begin(), dq.begin()+2, dq.end());
    assert((dq == std::array{3,4,5,1,2}));

    std::rotate(dq.rbegin(), dq.rbegin()+2, dq.rend());
    assert((dq == std::array{1,2,3,4,5}));
  }

  // ================================================================
  // Test: Non-trivial type with full capacity overwrites.
  // ================================================================
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

  // ================================================================
  // Test: Move-only type lifecycle.
  // ================================================================
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

  // ================================================================
  // Test: Self-assignment through a reference.
  // ================================================================
  {
    dq::array<int, 5> dq = {1, 2, 3};
    auto& dq_ref = dq;
    dq = dq_ref;
    assert(dq.size() == 3);
    assert(dq[0] == 1);
  }

  // ================================================================
  // Test: Self-swap with the NEW allocator.
  // ================================================================
  {
    dq::array<int, 5, dq::NEW> dq = {1, 2, 3};
    dq.swap(dq);
    assert(dq.size() == 3);
    assert(dq[0] == 1);
  }

  // ================================================================
  // Test: Range assignment from a vector.
  // ================================================================
  {
    std::vector<int> v = {10, 20, 30, 40};
    dq::array<int, 10> dq;
    dq.assign(v.begin(), v.end());
    assert(dq.size() == 4);
    assert(dq[0] == 10);
    assert(dq[3] == 40);
  }

  // ================================================================
  // Test: Destructor correctness via a counting type.
  // ================================================================
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

  // ================================================================
  // Test: Stress test with mixed operations.
  // ================================================================
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

  // ================================================================
  // Test: Heterogeneous lookup and multi-key erase.
  // ================================================================
  {
    dq::array<std::string, 5> dq = {"apple", "banana", "cherry"};
    auto it = dq::find(dq, "banana");
    assert(it && *it == "banana");
    it = dq::find(dq, std::string_view("cherry"));
    assert(it && *it == "cherry");
    dq::erase(dq, "apple", "banana", "cherry");
    assert(dq.empty());
  }

  // ================================================================
  // Test: Iterator navigation with std::advance, next, and prev.
  // ================================================================
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

  // ================================================================
  // Test: Static capacity and max_size accessors.
  // ================================================================
  {
    dq::array<int, 7> dq = {1, 2, 3};
    static_assert(dq.capacity() == 7);
    assert(dq.capacity() == 7);
    assert(dq.max_size() == PTRDIFF_MAX);
    assert(dq.size() == 3);
    assert(!dq.full());
  }

  // ================================================================
  // Test: Raw pointer accessors data(), first(), and last().
  // ================================================================
  {
    dq::array<int, 5> dq = {10, 20, 30};
    assert(dq.data() != nullptr);
    assert(dq.first() == &dq.front());
    assert(dq.last()  != nullptr);
    assert(dq.last() - dq.first() == static_cast<std::ptrdiff_t>(dq.size()));
  }

  // ================================================================
  // Test: Bulk pop_front and pop_back.
  // ================================================================
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

  // ================================================================
  // Test: assign with count and value.
  // ================================================================
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

  // ================================================================
  // Test: dq::copy to a raw buffer.
  // ================================================================
  {
    dq::array<int, 5> dq = {10, 20, 30, 40, 50};
    int buf[5]{};
    dq::copy(dq, buf);
    for (int i = 0; i < 5; ++i)
      assert(buf[i] == dq[i]);

    int buf2[3]{};
    dq::copy(dq, buf2, 3);
    assert(buf2[0] == 10 && buf2[1] == 20 && buf2[2] == 30);
  }

  // ================================================================
  // Test: append from a raw pointer region.
  // ================================================================
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

  // ================================================================
  // Test: split() and csplit() on unwrapped and wrapped states.
  // ================================================================
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

  // ================================================================
  // Test: dq::copy on a wrapped buffer.
  // ================================================================
  {
    dq::array<int, 4> circ;
    circ.push_back(10); circ.push_back(20);
    circ.push_back(30); circ.push_back(40);
    circ.pop_front(); circ.push_back(50);
    int out[4]{};
    dq::copy(circ, out);
    assert(out[0]==20 && out[1]==30 && out[2]==40 && out[3]==50);
  }

  // ================================================================
  // Test: Iterator explicit bool conversion.
  // ================================================================
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

  // ================================================================
  // Test: Variadic insert, push_back, and push_front.
  // ================================================================
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

  // ================================================================
  // Test: Member sort() with custom comparators.
  // ================================================================
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

  // ================================================================
  // Test: Member sort() on a custom struct.
  // ================================================================
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

  // ================================================================
  // Test: Range construction from views.
  // ================================================================
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

  // ================================================================
  // Test: Assignment from a range.
  // ================================================================
  {
    dq::array<int, 10> dq = {99, 99, 99};
    dq = std::views::iota(1, 6);
    assert(dq.size() == 5);
    assert((dq == std::array{1, 2, 3, 4, 5}));
  }

  // ================================================================
  // Test: dq::find_if with various predicates.
  // ================================================================
  {
    dq::array<int, 10> dq = {1, 3, 5, 7, 8, 9};

    auto it = dq::find_if(dq, [](int x){ return x % 2 == 0; });
    assert(it != dq.end() && *it == 8);

    it = dq::find_if(dq, [](int x){ return x > 100; });
    assert(it == dq.end());

    const auto& cdq = dq;
    auto cit = dq::find_if(cdq, [](int x){ return x == 5; });
    assert(cit != cdq.end() && *cit == 5);
  }

  // ================================================================
  // Test: dq::find with multiple keys.
  // ================================================================
  {
    dq::array<int, 10> dq = {10, 20, 30, 40, 50};

    auto it = dq::find(dq, 30, 40);
    assert(it != dq.end() && (*it == 30 || *it == 40));

    it = dq::find(dq, 99, 100);
    assert(it == dq.end());
    assert(!static_cast<bool>(it));
  }

  // ================================================================
  // Test: Circular overwrite and dq::copy consistency.
  // ================================================================
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

  // ================================================================
  // Test: insert(pos, count, value).
  // ================================================================
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

  // ================================================================
  // Test: Iterator distance across a wrap boundary.
  // ================================================================
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

  // ================================================================
  // Test: Reverse iteration on a wrapped buffer.
  // ================================================================
  {
    dq::array<int, 4> circ = {1, 2, 3, 4};
    circ.pop_front(); circ.push_back(5);

    std::vector<int> fwd(circ.begin(), circ.end());
    std::vector<int> rev(circ.rbegin(), circ.rend());
    std::reverse(rev.begin(), rev.end());
    assert(fwd == rev);
  }

  // ================================================================
  // Test: Self-range assign is a no-op.
  // ================================================================
  {
    dq::array<int, 5> dq = {1, 2, 3};
    dq.assign_range(dq);
    assert(dq.size() == 3 && dq[0] == 1 && dq[2] == 3);
  }

  // ================================================================
  // Test: Static capacity is compile-time constant.
  // ================================================================
  {
    using A = dq::array<double, 16>;
    static_assert(A::capacity() == 16);
    A dq;
    assert(dq.capacity() == 16);
    assert(!dq.full());
  }

  // ================================================================
  // Test: stable_sort() preserves relative order.
  // ================================================================
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

  // ================================================================
  // Test: stable_sort() on a wrapped buffer.
  // ================================================================
  {
    dq::array<int, 6> circ;
    for (int v : {9, 3, 7, 1, 5, 4}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(2); circ.push_back(8);
    circ.stable_sort(circ.begin(), circ.end());
    assert(std::is_sorted(circ.begin(), circ.end()));
    assert((circ == std::array{1, 2, 4, 5, 7, 8}));
  }

  // ================================================================
  // Test: stable_sort() with descending comparator.
  // ================================================================
  {
    dq::array<int, 8> dq = {3, 1, 4, 1, 5, 9, 2, 6};
    dq.stable_sort(dq.begin(), dq.end(), std::greater<int>{});
    assert(std::is_sorted(dq.begin(), dq.end(), std::greater<int>{}));
  }

  // ================================================================
  // Test: Zero-count bulk pop is a no-op.
  // ================================================================
  {
    dq::array<int, 6> dq = {1, 2, 3};
    dq.pop_front(0);
    dq.pop_back(0);
    assert(dq.size() == 3 && dq[0] == 1 && dq[2] == 3);
  }

  // ================================================================
  // Test: NEW allocator move constructor transfers ownership.
  // ================================================================
  {
    dq::array<int, 8, dq::NEW> a = {10, 20, 30};
    auto* raw = a.data();

    dq::array<int, 8, dq::NEW> b(std::move(a));
    assert(b.data() == raw);
    assert(b.size() == 3 && b[1] == 20);
    assert(a.empty());
    assert(a.data() != raw);
  }

  // ================================================================
  // Test: NEW allocator move assignment swaps ownership.
  // ================================================================
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

  // ================================================================
  // Test: erase_if returns the number of removed elements.
  // ================================================================
  {
    dq::array<int, 10> dq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto n = dq::erase_if(dq, [](int x){ return x % 3 == 0; });
    assert(n == 3);
    assert(dq.size() == 7);
    for (int x : dq) assert(x % 3 != 0);
  }

  // ================================================================
  // Test: erase_if on a wrapped buffer.
  // ================================================================
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

  // ================================================================
  // Test: insert_range returns iterator to first inserted element.
  // ================================================================
  {
    std::vector<int> src = {10, 20, 30};
    dq::array<int, 10> dq = {1, 2, 3};
    auto it = dq.insert_range(dq.begin() + 1, src);
    assert(*it == 10);
    assert((dq == std::array{1, 10, 20, 30, 2, 3}));
  }

  // ================================================================
  // Test: first() pointer arithmetic matches operator[].
  // ================================================================
  {
    dq::array<int, 8> dq = {5, 10, 15, 20};
    for (std::size_t i = 0; i < dq.size(); ++i)
      assert(dq.first()[i] == dq[i]);
  }

  // ================================================================
  // Test: Constexpr size and capacity queries.
  // ================================================================
  {
    using A = dq::array<int, 5>;
    static_assert(A::capacity() == 5);
    static_assert(A::max_size() == PTRDIFF_MAX);

    dq::array<int, 5> d = {10, 20, 30};
    d[1] = 99;
    assert(d[0] == 10 && d[1] == 99 && d[2] == 30);
  }

  // ================================================================
  // Test: sort() on a wrapped buffer.
  // ================================================================
  {
    dq::array<int, 6> circ;
    for (int v : {5, 3, 1, 4, 2, 6}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(9); circ.push_back(7);
    circ.sort(circ.begin(), circ.end());
    assert(std::is_sorted(circ.begin(), circ.end()));
  }

  // ================================================================
  // Test: std::partition via random-access iterator.
  // ================================================================
  {
    dq::array<int, 10> dq = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    auto mid = std::partition(dq.begin(), dq.end(), [](int x){ return x < 5; });
    assert(std::all_of(dq.begin(), mid, [](int x){ return x < 5; }));
    assert(std::all_of(mid, dq.end(), [](int x){ return x >= 5; }));
  }

  // ================================================================
  // Test: std::nth_element via random-access iterator.
  // ================================================================
  {
    dq::array<int, 8> dq = {7, 2, 5, 4, 1, 8, 3, 6};
    auto mid = dq.begin() + 3;
    std::nth_element(dq.begin(), mid, dq.end());
    assert(*mid == 4);
    assert(std::all_of(dq.begin(), mid, [](int x){ return x <= 4; }));
    assert(std::all_of(mid + 1, dq.end(), [](int x){ return x >= 4; }));
  }

  // ================================================================
  // Test: assign(count, value) overload.
  // ================================================================
  {
    dq::array<int, 10> dq = {1, 2, 3};
    dq.assign(5, 7);
    assert(dq.size() == 5);
    assert(std::all_of(dq.begin(), dq.end(), [](int x){ return x == 7; }));
    dq.assign(0, 99);
    assert(dq.empty());
  }

  // ----------------------------------------------------------------
  // Test: Emplace with multiple constructor arguments.
  // ----------------------------------------------------------------
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

  // ----------------------------------------------------------------
  // Test: Sort edge cases — empty, single element, all equal.
  // ----------------------------------------------------------------
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

  // ----------------------------------------------------------------
  // Test: Stable sort preserves order for all-equal keys.
  // ----------------------------------------------------------------
  {
    struct Track { int val, seq; };
    dq::array<Track, 10> dq;
    for (int i = 0; i < 5; ++i) dq.push_back({1, i});
    dq.stable_sort(dq.begin(), dq.end(),
      [](Track const& a, Track const& b){ return a.val < b.val; });
    for (int i = 0; i < 5; ++i) assert(dq[i].seq == i);
  }

  // ----------------------------------------------------------------
  // Test: split() and csplit() on an empty container.
  // ----------------------------------------------------------------
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

  // ----------------------------------------------------------------
  // Test: assign from an empty range.
  // ----------------------------------------------------------------
  {
    dq::array<int, 10> dq = {1, 2, 3};
    std::vector<int> empty;
    dq.assign(empty.begin(), empty.end());
    assert(dq.empty());

    dq.assign({});
    assert(dq.empty());
  }

  // ----------------------------------------------------------------
  // Test: Insert at capacity boundary behavior.
  // ----------------------------------------------------------------
  {
    dq::array<int, 4> dq = {1, 2, 3};
    dq.insert(dq.begin(), 0); // should succeed, size becomes 4
    assert(dq.size() == 4);
    assert(dq.full());
    assert(dq[0] == 0);
    assert(dq[3] == 3);
  }

  // ----------------------------------------------------------------
  // Test: Erase of the last element returns end().
  // ----------------------------------------------------------------
  {
    dq::array<int, 5> dq = {1, 2, 3};
    auto it = dq.erase(dq.end() - 1);
    assert(it == dq.end());
    assert(dq.size() == 2);
    assert(dq.back() == 2);
  }

  // ----------------------------------------------------------------
  // Test: find and find_if on empty containers.
  // ----------------------------------------------------------------
  {
    dq::array<int, 5> empty;
    assert(dq::find(empty, 1) == empty.end());
    assert(!dq::find(empty, 1));
    assert(dq::find_if(empty, [](int){ return true; }) == empty.end());
  }

  // ----------------------------------------------------------------
  // Test: erase_if returning zero on empty or non-matching ranges.
  // ----------------------------------------------------------------
  {
    dq::array<int, 5> empty;
    assert(dq::erase_if(empty, [](int){ return true; }) == 0);

    dq::array<int, 5> dq = {1, 3, 5};
    assert(dq::erase_if(dq, [](int x){ return x % 2 == 0; }) == 0);
    assert(dq.size() == 3);
  }

  // ----------------------------------------------------------------
  // Test: Iterator operator-> for struct types.
  // ----------------------------------------------------------------
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

  // ----------------------------------------------------------------
  // Test: first() / last() consistency after cyclic operations.
  // ----------------------------------------------------------------
  {
    dq::array<int, 4> dq;
    dq.push_back(1); dq.push_back(2);
    dq.push_back(3); dq.push_back(4);
    dq.pop_front();
    dq.push_back(5);
    // Logical contents: [2,3,4,5]
    assert(dq.first()[0] == dq[0]);
    assert(dq.first()[1] == dq[1]);
    assert(dq.first()[2] == dq[2]);
    assert(dq.first()[3] == dq[3]);
    assert(dq.first() - dq.last() == 1);
  }

  // ----------------------------------------------------------------
  // Test: pop_front(0) and pop_back(0) on an empty container.
  // ----------------------------------------------------------------
  {
    dq::array<int, 5> empty;
    empty.pop_front(0);
    empty.pop_back(0);
    assert(empty.empty());
  }

  // ----------------------------------------------------------------
  // Test: insert_range at begin() and end().
  // ----------------------------------------------------------------
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

  // ----------------------------------------------------------------
  // Test: append_range and prepend_range with empty ranges.
  // ----------------------------------------------------------------
  {
    dq::array<int, 10> dq = {1, 2, 3};
    std::vector<int> empty;
    dq.append_range(empty);
    assert(dq.size() == 3);
    dq.prepend_range(empty);
    assert(dq.size() == 3);
  }

  // ----------------------------------------------------------------
  // Test: Comparison operators with empty containers.
  // ----------------------------------------------------------------
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

  // ----------------------------------------------------------------
  // Test: push_back / push_front cyclic overwrite with string tracking.
  // ----------------------------------------------------------------
  {
    dq::array<std::string, 3> dq;
    dq.push_back("A");
    dq.push_back("B");
    dq.push_back("C");
    dq.push_back("D"); // overwrites A
    assert(dq.size() == 3);
    assert(dq[0] == "B");
    assert(dq[1] == "C");
    assert(dq[2] == "D");

    dq.push_front("E"); // overwrites B
    assert(dq.size() == 3);
    assert(dq[0] == "E");
    assert(dq[1] == "C");
    assert(dq[2] == "D");
  }

  // ----------------------------------------------------------------
  // Test: std::distance with reverse iterators across wrap boundary.
  // ----------------------------------------------------------------
  {
    dq::array<int, 5> circ;
    for (int v : {1, 2, 3, 4, 5}) circ.push_back(v);
    circ.pop_front(); circ.push_back(6); // wrap
    assert(std::distance(circ.rbegin(), circ.rend()) == 5);
    assert(std::distance(circ.crbegin(), circ.crend()) == 5);
  }

  // ----------------------------------------------------------------
  // Test: resize(0) and regrowth.
  // ----------------------------------------------------------------
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

  // ----------------------------------------------------------------
  // Test: data() pointer is non-null for NEW allocator after move.
  // ----------------------------------------------------------------
  {
    dq::array<int, 5, dq::NEW> a;
    assert(a.data() != nullptr); // NEW should always have an allocation
    dq::array<int, 5, dq::NEW> b(std::move(a));
    assert(b.data() != nullptr);
    assert(a.data() != nullptr); // moved-from still has an allocation
  }

  // ----------------------------------------------------------------
  // Test: dq::copy with empty source.
  // ----------------------------------------------------------------
  {
    dq::array<int, 5> empty;
    int buf[1] = {99};
    dq::copy(empty, buf, 0);
    assert(buf[0] == 99); // should not write anything
  }

  // ----------------------------------------------------------------
  // Test: Full container insert behavior.
  // ----------------------------------------------------------------
  {
    dq::array<int, 3> dq = {1, 2, 3};
    assert(dq.full());
    auto old_front = dq.front();
    dq.insert(dq.begin() + 1, 99);
    // After insert into full container, oldest element is dropped.
    assert(dq.size() == 3);
    assert(dq[0] == 99); // 99, 2, 3
  }
}

int main() {
  test();
  std::cout << "All tests passed!" << std::endl;
  return 0;
}
