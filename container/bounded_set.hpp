#ifndef _BOUNDED_SET_HPP
#define _BOUNDED_SET_HPP

// #define SET_STATISTIC

#ifdef SET_STATISTIC
#include <iostream>
#include <iomanip>
#include <iostream>
#endif

#include <cassert>

namespace IVMPG {
namespace Container {

template <class Key, class Hash = std::hash<Key> >
class bounded_set {

  struct Pair {
    Key key;
    Pair *next;
  };

  size_t bound;
  static const constexpr Hash hashfun = Hash();
  Pair *buckets; 
  Pair *first;
  #ifdef SET_STATISTIC
  size_t collide = 0, success = 0, request = 0;
  #endif
  class Iterator;

  Pair * sentinel() const { return buckets+bound; }

public:

  typedef Key      key_type;
  typedef Key      value_type;
  typedef Iterator iterator;

  // We use an extra uninitialized pair as a sentinel for the end of the linked list.
  bounded_set(const bounded_set& s) :
    bound(s.bound),  buckets(new Pair[bound+1]), first(s.first - s.buckets + buckets) {
    for (size_t i=0; i<bound; ++i) {
      buckets[i].key = s.buckets[i].key;
      buckets[i].next = s.buckets[i].next - s.buckets + buckets;
    }
  }
  bounded_set(size_t bound = 1024) :
    bound(bound),
    buckets(new Pair[bound+1]), first(buckets+bound) {
    for (size_t i=0; i<bound; ++i) buckets[i].next = nullptr;
  }

  bounded_set(bounded_set&& rhs) noexcept { *this = std::move(rhs); };
  bounded_set & operator=(bounded_set&& rhs) & noexcept;

  ~bounded_set() {
  #ifdef SET_STATISTIC
    if (buckets != nullptr) {
      std::cout << "[r" << request << std::setprecision(3) << 
	",s" << 100.*success/request << "%"
	",c" << 100.*collide/request << "%" << "]" << std::setprecision(6);
    }
  #endif
    delete [] buckets;
  }

  void insert(Key key);
  void clear();
  Iterator begin() const { return {*this, first}; }
  Iterator end() const { return {*this, sentinel()}; }
  size_t size() const;

private:

  class Iterator {
    friend class bounded_set;

    const bounded_set &set;
    Pair *it;

    Iterator(const bounded_set &set, Pair *it) : set(set), it(it) { }
  public:
    void operator++() { it = it->next; }
    Key operator*() const { return it->key;}
    bool operator==(const Iterator &other) const { return it == other.it; }
    bool operator!=(const Iterator &other) const { return it != other.it; }
  };

};

template <class Key, class Hash >
const constexpr Hash bounded_set<Key, Hash>::hashfun;

template <class Key, class Hash >
auto bounded_set<Key, Hash>::operator=(bounded_set&& rhs) & noexcept -> bounded_set & {
  assert(this != &rhs);
  bound   = rhs.bound;
  buckets = rhs.buckets;
  first   = rhs.first;
#ifdef SET_STATISTIC
  collide = rhs.collide;
  success = rhs.success;
  request = rhs.request;
#endif
  rhs.buckets = nullptr; // to avoid double free
  return *this;
}

template <class Key, class Hash >
void bounded_set<Key, Hash>::clear() {
  while (first != sentinel()) {
    Pair *tmp = first;
    first = first->next;
    tmp->next = nullptr;
  }
}

template <class Key, class Hash >
void bounded_set<Key, Hash>::insert(Key key) {
#ifdef SET_STATISTIC
  request++;
#endif
  size_t hash = hashfun(key) & (bound-1);
  while (buckets[hash].next != nullptr and buckets[hash].key != key) {
    hash = hash < bound-1 ? hash+1 : 0;
#ifdef SET_STATISTIC
    collide++;
#endif
  }
  if (buckets[hash].next == nullptr) {
#ifdef SET_STATISTIC
    success++;
#endif
    buckets[hash].key = key;
    buckets[hash].next = first;
    first = buckets + hash;
  }
}

template <class Key, class Hash >
size_t bounded_set<Key, Hash>::size() const {
  Pair *it = first;
  std::size_t count = 0;
  while (it != sentinel()) {count++; it = it->next;}
  return count;
}

template <class Key, class Hash>
std::ostream & operator<<(std::ostream & stream, bounded_set<Key, Hash> const &set) {
  bool first = true;
  stream << "{";
  for (auto x : set) {
    if (not first) stream << ", ";
    stream << x;
    first = false;
  }
  stream << "}";
  return stream;
}

} //  namespace Container
} //  namespace IVMPG

#endif // _BOUNDED_SET_HPP
