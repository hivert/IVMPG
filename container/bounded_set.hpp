#ifndef _BOUNDED_SET_HPP
#define _BOUNDED_SET_HPP

#include <cassert>

template <class Key, class Hash = std::hash<Key> >
class bounded_set {

  struct Pair {
    Key key;
    Pair *next;
  };

  size_t bound;
  Hash hashfun;
  Pair *buckets; // we use an extra pair as a sentinel for the end of the linked list.
  Pair *first;

  class Iterator;

  Pair * sentinel() const { return buckets+bound; }

public:

  typedef Key      key_type;
  typedef Key      value_type;
  typedef Iterator iterator;

  bounded_set(bounded_set&) = delete;

  bounded_set(size_t bound = 2048, Hash fun = Hash()) :
    bound(bound), hashfun(fun), buckets(new Pair[bound+1]), first(buckets+bound) {
    for (size_t i=0; i<bound; ++i) buckets[i].next = nullptr;
  }

  bounded_set(bounded_set&& rhs) noexcept { *this = std::move(rhs); };
  bounded_set & operator=(bounded_set&& rhs) & noexcept;

  ~bounded_set() { delete [] buckets; };

  // Iterator insert(Key key);
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

#include <iostream>

template <class Key, class Hash >
auto bounded_set<Key, Hash>::operator=(bounded_set&& rhs) & noexcept -> bounded_set & {
  assert(this != &rhs);
  hashfun = rhs.hashfun;
  bound   = rhs.bound;
  buckets = rhs.buckets;
  first   = rhs.first;
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
//auto bounded_set<Key, Hash>::insert(Key key) -> Iterator {
void bounded_set<Key, Hash>::insert(Key key) {
  size_t hash = hashfun(key) & (bound-1);
  while (buckets[hash].next != nullptr and buckets[hash].key != key)
    hash = hash < bound - 1 ? hash+1 : 0;
  if (buckets[hash].next == nullptr) {
    buckets[hash].key = key;
    buckets[hash].next = first;
    first = &buckets[hash];
  }
  //  return {*this, &buckets[hash]};
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

#endif // _BOUNDED_SET_HPP
