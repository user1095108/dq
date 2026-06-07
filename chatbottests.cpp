#include <algorithm>
#include <atomic>
#include <cassert>
#include <climits>
#include <deque>
#include <execution>
#include <forward_list>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <random>
#include <ranges>
#include <thread>
#include <vector>

#include "array.hpp"

void test()
{

// ============================================================================
// §1  CONSTRUCTION
//     Covers every constructor form: default, copy, move, initializer-list,
//     fill, multi-arg tag, range/from_range tag, and CTAD deduction.
// ============================================================================
{
  // 1.1  Default construction yields an empty container.
  {
    dq::array<int, 10> d;
    assert(d.empty());
    assert(d.size() == 0);
  }

  // 1.2  Initializer-list construction.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    assert(!d.empty());
    assert(d.size() == 5);
    assert(d[0] == 1 && d[4] == 5);
  }

  // 1.3  Fill constructor — count and value.
  {
    dq::array<int, 10> d(5, 42);
    assert(d.size() == 5);
    for (int x : d) assert(x == 42);
  }

  // 1.4  Count-only constructor (value-initialises to T{}).
  {
    dq::array<int, 10> d(7);
    assert(d.size() == 7);
  }

  // 1.5  multi_t variadic-arg constructor.
  {
    dq::array d(dq::multi, 10, 20, 30);
    assert(d.size() == 3);
    assert(d[0] == 10 && d[1] == 20 && d[2] == 30);
  }

  // 1.6  Iterator-pair constructor.
  {
    int src[] = {5, 6, 7, 8};
    dq::array<int, 10> d(std::begin(src), std::end(src));
    assert(d.size() == 4);
    assert(d[0] == 5 && d[3] == 8);
  }

  // 1.7  from_range tag with a raw C-array.
  {
    int const a[10]{};
    dq::array<int, 10> d(dq::from_range, a);
    assert(std::ranges::equal(a, d));
  }

  // 1.8  Copy construction — int.
  {
    dq::array src = {1, 2, 3, 4, 5};
    dq::array dst(src);
    assert(dst == src);
  }

  // 1.9  Copy construction — std::string (non-trivial type).
  {
    dq::array src = {"one", "two", "three"};
    dq::array dst(src);
    assert(dst == src);
  }

  // 1.10 Move construction — int.
  {
    dq::array src = {1, 2, 3};
    dq::array snapshot(src);
    dq::array dst(std::move(src));
    assert(dst == snapshot);
    assert(src.empty());
  }

  // 1.11 Move construction — std::string.
  {
    dq::array<std::string, 3> src = {"a", "b", "c"};
    dq::array snapshot(src);
    dq::array dst(std::move(src));
    assert(dst == snapshot);
    assert(src.empty());
  }

  // 1.12 NEW allocator — default construction, data() is non-null.
  {
    dq::array<int, 8, dq::NEW> d;
    assert(d.data() != nullptr);
    assert(d.empty());
  }

  // 1.13 NEW allocator — move construction swaps heap pointers.
  {
    dq::array<int, 8, dq::NEW> a = {10, 20, 30};
    auto* raw = a.data();
    dq::array<int, 8, dq::NEW> b(std::move(a));
    assert(b.data() == raw);
    assert(b.size() == 3 && b[1] == 20);
    assert(a.empty());
    assert(a.data() != raw); // a_ was swapped, not zeroed
  }

  // 1.14 NEW allocator — copy construction is independent.
  {
    dq::array<int, 6, dq::NEW> src = {1, 2, 3, 4};
    dq::array<int, 6, dq::NEW> dst(src);
    assert(dst == src);
    dst[0] = 999;
    assert(src[0] == 1); // deep copy, no aliasing
  }

  // 1.15 Range construction from a transformed view.
  {
    auto doubled = std::views::iota(1, 6)
                 | std::views::transform([](int x){ return x * 2; });
    dq::array<int, 10> d(doubled);
    assert(d.size() == 5);
    assert((d == std::array{2, 4, 6, 8, 10}));
  }

  // 1.16 Range construction from a filtered view via from_range.
  {
    auto evens = std::views::iota(1, 11)
               | std::views::filter([](int x){ return x % 2 == 0; });
    dq::array<int, 10> d(dq::from_range, evens);
    assert(d.size() == 5);
    assert((d == std::array{2, 4, 6, 8, 10}));
  }

  // 1.17 CTAD deduces value_type and capacity from an initializer list.
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

  // 1.18 CTAD with the multi_t tag.
  {
    dq::array arr = {1.5f, 2.5f};
    static_assert(std::same_as<decltype(arr)::value_type, float>);
    static_assert(decltype(arr)::capacity() == 2);
  }
}

// ============================================================================
// §2  ASSIGNMENT
//     Covers copy-assign, move-assign, initializer-list assign, range assign,
//     and the assign() / assign_range() member functions.
// ============================================================================
{
  // 2.1  Copy assignment.
  {
    dq::array<int, 10> a = {1, 2, 3};
    dq::array<int, 10> b = {4, 5, 6, 7};
    a = b;
    assert(a == b);
  }

  // 2.2  Move assignment — MEMBER allocator.
  {
    dq::array<int, 10> a = {1, 2, 3};
    dq::array<int, 10> b = {7, 8, 9};
    dq::array<int, 10> snap(b);
    a = std::move(b);
    assert(a == snap);
    assert(b.empty());
  }

  // 2.3  Move assignment — NEW allocator swaps heap pointers.
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

  // 2.4  Initializer-list assignment operator.
  {
    dq::array<int, 8> d = {1, 2, 3, 4, 5};
    d = {10, 20, 30};
    assert(d.size() == 3);
    assert((d == std::array{10, 20, 30}));
  }

  // 2.5  Range assignment operator= from a range.
  {
    dq::array<int, 8> d = {0, 0, 0};
    d = std::views::iota(1, 7);
    assert(d.size() == 6);
    assert((d == std::array{1, 2, 3, 4, 5, 6}));
  }

  // 2.6  Self copy-assignment is a no-op (does not corrupt).
  {
    dq::array<int, 5> d = {1, 2, 3};
    auto& ref = d;
    d = ref;
    assert(d.size() == 3 && d[0] == 1);
  }

  // 2.7  Self move-assignment is a no-op.
  {
    dq::array a{1, 2, 3, 4};
    a = std::move(a);
    assert((a == std::array{1, 2, 3, 4}));
  }

  // 2.8  assign(count, value) — replaces all elements.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    d.assign(4, 99);
    assert(d.size() == 4);
    assert(std::all_of(d.begin(), d.end(), [](int x){ return x == 99; }));

    d.assign(0, 42);
    assert(d.empty());
  }

  // 2.9  assign(iter, iter) — from a forward_list.
  {
    std::forward_list fl{10, 20, 30, 40};
    dq::array<int, 10> d;
    d.assign(fl.begin(), fl.end());
    assert(d.size() == 4 && d[0] == 10 && d[3] == 40);
  }

  // 2.10 assign(initializer_list).
  {
    dq::array<int, 10> d;
    d.assign({10, 20, 30});
    assert(d.size() == 3);
    assert((d == std::array{10, 20, 30}));

    d.assign({});
    assert(d.empty());
  }

  // 2.11 assign from an empty range clears the container.
  {
    dq::array<int, 10> d = {1, 2, 3};
    std::vector<int> empty;
    d.assign(empty.begin(), empty.end());
    assert(d.empty());
  }

  // 2.12 assign with move_iterator — source elements are moved-from.
  {
    std::vector<std::string> src = {"A", "B", "C"};
    dq::array<std::string, 10> d;
    d.assign(std::make_move_iterator(src.begin()),
             std::make_move_iterator(src.end()));
    assert(d.size() == 3 && d[0] == "A");
    assert(src[0].empty());
  }

  // 2.13 assign_range from a transformed view.
  {
    dq::array<int, 10> d = {100, 200, 300};
    auto sq = std::views::iota(1, 6)
            | std::views::transform([](int x){ return x * x; });
    d.assign_range(sq);
    assert(d.size() == 5);
    assert((d == std::array{1, 4, 9, 16, 25}));
  }

  // 2.14 assign_range from another dq::array.
  {
    dq::array<int, 6> d = {1, 2, 3};
    dq::array<int, 6> src = {7, 8, 9, 10};
    d.assign_range(src);
    assert(d.size() == 4);
    assert((d == std::array{7, 8, 9, 10}));
  }

  // 2.15 assign_range self-assign on same container.
  {
    dq::array<int, 5> d = {1, 2, 3};
    d.assign_range(d);
    assert(d.size() == 3 && d[0] == 1 && d[2] == 3);
  }

  // 2.16 assign_range with rvalue vector — source elements are moved.
  {
    std::vector<std::string> src = {"alpha", "beta", "gamma"};
    dq::array<std::string, 10> d;
    d.assign_range(std::move(src));
    assert(d.size() == 3 && d[0] == "alpha" && d[2] == "gamma");
    assert(src[0].empty() && src[1].empty() && src[2].empty());
  }
}

// ============================================================================
// §3  ELEMENT ACCESS
//     Covers operator[], at(), front(), back(), data(), first(), last().
// ============================================================================
{
  // 3.1  operator[] read and write.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    assert(d[0] == 1 && d[2] == 3 && d[4] == 5);
    d[2] = 99;
    assert(d[2] == 99);
  }

  // 3.2  at() read.
  {
    dq::array<int, 10> d = {10, 20, 30};
    assert(d.at(0) == 10 && d.at(2) == 30);
  }

  // 3.3  front() and back().
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    assert(d.front() == 1 && d.back() == 5);
  }

  // 3.4  data(), first(), last() pointer consistency.
  {
    dq::array<int, 5> d = {10, 20, 30};
    assert(d.data() != nullptr);
    assert(d.first() == &d.front());
    assert(d.last() != nullptr);
    assert(d.last() - d.first() == static_cast<std::ptrdiff_t>(d.size()));
  }

  // 3.5  first() / last() after cyclic operations.
  {
    dq::array<int, 4> d;
    d.push_back(1); d.push_back(2);
    d.push_back(3); d.push_back(4);
    d.pop_front(); d.push_back(5); // logical: [2,3,4,5]
    assert((d == std::initializer_list{2, 3, 4, 5}));
    assert(d.first() - d.last() == 1); // wrapped: last wraps around past first
    assert(d.full());
  }

  // 3.6  first() pointer arithmetic matches operator[].
  {
    dq::array<int, 8> d = {5, 10, 15, 20};
    for (std::size_t i = 0; i < d.size(); ++i)
      assert(d.first()[i] == d[i]);
  }

  // 3.7  Const element access.
  {
    const dq::array<int, 5> d = {1, 2, 3};
    assert(d[0] == 1 && d.front() == 1 && d.back() == 3);
    assert(d.data() != nullptr);
  }
}

// ============================================================================
// §4  CAPACITY QUERIES
//     Covers size(), empty(), full(), capacity(), max_size().
// ============================================================================
{
  // 4.1  Static capacity and max_size are compile-time constants.
  {
    static_assert(dq::array<int, 1>::capacity()    == 1);
    static_assert(dq::array<int, 100>::capacity()  == 100);
    static_assert(dq::array<int, 1000>::capacity() == 1000);
    static_assert(dq::array<int, 8>::max_size()    == PTRDIFF_MAX);
    static_assert(dq::array<char, 4>::max_size()   == PTRDIFF_MAX);
    using A = dq::array<double, 16>;
    static_assert(A::capacity() == 16);
  }

  // 4.2  Runtime capacity and max_size match the static values.
  {
    dq::array<int, 7> d = {1, 2, 3};
    static_assert(d.capacity() == 7);
    assert(d.capacity()  == 7);
    assert(d.max_size()  == PTRDIFF_MAX);
    assert(d.size()      == 3);
    assert(!d.full());
  }

  // 4.3  full() is true exactly when size() == capacity().
  {
    dq::array<int, 4> d{0, 1, 2, 3};
    assert(d.full());
    d.pop_front();
    assert(!d.full());
    d.push_back(99);
    assert(d.full());
  }
}

