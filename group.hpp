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

#ifdef USE_CILK
  #include <cilk/cilk.h>
  #include <cilk/cilk_api.h>
  #include <cilk/reducer_list.h>
  #include <cilk/reducer_opadd.h>
  #include <cilk/holder.h>
#else
  #define cilk_spawn
#endif

#ifdef USE_CILK
template <class vect>
struct BFS_storage {
  cilk::holder< set<vect> > analyse, new_analyse;

  set<vect> &get_to_analyse() { return analyse(); }
  set<vect> &get_new_to_analyse() { return new_analyse(); }
};
#else
template <class vect>
struct BFS_storage {
  set<vect> analyse, new_analyse;

  set<vect> &get_to_analyse() { return analyse; }
  set<vect> &get_new_to_analyse() { return new_analyse; }
};
#endif

#include "perm16.hpp"
template< class perm  = Perm16 >
class PermutationGroup {

  class Children;

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
  bool is_canonical(vect v, BFS_storage<vect> &store) const;
  vect canonical(vect v) const;
  vect canonical(vect v, BFS_storage<vect> &store) const;
  list elements_of_depth(uint64_t depth) const;
  uint64_t elements_of_depth_number(uint64_t depth) const;
  template<typename Res> //  should implement the following interface:
  // struct Res {
  //   using type = ...
  //   using type_result = ...
  //   static void update(type &res, vect v)   // update res with v
  //   static type_result get_value(type &res) // return value in res
  // };
  typename Res::type_result elements_of_depth_walk(uint64_t depth) const;

  Children children(const vect &v) const { return {*this, v}; }

private:

  class Children {

    class Iterator {
      const Children &ch;
      uint64_t ind;
    public:
      Iterator(const Children &ch) : ch(ch), ind(ch.father.last_non_zero(ch.gr.N)) {
	if (ind >= ch.gr.N) ind = 0; }
      Iterator(const Children &ch, uint64_t end) : ch(ch), ind(end) {}
      void operator++() { ++ind; }
      bool operator!=(const Iterator &end) const {return ind != end.ind;}
      vect operator *() const { vect res = ch.father; ++res.p[ind]; return res; }
    };

    const PermutationGroup &gr;
    const vect &father;
  public:
    Children(const PermutationGroup &gr, const vect &v) : gr(gr), father(v) {};
    Iterator begin() const { return {*this}; }
    Iterator end() const { return {*this, gr.N}; }
  };

#ifdef USE_CILK
  using counter = cilk::reducer_opadd< uint64_t >;
  using list_generator = cilk::reducer_list_append< vect, allocator<vect> >;
#else
  using counter = uint64_t;
  using list_generator = std::list< vect, allocator<vect> >;
#endif

  struct ResultList {
    using type = list_generator;
    using type_result = list;
    static void update(type &lst, vect v) { lst.push_back(v); }
    static type_result get_value(type &lst) {
      #ifdef USE_CILK
        return lst.get_value();
      #else
        return lst;
      #endif
    }
  };

  struct ResultCounter {
    using type = counter;
    using type_result = uint64_t;
    static void update(type &counter, vect v) { counter++; }
    static type_result get_value(type &counter) {
      #ifdef USE_CILK
        return counter.get_value();
      #else
        return counter;
      #endif
    }
  };

  template<class Res>
  void walk_tree(vect v, typename Res::type &res,
		 uint64_t target_depth, uint64_t depth,
		 BFS_storage<vect> &store) const;

};

#include <ostream>

template <class perm>
std::ostream & operator<<(std::ostream & stream, const PermutationGroup<perm> &g) {
  return stream << g.name;
}


template<class perm>
bool PermutationGroup<perm>::check_sgs() const {
  for (uint64_t level = 0; level<sgs.size(); level++)
    for (const perm &v : sgs[level]) {
      if (not v.is_permutation(N)) return false;
      for (uint64_t i=0; i<level; i++)
	if (not (v[i] == i)) return false;
    }
  return true;
}



template<class perm>
bool PermutationGroup<perm>::is_canonical(vect v, BFS_storage<vect> &store) const {
  set<vect> &to_analyse = store.get_to_analyse();
  set<vect> &new_to_analyse = store.get_new_to_analyse();
  to_analyse.clear();
  new_to_analyse.clear();
  to_analyse.insert(v);

  for (uint64_t i=0; i < N-1; i++) {
    new_to_analyse.clear();
    const auto &transversal = sgs[i];
    for (const vect &list_test : to_analyse) {
      for (const perm &x : transversal) {
        const vect child = list_test.permuted(x);
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
bool PermutationGroup<perm>::is_canonical(vect v) const {
  BFS_storage<vect> store {};
  return is_canonical(v, store);
}

template<class perm>
auto PermutationGroup<perm>::canonical(vect v, BFS_storage<vect> &store) const -> vect {
  set<vect> &to_analyse = store.get_to_analyse();
  set<vect> &new_to_analyse = store.get_new_to_analyse();
  to_analyse.clear();
  new_to_analyse.clear();
  to_analyse.insert(v);

  for (uint64_t i=0; i < N-1; i++) {
    new_to_analyse.clear();
    const auto &transversal = sgs[i];
    for (const vect &list_test : to_analyse) {
      for (const perm &x : transversal) {
        const vect child = list_test.permuted(x);
	// TODO: find a better algorithm !
	// TODO: the following doesn't work:
	//       if (v.less_partial(child, i+1) < 0) v = child;
	//       if (v < child) v = child;
	if (v < child) return canonical(child, store);
	else if (v.first_diff(child) > i) new_to_analyse.insert(child);
      }
    }
    std::swap(to_analyse, new_to_analyse);
  }
  return v;
}

template<class perm>
auto PermutationGroup<perm>::canonical(vect v) const -> vect {
  BFS_storage<vect> store {};
  return canonical(v, store);
}

template<class perm>
template<class Res>
void PermutationGroup<perm>::walk_tree(vect v, typename Res::type &res,
				       uint64_t target_depth, uint64_t depth,
				       BFS_storage<vect> &store) const {
  if (depth == target_depth) Res::update(res, v);
  else for (vect child : children(v)) {
      if (is_canonical(child, store))
	cilk_spawn this->walk_tree<Res>(child, res, target_depth, depth+1, store);
    }
}


template<class perm>
template<class Res>
typename Res::type_result
PermutationGroup<perm>::elements_of_depth_walk(uint64_t depth) const {
  vect zero_vect {};
  typename Res::type res {};
  BFS_storage<vect> store {};
  walk_tree<Res>(zero_vect, res, depth, 0, store);
  return Res::get_value(res);
}


template<class perm>
auto PermutationGroup<perm>::elements_of_depth(uint64_t depth) const -> list {
  return elements_of_depth_walk<ResultList>(depth);
}

template<class perm>
uint64_t PermutationGroup<perm>::elements_of_depth_number(uint64_t depth) const {
  return elements_of_depth_walk<ResultCounter>(depth);
}

#endif
