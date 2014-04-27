#ifndef _PERM16_HPP
#define _PERM16_HPP

#include <cstdint>
#include <x86intrin.h>
#include <array>
#include <cassert>

#include "config.h"

using vect16 = std::array<uint8_t, 16> ;
using epi8 = uint8_t __attribute__ ((vector_size (16)));

// Comparison mode for _mm_cpmestri
const char FIRST_DIFF = (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
const char LAST_DIFF = (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH |
			_SIDD_NEGATIVE_POLARITY | _SIDD_MOST_SIGNIFICANT );
const char FIRST_ZERO = (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY );
const char LAST_ZERO = (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_MOST_SIGNIFICANT);
const char FIRST_NON_ZERO = (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY |
			     _SIDD_MASKED_NEGATIVE_POLARITY);
const char LAST_NON_ZERO = (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY |
			    _SIDD_MASKED_NEGATIVE_POLARITY | _SIDD_MOST_SIGNIFICANT);


struct alignas(16) Vect16
{
  union {
    vect16 p;
    __m128i v;
    epi8 v8;
  };

  static const constexpr size_t N = 16;

  uint8_t operator[](uint64_t i) const { return p[i]; }
  uint8_t &operator[](uint64_t i) { return p[i]; }

  uint64_t first_diff(const Vect16 &b, size_t bound = N) const {
    return _mm_cmpestri (v, bound, b.v, bound, FIRST_DIFF);
  }

  bool operator==(const Vect16 &b) const {
    return first_diff(b) == N;
  }

  bool operator < (const Vect16 &b) const {
    uint64_t diff = first_diff(b);
    return (diff != N) and p[diff] < b[diff];
  }

  char less_partial(const Vect16 &b, int k) const {
    uint64_t diff = first_diff(b, k);
    return (diff == N) ? 0 : char(p[diff]) - char(b[diff]);
  }


  Vect16 permuted(const Vect16 &other) const {
    Vect16 res;
    res.v = _mm_shuffle_epi8(v, other.v);
    return res;
  };

  template <char IDX_MODE>
  uint64_t search_index(int bound) const {
    const __m128i zero {0, 0};
    return _mm_cmpestri(zero, 1, v, bound, IDX_MODE);
  }

  uint64_t last_non_zero(int bound=N) const { return search_index<LAST_NON_ZERO>(bound); }
  uint64_t first_non_zero(int bound=N) const { return search_index<FIRST_NON_ZERO>(bound); }
  uint64_t last_zero(int bound=N) const { return search_index<LAST_ZERO>(bound); }
  uint64_t first_zero(int bound=N) const { return search_index<FIRST_ZERO>(bound); }

  bool is_permutation(const size_t k = N) const {
    constexpr const __m128i idv = __m128i(epi8 {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15});
    uint64_t diff = _mm_cmpestri(v, N, idv, N, LAST_DIFF);
    return
      _mm_cmpestri(idv, N, v, N, FIRST_NON_ZERO) == N and // all(x in idv for x in v)
      _mm_cmpestri(v, N, idv, N, FIRST_NON_ZERO) == N and // all(x in v for x in idv)
      (diff == N or diff < k);     // v = idv    or    last diff index < N
  }

};

namespace std {

  template<>
  struct hash<Vect16> {
    size_t operator () (const Vect16 &ar) const {
    #ifdef GCC_VECT_CMP
      return ar.v[1] ^ ar.v[0];
    #else
      size_t h = 0;
      for (int i=0; i<Vect16::N; i++) h = hash<uint8_t>()(ar[i]) + (h << 6) + (h << 16) - h;
      return h;
    #endif
    }
  };

  template<>
  struct less<Vect16> {
    // WARNING: due to endianess this is not lexicographic comparison,
    //          but we don't care when using is std::set.
    // 10% faster than calling the lexicographic comparison operator !
    // returning a size_t is 2% even faster !
    size_t operator () (const Vect16 &v1, const Vect16 &v2) const {
      return v1.v[0] == v2.v[0] ? v1.v[1] < v2.v[1] : v1.v[0] < v2.v[0];
    }

  };
}

#include <ostream>
std::ostream & operator<<(std::ostream & stream, const Vect16 &term);

struct Perm16 : public Vect16 {

  using vect = Vect16;

  Perm16() : Vect16({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}) {};
  Perm16(std::initializer_list<uint8_t> il) {
    assert (il.size() <= vect::N);
    std::copy(il.begin(), il.end(), this->p.begin());
    for (uint64_t i = il.size(); i<vect::N; i++) this->p[i] = i;
  }
  Perm16 operator*(const Perm16&p) const { return permuted(p); }
  static Perm16 one() { return {}; }
  static Perm16 elementary_transposition(uint64_t i) {
    assert (i < vect::N);
    Perm16 res {}; res[i]=i+1; res[i+1]=i; return res; }

private:

  Perm16(const vect v) : vect(v) {};

};

static_assert(sizeof(Vect16) == sizeof(Perm16),
	      "Vect16 and Perm16 have a different memory layout !");

#endif