// ============================================================================
// §5  PUSH / POP — FRONT AND BACK
//     Covers push_back, push_front, emplace_back, emplace_front, pop_back,
//     pop_front, and their bulk / variadic overloads.
// ============================================================================
{
  // 5.1  Basic push_back and push_front, pop_back and pop_front.
  {
    dq::array<int, 10> d;
    d.push_front(1);
    d.push_back(2);
    assert(d.front() == 1 && d.back() == 2);
    d.pop_front();
    d.pop_back();
    assert(d.empty());
  }

  // 5.2  Alternating push/pop restores original state.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    d.pop_front();
    d.pop_back();
    assert(d.size() == 3 && d.front() == 2 && d.back() == 4);
    d.push_front(1);
    d.push_back(5);
    assert(d.size() == 5 && d.front() == 1 && d.back() == 5);
  }

  // 5.3  emplace_back and emplace_front — trivial type.
  {
    dq::array<int, 10> d;
    d.emplace_front(1);
    d.emplace_back(2);
    assert(d.front() == 1 && d.back() == 2);
  }

  // 5.4  emplace_back / emplace_front return references to the emplaced element.
  {
    dq::array<int, 5> d;
    int& r1 = d.emplace_back(42);
    assert(r1 == 42 && &r1 == &d.back());
    int& r2 = d.emplace_front(99);
    assert(r2 == 99 && &r2 == &d.front());
  }

  // 5.5  emplace_back / emplace_front return references — non-trivial type.
  {
    dq::array<std::string, 5> d;
    std::string& r1 = d.emplace_back("hello");
    assert(r1 == "hello" && &r1 == &d.back());
    std::string& r2 = d.emplace_front("world");
    assert(r2 == "world" && &r2 == &d.front());
  }

  // 5.6  Interleaved push_front / push_back yields correct logical order.
  {
    dq::array<int, 10> d;
    d.push_back(3);
    d.push_front(2);
    d.push_back(4);
    d.push_front(1);
    d.push_back(5);
    assert(d.size() == 5);
    assert((d == std::array{1, 2, 3, 4, 5}));
  }

  // 5.7  Variadic push_back — elements appended left-to-right.
  {
    dq::array<int, 10> d = {1, 2};
    d.push_back(3, 4, 5);
    assert(d.size() == 5);
    assert((d == std::array{1, 2, 3, 4, 5}));
  }

  // 5.8  Variadic push_front — elements prepended left-to-right.
  {
    dq::array<int, 10> d = {4, 5};
    d.push_front(1, 2, 3);
    assert(d.size() == 5);
    assert((d == std::array{1, 2, 3, 4, 5}));
  }

  // 5.9  Bulk pop_back(n) and pop_front(n).
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5, 6, 7, 8};
    d.pop_back(3);
    assert(d.size() == 5 && d.back() == 5 && d.front() == 1);
    d.pop_front(2);
    assert(d.size() == 3 && d.front() == 3 && d.back() == 5);
    d.pop_back(d.size());
    assert(d.empty());
  }

  // 5.10 pop_front(0) and pop_back(0) are no-ops.
  {
    dq::array<int, 6> d = {1, 2, 3};
    d.pop_front(0);
    d.pop_back(0);
    assert(d.size() == 3 && d[0] == 1 && d[2] == 3);
  }

  // 5.11 pop_front(0) and pop_back(0) on an empty container.
  {
    dq::array<int, 5> empty;
    empty.pop_front(0);
    empty.pop_back(0);
    assert(empty.empty());
  }

  // 5.12 pop_back(1) on a 1-capacity container.
  {
    dq::array<int, 1> tiny;
    tiny.push_back(7);
    assert(tiny.size() == 1 && tiny.front() == 7);
    tiny.pop_back(1);
    assert(tiny.empty());
  }

  // 5.13 Pop from a single-element container.
  {
    dq::array<int, 4> d = {99};
    d.pop_back();
    assert(d.empty());
    d.push_back(42);
    d.pop_front();
    assert(d.empty());
  }

  // 5.14 Alternating push_back / pop_front on a 1-slot container — stress.
  {
    dq::array<int, 1> tiny;
    for (int i = 0; i < 100; ++i) {
      tiny.push_back(i);
      assert(tiny.size() == 1 && tiny.front() == i);
      tiny.pop_front();
      assert(tiny.empty());
    }
  }

  // 5.15 pop_back(n) and pop_front(n) across a wrap boundary.
  {
    dq::array<int, 8> circ;
    for (int v : {1,2,3,4,5,6,7,8}) circ.push_back(v);
    circ.push_back(9); // [2,3,4,5,6,7,8,9], wrapped
    circ.pop_back(3);
    assert(circ.size() == 5 && circ.back() == 6);
    circ.pop_front(2);
    assert(circ.size() == 3 && circ.front() == 4);
    assert((circ == std::array{4, 5, 6}));
  }

  // 5.16 Bulk pop with count exactly matching size.
  {
    dq::array<int, 5> d{1, 2, 3, 4, 5};
    d.pop_front(2);
    d.pop_back(3);
    assert(d.empty());
    assert(d.first() == d.last());
  }
}

// ============================================================================
// §6  CIRCULAR / WRAP-AROUND BEHAVIOUR
//     Covers cyclic overwrite semantics for both push directions, and the
//     interaction with full(), size(), front(), and back().
// ============================================================================
{
  // 6.1  push_back on a full container advances front (drops oldest element).
  {
    dq::array<int, 5> d(dq::multi, 1, 2, 3, 4, 5);
    assert(d.full());
    d.push_back(6);
    assert(d.size() == 5);
    assert(d.front() == 2);
  }

  // 6.2  push_front on a full container overwrites the current front.
  {
    dq::array<int, 4> d{0, 1, 2, 3};
    assert(d.full());
    d.push_front(99);
    assert(d.full() && d.front() == 99 && d.back() == 3);
  }

  // 6.3  push_back then push_front cycle — sequential overwrite tracking.
  {
    dq::array<int, 4> ring;
    ring.push_back(1); ring.push_back(2);
    ring.push_back(3); ring.push_back(4);

    ring.push_back(5); // drops 1 → [2,3,4,5]
    assert((ring == std::array{2, 3, 4, 5}));

    ring.push_back(6); // drops 2 → [3,4,5,6]
    assert((ring == std::array{3, 4, 5, 6}));

    ring.push_front(10); // full: overwrites front → [10,4,5,6]
    assert(ring.front() == 10 && ring.back() == 6 && ring.size() == 4);

    ring.push_front(20); // full: overwrites front → [20,4,5,6]
    assert(ring.front() == 20 && ring.back() == 6);
  }

  // 6.4  String elements survive cyclic overwrites (destructor correctness).
  {
    dq::array<std::string, 3> d;
    d.push_back("A"); d.push_back("B"); d.push_back("C");
    d.push_back("D"); // overwrites "A" → [B,C,D]
    assert(d.size() == 3);
    assert((d == std::array{"B", "C", "D"}));
    d.push_front("E"); // overwrites "D"? No — overwrites front (B) → [E,C,D]
    assert(d.size() == 3);
    assert((d == std::array{"E", "C", "D"}));
  }

  // 6.5  Front element tracking through cyclic overwrites.
  {
    dq::array<char, 4> d;
    for (char c = 'a'; c <= 'z'; ++c) {
      d.push_back(c);
      if (d.size() > 1)
        assert(d.front() == char('a' + (c - 'a') - (d.size() - 1)));
    }
  }

  // 6.6  Rotational equivalence of push_front and pop_back.
  {
    dq::array<int, 8> a = {1, 2, 3, 4, 5, 6};
    auto b = a;
    for (unsigned i = 0; i < 6; ++i) {
      auto const t(b.back());
      b.pop_back();
      b.push_front(t);
    }
    assert(a == b);
  }

  // 6.7  FIFO ordering is preserved across many wrap cycles.
  {
    constexpr int CAP = 7;
    dq::array<int, CAP> d;
    int next_push = 0, next_pop = 0;

    while (!d.full()) d.push_back(next_push++);

    for (int round = 0; round < 3 * CAP; ++round) {
      assert(d.front() == next_pop++);
      d.pop_front();
      d.push_back(next_push++);
      assert(std::ptrdiff_t(d.size()) == CAP);
    }

    while (!d.empty()) {
      assert(d.front() == next_pop++);
      d.pop_front();
    }
    assert(next_pop == next_push);
  }
}

// ============================================================================
// §7  RESIZE AND CLEAR
//     Covers resize(n), resize(n, v), clear(), full-container erase →
//     re-insertion.
// ============================================================================
{
  // 7.1  resize shrinks and preserves front elements.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    d.resize(3);
    assert(d.size() == 3);
    assert((d == std::array{1, 2, 3}));
  }

  // 7.2  resize grows without a fill value (value-initialises new elements).
  {
    dq::array<int, 10> d = {1, 2, 3};
    d.resize(5);
    assert(d.size() == 5);
  }

  // 7.3  resize(n, v) fills new positions with v.
  {
    dq::array<int, 20> d = {1, 2, 3, 4, 5};
    d.resize(3);
    assert(d.size() == 3 && d[2] == 3);
    d.resize(5, 100);
    assert(d.size() == 5 && d[3] == 100 && d[4] == 100);
  }

  // 7.4  Repeated resize — shrink, grow, shrink — does not corrupt elements.
  {
    dq::array<int, 20> d;
    for (int i = 0; i < 10; ++i) d.push_back(i);
    d.resize(5);  assert(d.size() == 5 && d[4] == 4);
    d.resize(8, 42); assert(d.size() == 8 && d[5] == 42 && d[7] == 42);
    d.resize(3);  assert(d.size() == 3 && d[0] == 0 && d[2] == 2);
    d.resize(0);  assert(d.empty());
  }

  // 7.5  resize(0) followed by reinsertion.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    d.resize(0);
    assert(d.empty());
    d.push_back(10); d.push_front(20);
    assert(d.size() == 2 && d.front() == 20 && d.back() == 10);
  }

  // 7.6  resize on a wrapped buffer — shrink then grow.
  {
    dq::array<int, 6> wrap;
    for (int i = 0; i < 6; ++i) wrap.push_back(i);
    wrap.pop_front(); wrap.push_back(6); // [1,2,3,4,5,6], wrapped
    wrap.resize(3);
    assert(wrap.size() == 3);
    assert((wrap == std::array{1, 2, 3}));
    wrap.resize(5, 99);
    assert(wrap.size() == 5 && wrap[3] == 99 && wrap[4] == 99);
  }

  // 7.7  Shrink via resize from an iota range.
  {
    dq::array<int, 100> d(std::ranges::iota_view{0, 50});
    d.resize(10);
    assert(d.size() == 10 && d.front() == 0 && d.back() == 9);
  }

  // 7.8  clear() leaves the container empty.
  {
    dq::array<int, 10> d = {1, 2, 3};
    d.clear();
    assert(d.empty() && d.size() == 0);
  }

  // 7.9  Full-container erase followed by reinsertion.
  {
    dq::array<int, 5> d = {1, 2, 3, 4, 5};
    assert(d.full());
    d.erase(d.begin(), d.end());
    assert(d.empty());
    for (int i = 10; i <= 14; ++i) d.push_back(i);
    assert(d.full());
    assert((d == std::array{10, 11, 12, 13, 14}));
  }
}

