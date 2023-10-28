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

#ifndef _GROUP_HPP
#define _GROUP_HPP

// #include "config.h"
#include <cassert>
#include <list>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#ifdef USE_CILK
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_list.h>
#include <cilk/reducer_opadd.h>
#else
#define cilk_spawn
#endif

#ifdef USE_TBB
#include "tbb/scalable_allocator.h"
namespace IVMPG {
  template <class T>
  using allocator = tbb::scalable_allocator<T>;
}
#else
namespace IVMPG {
  template <class T>
  using allocator = std::allocator<T>;
}
#endif

#if GROUP_USE_SET == 1  // UNORDERED_SET
#include <unordered_set>
namespace IVMPG {
  template <class T>
  using set
      = std::unordered_set<T, std::hash<T>, std::equal_to<T>, allocator<T>>;
}
#elif GROUP_USE_SET == 2  // BOOST_FLAT_SET
// #ifdef USE_BOOST_FLAT_SET
#include <boost/container/flat_set.hpp>
namespace IVMPG {
  template <class T>
  using set = boost::container::flat_set<T, std::less<T>, allocator<T>>;
}
#elif GROUP_USE_SET == 3  // STD_SET
#include <set>
namespace IVMPG {
  template <class T>
  using set = std::set<T, std::less<T>, allocator<T>>;
}
#else  // BOUNDED_SET
#include "bounded_set.hpp"
namespace IVMPG {
  template <class T>
  using set = IVMPG::Container::bounded_set<T>;
}
#endif

#include "hpcombi/include/perm16.hpp"
#include "hpcombi/include/simde/x86/sse4.1.h"
#include "temp_storage.hpp"

namespace IVMPG {
  using Perm16 = HPCombi::Perm16;

  template <class perm = Perm16>
  class PermutationGroup {
   public:
    using vect                = typename perm::vect;
    using list                = std::list<vect, allocator<vect>>;
    using StrongGeneratingSet = std::vector<std::vector<perm>>;
    using TemporaryStorage    = std::pair<set<vect>, set<vect>>;

    std::string         name;
    uint64_t            N;
    StrongGeneratingSet sgs;

   private:
#ifdef USE_CILK
#define CILK_GET_VALUE(v) (v).get_value()
    using counter        = cilk::reducer_opadd<uint64_t>;
    using list_generator = cilk::reducer_list_append<vect, allocator<vect>>;

    // Using thread local only gain a few percent
    // using BFS_storage = Storage_holder< TemporaryStorage >;
    using BFS_storage = Storage_thread_local<TemporaryStorage>;
#else
#define CILK_GET_VALUE(v) (v)
    using counter        = uint64_t;
    using list_generator = std::list<vect, allocator<vect>>;
    using BFS_storage    = Storage_dummy<TemporaryStorage>;
#endif

   public:
    PermutationGroup(std::string name, uint64_t N, StrongGeneratingSet sgs)
        : name(name), N(N), sgs(sgs) {
      assert(check_sgs());
    };
    bool     check_sgs() const;
    bool     is_canonical(vect v) const;
    bool     is_canonical(vect v, TemporaryStorage&) const;
    vect     canonical(vect v) const;
    vect     canonical(vect v, TemporaryStorage&) const;
    list     elements_of_depth(uint64_t depth) const;
    list     elements_of_depth(uint64_t depth, uint64_t max_part) const;
    list     elements_of_evaluation(vect eval) const;
    uint64_t elements_of_depth_number(uint64_t depth) const;
    uint64_t elements_of_depth_number(uint64_t depth, uint64_t max_part) const;

    template <typename Res>  //  should implement the following interface:
    // struct Res {
    //   using type = ...
    //   using type_result = ...
    //   static void update(type &res, vect v)   // update res with v
    //   static type_result get_value(type &res) // return value in res
    // };
    typename Res::type_result elements_of_depth_walk(uint64_t depth,
                                                     uint64_t max_part) const;
    template <typename Res>
    typename Res::type_result elements_of_evaluation_walk(vect eval) const;

    uint64_t first_child_index(const vect& v) const {
      uint64_t res = v.last_non_zero(N);
      if (res >= N)
        return 0;
      else
        return res;
    }
    vect ith_child(vect v, uint64_t i) const {
      v.v[i]++;
      return v;
    }

    struct ResultList {
      using type        = list_generator;
      using type_result = list;
      static void update(type& lst, vect v) {
        lst.push_back(v);
      }
      static type_result get_value(type& lst) {
        return CILK_GET_VALUE(lst);
      }
    };

    struct ResultCounter {
      using type        = counter;
      using type_result = uint64_t;
      static void update(type& counter, vect v) {
        counter++;
      }
      static type_result get_value(type& counter) {
        return CILK_GET_VALUE(counter);
      }
    };

