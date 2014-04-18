#ifndef _GROUP_HPP
#define _GROUP_HPP

#include <cassert>
#include <vector>
#include <list>
#include <string>

#ifdef USE_TBB
  #include "tbb/scalable_allocator.h"
  template<class T>
  using allocator = tbb::scalable_allocator<T>;
#else
  template<class T>
  using allocator = std::allocator<T>;
#endif

#ifdef USE_CILK
  #include <cilk/cilk_api.h>
  #include <cilk/reducer_list.h>
#endif



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
      //  for (auto &v : sgs[level]) {
      for (auto ip = sgs[level].begin(); ip!=sgs[level].end(); ip++) {
	Perm16 v = *ip;
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

public:

#ifdef USE_CILK
  using list_generator = cilk::reducer_list_append< vect >;
#else
  using list_generator = std::list< vect >;
#endif

//  void walk_tree(vect v, list_generator &res, int remaining_depth) const;

  ChildrenIterator children(const vect &v) const { return {*this, v}; }

};

#include <ostream>

template <class perm>
std::ostream & operator<<(std::ostream & stream, const PermutationGroup<perm> &g) {
  stream << g.name;
  return stream;
}



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
    //    for (vect list_test : to_analyse) {
    for (auto itl = to_analyse.begin(); itl != to_analyse.end(); itl++) {
      vect list_test = *itl;
      // for (vect x : transversal) {
      for (auto itv = transversal.begin(); itv != transversal.end(); itv++) {
	vect x = *itv;
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

template<class perm>
void walk_tree(const PermutationGroup<perm> &grp,
               const typename PermutationGroup<perm>::vect v,
               typename PermutationGroup<perm>::list_generator &res,
               uint64_t remaining_depth)
{
  if (remaining_depth == 0) res.push_back(v);
  else for (auto ch = grp.children(v); ch.is_not_end(); ++ch) {
      typename PermutationGroup<perm>::vect child = *ch;
      if (grp.is_canonical(child))
#ifdef USE_CILK
	cilk_spawn walk_tree(grp, child, res, remaining_depth-1);
#else
        walk_tree(grp, child, res, remaining_depth-1);
#endif
    }
}


template<class perm>
std::list<typename PermutationGroup<perm>::vect>
PermutationGroup<perm>::elements_of_depth(uint64_t depth) const {
  vect zero_vect;
  list_generator list_res;
  zero_vect.v = __m128i {0, 0};
  walk_tree<perm>(*this, zero_vect, list_res, depth);
#ifdef USE_CILK
  return list_res.get_value();
#else
  return list_res;
#endif
}


#endif
/*
walk_tree(const PermutationGroup<Perm16>&, PermutationGroup<Perm16>::vect&, PermutationGroup<Perm16>::list_generator&, uint64_t&)
walk_tree(const PermutationGroup<perm>&, PermutationGroup<perm>::vect, PermutationGroup<perm>::list_generator&, int)
*/