// ============================================================================
// §8  INSERT
//     Covers single-element insert, emplace, count × value insert,
//     initializer-list insert, range insert, append_range, prepend_range,
//     insert_range, and the variadic multi-tag insert.
// ============================================================================
{
  // 8.1  Single-element insert at begin, middle, and end.
  {
    dq::array<int, 20> d = {1, 2, 3, 4, 5};

    auto it = d.insert(d.begin(), 0);
    assert(d.size() == 6 && *it == 0 && d.front() == 0);

    it = d.insert(d.begin() + 3, 6);
    assert(d.size() == 7 && *it == 6 && d[3] == 6);

    it = d.insert(d.end(), 7);
    assert(d.size() == 8 && *it == 7 && d.back() == 7);
  }

  // 8.2  Single-element emplace.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    d.emplace(d.begin() + 2, 6);
    assert((d == std::array{1, 2, 6, 3, 4, 5}));
  }

  // 8.3  Emplace with multiple constructor arguments.
  {
    struct MultiArg {
      int a, b, c;
      MultiArg() = default;
      MultiArg(int x, int y, int z) : a(x), b(y), c(z) {}
      bool operator==(const MultiArg& o) const { return a==o.a && b==o.b && c==o.c; }
    };
    dq::array<MultiArg, 10> d;
    d.emplace_back(1, 2, 3);
    d.emplace_front(4, 5, 6);
    d.emplace(d.begin() + 1, 7, 8, 9);
    assert(d.size() == 3);
    assert(d.front() == MultiArg(4, 5, 6));
    assert(d[1] == MultiArg(7, 8, 9));
    assert(d.back() == MultiArg(1, 2, 3));
  }

  // 8.4  insert(pos, count, value).
  {
    dq::array<int, 20> d = {1, 2, 5};
    auto it = d.insert(d.begin() + 2, 3, 99);
    assert(d.size() == 6 && *it == 99);
    assert(d[2] == 99 && d[3] == 99 && d[4] == 99 && d[5] == 5);

    // Zero-count insert is a no-op — returns position iterator.
    auto it2 = d.insert(d.begin() + 1, 0, 42);
    assert(d.size() == 6);
    assert(it2 == d.begin() + 1);
  }

  // 8.5  insert with count == 0 is a no-op (explicit verification).
  {
    dq::array<int, 10> d = {1, 2, 3};
    auto original = d;
    auto ret = d.insert(d.begin() + 1, 0, 99);
    assert(d == original);
    assert(ret == d.begin() + 1);
  }

  // 8.6  insert(pos, count, value) across a wrap boundary.
  {
    dq::array<int, 12> circ;
    for (int v : {10, 20, 30, 40, 50}) circ.push_back(v);
    circ.pop_front(); circ.push_back(60); // [20,30,40,50,60], wrapped
    auto it = circ.insert(circ.begin() + 2, std::size_t(3), 99);
    assert(circ.size() == 8 && *it == 99);
    assert(circ[2] == 99 && circ[3] == 99 && circ[4] == 99);
    assert(circ[0] == 20 && circ[1] == 30 && circ[5] == 40);
  }

  // 8.7  Initializer-list insert at various positions.
  {
    dq::array<int, 20, dq::NEW> d = {1, 2, 3, 4, 5};

    d.insert(d.begin() + 2, {6, 7, 8});
    assert((d == std::array{1, 2, 6, 7, 8, 3, 4, 5}));

    d = {1, 2, 3, 4, 5};
    d.insert(d.begin(), {6, 7, 8});
    assert((d == std::array{6, 7, 8, 1, 2, 3, 4, 5}));

    d = {1, 2, 3, 4, 5};
    d.insert(d.end(), {6, 7, 8});
    assert((d == std::array{1, 2, 3, 4, 5, 6, 7, 8}));

    d = {};
    d.insert(d.begin(), {6, 7, 8});
    d.insert(d.end(), {9, 10, 11});
    assert((d == std::array{6, 7, 8, 9, 10, 11}));
  }

  // 8.8  insert(pos, initializer_list) on a wrapped buffer.
  {
    dq::array<int, 10> circ;
    for (int v : {10, 20, 30, 40, 50}) circ.push_back(v);
    circ.pop_front(); circ.push_back(60); // [20,30,40,50,60], wrapped
    circ.insert(circ.begin() + 2, {100, 200});
    assert(circ.size() == 7);
    assert(circ[0] == 20 && circ[1] == 30);
    assert(circ[2] == 100 && circ[3] == 200);
    assert(circ[4] == 40 && circ[5] == 50 && circ[6] == 60);
  }

  // 8.9  Range insert from a different container type.
  {
    dq::array<int, 10> d = {1, 2, 4, 5};
    dq::array<int, 10> src = {3};
    d.insert(d.begin() + 2, src.begin(), src.end());
    assert((d == std::array{1, 2, 3, 4, 5}));
  }

  // 8.10 Range insert from a raw C-array.
  {
    int raw[] = {9, 8, 7};
    dq::array<int, 10> d = {1, 2, 3};
    d.insert(d.begin() + 1, std::begin(raw), std::end(raw));
    assert((d == std::array{1, 9, 8, 7, 2, 3}));
  }

  // 8.11 Multi-container range insert (several elements in sequence).
  {
    dq::array<int, 20> d = {1, 2, 3, 4};
    dq::array<int, 10> src = {8, 9, 10};
    d.insert(d.begin() + 2, src.begin(), src.end());
    assert(d.size() == 7 && d[2] == 8 && d[3] == 9 && d[4] == 10);
  }

  // 8.12 insert_range returns iterator to first inserted element.
  {
    std::vector<int> src = {10, 20, 30};
    dq::array<int, 10> d = {1, 2, 3};
    auto it = d.insert_range(d.begin() + 1, src);
    assert(*it == 10);
    assert((d == std::array{1, 10, 20, 30, 2, 3}));
  }

  // 8.13 insert_range at begin() and end().
  {
    std::vector<int> src = {0, 9};
    dq::array<int, 10> d = {1, 2, 3};
    auto it = d.insert_range(d.begin(), src);
    assert(*it == 0);
    assert(d[0] == 0 && d[1] == 9 && d[2] == 1);

    it = d.insert_range(d.end(), src);
    assert(*it == 0);
    assert(d.back() == 9);
  }

  // 8.14 insert_range from a lazy view.
  {
    dq::array<int, 12> d = {1, 5};
    auto mid = std::views::iota(2, 5);
    auto it = d.insert_range(d.begin() + 1, mid);
    assert(*it == 2);
    assert((d == std::array{1, 2, 3, 4, 5}));
  }

  // 8.15 insert_range with rvalue move semantics.
  {
    std::vector<std::string> src = {"M", "N"};
    dq::array<std::string, 10> d = {"A", "D"};
    d.insert_range(d.begin() + 1, std::move(src));
    assert(d.size() == 4);
    assert(d[0] == "A" && d[1] == "M" && d[2] == "N" && d[3] == "D");
    assert(src[0].empty() && src[1].empty());
  }

  // 8.16 insert_range returning correct iterator to first inserted element (2).
  {
    std::vector<int> src{10, 20, 30};
    dq::array<int, 10> d{1, 2, 3};
    auto it = d.insert_range(d.begin() + 1, src);
    assert(*it == 10);
    assert(d[1] == 10 && d[2] == 20 && d[3] == 30 && d[4] == 2);
  }

  // 8.17 append_range, prepend_range, insert_range round-trip.
  {
    dq::array<int, 30> d{1, 2, 3};
    std::vector<int> vec{4, 5, 6};
    d.append_range(vec);
    assert((d == std::array{1, 2, 3, 4, 5, 6}));
    d.prepend_range(vec);
    assert((d == std::array{4, 5, 6, 1, 2, 3, 4, 5, 6}));
    auto it = d.begin();
    std::advance(it, 3);
    d.insert_range(it, vec);
    assert((d == std::array{4, 5, 6, 4, 5, 6, 1, 2, 3, 4, 5, 6}));
  }

  // 8.18 append_range and prepend_range with empty ranges are no-ops.
  {
    dq::array<int, 10> d = {1, 2, 3};
    std::vector<int> empty;
    d.append_range(empty);
    assert(d.size() == 3);
    d.prepend_range(empty);
    assert(d.size() == 3);
  }

  // 8.19 prepend_range on a fresh container.
  {
    dq::array<int, 10> d = {4, 5, 6};
    d.prepend_range({1, 2, 3});
    assert((d == std::array{1, 2, 3, 4, 5, 6}) && d.size() == 6);
  }

  // 8.20 prepend_range on a wrapped buffer.
  {
    dq::array<int, 8> circ;
    for (int v : {10, 20, 30, 40, 50, 60}) circ.push_back(v);
    circ.pop_front(); circ.pop_front(); // [30,40,50,60]
    circ.prepend_range({1, 2});
    assert((circ == std::array{1, 2, 30, 40, 50, 60}));
  }

  // 8.21 prepend_range with rvalue move semantics.
  {
    std::vector<std::string> src = {"X", "Y", "Z"};
    dq::array<std::string, 10> d = {"A", "B"};
    d.prepend_range(std::move(src));
    assert(d.size() == 5);
    assert(d[0] == "X" && d[1] == "Y" && d[2] == "Z");
    assert(d[3] == "A" && d[4] == "B");
    assert(src[0].empty() && src[1].empty() && src[2].empty());
  }

  // 8.22 append_range with rvalue move semantics.
  {
    std::vector<std::string> src = {"P", "Q"};
    dq::array<std::string, 10> d = {"A", "B"};
    d.append_range(std::move(src));
    assert(d.size() == 4 && d[2] == "P" && d[3] == "Q");
    assert(src[0].empty() && src[1].empty());
  }

  // 8.23 Variadic multi-tag insert.
  {
    dq::array<int, 20> d = {1, 5};
    auto it = d.insert(dq::multi, d.cbegin() + 1, 2, 3, 4);
    assert(d.size() == 5);
    assert((d == std::array{1, 2, 3, 4, 5}));
    assert(*it == 2);
  }

  // 8.24 Insert at capacity boundary — fills the last slot.
  {
    dq::array<int, 4> d = {1, 2, 3};
    d.insert(d.begin(), 0);
    assert(d.size() == 4 && d.full());
    assert(d[0] == 0 && d[3] == 3);
  }

  // 8.25 multi-tag insert on a full container causes cyclic overwrite.
  {
    dq::array<int, 4> full{1, 2, 3, 4};
    assert(full.full());
    full.insert(dq::multi, full.begin() + 1, 99, 88);
    assert(full.size() == 4);
    assert((full == std::array{88, 2, 3, 4}));
  }

  // 8.26 Full-container insert behavior — front element dropped.
  {
    dq::array<int, 3> d = {1, 2, 3};
    assert(d.full());
    d.insert(d.begin() + 1, 99);
    assert(d.size() == 3);
    assert((d == std::array{99, 2, 3}));
  }
}

