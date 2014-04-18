#ifndef _PERM16_HPP
#define _PERM16_HPP

#include <cstdint>
#include <x86intrin.h>
#include <array>

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


struct Vect16
{
  union {
    vect16 p;
    __m128i v;
    epi8 v8;
  };

  uint8_t operator[](uint64_t i) const { return p[i]; }
  uint8_t &operator[](uint64_t i) { return p[i]; }

  bool operator==(const Vect16 &vp) const {
    #ifdef GCC_VECT_CMP
    return _mm_movemask_epi8(v8 == vp.v8)==0xffff;
    #else
    return _mm_movemask_epi8(_mm_cmpeq_epi8(v, vp.v))==0xffff;
    #endif
  }

  bool operator < (const Vect16 &b) const {
    uint64_t diff = _mm_cmpestri (v, 16, b.v, 16, FIRST_DIFF);
    // Check for speed:
    // return (diff == 16) ? false : p[diff] < b.p[diff];
    #ifdef GCC_VECT_CMP
    uint64_t diffs = _mm_movemask_epi8(v8 < b.v8);
    #else
    uint64_t diffs = _mm_movemask_epi8(_mm_cmplt_epi8(v, b.v));
    #endif
    return (diffs >> diff) & 0x1;;
  }

  char less_partial(const Vect16 &b, int k) const {
    uint64_t diff = _mm_cmpestri (v, k, b.v, k, FIRST_DIFF);
    return (diff == 16) ? 0 : char(p[diff]) - char(b.p[diff]);
  }

  Vect16 permuted(const Vect16 &other) const {
    Vect16 res;
    res.v = _mm_shuffle_epi8(v, other.v);
    return res;
  };

  template <char IDX_MODE>
  uint64_t search_index(int bound = 16) const {
    const __m128i zero {0, 0};
    return _mm_cmpestri(zero, 1, v, bound, IDX_MODE);
  }

  bool is_permutation(const size_t N = 16) const {
    constexpr const __m128i idv = __m128i(epi8 {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15});
    uint64_t diff = _mm_cmpestri(v, 16, idv, 16, LAST_DIFF);
    return
      _mm_cmpestri(idv, 16, v, 16, FIRST_NON_ZERO) == 16 and // all(x in idv for x in v)
      _mm_cmpestri(v, 16, idv, 16, FIRST_NON_ZERO) == 16 and // all(x in v for x in idv)
      (diff == 16 or diff < N);     // v = idv    or    last diff index < N
      ;
  }

} __attribute__ ((aligned (16)));

namespace std {

  template<>
  struct hash<Vect16> {
    size_t operator () (const Vect16 &ar) const {
    #ifdef GCC_VECT_CMP
      return ar.v[1] ^ ar.v[0];
    #else
      size_t h = 0;
      for (int i=0; i<16; i++) h = hash<uint8_t>()(ar[i]) + (h << 6) + (h << 16) - h;
      return h;
    #endif
    }
  };

  // TODO test for speed
  // template<>
  // struct less<Vect16> {
  //   size_t operator () (const Vect16 &v1, const Vect16 &v2) const {
  //     return v1[0] == v2[0] ? v1[1] < v2[1] : v1[0] < v2[0];
  //   }
  // };
}

#include <ostream>
std::ostream & operator<<(std::ostream & stream, const Vect16 &term);

struct Perm16 : public Vect16 {

  using vect = Vect16;

  Perm16() : Vect16({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}) {};
  Perm16(std::initializer_list<uint8_t> il) {
    uint64_t i=0;
    for (auto x : il) { v8[i++] = x; }
    for (/**/; i<16; i++) v8[i] = i;
  }

  Perm16 operator*(const Perm16&p) const { return permuted(p); }
  static Perm16 one() { return {}; }
  static Perm16 elementary_transposition(uint64_t i) {
    Perm16 res {}; res[i]=i+1; res[i+1]=i; return res; }

private:

  Perm16(const vect v) : Vect16(v) {};

};

static_assert(sizeof(Vect16) == sizeof(Perm16),
	      "Vect16 ans Perm16 have a different memory layout !");

#endif

