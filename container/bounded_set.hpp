/******************************************************************************/
/*       Copyright (C) 2014 Florent Hivert <Florent.Hivert@lri.fr>,           */
/*                                                                            */
/*  Distributed under the terms of the GNU General Public License (GPL)       */
/*                                                                            */
/*    This code is distributed in the hope that it will be useful,            */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       */
/*   General Public License for more details.                                 */
/*                                                                            */
/*  The full text of the GPL is available at:                                 */
/*                                                                            */
/*                  http://www.gnu.org/licenses/                              */
/******************************************************************************/

#ifndef _BOUNDED_SET_HPP
#define _BOUNDED_SET_HPP

// #define SET_STATISTIC

#ifdef SET_STATISTIC
#include <iostream>
#include <iomanip>
#include <iostream>
#endif

#include <memory>
#include <cassert>

namespace IVMPG {
namespace Container {

template <class Key, class Hash = std::hash<Key>, size_t bound = 1024>
class bounded_set {

  struct Pair {
    Key key;
    Pair *next;
  };

  static const constexpr Hash hashfun = Hash();
  // I'm using unique_ptr to ensure safety of deallocation an copying.
  std::unique_ptr<Pair[]> buckets_own;
  // This is just a copy of the previous unique_ptr which allows to gain a few
  // percents speed using direct access.
  Pair *buckets;
  Pair *first;
  #ifdef SET_STATISTIC
  size_t collide = 0, success = 0, request = 0;
  #endif
  class Iterator;

  Pair * sentinel() const { return &(buckets[bound]); }

public:

  typedef Key      key_type;
  typedef Key      value_type;
  typedef Iterator iterator;

  // We use an extra uninitialized pair as a sentinel for the end of the linked list.
  bounded_set() :
    buckets_own(new Pair[bound+1]), buckets(buckets_own.get()),
    first(&(buckets[bound])) {
    for (size_t i=0; i<bound; ++i) buckets[i].next = nullptr;
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

template <class Key, class Hash, size_t bound>
const constexpr Hash bounded_set<Key, Hash, bound>::hashfun;

  
// template <class Key, class Hash, size_t bound>
// auto bounded_set<Key, Hash, bound>::operator=(bounded_set&& rhs) & noexcept -> bounded_set & {
//   assert(this != &rhs);
//   buckets = std::move(rhs.buckets);
//   first   = rhs.first;
// #ifdef SET_STATISTIC
//   collide = rhs.collide;
//   success = rhs.success;
//   request = rhs.request;
// #endif
//   rhs.buckets = nullptr; // to avoid double free
//   return *this;
// }

template <class Key, class Hash, size_t bound>
void bounded_set<Key, Hash, bound>::clear() {
  while (first != sentinel()) {
    Pair *tmp = first;
    first = first->next;
    tmp->next = nullptr;
  }
}

template <class Key, class Hash, size_t bound>
void bounded_set<Key, Hash, bound>::insert(Key key) {
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
    first = &(buckets[hash]);
  }
}

template <class Key, class Hash, size_t bound>
size_t bounded_set<Key, Hash, bound>::size() const {
  Pair *it = first;
  std::size_t count = 0;
  while (it != sentinel()) {count++; it = it->next;}
  return count;
}

template <class Key, class Hash, size_t bound>
std::ostream & operator<<(std::ostream & stream, bounded_set<Key, Hash, bound> const &set) {
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