// ============================================================================
// §9  ERASE
//     Covers single-element erase, range erase, the erase-remove idiom,
//     erase returning correct successor iterators, and erase on wrapped
//     buffers.
// ============================================================================
{
  // 9.1  Single-element erase at front, middle, and back.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    d.erase(d.begin() + 2);
    assert((d == std::array{1, 2, 4, 5}));

    d = {1, 2, 3, 4, 5};
    d.erase(d.begin());
    assert((d == std::array{2, 3, 4, 5}));

    d = {1, 2, 3, 4, 5};
    d.erase(d.end() - 1);
    assert((d == std::array{1, 2, 3, 4}));
  }

  // 9.2  Erase of the last element returns end().
  {
    dq::array<int, 5> d = {1, 2, 3};
    auto it = d.erase(d.end() - 1);
    assert(it == d.end());
    assert(d.size() == 2 && d.back() == 2);
  }

  // 9.3  Range erase at various positions.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    d.erase(d.begin() + 1, d.begin() + 3);
    assert((d == std::array{1, 4, 5}));

    d = {1, 2, 3, 4, 5};
    d.erase(d.begin(), d.begin() + 2);
    assert((d == std::array{3, 4, 5}));

    d = {1, 2, 3, 4, 5};
    d.erase(d.begin() + 3, d.end());
    assert((d == std::array{1, 2, 3}));

    d = {1, 2, 3, 4, 5};
    d.erase(d.begin(), d.end());
    assert(d.empty());
  }

  // 9.4  Range erase that returns exactly end().
  {
    dq::array<int, 6> d = {1, 2, 3, 4};
    auto it = d.erase(d.begin() + 2, d.end());
    assert(it == d.end());
    assert(d.size() == 2);
    assert((d == std::array{1, 2}));
  }

  // 9.5  Empty-range erase (begin == end) is a no-op.
  {
    dq::array<int, 10> d{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto it = d.begin() + 3;
    d.erase(it, it);
    assert((d == std::array{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
  }

  // 9.6  Range erase on a wrapped buffer.
  {
    dq::array<int, 6> circ;
    for (int v : {10, 20, 30, 40, 50, 60}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(70); circ.push_back(80); // [30,40,50,60,70,80], wrapped
    auto it = circ.erase(circ.begin() + 1, circ.begin() + 3); // erase 40, 50
    assert(*it == 60);
    assert(circ.size() == 4);
    assert((circ == std::array{30, 60, 70, 80}));
  }

  // 9.7  erase() on a wrapped buffer returns correct successor.
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

  // 9.8  Erase all even elements via a loop.
  {
    dq::array<int, 10> d = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    d.erase(d.begin());
    assert((d == std::array{1, 2, 3, 4, 5, 6, 7, 8, 9}));

    for (auto it = d.begin(); it != d.end(); )
      *it % 2 == 0 ? it = d.erase(it) : ++it;
    assert((d == std::array{1, 3, 5, 7, 9}));

    d.erase(d.begin(), d.end());
    assert(d.empty());
  }

  // 9.9  std::remove_if integration.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    d.erase(std::remove_if(d.begin(), d.end(), [](int n){ return n % 2 == 0; }),
            d.end());
    for (int n : d) assert(n % 2 != 0);
  }

  // 9.10 Erase-remove idiom with std::ranges::remove.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5, 6, 7, 8};
    auto rem = std::ranges::remove(d, 4);
    d.erase(rem.begin(), rem.end());
    assert(d.size() == 7);
    assert(std::find(d.begin(), d.end(), 4) == d.end());
  }

  // 9.11 Erase with move-only types.
  {
    dq::array<std::unique_ptr<int>, 10> d;
    for (int i = 0; i < 10; ++i) d.push_back(std::make_unique<int>(i));
    auto it = d.erase(d.begin() + 5);
    assert(d.size() == 9);
    assert(*d[5] == 6 && **it == 6);
  }

  // 9.12 Interleaved erase + insert keeps correct element count and values.
  {
    dq::array<int, 10> d = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (auto it = d.begin(); it != d.end(); ) {
      if (*it % 2 == 0) {
        it = d.erase(it);
        it = d.insert(it, -1);
        ++it;
      } else {
        ++it;
      }
    }
    // Result: [-1,1,-1,3,-1,5,-1,7,-1,9]
    assert(d.size() == 10);
    for (std::size_t i = 0; i < d.size(); i += 2) assert(d[i] == -1);
    for (std::size_t i = 1; i < d.size(); i += 2) assert(d[i] % 2 == 1);
  }
}

// ============================================================================
// §10 SWAP
// ============================================================================
{
  // 10.1 Member swap — MEMBER allocator.
  {
    dq::array<int, 10> a = {1, 2, 3};
    dq::array<int, 10> b = {4, 5, 6, 7};
    a.swap(b);
    assert((a == std::array{4, 5, 6, 7}));
    assert((b == std::array{1, 2, 3}));
  }

  // 10.2 Member swap — NEW allocator.
  {
    dq::array<int, 10, dq::NEW> a = {1, 2, 3};
    dq::array<int, 10, dq::NEW> b = {4, 5, 6, 7};
    a.swap(b);
    assert((a == std::array{4, 5, 6, 7}));
    assert((b == std::array{1, 2, 3}));
  }

  // 10.3 Swap with one empty container.
  {
    dq::array<int, 10, dq::NEW> a = {1, 2, 3};
    dq::array<int, 10, dq::NEW> empty;
    a.swap(empty);
    assert(a.empty());
    assert((empty == std::array{1, 2, 3}));
  }

  // 10.4 Swap two empty containers.
  {
    dq::array<int, 10, dq::NEW> a, b;
    a.swap(b);
    assert(a.empty() && b.empty());
  }

  // 10.5 Swap between empty and full containers.
  {
    dq::array<int, 4> full{1, 2, 3, 4};
    dq::array<int, 4> empty;
    full.swap(empty);
    assert(full.empty() && empty.full());
    assert((empty == std::array{1, 2, 3, 4}));
  }

  // 10.6 Swap verifies deep content exchange.
  {
    dq::array<int, 10> a, b;
    for (int i = 1; i <= 5; ++i) a.push_back(i);
    for (int i = 6; i <= 10; ++i) b.push_back(i);
    a.swap(b);
    for (int i = 1; i <= 5; ++i) assert(b[i-1] == i);
    for (int i = 6; i <= 10; ++i) assert(a[i-6] == i);
  }

  // 10.7 Self-swap is a no-op — MEMBER allocator.
  {
    dq::array<int, 5> d = {1, 2, 3};
    d.swap(d);
    assert(d.size() == 3 && d[0] == 1);
  }

  // 10.8 Self-swap is a no-op — NEW allocator.
  {
    dq::array<int, 5, dq::NEW> d = {1, 2, 3};
    d.swap(d);
    assert(d.size() == 3 && d[0] == 1);
  }

  // 10.9 dq::swap free function.
  {
    dq::array<int, 6> a = {1, 2, 3};
    dq::array<int, 6> b = {4, 5, 6, 7};
    dq::swap(a, b);
    assert((a == std::array{4, 5, 6, 7}));
    assert((b == std::array{1, 2, 3}));
  }

  // 10.10 std::swap overload.
  {
    dq::array<std::string, 5> a = {"a", "b"};
    dq::array<std::string, 5> b = {"x", "y", "z"};
    std::swap(a, b);
    assert((a == std::initializer_list{"x", "y", "z"}));
    assert((b == std::initializer_list{"a", "b"}));
  }
}

// ============================================================================
// §11 COMPARISON OPERATORS
//     Covers ==, !=, <, <=, >, >=, <=>, cross-container comparisons, and
//     comparisons with empty containers.
// ============================================================================
{
  // 11.1 Basic relational operators.
  {
    dq::array<int, 20> a = {1, 2, 3, 4, 5};
    dq::array<int, 20> b = {1, 2, 3, 4, 5};
    dq::array<int, 20> c = {1, 2, 3, 4, 6};
    assert(a == b);
    assert(a != c);
    assert(a < c);
    assert(a <= b);
    assert(c > a);
    assert(b >= a);
  }

  // 11.2 Three-way comparison <=> — equal, less, greater.
  {
    dq::array<int, 10> a{1, 2, 3};
    dq::array<int, 10> b{1, 2, 3, 4};
    assert((a <=> b) < 0);
    assert((b <=> a) > 0);
    assert((a <=> a) == 0);
  }

  // 11.3 Comparison with empty containers.
  {
    dq::array<int, 5> e1, e2;
    assert(e1 == e2);
    assert(!(e1 != e2));
    assert(!(e1 < e2));
    assert(e1 <= e2);
    assert(!(e1 > e2));
    assert(e1 >= e2);

    dq::array<int, 5> ne = {1};
    assert(e1 != ne);
    assert(e1 < ne);
    assert(ne > e1);
    assert((e1 <=> ne) < 0);
    assert((ne <=> e1) > 0);
  }

  // 11.4 Cross-container comparison: dq::array vs std::vector and std::deque.
  {
    dq::array<int, 10> da = {1, 2, 3, 4, 5};
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::deque<int>  deq = {1, 2, 3, 4, 5};
    assert(da == vec);
    assert(da == deq);
    assert(!(da < vec));
    assert((da <=> vec) == 0);
    vec.push_back(6);
    assert(da < vec);
    assert((da <=> vec) < 0);
  }

  // 11.5 Cross-capacity comparison (different CAP, same values).
  {
    dq::array<int, 5>  a = {1, 2, 3};
    dq::array<int, 10> b = {1, 2, 3};
    assert(a == b);
    b.push_back(4);
    assert(a < b);
  }

  // 11.6 Lexicographical comparison via std::ranges.
  {
    dq::array<int, 20> d = {1, 2, 3};
    std::vector<int> v = {1, 2, 3, 4};
    assert(std::ranges::lexicographical_compare(d, v));
    assert(!std::ranges::lexicographical_compare(v, d));
    assert(!std::ranges::lexicographical_compare(d, d));
  }
}

// ============================================================================
// §12 ITERATORS
//     Covers forward, reverse, and const iterators; iterator arithmetic;
//     bool conversion; and the const_iterator conversion path.
// ============================================================================
{
  // 12.1  begin() / end() traversal.
  {
    dq::array<int, 20> d = {1, 2, 3, 4, 5};
    assert(*d.begin() == 1);
    assert(*(d.end() - 1) == 5);
    auto it = d.begin();
    assert(*it == 1); ++it; assert(*it == 2); ++it; assert(*it == 3);
  }

  // 12.2  Reverse iterator traversal.
  {
    dq::array<int, 20> d = {1, 2, 3, 4, 5};
    assert(*d.rbegin() == 5);
    assert(*(d.rend() - 1) == 1);
    dq::array<int, 10> d2 = {1, 2, 3, 2, 1};
    assert(std::ranges::count(d2.rbegin(), d2.rend(), 2) == 2);
  }

  // 12.3  Const iterators from a const container.
  {
    const dq::array<int, 5> d = {5, 10, 15};
    static_assert(std::same_as<decltype(d.begin()), dq::array<int,5>::const_iterator>);
    static_assert(std::same_as<decltype(d.end()),   dq::array<int,5>::const_iterator>);
    int sum = 0;
    for (auto it = d.begin(); it != d.end(); ++it) sum += *it;
    assert(sum == 30);
  }

  // 12.4  cbegin() / cend() / crbegin() / crend().
  {
    const dq::array<int, 20> d = {1, 2, 3, 4, 5};
    assert(*d.cbegin() == 1);
    assert(*(d.cend() - 1) == 5);
    assert(*d.crbegin() == 5);
    assert(*(d.crend() - 1) == 1);
  }

  // 12.5  Iterator arithmetic: advance, next, prev.
  {
    dq::array<int, 20> d = {10, 20, 30, 40, 50, 60, 70};
    auto it = d.begin();
    std::advance(it, 3); assert(*it == 40);
    std::advance(it, -2); assert(*it == 20);
    std::advance(it, 0); assert(*it == 20);
    assert(*std::next(it, 4)  == 60);
    assert(*std::next(it, -1) == 10);
    it = d.end();
    assert(*std::prev(it, 1) == 70);
    assert(*std::prev(it, 3) == 50);
    it = d.begin();
    std::advance(it, static_cast<int>(d.size()));
    assert(it == d.end());
    std::advance(it, -static_cast<int>(d.size()));
    assert(it == d.begin());
    assert(*std::next(d.rbegin(), 2) == 50);
    assert(*std::prev(d.rend(),  3)  == 30);
  }

  // 12.6  operator+= and operator-=.
  {
    dq::array<int, 8> d = {10, 20, 30, 40, 50};
    auto it = d.begin();
    it += 2; assert(*it == 30);
    it += 2; assert(*it == 50);
    it -= 4; assert(*it == 10);
    it -= 0; assert(*it == 10);
    it += static_cast<int>(d.size()) - 1;
    assert(*it == 50);
  }

  // 12.7  operator- (iterator subtraction).
  {
    dq::array<int, 6> d = {1, 2, 3, 4, 5};
    auto b = d.begin(), e = d.end();
    assert(e - b ==  5);
    assert(b - e == -5);
    assert(b - b ==  0);
    auto m = b + 2;
    assert(m - b ==  2 && e - m == 3);
  }

  // 12.8  operator[] on the iterator.
  {
    dq::array<int, 8> d = {0, 1, 2, 3, 4, 5, 6, 7};
    auto it = d.begin();
    for (int i = 0; i < 8; ++i) assert(it[i] == i);
  }

  // 12.9  Post-increment / post-decrement return copies.
  {
    dq::array<int, 5> d = {10, 20, 30};
    auto it = d.begin();
    auto old = it++;
    assert(*old == 10 && *it == 20);
    auto old2 = it--;
    assert(*old2 == 20 && *it == 10);
  }

  // 12.10 Three-way comparison on iterators.
  {
    dq::array<int, 6> d = {1, 2, 3, 4, 5};
    auto a = d.begin(), b = d.begin() + 2, e = d.end();
    assert((a <=> b) < 0);
    assert((b <=> a) > 0);
    assert((a <=> a) == 0);
    assert((b <=> e) < 0);
    assert((e <=> e) == 0);
  }

  // 12.11 Non-member n + iterator operator.
  {
    dq::array<int, 6> d = {0, 1, 2, 3, 4};
    auto it = d.begin();
    assert(*(2 + it) == 2);
    assert(*(0 + it) == 0);
    assert(*(4 + it) == 4);
    assert((2 + it) == (it + 2));
  }

  // 12.12 Negative offset arithmetic: it + (-n) == it - n.
  {
    dq::array<int, 8> d = {10, 20, 30, 40, 50};
    auto e = d.end();
    assert(*(e + (-1)) == 50);
    assert(*(e + (-3)) == 30);
    assert(*(e - 1) == *(e + (-1)));
  }

  // 12.13 Implicit iterator to const_iterator conversion.
  {
    dq::array<int, 5> d = {1, 2, 3};
    using iter_t       = decltype(d)::iterator;
    using const_iter_t = decltype(d)::const_iterator;
    iter_t it = d.begin() + 1;
    const_iter_t cit = it;  // implicit conversion
    assert(*cit == *it);
    assert(cit == it);
    ++cit;
    assert(*cit == 3 && *it == 2);
  }

  // 12.14 Cross-type iterator equality and ordering (iterator vs const_iterator).
  {
    dq::array<int, 6> d = {1, 2, 3, 4, 5};
    auto it  = d.begin() + 2;
    auto cit = d.cbegin() + 2;
    assert(it == cit && cit == it && !(it != cit));
    auto it2 = d.begin() + 3;
    assert(it2 != cit);
    assert((it2 <=> cit) > 0);
    assert((cit <=> it2) < 0);

    auto ita  = d.begin() + 1;
    auto cita = d.cbegin() + 3;
    assert((ita <=> cita) < 0);
    assert((cita <=> ita) > 0);
    assert(ita != cita);
  }

  // 12.15 operator* and operator-> on const_iterator.
  {
    struct Pair { int first, second; };
    dq::array<Pair, 4> d;
    d.push_back({1, 10}); d.push_back({2, 20}); d.push_back({3, 30});
    const auto& cd = d;
    auto it = cd.begin();
    assert((*it).first == 1);
    assert(it++->second == 10);
    assert((*it).first == 2 && it->second == 20);
  }

  // 12.16 operator-> on a non-const iterator.
  {
    struct Node { int value; };
    dq::array<Node, 5> d;
    d.push_back({10}); d.push_back({20});
    auto it = d.begin();
    assert(it->value == 10);
    ++it;
    assert(it->value == 20);
  }

  // 12.17 Explicit bool conversion: true for non-end, false for end.
  {
    dq::array<int, 5> d = {1, 2, 3};
    auto it = d.begin(), end = d.end();
    while (it != end) { assert(static_cast<bool>(it)); ++it; }
    assert(!static_cast<bool>(end));
    const auto& cd = d;
    auto cit = cd.begin();
    assert(static_cast<bool>(cit));
    std::advance(cit, static_cast<int>(cd.size()));
    assert(!static_cast<bool>(cit));
  }

  // 12.18 random_access_iterator concept satisfaction.
  {
    using A = dq::array<int, 8>;
    static_assert(std::random_access_iterator<A::iterator>);
    static_assert(std::same_as<A::const_iterator::iterator_category,
                               std::random_access_iterator_tag>);
  }

  // 12.19 Const iterators from const container yield const_iterator type.
  {
    const dq::array<int, 5> cd = {10, 20, 30};
    static_assert(std::same_as<decltype(cd.begin()), dq::array<int,5>::const_iterator>);
    assert(std::accumulate(cd.begin(), cd.end(), 0) == 60);
  }

  // 12.20 Exact reverse-iteration copy matches forward range (non-wrapped).
  {
    dq::array<int, 6> d = {0, 1, 2, 3, 4, 5};
    std::vector<int> expected = {5, 4, 3, 2, 1, 0};
    std::vector<int> actual;
    std::copy(d.rbegin(), d.rend(), std::back_inserter(actual));
    assert(actual == expected);
  }

  // --- Wrapped-buffer iterator behaviour ---

  // 12.21 Iterator operator[] on a wrapped buffer.
  {
    dq::array<int, 4> circ;
    circ.push_back(1); circ.push_back(2);
    circ.push_back(3); circ.push_back(4);
    circ.pop_front(); circ.push_back(5); // logical: [2,3,4,5]
    auto cit = circ.begin();
    assert(cit[0]==2 && cit[1]==3 && cit[2]==4 && cit[3]==5);
  }

  // 12.22 Iterator three-way comparison across a wrap boundary.
  {
    dq::array<int, 4> circ;
    for (int v : {10, 20, 30, 40}) circ.push_back(v);
    circ.pop_front(); circ.push_back(50); // [20,30,40,50]
    auto ci = circ.begin(), cj = circ.begin() + 3;
    assert((ci <=> cj) < 0);
    assert((cj <=> ci) > 0);
    assert((ci <=> ci) == 0);
  }

  // 12.23 operator- across a wrap boundary.
  {
    dq::array<int, 4> circ;
    for (int v : {1, 2, 3, 4}) circ.push_back(v);
    circ.pop_front(); circ.push_back(5); // [2,3,4,5]
    auto cb = circ.begin(), ce = circ.end();
    assert(ce - cb ==  4);
    assert(cb - ce == -4);
    assert((cb + 1) - cb == 1);
  }

  // 12.24 std::distance with iterator and reverse iterator across wrap boundary.
  {
    dq::array<int, 5> circ;
    for (int v : {1,2,3,4,5}) circ.push_back(v);
    circ.pop_front(); circ.push_back(6); // wrap
    assert(std::distance(circ.begin(), circ.end()) == 5);
    assert(std::distance(circ.rbegin(), circ.rend()) == 5);
    assert(std::distance(circ.crbegin(), circ.crend()) == 5);
    auto mid = circ.begin() + 2;
    assert(*mid == 4);
    assert(mid - circ.begin() == 2);
    assert(circ.end() - mid == 3);
  }

  // 12.25 Reverse iteration on a wrapped buffer matches forward traversal.
  {
    dq::array<int, 4> circ = {1, 2, 3, 4};
    circ.pop_front(); circ.push_back(5);
    std::vector<int> fwd(circ.begin(), circ.end());
    std::vector<int> rev(circ.rbegin(), circ.rend());
    std::reverse(rev.begin(), rev.end());
    assert(fwd == rev);
  }

  // 12.26 Reverse-iterator arithmetic on a wrapped buffer.
  {
    dq::array<int, 5> circ;
    for (int v : {1,2,3,4,5}) circ.push_back(v);
    circ.pop_front(); circ.push_back(6); // [2,3,4,5,6], wrapped
    auto rb = circ.rbegin(), re = circ.rend();
    assert(*rb == 6);
    auto rm = rb + 3; assert(*rm == 3);
    auto rm2 = rm - 1; assert(*rm2 == 4);
    assert(re - rb == 5 && rb - re == -5);
    rb += 2; assert(*rb == 4);
    rb -= 1; assert(*rb == 5);
  }

  // 12.27 Reverse-iterator subscript operator[].
  {
    dq::array<int, 6> d = {0, 1, 2, 3, 4};
    auto r = d.rbegin();
    assert(r[0]==4 && r[1]==3 && r[4]==0);

    dq::array<int, 4> circ;
    for (int v : {1,2,3,4}) circ.push_back(v);
    circ.pop_front(); circ.push_back(5); // [2,3,4,5]
    auto cr = circ.rbegin();
    assert(cr[0]==5 && cr[1]==4 && cr[3]==2);
  }

  // 12.28 Bool conversion on a wrapped buffer.
  {
    dq::array<int, 3> circ;
    circ.push_back(1); circ.push_back(2); circ.push_back(3);
    circ.pop_front(); circ.push_back(4); // [2,3,4]
    int count = 0;
    for (auto it = circ.begin(); it; ++it) ++count;
    assert(count == 3);
    assert(!circ.end());
  }

  // 12.29 Reverse-iterator arithmetic and base() on a wrapped buffer.
  {
    dq::array<int, 5> wrap{1,2,3,4,5};
    wrap.pop_front(); wrap.push_back(6); // [2,3,4,5,6]
    auto r = wrap.rbegin();
    assert(*r == 6);
    r += 2; assert(*r == 4);
    r -= 1; assert(*r == 5);
    assert(std::distance(wrap.begin(), r.base() - 1) == 3);
  }
}

// ============================================================================
// §13 HELPER ALGORITHMS: find, find_if, count, count_if, erase, erase_if
// ============================================================================
{
  // 13.1  dq::find — basic hit and miss.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto it = dq::find(d, 3);
    assert(it && *it == 3);
    it = std::find(d.begin(), d.end(), 6);
    assert(it && *it == 6);
  }

  // 13.2  dq::find — on empty container.
  {
    dq::array<int, 5> empty;
    assert(dq::find(empty, 1) == empty.end());
    assert(!dq::find(empty, 1));
  }

  // 13.3  dq::find with multiple keys — returns first matching.
  {
    dq::array<int, 10> d = {10, 20, 30, 40, 50};
    auto it = dq::find(d, 30, 40);
    assert(it != d.end() && (*it == 30 || *it == 40));
    it = dq::find(d, 99, 100);
    assert(it == d.end() && !static_cast<bool>(it));
  }

  // 13.4  dq::find with heterogeneous string_view lookup.
  {
    dq::array<std::string, 5> d{"apple", "banana", "cherry", "date", "elderberry"};
    auto it = dq::find(d, std::string_view("cherry"));
    assert(it && *it == "cherry");
    it = dq::find(d, std::string_view("grape"));
    assert(!it);
  }

  // 13.5  dq::find_if — various predicates.
  {
    dq::array<int, 10> d = {1, 3, 5, 7, 8, 9};
    auto it = dq::find_if(d, [](int x){ return x % 2 == 0; });
    assert(it && *it == 8);
    it = dq::find_if(d, [](int x){ return x > 100; });
    assert(!it);
    const auto& cd = d;
    auto cit = dq::find_if(cd, [](int x){ return x == 5; });
    assert(cit && *cit == 5);
  }

  // 13.6  dq::find_if returns const_iterator from a const container.
  {
    const dq::array<int, 6> cd = {2, 4, 6, 8, 10};
    auto cit = dq::find_if(cd, [](int x){ return x == 2; });
    static_assert(std::same_as<decltype(cit), dq::array<int,6>::const_iterator>);
    assert(cit != cd.end() && *cit == 2);
    auto none = dq::find_if(cd, [](int x){ return x > 100; });
    assert(none == cd.end());
  }

  // 13.7  dq::find_if — first and last element matches.
  {
    dq::array<int, 5> d = {10, 20, 30};
    assert(dq::find_if(d, [](int x){ return x == 10; }) == d.begin());
    dq::array<int, 5> d2 = {1, 3, 5, 7, 9};
    auto it = dq::find_if(d2, [](int x){ return x == 9; });
    assert(it != d2.end() && *it == 9);
  }

  // 13.8  dq::find_if returns end() on no match (non-empty, wrapped).
  {
    dq::array<int, 6> circ;
    for (int v : {1,3,5,7,9,11}) circ.push_back(v);
    circ.push_back(13); // [3,5,7,9,11,13], all odd, wrapped
    auto it = dq::find_if(circ, [](int x){ return x % 2 == 0; });
    assert(it == circ.end() && !it);
  }

  // 13.9  dq::find_if on a wrapped buffer — finds element in second segment.
  {
    dq::array<int, 4> circ;
    for (int v : {10,20,30,40}) circ.push_back(v);
    circ.pop_front(); circ.push_back(50); // [20,30,40,50], wrapped
    auto it = dq::find_if(circ, [](int x){ return x == 50; });
    assert(it != circ.end() && *it == 50);
  }

  // 13.10 dq::count — basic.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    assert(dq::count(d, 1, 3, 10, 11) == 3);
  }

  // 13.11 dq::count on an empty container.
  {
    dq::array<int, 5> empty;
    assert(!dq::count(empty, 1));
  }

  // 13.12 dq::count with bool elements.
  {
    dq::array<bool, 8> d{true, false, true, false, true};
    assert(dq::count(d, true) == 3);
    assert(dq::count(d, false) == 2);
  }

  // 13.13 dq::count multi-key — zero when none present (wrapped buffer).
  {
    dq::array<int, 6> circ;
    for (int v : {1,3,5,7,9,11}) circ.push_back(v);
    circ.push_back(13); // [3,5,7,9,11,13], wrapped
    assert(dq::count(circ, 2, 4, 6) == 0);
  }

  // 13.14 dq::count multi-key — finds each value exactly once (wrapped).
  {
    dq::array<int, 6> circ;
    for (int v : {1,2,3,4,5,6}) circ.push_back(v);
    circ.push_back(7); // [2,3,4,5,6,7], wrapped
    assert(dq::count(circ, 3, 6, 99) == 2);
  }

  // 13.15 dq::count_if — all match.
  {
    dq::array<int, 6> d = {2, 4, 6, 8, 10};
    assert(dq::count_if(d, [](int x){ return x % 2 == 0; }) == 5);
  }

  // 13.16 dq::count_if — none match.
  {
    dq::array<int, 6> d = {1, 3, 5, 7};
    assert(dq::count_if(d, [](int x){ return x % 2 == 0; }) == 0);
  }

  // 13.17 dq::count_if on an empty container.
  {
    dq::array<int, 4> empty;
    assert(dq::count_if(empty, [](int){ return true; }) == 0);
  }

  // 13.18 dq::count_if returns correct count on a const container.
  {
    const dq::array<int, 8> cd = {1, 2, 3, 4, 5, 6};
    assert(dq::count_if(cd, [](int x){ return x > 3; }) == 3);
  }

  // 13.19 dq::count_if across a wrap boundary.
  {
    dq::array<int, 6> circ;
    for (int v : {1,2,3,4,5,6}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(7); circ.push_back(8); // [3,4,5,6,7,8], wrapped
    auto odd = dq::count_if(circ, [](int x){ return x % 2 != 0; });
    auto evn = dq::count_if(circ, [](int x){ return x % 2 == 0; });
    assert(odd == 3 && evn == 3);
  }

  // 13.20 dq::count_if with complex predicate on wrapped buffer.
  {
    dq::array<int, 6> wrap{10,20,30,40,50,60};
    wrap.pop_front(); wrap.push_back(70); // [20..70], wrapped
    auto cnt = dq::count_if(wrap, [](int x){ return x >= 40; });
    assert(cnt == 4);
  }

  // 13.21 dq::erase — removes all occurrences of a value.
  {
    dq::array<int, 8> d = {1, 2, 3, 2, 4};
    auto removed = dq::erase(d, 2);
    assert(removed == 2);
    assert(d.size() == 3);
    assert((d == std::array{1, 3, 4}));
  }

  // 13.22 dq::erase — value not present returns zero.
  {
    dq::array<int, 6> d = {1, 2, 3};
    assert(dq::erase(d, 99) == 0 && d.size() == 3);
  }

  // 13.23 dq::erase — all elements by value.
  {
    dq::array<int, 5> d = {7, 7, 7, 7};
    assert(dq::erase(d, 7) == 4 && d.empty());
  }

  // 13.24 dq::erase — multiple keys in a single call.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5, 6};
    auto removed = dq::erase(d, 2, 4, 6);
    assert(removed == 3);
    assert((d == std::array{1, 3, 5}));
  }

  // 13.25 dq::erase with multiple keys on a fully wrapped buffer.
  {
    dq::array<int, 6> wrap;
    for (int i = 0; i < 6; ++i) wrap.push_back(i);
    wrap.pop_front(); wrap.push_back(6); // [1,2,3,4,5,6], wrapped
    auto removed = dq::erase(wrap, 2, 5, 99);
    assert(removed == 2);
    assert(wrap.size() == 4);
    assert((wrap == std::array{1,3,4,6}));
  }

  // 13.26 dq::erase_if — removes matching elements and returns count.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    assert(dq::erase_if(d, [](int n){ return n % 2 == 0; }) == 2);
    for (int n : d) assert(n % 2 != 0);
  }

  // 13.27 dq::erase_if — returns zero on empty container.
  {
    dq::array<int, 5> empty;
    assert(dq::erase_if(empty, [](int){ return true; }) == 0);
  }

  // 13.28 dq::erase_if — returns zero when no element matches.
  {
    dq::array<int, 5> d = {1, 3, 5};
    assert(dq::erase_if(d, [](int x){ return x % 2 == 0; }) == 0 && d.size() == 3);
  }

  // 13.29 dq::erase_if — removes by count and leaves sorted remainder.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto n = dq::erase_if(d, [](int x){ return x % 3 == 0; });
    assert(n == 3 && d.size() == 7);
    for (int x : d) assert(x % 3 != 0);
  }

  // 13.30 dq::erase_if with a lambda capture.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5, 6};
    int threshold = 4;
    dq::erase_if(d, [threshold](int x){ return x > threshold; });
    assert((d == std::array{1,2,3,4}));
  }

  // 13.31 dq::erase_if on a wrapped buffer.
  {
    dq::array<int, 6> circ;
    for (int v : {1,2,3,4,5,6}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(7); circ.push_back(8); // [3,4,5,6,7,8], wrapped
    auto n = dq::erase_if(circ, [](int x){ return x % 2 == 0; });
    assert(n == 3 && circ.size() == 3);
    for (int x : circ) assert(x % 2 != 0);
  }

  // 13.32 dq::erase_if on a wrapped buffer with complex (std::string) type.
  {
    dq::array<std::string, 4> d;
    d.push_back("hello"); d.push_back("world");
    d.push_back("foo");   d.push_back("bar");
    d.pop_front(); d.push_back("baz"); // [world,foo,bar,baz], wrapped
    auto removed = dq::erase_if(d, [](const std::string& s){
      return s == "foo" || s == "baz";
    });
    assert(removed == 2 && d.size() == 2);
    assert(d[0] == "world" && d[1] == "bar");
  }

  // 13.33 dq::erase_if on a fully wrapped buffer.
  {
    dq::array<int, 8> wrap;
    for (int i = 0; i < 8; ++i) wrap.push_back(i);
    wrap.pop_front(); wrap.push_back(8); // [1..8], wrapped
    auto removed = dq::erase_if(wrap, [](int x){ return x % 3 == 0; });
    assert(removed == 2); // 3 and 6
    assert(wrap.size() == 6);
    assert((wrap == std::array{1,2,4,5,7,8}));
  }
}