    template <class Res>
    void walk_tree(vect                v,
                   typename Res::type& res,
                   uint64_t            target_depth,
                   uint64_t            depth,
                   uint64_t            max_part,
                   BFS_storage&        store) const;

    template <class Res>
    void walk_tree_evaluation(vect                v,
                              typename Res::type& res,
                              vect                eval,
                              uint64_t            sum_eval,
                              BFS_storage&        store) const;
  };

  template <class perm>
  std::ostream& operator<<(std::ostream&                 stream,
                           const PermutationGroup<perm>& g) {
    return stream << g.name;
  }

  template <class perm>
  bool PermutationGroup<perm>::check_sgs() const {
    for (uint64_t level = 0; level < sgs.size(); level++) {
      if (sgs[level][0] != perm::one())
        return false;
      for (const perm& v : sgs[level]) {
        if (not v.is_permutation(N))
          return false;
        for (uint64_t i = 0; i < level; i++)
          if (not(v[i] == i))
            return false;
      }
    }
    return true;
  }

  template <class perm>
  bool PermutationGroup<perm>::is_canonical(vect              v,
                                            TemporaryStorage& st) const {
    set<vect>& to_analyse     = st.first;
    set<vect>& new_to_analyse = st.second;

    to_analyse.clear();
    new_to_analyse.clear();
    to_analyse.insert(v);

    for (uint64_t i = 0; i < N - 1; i++) {
      new_to_analyse.clear();
      const auto& transversal = sgs[i];
      for (const vect& list_test : to_analyse) {
        // transversal always start with the identity.
        if (v[i] == list_test[i])
          new_to_analyse.insert(list_test);
        for (auto it = transversal.begin() + 1; it != transversal.end(); it++) {
          const vect child = list_test.permuted(*it);
          // Slight change from Borie's algorithm's: we do a full lex comparison
          // first.
          uint64_t first_diff = v.first_diff(child);
          if ((first_diff < N) and v[first_diff] < child[first_diff])
            return false;
          if (first_diff > i)
            new_to_analyse.insert(child);
        }
      }
      std::swap(to_analyse, new_to_analyse);
    }
    return true;
  }

  // #define SET_SIZE_STATISTIC

#ifdef SET_SIZE_STATISTIC
  size_t set_number = 0;
  size_t set_size   = 0;
#endif

  template <>
  bool PermutationGroup<Perm16>::is_canonical(vect              v,
                                              TemporaryStorage& st) const {
    set<vect>& to_analyse     = st.first;
    set<vect>& new_to_analyse = st.second;

    to_analyse.clear();
    new_to_analyse.clear();
    to_analyse.insert(v);

    for (uint64_t i = 0; i < N - 1; i++) {
      new_to_analyse.clear();
      const auto& transversal = sgs[i];
      for (const vect& list_test : to_analyse) {
        // transversal always start with the identity.
        if (v[i] == list_test[i])
          new_to_analyse.insert(list_test);
        for (auto it = transversal.begin() + 1; it != transversal.end(); it++) {
          const vect child = list_test.permuted(*it);
          // Slight change from Borie's algorithm's: we do a full lex comparison
          // first.
          const uint64_t diff = ~unsigned(
              simde_mm_movemask_epi8(simde_mm_cmpeq_epi8(v.v, child.v)));
          const uint64_t lt = unsigned(
              simde_mm_movemask_epi8(simde_mm_cmplt_epi8(v.v, child.v)));
          const uint64_t first_diff = diff & (-diff);
          if (first_diff & lt)
            return false;
          if (!(diff & (1 << i)))
            new_to_analyse.insert(child);
        }
      }
#ifdef SET_SIZE_STATISTIC
      set_number++;
      set_size += new_to_analyse.size();
#endif
      std::swap(to_analyse, new_to_analyse);
    }
    return true;
  }

  template <class perm>
  bool PermutationGroup<perm>::is_canonical(vect v) const {
    TemporaryStorage storage;
    return is_canonical(v, storage);
  }

  template <class perm>
  auto PermutationGroup<perm>::canonical(vect v, TemporaryStorage& st) const
      -> vect {
    set<vect>& to_analyse     = st.first;
    set<vect>& new_to_analyse = st.second;

    to_analyse.clear();
    new_to_analyse.clear();
    to_analyse.insert(v);

    for (uint64_t i = 0; i < N - 1; i++) {
      new_to_analyse.clear();
      const auto& transversal = sgs[i];
      for (const vect& list_test : to_analyse) {
        if (v[i] == list_test[i])
          new_to_analyse.insert(list_test);
        for (auto it = transversal.begin() + 1; it != transversal.end(); it++) {
          const vect child = list_test.permuted(*it);
          // TODO: find a better algorithm !
          // TODO: the following doesn't work:
          //       if (v.less_partial(child, i+1) < 0) v = child;
          //       if (v < child) v = child;
          if (v < child)
            return canonical(child, st);
          else if (v.first_diff(child) > i)
            new_to_analyse.insert(child);
        }
      }
      std::swap(to_analyse, new_to_analyse);
    }
    return v;
  }

