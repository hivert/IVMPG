#ifndef _PERM_GENERIC_HPP
#define _PERM_GENERIC_HPP

#include <cstdint>
#include <array>
#include <cassert>
#include <algorithm>

#include "config.h"

template < size_t _N, typename Expo=uint8_t >
struct VectGeneric {

  static const constexpr size_t N = _N;
  std::array<Expo, N> p;

  Expo operator[](uint64_t i) const { return p[i]; }
  Expo &operator[](uint64_t i) { return p[i]; }

  inline uint64_t first_diff(const VectGeneric &b, size_t bound = N) const {
    for (uint64_t i=0; i<bound; i++) if (p[i] != b[i]) return i;
    return N;
  }

  bool operator==(const VectGeneric &b) const {
    return first_diff(b) == N;
  }

  bool operator < (const VectGeneric &b) const {
    uint64_t diff = first_diff(b);
    return (diff != N) and p[diff] < b[diff];
  }

  char less_partial(const VectGeneric &b, int k) const {
    uint64_t diff = first_diff(b, k);
    return (diff == N) ? 0 : char(p[diff]) - char(b[diff]);
  }

  VectGeneric permuted(const VectGeneric &other) const {
    VectGeneric res;
    for (uint64_t i=0; i<N; i++) res.p[i] = p[other[i]];
    return res;
  };

  uint64_t first_non_zero(int bound=N) const {
    for (uint64_t i=0; i<bound; i++) if (p[i] != 0) return i;
    return N;
  }
  uint64_t first_zero(int bound=N) const {
    for (uint64_t i=0; i<bound; i++) if (p[i] == 0) return i;
    return N;
  }
  uint64_t last_non_zero(int bound=16) const {
    for (int64_t i=bound-1; i >= 0; i--) if (p[i] != 0) return i;
    return N;
  }
  uint64_t last_zero(int bound=16) const {
    for (int64_t i=bound-1; i >= 0; i--) if (p[i] == 0) return i;
    return N;
  }

  bool is_permutation(const size_t k = N) const {
    auto temp = this->p;
    std::sort(temp.begin(), temp.end());
    for (uint64_t i=0; i<N; i++) if (temp[i] != i) return false;
    for (uint64_t i=k; i<N; i++) if (p[i] != i) return false;
    return true;
  }
};

// Definition since previously *only* declared
template < size_t _N, typename Expo >
const constexpr size_t VectGeneric<_N,Expo>::N;

namespace std {

  template<>
  template < size_t N, typename Expo >
  struct hash< VectGeneric<N,Expo> > {
    size_t operator () (const VectGeneric<N,Expo> &ar) const {
      size_t h = 0;
      for (int i=0; i<VectGeneric<N,Expo>::N; i++)
	h = hash<Expo>()(ar[i]) + (h << 6) + (h << 16) - h;
      return h;
    }
  };

}


#include <ostream>
template < size_t N, typename Expo >
std::ostream & operator<<(std::ostream & stream, const VectGeneric<N,Expo> &term){
  stream << "[" << unsigned(term[0]);
  for (unsigned i=1; i < N; i++) stream << "," << unsigned(term[i]);
  stream << "]";
  return stream;
}


template < size_t _N, typename Expo=uint8_t >
struct PermGeneric : public VectGeneric<_N, Expo> {

  using vect = VectGeneric<_N, Expo>;

  PermGeneric() { for (uint64_t i=0; i<=_N; i++) this->p[i] = i; };
  PermGeneric(std::initializer_list<Expo> il) {
    assert (il.size() <= vect::N);
    std::copy(il.begin(), il.end(), this->p.begin());
    for (uint64_t i = il.size(); i<vect::N; i++) this->p[i] = i;
  }

  PermGeneric operator*(const PermGeneric&p) const { return this->permuted(p); }
  static PermGeneric one() { return {}; }
  static PermGeneric elementary_transposition(uint64_t i) {
    assert (i < vect::N);
    PermGeneric res {}; res[i]=i+1; res[i+1]=i; return res; }

private:

  PermGeneric(const vect v) : VectGeneric<_N, Expo>(v) {};

};

#endif