// ============================================================================
// §14 SPLIT, CSPLIT, AND COPY
//     Covers the split() / csplit() segment layout, and dq::copy to a raw
//     buffer, including on wrapped buffers.
// ============================================================================
{
  // 14.1  split() on an empty container — both segments are empty.
  {
    dq::array<int, 5> empty;
    auto segs = empty.split();
    assert(segs[0][0] == segs[0][1]);
    assert(segs[1][0] == segs[1][1]);
    int sum = 0;
    for (auto [i, j] : empty.csplit())
      for (auto k = i; k != j; ++k) sum += *k;
    assert(sum == 0);
  }

  // 14.2  split() on a contiguous (unwrapped) buffer.
  {
    dq::array<int, 6> d = {1, 2, 3, 4};
    auto segs = d.split();
    assert(segs[0][1] - segs[0][0] == 4);
    assert(segs[1][0] == segs[1][1]);
  }

  // 14.3  split() on a wrapped buffer — two non-empty segments.
  {
    dq::array<int, 4> circ;
    circ.push_back(1); circ.push_back(2);
    circ.push_back(3); circ.push_back(4);
    circ.pop_front(); circ.push_back(5); // logical [2,3,4,5]
    auto s = circ.split();
    std::size_t total = (s[0][1]-s[0][0]) + (s[1][1]-s[1][0]);
    assert(total == 4);
  }

  // 14.4  csplit() reconstruction matches logical order on a wrapped buffer.
  {
    dq::array<int, 4> circ;
    circ.push_back(1); circ.push_back(2);
    circ.push_back(3); circ.push_back(4);
    circ.pop_front(); circ.push_back(5);
    int rebuilt[4]{};
    int* p = rebuilt;
    for (auto [i, j] : circ.csplit())
      for (auto k = i; k != j; ++k) *p++ = *k;
    assert(rebuilt[0]==2 && rebuilt[1]==3 && rebuilt[2]==4 && rebuilt[3]==5);
  }

  // 14.5  Wrap condition detection via split() segment counts.
  {
    dq::array<int, 4> d = {1, 2, 3, 4};
    {
      auto segs = d.split();
      assert(segs[0][1] - segs[0][0] == 4); // all in first segment
      assert(segs[1][0] == segs[1][1]);
    }
    d.pop_front(); d.push_back(5);
    d.pop_front(); d.push_back(6); // two wrap cycles
    assert((d == std::array{3, 4, 5, 6}));
    auto segs = d.split();
    auto total = (segs[0][1]-segs[0][0]) + (segs[1][1]-segs[1][0]);
    assert(total == 4);
    assert(segs[1][1] - segs[1][0] > 0); // second segment non-empty
  }

  // 14.6  dq::copy on a contiguous buffer.
  {
    dq::array<int, 5> d = {10, 20, 30, 40, 50};
    int buf[5]{};
    dq::copy(d, buf);
    assert(d == buf);
  }

  // 14.7  dq::copy with size limit.
  {
    dq::array<int, 5> d = {10, 20, 30, 40, 50};
    int buf2[3]{};
    dq::copy(d, buf2, 3);
    assert(buf2[0]==10 && buf2[1]==20 && buf2[2]==30);
  }

  // 14.8  dq::copy on a wrapped buffer.
  {
    dq::array<int, 4> circ;
    circ.push_back(10); circ.push_back(20);
    circ.push_back(30); circ.push_back(40);
    circ.pop_front(); circ.push_back(50);
    int out[4]{};
    dq::copy(circ, out);
    assert(out[0]==20 && out[1]==30 && out[2]==40 && out[3]==50);
  }

  // 14.9  dq::copy with size limit on a wrapped buffer.
  {
    dq::array<int, 5> circ;
    for (int i = 0; i < 5; ++i) circ.push_back(i);
    circ.pop_front(); circ.push_back(5); // [1,2,3,4,5]
    int small[3];
    dq::copy(circ, small, 3);
    assert(small[0]==1 && small[1]==2 && small[2]==3);
    int large[10]{};
    dq::copy(circ, large, 10);
    assert(large[0]==1 && large[4]==5);
  }

  // 14.10 dq::copy with empty source — must not write.
  {
    dq::array<int, 5> empty;
    int buf[1] = {99};
    dq::copy(empty, buf, 0);
    assert(buf[0] == 99);
  }

  // 14.11 dq::copy on a wrapped buffer (six-element).
  {
    dq::array<int, 6> src;
    for (int i = 0; i < 6; ++i) src.push_back(i * 10); // [0,10,20,30,40,50]
    src.pop_front(); src.push_back(70); // [10,20,30,40,50,70]
    int dest[6]{};
    dq::copy(src, dest, 6);
    assert((src == std::array{10,20,30,40,50,70}));
    assert(dest == src);
  }
}

