#ifndef _GROUP_HPP
#define _GROUP_HPP

#include <cassert>
#include <vector>
#include <list>
#include <string>

#include "config.h"

#ifdef USE_TBB
  #include "tbb/scalable_allocator.h"
  template<class T>
  using allocator = tbb::scalable_allocator<T>;
#else
  template<class T>
  using allocator = std::allocator<T>;
#endif

#ifdef USE_CILK
  #include <cilk/cilk.h>
  #include <cilk/cilk_api.h>
  #include <cilk/reducer_list.h>
  #include <cilk/reducer_opadd.h>
#else
  #define cilk_spawn
#endif


#include "perm16.hpp"
template< class perm  = Perm16 >
class PermutationGroup {

public:

  using vect = typename perm::vect;
  using list = std::list<vect, allocator<vect> >;
  using StrongGeneratingSet = std::vector< std::vector< perm > >;

  const std::string name;
  const uint64_t N;
  const StrongGeneratingSet sgs;

  PermutationGroup(std::string name, uint64_t N, StrongGeneratingSet sgs) :
    name(name), N(N), sgs(sgs) { assert(check_sgs()); };
  bool check_sgs() const;
  bool is_canonical(vect v) const;
  vect canonical(vect v) const;
  list elements_of_depth(uint64_t depth) const;
  uint64_t elements_of_depth_number(uint64_t depth) const;

private:

  class ChildrenIterator {
    const PermutationGroup &gr;
    const vect &father;
    uint64_t ind;
  public:
    ChildrenIterator(const PermutationGroup &gr, const vect &v) :
      gr(gr), father(v), ind(father.last_non_zero(gr.N)) { if (ind >= gr.N) ind = 0; };
    void operator++() { ind++;}
    bool is_not_end() const { return ind < gr.N; }
    vect operator *() const { vect res = father; res.p[ind]++; return res; }
  };

#ifdef USE_CILK
  using counter = cilk::reducer_opadd< uint64_t >;
  using list_generator = cilk::reducer_list_append< vect, allocator<vect> >;
#else
  using counter = uint64_t;
  using list_generator = std::list< vect, allocator<vect> >;
#endif

  void walk_tree(vect v, list_generator &res, int remaining_depth) const;
  void walk_tree_counter(vect v, counter &res, int remaining_depth) const;

public:

  ChildrenIterator children(const vect &v) const { return {*this, v}; }

};

#include <ostream>

template <class perm>
std::ostream & operator<<(std::ostream & stream, const PermutationGroup<perm> &g) {
  return stream << g.name;
}


template<class perm>
bool PermutationGroup<perm>::check_sgs() const {
  for (uint64_t level = 0; level<sgs.size(); level++)
    // CILK bugs with range for
    //for (auto &v : sgs[level]) {
    for (auto ip = sgs[level].begin(); ip!=sgs[level].end(); ip++) {
      auto v = *ip;
      if (not v.is_permutation(N)) return false;
      for (uint64_t i=0; i<level; i++)
	if (not (v[i] == i)) return false;
    }
  return true;
}


//template<class T>
//using set = std::unordered_set<T, std::hash<T>, std::equal_to<T>, allocator>;

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
bool PermutationGroup<perm>::is_canonical(vect v) const {
  set<vect> to_analyse, new_to_analyse;
  to_analyse.insert(v);
  vect child;
  for (uint64_t i=0; i < N-1; i++) {
    new_to_analyse.clear();
    auto &transversal = sgs[i];
    // CILK bugs with range for
    //    for (vect list_test : to_analyse) {
    for (auto itl = to_analyse.begin(); itl != to_analyse.end(); itl++) {
      vect list_test = *itl;
      // CILK bugs with range for
      // for (vect x : transversal) {
      for (auto itv = transversal.begin(); itv != transversal.end(); itv++) {
	vect x = *itv;
        child = list_test.permuted(x);
	// Slight change from Borie's algorithm's: we do a full lex comparison first.
	if (v < child) return false;
        if (v.first_diff(child) > i) new_to_analyse.insert(child);
      }
    }
    to_analyse.swap(new_to_analyse);
  }
  return true;
}


template<class perm>
typename PermutationGroup<perm>::vect
PermutationGroup<perm>::canonical(vect v) const {
  set<vect> to_analyse, new_to_analyse;
  to_analyse.insert(v);
  vect child;
  for (uint64_t i=0; i < N-1; i++) {
    new_to_analyse.clear();
    auto &transversal = sgs[i];
    for (auto itl = to_analyse.begin(); itl != to_analyse.end(); itl++) {
      vect list_test = *itl;
      for (auto itv = transversal.begin(); itv != transversal.end(); itv++) {
	vect x = *itv;
        child = list_test.permuted(x);
	// TODO: find a better algorithm !
	// TODO: the following doesn't work:
	//       if (v.less_partial(child, i+1) < 0) v = child;
	//       if (v < child) v = child;
	if (v < child) return canonical(child);
	else if (v.first_diff(child) > i) new_to_analyse.insert(child);
      }
    }
    to_analyse.swap(new_to_analyse);
  }
  return v;
}

template<class perm>
void PermutationGroup<perm>::walk_tree(
   vect v, list_generator &res, int remaining_depth) const
{
  if (remaining_depth == 0) res.push_back(v);
  else for (auto ch = children(v); ch.is_not_end(); ++ch) {
      vect child = *ch;
      if (is_canonical(child))
	cilk_spawn this->walk_tree(child, res, remaining_depth-1);
    }
}


template<class perm>
typename PermutationGroup<perm>::list
PermutationGroup<perm>::elements_of_depth(uint64_t depth) const {
  vect zero_vect;
  list_generator list_res;
  zero_vect.v = __m128i {0, 0};
  walk_tree(zero_vect, list_res, depth);
#ifdef USE_CILK
  return list_res.get_value();
#else
  return list_res;
#endif
}



template<class perm>
void PermutationGroup<perm>::walk_tree_counter(
   vect v, counter &res, int remaining_depth) const
{
  if (remaining_depth == 0) res++;
  else for (auto ch = children(v); ch.is_not_end(); ++ch) {
      vect child = *ch;
      if (is_canonical(child))
	cilk_spawn this->walk_tree_counter(child, res, remaining_depth-1);
    }
}


template<class perm>
uint64_t
PermutationGroup<perm>::elements_of_depth_number(uint64_t depth) const {
  vect zero_vect;
  counter cnt_res;
  zero_vect.v = __m128i {0, 0};
  walk_tree_counter(zero_vect, cnt_res, depth);
#ifdef USE_CILK
  return cnt_res.get_value();
#else
  return cnt_res;
#endif
}

#endif
