#ifndef _GROUP_HPP
#define _GROUP_HPP

#include <cassert>
#include <vector>
#include <list>
#include <string>

struct Perm16;

#include "perm16.hpp"


template< class perm  = Perm16 >
class PermutationGroup {

public:

  using vect = typename perm::vect;
  using StrongGeneratingSet = std::vector< std::vector< perm > >;

  const std::string name;
  const uint64_t N;
  const StrongGeneratingSet sgs;

public:

  using list = std::list<vect>;

  PermutationGroup(std::string name, uint64_t N, StrongGeneratingSet sgs) :
    name(name), N(N), sgs(sgs) { assert(check_sgs()); };

  bool is_canonical(const vect &v) const;
  list elements_of_depth(uint64_t depth) const;

  bool check_sgs() {
    for (uint64_t level = 0; level<sgs.size(); level++)
      for (auto &v : sgs[level]) {
	if (not v.is_permutation(N)) return false;
	for (uint64_t i=0; i<level; i++)
	  if (not (v[i] == i)) return false;
      }
    return true;
  }

private:

  class ChildrenIterator {

    const PermutationGroup &gr;
    const vect &father;
    uint64_t ind;

  public:

    ChildrenIterator(const PermutationGroup &gr, const vect &v) : gr(gr), father(v) {
      uint64_t id = father.template search_index<LAST_NON_ZERO>(gr.N);
      ind = id;
      if (ind >= gr.N) ind = 0;
    };
    void operator++() { ind++;}
    bool is_not_end() const { return ind < gr.N; }
    vect operator *() const {
      vect res = father;
      res.p[ind]++;
      return res;
    }
  };

  void walk_tree(vect v, list &res, int remaining_depth) const;

public:

  ChildrenIterator children(const vect &v) const { return {*this, v}; }

};

#include <ostream>

template <class perm>
std::ostream & operator<<(std::ostream & stream, const PermutationGroup<perm> &g) {
  stream << g.name;
  return stream;
}


#ifdef USE_TBB
  #include "tbb/scalable_allocator.h"
  template<class T>
  using allocator = tbb::scalable_allocator< T >;
#else
  template<class T>
  using allocator = std::allocator< T >;
#endif

//template<class T>
//using set = std::unordered_set<T, std::hash<T>, std::equal_to<T>, allocator>;

#define USE_BOOST_FLAT_SET

#ifdef USE_BOOST_FLAT_SET
  #include <boost/container/flat_set.hpp>
  template<class T>
  using set = boost::container::flat_set< T, std::less<T>, allocator<T> >;
#else
  #include <set>
  template<class T>
  using set = std::set< T, std::less<T>, allocator<T> >;
#endif

template<class perm>
bool PermutationGroup<perm>::is_canonical(const vect &v) const {
  set<vect> to_analyse, new_to_analyse;
  to_analyse.insert(v);
  vect child;
  for (uint64_t i=0; i < N-1; i++) {
    new_to_analyse.clear();
    auto &transversal = sgs[i];
    for (vect list_test : to_analyse) {
      for (vect x : transversal) {
        child = list_test.permuted(x);
        char comp = v.less_partial(child, i+1);
        if (comp == 0) new_to_analyse.insert(child);
        else if (comp < 0) return false;
      }
    }
    to_analyse.swap(new_to_analyse);
  }
  return true;
}


#ifdef USE_CILK
  #include <cilk/cilk_api.h>
  #include <cilk/reducer_list.h>
  template<class T> using list_generator = cilk::reducer_list_append< T >;
#else
  template<class T> using list_generator = std::list< T >;
#endif

template<class perm>
void PermutationGroup<perm>::walk_tree(
   vect v, list_generator<vect> &res, int remaining_depth) const
{
  if (remaining_depth == 0) res.push_back(v);
  else for (auto ch = children(v); ch.is_not_end(); ++ch) {
      vect child = *ch;
      if (is_canonical(child))
#ifdef USE_CILK
	cilk_spawn
#endif
	  walk_tree(child, res, remaining_depth-1);
    }
}


template<class perm>
std::list<typename PermutationGroup<perm>::vect>
PermutationGroup<perm>::elements_of_depth(uint64_t depth) const {
  vect zero_vect;
  list_generator<vect> list_res;
  zero_vect.v = __m128i {0, 0};
  walk_tree(zero_vect, list_res, depth);
#ifdef USE_CILK
  return list_res.get_value();
#else
  return list_res;
#endif
}


#endif