// ============================================================================
// §15 APPEND (from raw pointer region)
// ============================================================================
{
  // 15.1  append adds elements from a raw pointer range.
  {
    int src[] = {4, 5, 6};
    dq::array<int, 10> d = {1, 2, 3};
    auto added = d.append(src, 3);
    assert(added == 3 && d.size() == 6);
    assert((d == std::array{1, 2, 3, 4, 5, 6}));
  }

  // 15.2  append is clamped by available capacity.
  {
    int big[20]{};
    dq::array<int, 4> small = {1, 2};
    auto capped = small.append(big, 20);
    assert(capped == 2 && small.size() == 4 && small.full());
  }

  // 15.3  append returns zero when the container is already full.
  {
    dq::array<int, 3> full{1, 2, 3};
    assert(full.full());
    int extra[] = {4, 5};
    assert(full.append(extra, 2) == 0);
    assert((full == std::array{1, 2, 3}));
  }

  // 15.4  append capacity clamping: only fills remaining slots.
  {
    dq::array<int, 5> d{1, 2, 3}; // size=3, space=2
    int overflow[] = {4, 5, 6, 7, 8};
    auto added = d.append(overflow, 5);
    assert(added == 2 && d.size() == 5 && d.full());
    assert((d == std::array{1, 2, 3, 4, 5}));
  }
}

// ============================================================================
// §16 SORT AND STABLE_SORT
//     Covers member sort() and stable_sort() with default and custom
//     comparators, on contiguous and wrapped buffers, including sub-range
//     and edge-case variants.
// ============================================================================
{
  // 16.1  sort() — ascending.
  {
    dq::array<int, 10> d = {5, 3, 8, 1, 9, 2};
    d.sort(d.begin(), d.end());
    assert(std::is_sorted(d.begin(), d.end()));
  }

  // 16.2  sort() — descending comparator.
  {
    dq::array<int, 10> d = {5, 3, 8, 1, 9, 2};
    d.sort(d.begin(), d.end(), std::greater<int>{});
    assert(std::is_sorted(d.begin(), d.end(), std::greater<int>{}));
  }

  // 16.3  sort() — sub-range only; extremes are untouched.
  {
    dq::array<int, 10> d = {5, 3, 8, 1, 9, 2};
    d.sort(d.begin() + 1, d.begin() + 5);
    assert(std::is_sorted(d.begin() + 1, d.begin() + 5));
    assert(d[0] == 5 && d[5] == 2);
  }

  // 16.4  sort() — edge cases: empty, single, all-equal.
  {
    dq::array<int, 5> empty;
    empty.sort(empty.begin(), empty.end());
    assert(empty.empty());

    dq::array<int, 5> single = {42};
    single.sort(single.begin(), single.end());
    assert(single.size() == 1 && single[0] == 42);

    dq::array<int, 5> equal = {5, 5, 5, 5};
    equal.sort(equal.begin(), equal.end());
    assert(std::all_of(equal.begin(), equal.end(), [](int x){ return x==5; }));
  }

  // 16.5  sort() on a wrapped buffer.
  {
    dq::array<int, 6> circ;
    for (int v : {5,3,1,4,2,6}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(9); circ.push_back(7);
    circ.sort(circ.begin(), circ.end());
    assert(std::is_sorted(circ.begin(), circ.end()));
  }

  // 16.6  sort() on a sub-range of a wrapped buffer; extremes untouched.
  {
    dq::array<int, 8> circ;
    for (int v : {5,3,9,1,7,4,2,8}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(6); circ.push_back(0);
    auto lo = circ.begin() + 1, hi = circ.begin() + 5;
    circ.sort(lo, hi);
    assert(std::is_sorted(circ.begin() + 1, circ.begin() + 5));
    assert(*circ.begin() == 9);
    assert(*(circ.begin() + 5) == 8);
  }

  // 16.7  sort() with descending comparator across a wrap boundary.
  {
    dq::array<int, 8> circ;
    for (int v : {4,2,7,1,5,3,6}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(9); circ.push_back(8);
    circ.sort(circ.begin(), circ.end(), std::greater<int>{});
    assert(std::is_sorted(circ.begin(), circ.end(), std::greater<int>{}));
  }

  // 16.8  sort() on a custom struct (key-based comparator).
  {
    struct Item { int key, val; };
    dq::array<Item, 10> d;
    d.push_back({3,30}); d.push_back({1,10});
    d.push_back({4,40}); d.push_back({2,20});
    d.sort(d.begin(), d.end(),
      [](Item const& a, Item const& b){ return a.key < b.key; });
    assert(d[0].key==1 && d[1].key==2 && d[2].key==3 && d[3].key==4);
  }

  // 16.9  sort() triggers std::inplace_merge across wrap boundary.
  {
    dq::array<int, 6> d;
    d.push_back(10); d.push_back(20); d.push_back(5);
    d.push_back(6);  d.push_back(7);  d.push_back(8);
    d.pop_front(); d.push_back(30);
    d.sort(d.begin(), d.end());
    assert(std::is_sorted(d.begin(), d.end()));
    assert(d.front() == 5 && d.back() == 30);
  }

  // 16.10 sort() on std::string elements.
  {
    dq::array<std::string, 5> d;
    d.push_back("cat"); d.push_back("dog"); d.push_back("fox");
    d.push_back("owl"); d.push_back("ant"); // full
    d.push_back("bee"); // overwrites "cat" → [dog,fox,owl,ant,bee]
    assert(d.front() == "dog" && d.back() == "bee");
    d.sort(d.begin(), d.end());
    assert(std::is_sorted(d.begin(), d.end()));
    assert(d.front() == "ant" && d.back() == "owl");
  }

  // 16.11 stable_sort() — ascending; relative order within equal keys preserved.
  {
    struct Item { int key, idx; };
    dq::array<Item, 10> d;
    d.push_back({2,0}); d.push_back({1,1});
    d.push_back({2,2}); d.push_back({1,3});
    d.push_back({3,4});
    d.stable_sort(d.begin(), d.end(),
      [](Item const& a, Item const& b){ return a.key < b.key; });
    assert(std::is_sorted(d.begin(), d.end(),
      [](Item const& a, Item const& b){ return a.key < b.key; }));
    assert(d[0].idx==1 && d[1].idx==3);
    assert(d[2].idx==0 && d[3].idx==2);
  }

  // 16.12 stable_sort() on a wrapped buffer.
  {
    dq::array<int, 6> circ;
    for (int v : {9,3,7,1,5,4}) circ.push_back(v);
    circ.pop_front(); circ.pop_front();
    circ.push_back(2); circ.push_back(8);
    circ.stable_sort(circ.begin(), circ.end());
    assert(std::is_sorted(circ.begin(), circ.end()));
    assert((circ == std::array{1,2,4,5,7,8}));
  }

  // 16.13 stable_sort() with descending comparator.
  {
    dq::array<int, 8> d = {3,1,4,1,5,9,2,6};
    d.stable_sort(d.begin(), d.end(), std::greater<int>{});
    assert(std::is_sorted(d.begin(), d.end(), std::greater<int>{}));
  }

  // 16.14 stable_sort() preserves order for all-equal keys.
  {
    struct Track { int val, seq; };
    dq::array<Track, 10> d;
    for (int i = 0; i < 5; ++i) d.push_back({1, i});
    d.stable_sort(d.begin(), d.end(),
      [](Track const& a, Track const& b){ return a.val < b.val; });
    for (int i = 0; i < 5; ++i) assert(d[i].seq == i);
  }

  // 16.15 stable_sort() on all-equal-key container preserves insertion order.
  {
    struct Item { int key, id; };
    dq::array<Item, 8> d;
    for (int i = 0; i < 8; ++i) d.push_back({42, i});
    d.stable_sort(d.begin(), d.end(),
      [](Item const& a, Item const& b){ return a.key < b.key; });
    for (int i = 0; i < 8; ++i) assert(d[i].id == i);
  }

  // 16.16 stable_sort() on a sub-range of a wrapped buffer preserves stability.
  {
    struct S { int key, seq; };
    dq::array<S, 8> circ;
    int idx = 0;
    for (int k : {4,2,4,1,3,2,3,1}) circ.push_back({k, idx++});
    circ.pop_front(); circ.pop_front();
    circ.push_back({5, idx++}); circ.push_back({0, idx++});
    circ.stable_sort(circ.begin(), circ.end(),
      [](S const& a, S const& b){ return a.key < b.key; });
    assert(std::is_sorted(circ.begin(), circ.end(),
      [](S const& a, S const& b){ return a.key < b.key; }));
    for (std::size_t i = 1; i < circ.size(); ++i)
      if (circ[i].key == circ[i-1].key)
        assert(circ[i].seq > circ[i-1].seq);
  }

  // 16.17 stable_sort() correctness on a heavily wrapped buffer.
  {
    dq::array<int, 7> wrap;
    for (int i = 0; i < 7; ++i) wrap.push_back(i);
    wrap.pop_front(); wrap.pop_front();
    wrap.push_back(7); wrap.push_back(8); // [2..8], wrapped
    wrap.stable_sort(wrap.begin(), wrap.end());
    assert(std::is_sorted(wrap.begin(), wrap.end()));
    assert(wrap.front() == 2 && wrap.back() == 8);
  }
}

// ============================================================================
// §17 STD ALGORITHM AND RANGES INTEGRATION
//     Covers std:: and std::ranges algorithms used with dq::array iterators.
// ============================================================================
{
  // 17.1  std::ranges concepts satisfied.
  {
    using A = dq::array<int, 8>;
    static_assert(std::ranges::range<A>);
    static_assert(std::ranges::sized_range<A>);
    static_assert(std::ranges::random_access_range<A>);
    static_assert(std::ranges::common_range<A>);
    static_assert(!std::ranges::contiguous_range<A>);
    A d = {5, 3, 1, 4, 2};
    assert(std::ranges::distance(d) == 5);
    assert(*std::ranges::min_element(d) == 1);
    assert(*std::ranges::max_element(d) == 5);
  }

  // 17.2  C++20 ranges customisation points.
  {
    dq::array<int, 5> d = {10, 20, 30};
    assert(std::ranges::data(d) == d.data());
    assert(std::ranges::size(d) == d.size());
    assert(std::ranges::empty(d) == d.empty());
    assert(*std::ranges::begin(d) == 10);
    assert(std::ranges::distance(d) == 3);
  }

  // 17.3  std::ranges::sort and is_sorted.
  {
    dq::array<int, 20> d = {3, 1, 4, 1, 5};
    std::ranges::sort(d);
    assert(std::ranges::is_sorted(d));
    assert(std::ranges::equal(d, std::array{1, 1, 3, 4, 5}));
    assert(std::ranges::find(d, 3) != d.end());
    assert(std::ranges::count(d, 1) == 2);
  }

  // 17.4  std::ranges::sort with projection.
  {
    struct Point { int x, y; };
    dq::array<Point, 10> d = {{5,6}, {1,2}, {3,4}};
    std::ranges::sort(d, std::less<>{}, &Point::y);
    assert(d[0].y == 2 && d[2].y == 6);
  }

  // 17.5  std::ranges::copy_if into a back_inserter.
  {
    dq::array<int, 10> src = {1, 2, 3, 4, 5, 6};
    dq::array<int, 10> dst;
    std::ranges::copy_if(src, std::back_inserter(dst), [](int x){ return x % 2 == 0; });
    assert(dst.size() == 3);
    assert((dst == std::array{2, 4, 6}));
  }

  // 17.6  std::ranges::transform into another dq::array.
  {
    dq::array<int, 6> src = {1, 2, 3, 4, 5};
    dq::array<int, 6> dst(src.size());
    std::ranges::transform(src, dst.begin(), [](int x){ return x * x; });
    assert((dst == std::array{1, 4, 9, 16, 25}));
  }

  // 17.7  std::merge and std::reverse integration.
  {
    dq::array<int, 10> a{1, 2, 3, 4, 5}, b{6, 7, 8, 9, 10};
    dq::array<int, 10> merged(10);
    std::merge(a.begin(), a.end(), b.begin(), b.end(), merged.begin());
    assert((merged == dq::array<int,10>{1,2,3,4,5,6,7,8,9,10}));
    std::reverse(merged.begin(), merged.end());
    assert((merged == std::array{10,9,8,7,6,5,4,3,2,1}));
  }

  // 17.8  std::rotate and reverse-rotate round-trip.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    std::rotate(d.begin(), d.begin() + 2, d.end());
    assert((d == std::array{3, 4, 5, 1, 2}));
    std::rotate(d.rbegin(), d.rbegin() + 2, d.rend());
    assert((d == std::array{1, 2, 3, 4, 5}));
  }

  // 17.9  Shuffle, rotate, reverse, sort round-trip.
  {
    dq::array<int, 10> original{1, 2, 3, 4, 5};
    auto d{original};
    std::shuffle(d.begin(), d.end(), std::mt19937{std::random_device{}()});
    std::rotate(d.begin(), d.begin() + 1 + std::rand() % (d.size() - 1), d.end());
    auto mid = d.begin() + d.size() / 2;
    std::reverse(mid, d.end());
    std::sort(d.begin(), d.end());
    assert(std::equal(d.begin(), d.end(), original.begin()));
  }

  // 17.10 std::unique integration.
  {
    dq::array<int, 20> d{1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
    d.erase(std::unique(d.begin(), d.end()), d.end());
    assert((d == std::array{1, 2, 3, 4}));
  }

  // 17.11 std::remove_if and erase-remove idiom.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    d.erase(std::remove_if(d.begin(), d.end(), [](int n){ return n % 2 == 0; }),
            d.end());
    for (int n : d) assert(n % 2 != 0);
  }

  // 17.12 std::partition.
  {
    dq::array<int, 10> d = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    auto mid = std::partition(d.begin(), d.end(), [](int x){ return x < 5; });
    assert(std::all_of(d.begin(), mid,  [](int x){ return x <  5; }));
    assert(std::all_of(mid, d.end(),    [](int x){ return x >= 5; }));
  }

  // 17.13 std::nth_element.
  {
    dq::array<int, 8> d = {7, 2, 5, 4, 1, 8, 3, 6};
    auto mid = d.begin() + 3;
    std::nth_element(d.begin(), mid, d.end());
    assert(*mid == 4);
    assert(std::all_of(d.begin(), mid,   [](int x){ return x <= 4; }));
    assert(std::all_of(mid + 1, d.end(), [](int x){ return x >= 4; }));
  }

  // 17.14 std::lower_bound and upper_bound.
  {
    dq::array<int, 10> d = {1, 3, 5, 7, 9, 11};
    auto lo = std::lower_bound(d.begin(), d.end(), 5);
    assert(*lo == 5 && std::distance(d.begin(), lo) == 2);
    auto hi = std::upper_bound(d.begin(), d.end(), 5);
    assert(*hi == 7 && std::distance(d.begin(), hi) == 3);
    auto it = std::lower_bound(d.begin(), d.end(), 6);
    assert(*it == 7);
    it = std::lower_bound(d.begin(), d.end(), 99);
    assert(it == d.end());
  }

  // 17.15 std::binary_search.
  {
    dq::array<int, 10> d = {2, 4, 6, 8, 10};
    assert(std::binary_search(d.begin(), d.end(), 4));
    assert(std::binary_search(d.begin(), d.end(), 10));
    assert(!std::binary_search(d.begin(), d.end(), 5));
    assert(!std::binary_search(d.begin(), d.end(), 0));
  }

  // 17.16 std::equal_range on a sorted buffer with duplicates.
  {
    dq::array<int, 10> d = {1, 2, 2, 2, 3, 4};
    auto [lo, hi] = std::equal_range(d.begin(), d.end(), 2);
    assert(std::distance(lo, hi) == 3 && *lo == 2 && *hi == 3);
  }

  // 17.17 std::accumulate / std::inner_product / std::transform_reduce.
  {
    dq::array<int, 5> a = {1, 2, 3};
    dq::array<int, 5> b = {4, 5, 6};
    assert(std::inner_product(a.begin(), a.end(), b.begin(), 0) == 32);

    dq::array<int, 8> x = {1, 2, 3, 4};
    dq::array<int, 8> y = {10, 20, 30, 40};
    int dot = std::transform_reduce(x.begin(), x.end(), y.begin(), 0);
    assert(dot == 1*10 + 2*20 + 3*30 + 4*40);
  }

  // 17.18 std::for_each accumulates even and odd sums.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5, 6, 7, 8};
    int even_sum = 0, odd_sum = 0;
    std::for_each(d.begin(), d.end(), [&](int x){
      if (x % 2 == 0) even_sum += x; else odd_sum += x;
    });
    assert(even_sum == 20 && odd_sum == 16);
  }

  // 17.19 std::reduce with unsequenced execution policy.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    int sum = std::reduce(std::execution::unseq, d.begin(), d.end(), 0);
    assert(sum == 15);
  }

  // 17.20 std::min_element / max_element on a wrapped buffer.
  {
    dq::array<int, 6> circ;
    for (int v : {5,1,9,3,7,2}) circ.push_back(v);
    circ.pop_front(); circ.push_back(8); // [1,9,3,7,2,8], wrapped
    assert(*std::min_element(circ.begin(), circ.end()) == 1);
    assert(*std::max_element(circ.begin(), circ.end()) == 9);
    assert(*std::ranges::min_element(circ) == 1);
    assert(*std::ranges::max_element(circ) == 9);
  }

  // 17.21 std::transform into another dq via resize + transform.
  {
    dq::array<int, 10> src = {1, 2, 3, 4, 5, 6};
    dq::array<int, 10> dst;
    dst.resize(src.size());
    std::transform(src.begin(), src.end(), dst.begin(), [](int x){ return x * x; });
    assert((dst == std::array{1, 4, 9, 16, 25, 36}));

    std::vector<int> evens;
    std::copy_if(src.begin(), src.end(), std::back_inserter(evens),
                 [](int x){ return x % 2 == 0; });
    assert((evens == std::vector{2, 4, 6}));
  }

  // 17.22 Heterogeneous lookup via std::ranges::find with std::string_view.
  {
    dq::array<std::string, 10> d = {"apple", "banana", "pear"};
    assert(std::ranges::find(d, std::string_view("banana")) != d.end());
    assert(std::ranges::find(d, std::string_view("grape"))  == d.end());
  }
}