  template <class perm>
  auto PermutationGroup<perm>::canonical(vect v) const -> vect {
    TemporaryStorage storage;
    return canonical(v, storage);
  }

  template <class perm>
  template <class Res>
  void PermutationGroup<perm>::walk_tree(vect                v,
                                         typename Res::type& res,
                                         uint64_t            target_depth,
                                         uint64_t            depth,
                                         uint64_t            max_part,
                                         BFS_storage&        store) const {
    if (depth == target_depth)
      Res::update(res, v);
    else {
      uint64_t i = first_child_index(v);
      if (v[i] >= max_part)
        i++;
      for (/**/; i < N; i++) {
        vect child = ith_child(v, i);
        if (is_canonical(child, store.get_store()))
          cilk_spawn this->walk_tree<Res>(
              child, res, target_depth, depth + 1, max_part, store);
      }
    }
  }

  template <class perm>
  template <class Res>
  typename Res::type_result
  PermutationGroup<perm>::elements_of_depth_walk(uint64_t depth,
                                                 uint64_t max_part) const {
    vect               zero_vect{};
    typename Res::type res{};
    BFS_storage        store{};
#ifdef SET_SIZE_STATISTIC
    set_number = 0;
    set_size   = 0;
#endif
    walk_tree<Res>(zero_vect, res, depth, 0, max_part, store);
#ifdef SET_SIZE_STATISTIC
    std::cout << "Number of sets = " << set_number
              << ", Mean size = " << 1. * set_size / set_number << std::endl;
#endif
    return Res::get_value(res);
  }

  template <class perm>
  auto PermutationGroup<perm>::elements_of_depth(uint64_t depth) const -> list {
    return elements_of_depth_walk<ResultList>(depth, depth);
  }
  template <class perm>
  auto PermutationGroup<perm>::elements_of_depth(uint64_t depth,
                                                 uint64_t max_part) const
      -> list {
    return elements_of_depth_walk<ResultList>(depth, max_part);
  }

  template <class perm>
  uint64_t
  PermutationGroup<perm>::elements_of_depth_number(uint64_t depth) const {
    return elements_of_depth_walk<ResultCounter>(depth, depth);
  }
  template <class perm>
  uint64_t
  PermutationGroup<perm>::elements_of_depth_number(uint64_t depth,
                                                   uint64_t max_part) const {
    return elements_of_depth_walk<ResultCounter>(depth, max_part);
  }

  template <class perm>
  template <class Res>
  void PermutationGroup<perm>::walk_tree_evaluation(vect                v,
                                                    typename Res::type& res,
                                                    vect                eval,
                                                    uint64_t     sum_eval,
                                                    BFS_storage& store) const {
    // Invariant: sum = sum(eval)
    if (sum_eval == eval[0]) {
      Res::update(res, v);
      return;
    }
    uint64_t first = v.last_non_zero(N);
    if (first >= N)
      first = 0;
    else
      first++;
    // std::cout << v << " eval = " << eval << ", sum = " << sum_eval <<
    // std::endl;
    for (uint64_t i = 0; i <= eval[0]; i++) {
      for (uint64_t ival = 1; ival < N; ival++) {
        if (eval[ival] > 0) {
          vect child       = v;
          child[first + i] = ival;
          vect new_eval    = eval;
          new_eval[ival]--;
          new_eval[0] -= i;
          if (is_canonical(child, store.get_store())) {
            cilk_spawn this->walk_tree_evaluation<Res>(
                child, res, new_eval, sum_eval - 1 - i, store);
          }
        }
      }
    }
  }

  template <class perm>
  template <class Res>
  typename Res::type_result
  PermutationGroup<perm>::elements_of_evaluation_walk(vect eval) const {
    vect               zero_vect{};
    uint64_t           sum = 0;
    typename Res::type res{};
    BFS_storage        store{};
#ifdef SET_SIZE_STATISTIC
    set_number = 0;
    set_size   = 0;
#endif
    for (size_t i = 0; i < N; i++) {
      sum += eval[i];
    }
    assert(sum == N);
    walk_tree_evaluation<Res>(zero_vect, res, eval, sum, store);
#ifdef SET_SIZE_STATISTIC
    std::cout << "Number of sets = " << set_number
              << ", Mean size = " << 1. * set_size / set_number << std::endl;
#endif
    return Res::get_value(res);
  }

  template <class perm>
  auto PermutationGroup<perm>::elements_of_evaluation(vect eval) const -> list {
    return elements_of_evaluation_walk<ResultList>(eval);
  }

}  //  namespace IVMPG

#endif
