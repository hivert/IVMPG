#ifndef _PERM16_HPP
#define _PERM16_HPP

#include <cstdint>
#include <x86intrin.h>
#include <array>

using vect16 = std::array<uint8_t, 16> ;
using epi8 = uint8_t __attribute__ ((vector_size (16)));

const char FIRST_DIFF = (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
const char FIRST_ZERO = (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY );
const char LAST_ZERO = (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_MOST_SIGNIFICANT);
const char FIRST_NON_ZERO = (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY |
			     _SIDD_MASKED_NEGATIVE_POLARITY);
const char LAST_NON_ZERO = (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY |
			    _SIDD_MASKED_NEGATIVE_POLARITY | _SIDD_MOST_SIGNIFICANT);

struct Perm16
{
  union {
    vect16 p;
    __m128i v;
    epi8 v8;
  };

  uint8_t operator[](uint64_t i) const { return p[i]; }
  uint8_t &operator[](uint64_t i) { return p[i]; }

  bool operator==(const Perm16 &vp) const {
    #ifdef GCC_VECT_CMP
    return _mm_movemask_epi8(v8 == vp.v8)==0xffff;
    #else
    return _mm_movemask_epi8(_mm_cmpeq_epi8(v, vp.v))==0xffff;
    #endif
  }

  bool operator < (const Perm16 &b) const {
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

  int less_partial(const Perm16 &b, int k) const {
    uint64_t diff = _mm_cmpestri (v, k, b.v, k, FIRST_DIFF);
    return (diff == 16) ? 0 : char(p[diff]) - char(b.p[diff]);
  }

  Perm16 operator*(const Perm16 &other) const {
    Perm16 res;
    res.v = _mm_shuffle_epi8(v, other.v);
    return res;
  };

  template <char IDX_MODE>
  uint64_t search_index(int bound = 16) const {
    return _mm_cmpestri((__m128i {0,0}), 1, v, bound, IDX_MODE);
  }

  bool is_permutation() const {
    Perm16 p {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    return
      _mm_cmpestri(p.v, 16, v, 16, FIRST_NON_ZERO) == 16 and
      _mm_cmpestri(v, 16, p.v, 16, FIRST_NON_ZERO) == 16;
  }

} __attribute__ ((aligned (16)));

std::ostream & operator<<(std::ostream & stream, const Perm16 &term);

namespace std {

  template<>
  struct hash<Perm16> {
    size_t operator () (const Perm16 &ar) const {
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
  // struct less<Perm16> {
  //   size_t operator () (const Perm16 &v1, const Perm16 &v2) const {
  //     return v1[0] == v2[0] ? v1[1] < v2[1] : v1[0] < v2[0];
  //   }
  // };
}


#endif