// ============================================================================
// §18 MOVE-ONLY TYPES
// ============================================================================
{
  // 18.1  push_back and emplace_front with unique_ptr.
  {
    dq::array<std::unique_ptr<int>, 10> d;
    d.emplace_back(std::make_unique<int>(7));
    d.emplace_front(std::make_unique<int>(9));
    assert(*d.front() == 9 && *d.back() == 7);
    d.erase(d.begin());
    assert(d.size() == 1 && *d.front() == 7);
  }

  // 18.2  Move from a slot — slot becomes null.
  {
    dq::array<std::unique_ptr<int>, 3> d;
    d.push_back(std::make_unique<int>(1));
    d.push_back(std::make_unique<int>(2));
    d.push_front(std::make_unique<int>(3));
    assert(*d[0]==3 && *d[1]==1 && *d[2]==2);
    auto p = std::move(d[1]);
    assert(*p == 1 && !d[1]);
    d.pop_back(); assert(d.size() == 2);
    d.pop_back(); assert(d.size() == 1);
    assert(*d[0] == 3);
  }

  // 18.3  Move semantics via std::move_iterator — insert.
  {
    std::vector<std::string> src = {"A", "B", "C"};
    dq::array<std::string, 10> dst;
    dst.assign(std::make_move_iterator(src.begin()),
               std::make_move_iterator(src.end()));
    assert(dst.size()==3 && dst[0]=="A" && dst[2]=="C");
    assert(src[0].empty() && src[1].empty() && src[2].empty());

    std::vector<std::string> src2 = {"X", "Y"};
    dst.insert(dst.begin(), std::make_move_iterator(src2.begin()),
                             std::make_move_iterator(src2.end()));
    assert(dst.size()==5 && dst[0]=="X" && dst[1]=="Y");
    assert(src2[0].empty());
  }
}

// ============================================================================
// §19 NON-TRIVIAL / DESTRUCTOR CORRECTNESS
// ============================================================================
{
  // 19.1  Destructor is called the right number of times.
  {
    static int count;
    count = 0;
    struct Counter {
      int x = 0;
      Counter()              { ++count; }
      Counter(int x_) : x(x_) { ++count; }
      ~Counter()             { --count; }
      Counter(const Counter& o) : x(o.x) { ++count; }
      Counter(Counter&& o)  : x(o.x)    { ++count; }
      Counter& operator=(const Counter&) = default;
      Counter& operator=(Counter&&)      = default;
    };
    {
      dq::array<Counter, 10> d;
      d.push_back(1);
      d.push_back(2);
      assert(count == 11); // 9 default + 2 via push_back (move from temp)
      d.pop_back();
      assert(count == 11); // pop_back does not call dtor on array slot
      d.clear();
      assert(count == 11); // clear resets logical pointers, doesn't destruct members
    }
    assert(count == 0); // d goes out of scope: array<Counter,10>[N] destructs
  }

  // 19.2  String elements correctly destruct after cyclic overwrites.
  {
    dq::array<std::string, 5> d;
    d.push_back("hello"); d.push_back("world"); d.push_front("start");
    assert(d.size()==3 && d[0]=="start" && d[1]=="hello" && d[2]=="world");
    d.pop_front();
    assert(d.front() == "hello");
    d = {"a","b","c","d","e"};
    assert(d.full());
    d.push_back("f");
    assert(d.size()==5 && d[0]=="b" && d[4]=="f");
    d = {"a","b","c","d","e"};
    d.push_front("f");
    assert(d.size()==5 && d[0]=="f");
  }
}

// ============================================================================
// §20 THREADING (single-producer / single-consumer)
// ============================================================================
{
  dq::array<int, 10> buffer;
  std::atomic<bool> done{};
  std::mutex m;
  int consumed{};

  std::thread producer([&]{
    for (int i = 0; i < 1000;) {
      if (std::lock_guard l(m); !buffer.full())
        buffer.push_back(i++);
    }
    done = true;
  });

  std::thread consumer([&]{
    for (;;) {
      if (std::lock_guard l(m); !buffer.empty())
        consumed += buffer.front(), buffer.pop_front();
      else if (done) break;
    }
  });

  producer.join();
  consumer.join();
  assert(consumed == 999*1000/2);
}

// ============================================================================
// §21 STRESS TESTS
// ============================================================================
{
  // 21.1  Randomised push / pop stress — size invariant holds.
  {
    constexpr int N = 1'000;
    dq::array<int, N> d;
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> op(0, 3);
    for (int i = 0; i < 10 * N; ++i) {
      switch (op(rng)) {
        case 0: d.push_back(i); break;
        case 1: d.push_front(i); break;
        case 2: if (!d.empty()) d.pop_back(); break;
        case 3: if (!d.empty()) d.pop_front(); break;
      }
      assert(d.size() <= N);
    }
  }

  // 21.2  Mixed ops stress with element-value verification vs std::deque.
  {
    constexpr int CAP = 16;
    dq::array<int, CAP> d;
    std::mt19937 rng{9999};
    std::uniform_int_distribution<int> opd(0, 5);
    std::deque<int> ref;
    int val = 0;
    for (int iter = 0; iter < 2000; ++iter) {
      switch (opd(rng)) {
        case 0: if (!d.full()) { d.push_back(val); ref.push_back(val++); } break;
        case 1: if (!d.full()) { d.push_front(val); ref.push_front(val++); } break;
        case 2: if (!d.empty()) { d.pop_back();  ref.pop_back(); } break;
        case 3: if (!d.empty()) { d.pop_front(); ref.pop_front(); } break;
        case 4: if (!d.empty()) {
          std::size_t idx = std::uniform_int_distribution<std::size_t>(0, d.size()-1)(rng);
          d.erase(d.begin() + static_cast<std::ptrdiff_t>(idx));
          ref.erase(ref.begin() + static_cast<std::ptrdiff_t>(idx));
        } break;
        case 5: if (!d.full()) {
          std::size_t idx = std::uniform_int_distribution<std::size_t>(0, d.size())(rng);
          d.insert(d.begin() + static_cast<std::ptrdiff_t>(idx), val);
          ref.insert(ref.begin() + static_cast<std::ptrdiff_t>(idx), val++);
        } break;
      }
      assert(d.size() == ref.size() && d.size() <= std::size_t(CAP));
      for (std::size_t i = 0; i < d.size(); ++i) assert(d[i] == ref[i]);
    }
  }

  // 21.3  Deterministic cycle + sort + erase_if on a large container.
  {
    dq::array<int, 200> d;
    for (int i = 0; i < 200; ++i) d.push_back(200 - i);
    d.sort(d.begin(), d.end());
    assert(std::is_sorted(d.begin(), d.end()));
    auto removed = dq::erase_if(d, [](int x){ return x % 3 == 0; });
    assert(removed > 0);
    assert(std::is_sorted(d.begin(), d.end()));
    for (int x : d) assert(x % 3 != 0);
  }

  // 21.4  NEW allocator — push stress to full capacity.
  {
    dq::array<int, 100, dq::NEW> a;
    for (int i = 0; i < 100; ++i) a.push_back(i);
    assert(a.full() && a.size() == 100);
    for (std::size_t i = 0; i < 100; ++i) assert(a[i] == static_cast<int>(i));
  }
}

// ============================================================================
// §22 NICESORT — PUBLIC INTERFACE
//     Tests nicesort::sort and nicesort::insertion_sort as standalone
//     functions (independent of dq::array).
// ============================================================================
{
  // 22.1  Empty range — must not crash.
  {
    std::vector<int> v;
    nicesort::sort(v.begin(), v.end());
    assert(v.empty());
  }

  // 22.2  Single element — no-op.
  {
    std::vector<int> v{42};
    nicesort::sort(v.begin(), v.end());
    assert(v[0] == 42);
  }

  // 22.3  Two elements, already ordered.
  {
    std::vector<int> v{1, 2};
    nicesort::sort(v.begin(), v.end());
    assert((v == std::vector{1, 2}));
  }

  // 22.4  Two elements, reversed.
  {
    std::vector<int> v{2, 1};
    nicesort::sort(v.begin(), v.end());
    assert((v == std::vector{1, 2}));
  }

  // 22.5  Already sorted range — remains sorted.
  {
    std::vector<int> v(64);
    std::iota(v.begin(), v.end(), 0);
    nicesort::sort(v.begin(), v.end());
    assert(std::is_sorted(v.begin(), v.end()));
    assert(v[0] == 0 && v[63] == 63);
  }

  // 22.6  Reverse-sorted range.
  {
    std::vector<int> v(64);
    std::iota(v.begin(), v.end(), 0);
    std::reverse(v.begin(), v.end());
    nicesort::sort(v.begin(), v.end());
    assert(std::is_sorted(v.begin(), v.end()));
  }

  // 22.7  All elements equal.
  {
    std::vector v(50, 7);
    nicesort::sort(v.begin(), v.end());
    assert(v.size() == 50);
    for (int x : v) assert(x == 7);
  }

  // 22.8  Duplicate-heavy — many equal groups.
  {
    std::vector<int> v;
    for (int k = 0; k < 10; ++k)
      for (int j = 0; j < 10; ++j) v.push_back(k);
    std::shuffle(v.begin(), v.end(), std::mt19937{42});
    nicesort::sort(v.begin(), v.end());
    assert(std::is_sorted(v.begin(), v.end()));
    assert(v.front() == 0 && v.back() == 9);
  }

  // 22.9  Large random permutation.
  {
    std::vector<int> v(1024);
    std::iota(v.begin(), v.end(), -512);
    std::shuffle(v.begin(), v.end(), std::mt19937{123});
    nicesort::sort(v.begin(), v.end());
    assert(std::is_sorted(v.begin(), v.end()));
    assert(v[0] == -512 && v[1023] == 511);
  }

  // 22.10 Custom descending comparator.
  {
    std::vector<int> v{3, 1, 4, 1, 5, 9, 2, 6, 5};
    nicesort::sort(v.begin(), v.end(), std::greater<int>{});
    assert(std::is_sorted(v.begin(), v.end(), std::greater<int>{}));
  }

  // 22.11 Stability: relative order of equal keys is preserved.
  {
    struct Keyed { int key, seq; };
    std::vector<Keyed> v;
    for (int i = 0; i < 20; ++i) v.push_back({i % 4, i});
    nicesort::sort(v.begin(), v.end(),
      [](Keyed const& a, Keyed const& b){ return a.key < b.key; });
    assert(std::is_sorted(v.begin(), v.end(),
      [](Keyed const& a, Keyed const& b){ return a.key < b.key; }));
    for (std::size_t i = 1; i < v.size(); ++i)
      if (v[i].key == v[i-1].key) assert(v[i].seq > v[i-1].seq);
  }

  // 22.12 Stability preserved across sizes that span multiple block merges.
  {
    struct KV { int key, seq; };
    for (int n : {8, 16, 17, 32, 64, 128}) {
      std::vector<KV> v;
      for (int i = 0; i < n; ++i) v.push_back({i % 5, i});
      nicesort::sort(v.begin(), v.end(),
        [](KV const& a, KV const& b){ return a.key < b.key; });
      assert(std::is_sorted(v.begin(), v.end(),
        [](KV const& a, KV const& b){ return a.key < b.key; }));
      for (std::size_t i = 1; i < v.size(); ++i)
        if (v[i].key == v[i-1].key) assert(v[i].seq > v[i-1].seq);
    }
  }

  // 22.13 Plain array (pointer iterators).
  {
    int arr[] = {5, 3, 8, 1, 9, 2, 7, 4, 6};
    nicesort::sort(arr, arr + 9);
    assert(std::is_sorted(arr, arr + 9));
  }

  // 22.14 Boundary sizes around bsize0 (default 16) exercise every merge path.
  {
    for (std::size_t n : {15u, 16u, 17u, 31u, 32u, 33u, 63u, 64u, 65u,
                          127u, 128u, 129u}) {
      std::vector<int> v(n);
      std::iota(v.begin(), v.end(), 0);
      std::shuffle(v.begin(), v.end(),
        std::mt19937{static_cast<unsigned>(n * 31 + 7)});
      auto ref = v;
      nicesort::sort(v.begin(), v.end());
      std::sort(ref.begin(), ref.end());
      assert(v == ref);
    }
  }

  // 22.15 Floating-point and negative values.
  {
    std::vector<double> v{3.14, -2.71, 0.0, 1.41, -1.0, 2.72, -0.5, 100.0, -100.0};
    nicesort::sort(v.begin(), v.end());
    assert(std::is_sorted(v.begin(), v.end()));
    assert(v.front() == -100.0 && v.back() == 100.0);

    std::vector<int> neg{-5, -3, -9, -1, -7, -2, -8, -4, -6};
    nicesort::sort(neg.begin(), neg.end());
    assert(std::is_sorted(neg.begin(), neg.end()));
    assert(neg.front() == -9 && neg.back() == -1);
  }

  // 22.16 Custom comparator on strings.
  {
    std::vector<std::string> v{"banana", "apple", "cherry", "date", "elderberry"};
    nicesort::sort(v.begin(), v.end());
    assert(std::is_sorted(v.begin(), v.end()));
    assert(v.front() == "apple" && v.back() == "elderberry");
    nicesort::sort(v.begin(), v.end(), std::greater<std::string>{});
    assert(std::is_sorted(v.begin(), v.end(), std::greater<std::string>{}));
  }

  // 22.17 Sort a sub-range; elements outside are untouched.
  {
    std::vector<int> v{9, 5, 3, 7, 1, 8, 2, 6, 4};
    nicesort::sort(v.begin() + 2, v.begin() + 7);
    assert(std::is_sorted(v.begin() + 2, v.begin() + 7));
    assert(v.front() == 9 && v[1] == 5);
    assert(v.back() == 4 && v[7] == 6);
  }

  // 22.18 Correctness vs std::sort on a 2000-element permutation.
  {
    std::vector<int> v(2000);
    std::iota(v.begin(), v.end(), -1000);
    std::shuffle(v.begin(), v.end(), std::mt19937{77777u});
    auto ref = v;
    nicesort::sort(v.begin(), v.end());
    std::sort(ref.begin(), ref.end());
    assert(v == ref);
  }

  // 22.19 insertion_sort direct invocation.
  {
    std::vector<int> v{5, 3, 8, 1, 9, 2, 7, 4, 6};
    nicesort::insertion_sort(v.begin(), v.end(), std::less<int>{});
    assert(std::is_sorted(v.begin(), v.end()));

    auto sorted = v;
    nicesort::insertion_sort(v.begin(), v.end(), std::less<int>{});
    assert(v == sorted);

    nicesort::insertion_sort(v.begin(), v.end(), std::greater<int>{});
    assert(std::is_sorted(v.begin(), v.end(), std::greater<int>{}));

    std::vector<int> two{2, 1};
    nicesort::insertion_sort(two.begin(), two.end(), std::less<int>{});
    assert((two == std::vector{1, 2}));

    std::vector<int> one{42};
    nicesort::insertion_sort(one.begin(), one.end(), std::less<int>{});
    assert(one[0] == 42);
  }
}

// ============================================================================
// §23 MISCELLANEOUS AND SPECIAL-CASE COVERAGE
// ============================================================================
{
  // 23.1  Range construction from a C array.
  {
    int const a[10]{};
    assert(std::ranges::equal(a, dq::array<int, 10>(a)));
  }

  // 23.2  Palindrome verification using front and back.
  {
    dq::array<char, 10> d(dq::from_range, std::string_view("racecar"));
    while (d.size() > 1) {
      assert(d.front() == d.back());
      d.pop_front(); d.pop_back();
    }
    assert(1 == d.size());
  }

  // 23.3  Deck-of-cards simulation.
  {
    dq::array<std::string, 52> deck;
    std::string suits[4] = {"Hearts","Diamonds","Clubs","Spades"};
    std::string ranks[13] = {"Ace","2","3","4","5","6","7","8","9","10","Jack","Queen","King"};
    for (auto& s : suits)
      for (auto& r : ranks)
        deck.push_back(r + " of " + s);
    assert(deck.size() == 52);
    auto top = deck.front(); deck.pop_front();
    auto bot = deck.back();  deck.pop_back();
    assert(deck.size() == 50 && top != bot);
  }

  // 23.4  Queue with VIP front insertion.
  {
    dq::array<std::string, 10> line;
    line.push_back("Alice"); line.push_back("Bob"); line.push_back("Charlie");
    assert(line.front() == "Alice");
    line.push_front("Dave");
    assert(line.front() == "Dave");
    line.pop_front();
    assert(line.front() == "Alice");
    while (!line.empty()) line.pop_front();
    assert(line.empty());
  }

  // 23.5  Range-based for loop.
  {
    dq::array<int, 10> d = {1, 2, 3, 4, 5};
    int sum = 0;
    for (const auto& x : d) sum += x;
    assert(sum == 15);
  }

  // 23.6  Range moving (std::string array assignment).
  {
    std::string a[]{"A", "B", "C"};
    dq::array<std::string, std::size(a)> b;
    assert(std::ranges::none_of(a, [](auto const& s){ return s.empty(); }));
    b = std::move(a);
    assert(std::ranges::all_of(a, [](auto const& s){ return s.empty(); }));
    assert(std::ranges::none_of(b, [](auto const& s){ return s.empty(); }));
    b.assign_range({"a", {"b"}, {"c"}});
    assert(3 == b.size());
  }

  // 23.7  Iterator stability after insert and erase.
  {
    dq::array<int, 20> d = {0, 1, 2, 3, 4, 5};
    auto it = d.begin() + 3;
    auto dist = std::distance(d.begin(), it);
    d.insert(it, 42);
    it = d.begin() + dist;
    assert(*it == 42);
    it = d.erase(it);
    assert(*it == 3);
  }

  // 23.8  align & memory layout — alignas type.
  {
    struct alignas(64) AlignedInt { int val; };
    dq::array<AlignedInt, 4> d;
    d.push_back({1}); d.push_back({2});
    assert(reinterpret_cast<uintptr_t>(&d.front()) % 64 == 0);
    assert(reinterpret_cast<uintptr_t>(&d[1])      % 64 == 0);
    assert(reinterpret_cast<uintptr_t>(d.data())   % 64 == 0);
  }

  // 23.9  FIFO semantics verified by mirroring with std::deque.
  {
    constexpr int CAP = 5;
    dq::array<int, CAP> d;
    std::deque<int> ref;
    std::mt19937 rng{12345u};
    std::uniform_int_distribution<int> opd(0, 1);
    int next_push = 0;
    for (int iter = 0; iter < 200; ++iter) {
      if (opd(rng) == 0 && !d.full()) {
        d.push_back(next_push); ref.push_back(next_push++);
      } else if (!d.empty()) {
        assert(d.front() == ref.front());
        d.pop_front(); ref.pop_front();
      }
      assert(d.size() == ref.size());
    }
    while (!d.empty()) {
      assert(d.front() == ref.front());
      d.pop_front(); ref.pop_front();
    }
  }

  // 23.10 assign() with initializer list clears an existing wrapped state.
  {
    dq::array<int, 10> d{1,2,3,4,5};
    d.pop_front(); d.push_back(6); // wrapped
    d.assign({100, 200});
    assert(d.size()==2 && d[0]==100 && d[1]==200 && !d.full());
    d.append_range({300, 400});
    assert((d == std::array{100, 200, 300, 400}));
  }

  // 23.11 assign() with empty initializer list on a full container.
  {
    dq::array<int, 4> full_arr{1,2,3,4};
    assert(full_arr.full());
    full_arr.assign({});
    assert(full_arr.empty());
    assert(full_arr.first() == full_arr.last());
  }

  // 23.12 data() pointer remains non-null for NEW allocator after move.
  {
    dq::array<int, 5, dq::NEW> a;
    assert(a.data() != nullptr);
    dq::array<int, 5, dq::NEW> b(std::move(a));
    assert(b.data() != nullptr);
    assert(a.data() != nullptr); // moved-from still has an allocation
  }

  // 23.13 Shrink via resize on an iota-constructed container.
  {
    dq::array<int, 100> d(std::ranges::iota_view{0, 50});
    d.resize(10);
    assert(d.size() == 10 && d.front() == 0 && d.back() == 9);
  }

  // 23.14 dq::from_range with an odds-only filtered view.
  {
    auto odds = std::views::iota(1, 10)
              | std::views::filter([](int x){ return x % 2 != 0; });
    dq::array<int, 5> d(dq::from_range, odds);
    assert(d.size() == 5);
    assert((d == std::array{1, 3, 5, 7, 9}));
  }

  // 23.15 std::inner_product on two dq::arrays.
  {
    dq::array<int, 5> a = {1,2,3};
    dq::array<int, 5> b = {4,5,6};
    assert(std::inner_product(a.begin(), a.end(), b.begin(), 0) == 32);
  }
}
}

int main()
{
  test();
  std::cout << "All tests passed!" << std::endl;
  return 0;
}
