#define BOOST_TEST_MODULE perm_generic

#include "config.h"

#ifdef BOOST_TEST_USE_LIB
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>
#endif
#ifdef BOOST_TEST_USE_INCLUDE
#define BOOST_TEST_NO_LIB
#include <boost/test/included/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>
#endif

//____________________________________________________________________________//


template <class VectType> struct IsPermFunctions {
  static bool is_perm(const VectType a)             {return     a.is_permutation();};
  static bool is_not_perm(const VectType a)         {return not a.is_permutation();};
  static bool is_perm2(const VectType a, int i)     {return     a.is_permutation(i);};
  static bool is_not_perm2(const VectType a, int i) {return not a.is_permutation(i);};
};

//____________________________________________________________________________//

template <class PermType>
struct Fixture : public IsPermFunctions<typename PermType::vect> {

  using VectType = typename PermType::vect;
  Fixture() : zero({0}), P01({0,1}), P10({1,0}), P11({1,1}),
	      P1( { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}),
	      PPa(completePerm({ 1, 2, 3, 4, 0, 5, 6, 7, 8, 9,10,11,12,13,14,15})),
	      PPb(completePerm({ 1, 2, 3, 6, 0, 5, 6, 7, 8, 9,10,11,12,13,14,15})),
	      czero(zero), cP01 (P01),
	      RandPerm(completePerm({3,1,0,14,15,13,5,10,2,11,6,12,7,4,8,9})),
	      Plist({zero, P01, P10, P11, P1, PPa, PPb, RandPerm})
  { BOOST_TEST_MESSAGE( "setup fixture" ); }
  ~Fixture()         { BOOST_TEST_MESSAGE( "teardown fixture" ); }

  VectType zero, P01, P10, P11, P1, PPa, PPb;
  const VectType czero, cP01, RandPerm;
  const std::vector<VectType> Plist;

  static VectType complete(VectType v, int k) {
    for (size_t i=16; i<VectType::Size; i++) v[i] = k;
    return v;
  }
  static VectType completePerm(VectType v) {
    for (size_t i=16; i<VectType::Size; i++) v[i] = i;
    return v;
  }

  static bool less(const VectType a, const VectType b) {return a < b;};
  static bool not_less(const VectType a, const VectType b) {return not(a < b);};
};

//____________________________________________________________________________//

#include "perm16.hpp"
#include "perm_generic.hpp"

typedef boost::mpl::list<
  Fixture< IVMPG::Perm16 >,
  Fixture< IVMPG::PermGeneric<16> >,
  Fixture< IVMPG::PermGeneric<32> >
> Fixtures;

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE( VectType_test )
//____________________________________________________________________________//

BOOST_FIXTURE_TEST_CASE_TEMPLATE( sizeof_test, F, Fixtures, F )
{
  BOOST_CHECK_EQUAL( sizeof(F::zero), F::VectType::Size );
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( equal_test, F, Fixtures, F )
{
  BOOST_CHECK_EQUAL( F::zero, F::zero );
  BOOST_CHECK_NE( F::zero, F::P01 );
  for (unsigned i = 0; i<F::Plist.size(); i++)
    for (unsigned j = 0; j<F::Plist.size(); j++)
      if (i == j)
	BOOST_CHECK_EQUAL( F::Plist[i], F::Plist[j] );
      else
	BOOST_CHECK_NE( F::Plist[i], F::Plist[j] );
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( operator_bracket_const_test, F, Fixtures, F )
{
  BOOST_CHECK_EQUAL( F::czero[0], 0u );
  BOOST_CHECK_EQUAL( F::czero[1], 0u );
  BOOST_CHECK_EQUAL( F::czero[15], 0u );
  BOOST_CHECK_EQUAL( F::cP01[0], 0u );
  BOOST_CHECK_EQUAL( F::cP01[1], 1u );
  BOOST_CHECK_EQUAL( F::cP01[2], 0u );
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( operator_bracket_test, F, Fixtures, F )
{
  BOOST_CHECK_EQUAL( F::zero[0], 0u );
  BOOST_CHECK_EQUAL( F::zero[1], 0u );
  BOOST_CHECK_EQUAL( F::zero[15], 0u );
  BOOST_CHECK_EQUAL( F::P01[0], 0u );
  BOOST_CHECK_EQUAL( F::P01[1], 1u );
  BOOST_CHECK_EQUAL( F::P01[2], 0u );
  BOOST_CHECK_EQUAL( F::PPa[4], 0u );
  BOOST_CHECK_EQUAL( F::PPa[5], 5u );
  F::zero[0] = 3;
  BOOST_CHECK_EQUAL( F::zero[0], 3u );
  BOOST_CHECK_EQUAL( F::zero[1], 0u );
  BOOST_CHECK_EQUAL( F::zero[15], 0u );
  F::PPa[2] = 0;
  BOOST_CHECK_EQUAL( F::PPa[1], 2u );
  BOOST_CHECK_EQUAL( F::PPa[2], 0u );
  BOOST_CHECK_EQUAL( F::PPa[3], 4u );
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( operator_less_test, F, Fixtures, F )
{
  for (unsigned i = 0; i<F::Plist.size(); i++)
    for (unsigned j = 0; j<F::Plist.size(); j++)
      if (i < j) BOOST_CHECK_PREDICATE(F::less,     (F::Plist[i])(F::Plist[j]) );
      else       BOOST_CHECK_PREDICATE(F::not_less, (F::Plist[i])(F::Plist[j]) );
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( operator_less_partial_test, F, Fixtures, F )
{
  // for (auto p : F::Plist)
  for (auto it = F::Plist.begin(); it != F::Plist.end(); ++it) {
    auto p = *it;
    for (unsigned k = 0; k<16; k++)
      BOOST_CHECK_EQUAL( p.less_partial(p, k), 0 );
  }
  // for (auto p : F::Plist)
  //   for (auto q : F::Plist)
  for (auto itp = F::Plist.begin(); itp != F::Plist.end(); ++itp) {
    auto p = *itp;
    for (auto itq = F::Plist.begin(); itq != F::Plist.end(); ++itq) {
      auto q = *itq;
      BOOST_CHECK_EQUAL( p.less_partial(q, 0), 0 );
    }
  }

  BOOST_CHECK_EQUAL( F::zero.less_partial(F::P01, 1), 0 );
  BOOST_CHECK_EQUAL( F::P01.less_partial(F::zero, 1), 0 );
  BOOST_CHECK_LT( F::zero.less_partial(F::P01, 2), 0 );
  BOOST_CHECK_GT( F::P01.less_partial(F::zero, 2), 0 );

  BOOST_CHECK_LT( F::zero.less_partial(F::P10, 1), 0 );
  BOOST_CHECK_LT( F::zero.less_partial(F::P10, 2), 0 );
  BOOST_CHECK_GT( F::P10.less_partial(F::zero, 1), 0 );
  BOOST_CHECK_GT( F::P10.less_partial(F::zero, 2), 0 );

  BOOST_CHECK_EQUAL( F::PPa.less_partial(F::PPb, 1), 0 );
  BOOST_CHECK_EQUAL( F::PPa.less_partial(F::PPb, 2), 0 );
  BOOST_CHECK_EQUAL( F::PPa.less_partial(F::PPb, 3), 0 );
  BOOST_CHECK_LT( F::PPa.less_partial(F::PPb, 4), 0 );
  BOOST_CHECK_LT( F::PPa.less_partial(F::PPb, 5), 0 );
  BOOST_CHECK_GT( F::PPb.less_partial(F::PPa, 4), 0 );
  BOOST_CHECK_GT( F::PPb.less_partial(F::PPa, 5), 0 );
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( first_zero_test, F, Fixtures, F )
{
  BOOST_CHECK_EQUAL( F::zero.first_zero(), 0u);
  BOOST_CHECK_EQUAL( F::P01.first_zero(), 0u);
  BOOST_CHECK_EQUAL( F::PPa.first_zero(), 4u);
  BOOST_CHECK_EQUAL( F::P10.first_zero(), 1u);
  BOOST_CHECK_EQUAL( F::P1.first_zero(), 16u);
  BOOST_CHECK_EQUAL( F::P10.first_zero(1), F::VectType::Size);
  BOOST_CHECK_EQUAL( F::PPa.first_zero(5), 4u);
  BOOST_CHECK_EQUAL( F::PPa.first_zero(3), F::VectType::Size);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( last_zero_test, F, Fixtures, F )
{
  BOOST_CHECK_EQUAL( F::zero.last_zero(), 15u);
  BOOST_CHECK_EQUAL( F::P01.last_zero(), 15u);
  BOOST_CHECK_EQUAL( F::PPa.last_zero(), 4u);
  BOOST_CHECK_EQUAL( F::P1.last_zero(), F::VectType::Size);
  BOOST_CHECK_EQUAL( F::P01.last_zero(1), 0u);
  BOOST_CHECK_EQUAL( F::P10.last_zero(1), F::VectType::Size);
  BOOST_CHECK_EQUAL( F::PPa.last_zero(5), 4u);
  BOOST_CHECK_EQUAL( F::PPa.last_zero(3), F::VectType::Size);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( first_non_zero_test, F, Fixtures, F )
{
  BOOST_CHECK_EQUAL( F::zero.first_non_zero(), F::VectType::Size);
  BOOST_CHECK_EQUAL( F::P01.first_non_zero(), 1u);
  BOOST_CHECK_EQUAL( F::PPa.first_non_zero(), 0u);
  BOOST_CHECK_EQUAL( F::P01.first_non_zero(), 1u);
  BOOST_CHECK_EQUAL( F::P01.first_non_zero(1), F::VectType::Size);
  BOOST_CHECK_EQUAL( F::PPa.first_non_zero(5), 0u);
  BOOST_CHECK_EQUAL( F::PPa.first_non_zero(3), 0u);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( last_non_zero_test, F, Fixtures, F )
{
  BOOST_CHECK_EQUAL( F::zero.last_non_zero(), F::VectType::Size);
  BOOST_CHECK_EQUAL( F::P01.last_non_zero(), 1u);
  BOOST_CHECK_EQUAL( F::PPa.last_non_zero(), 15u);
  BOOST_CHECK_EQUAL( F::P01.last_non_zero(), 1u);
  BOOST_CHECK_EQUAL( F::P01.last_non_zero(1), F::VectType::Size);
  BOOST_CHECK_EQUAL( F::PPa.last_non_zero(5), 3u);
  BOOST_CHECK_EQUAL( F::PPa.last_non_zero(3), 2u);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( permuted_test, F, Fixtures, F )
{
  BOOST_CHECK_EQUAL( F::zero.permuted(F::zero), F::zero );
  BOOST_CHECK_EQUAL( F::P01.permuted(F::P01), F::P01 );
  BOOST_CHECK_EQUAL( F::P10.permuted(F::P10),
		     F::complete(typename F::VectType({0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}), 1)
		   );
  BOOST_CHECK_EQUAL( F::P10.permuted(F::P01),
		     F::complete(typename F::VectType({1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1}), 1)
		   );
  BOOST_CHECK_EQUAL( F::P01.permuted(F::P10), F::P10 );
// TODO: check mult
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE( operator_insert_test, F, Fixtures, F )
{
  std::ostringstream out, out2;
  out << F::zero;
  out2 << "[0";
  for (size_t i=1; i<F::VectType::Size; i++) out2 << ",0"; out2 << "]";
  BOOST_CHECK_EQUAL(out.str(), out2.str());

  out.str(""); out2.str("");
  out << F::P01;
  out2 << "[0,1";
  for (size_t i=2; i<F::VectType::Size; i++) out2 << ",0"; out2 << "]";
  BOOST_CHECK_EQUAL(out.str(), out2.str());

  out.str(""); out2.str("");
  out << F::PPa;
  out2 << "[1,2,3,4,0";
  for (size_t i=5; i<F::VectType::Size; i++) out2 << "," << i; out2 << "]";
  BOOST_CHECK_EQUAL(out.str(), out2.str());
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( is_permutation_test, F, Fixtures, F )
{
  BOOST_CHECK_PREDICATE(F::is_not_perm, (F::zero));
  BOOST_CHECK_PREDICATE(F::is_perm2, (F::PPa)(16));
  BOOST_CHECK_PREDICATE(F::is_not_perm, (F::PPb));
  BOOST_CHECK_PREDICATE(F::is_perm, (F::RandPerm));
  BOOST_CHECK_PREDICATE(F::is_not_perm,
			(typename F::VectType({3,1,0,14,15,13,3,10,2,11,6,12,7,4,8,9})));
  BOOST_CHECK_PREDICATE(F::is_not_perm2, (F::RandPerm)(4));
  BOOST_CHECK_PREDICATE(F::is_perm2, (F::PPa)(5));
  BOOST_CHECK_PREDICATE(F::is_not_perm2, (F::PPa)(4));
}

BOOST_AUTO_TEST_SUITE_END()



//____________________________________________________________________________//
//____________________________________________________________________________//

template <class _Perm>
struct PermFixture : public IsPermFunctions<_Perm> {
  using PermType = _Perm;
  PermFixture() : id(PermType::one()),
		  RandPerm({3,1,0,14,15,13,5,10,2,11,6,12,7,4,8,9}),
		  Plist({id, RandPerm})
  {
    for (uint64_t i=0; i<15; i++)
      Plist.insert(Plist.begin()+1+i, PermType::elementary_transposition(i));
    BOOST_TEST_MESSAGE( "setup fixture" );
  }

  ~PermFixture()         { BOOST_TEST_MESSAGE( "teardown fixture" ); }

  PermType id, s1, s2, s3;
  const PermType RandPerm;
  std::vector<PermType> Plist;
};

//____________________________________________________________________________//

typedef boost::mpl::list<
  PermFixture< IVMPG::Perm16 >,
  PermFixture< IVMPG::PermGeneric<16> >,
  PermFixture< IVMPG::PermGeneric<32> >
> PermFixtures;

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE( PermType_test )
//____________________________________________________________________________//


BOOST_FIXTURE_TEST_CASE_TEMPLATE( constructor_is_permutation_test, F, PermFixtures, F )
{
  //for (auto x : Plist) {
  for (auto it = F::Plist.begin(); it != F::Plist.end(); ++it) {
    auto x = *it;
    BOOST_CHECK_PREDICATE(F::is_perm, (x));
  }
  BOOST_CHECK_PREDICATE(F::is_perm, (typename F::PermType()));
  BOOST_CHECK_PREDICATE(F::is_perm, (typename F::PermType({1,0})));
  BOOST_CHECK_PREDICATE(F::is_perm, (typename F::PermType({1,2,0})));
  BOOST_CHECK_PREDICATE(F::is_not_perm, (typename F::PermType({1,2})));
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( operator_mult_coxeter_test, F, PermFixtures, F )
{
  for (uint64_t i=0; i<15; i++) {
    auto si = F::PermType::elementary_transposition(i);
    BOOST_CHECK_NE(si, F::id);
    BOOST_CHECK_EQUAL(si * si, F::id);
    if (i+1 < 15) {
      auto si1 = F::PermType::elementary_transposition(i+1);
      BOOST_CHECK_EQUAL(si * si1 * si, si1 * si * si1);
    }
    for (uint64_t j=i+2; j<15; j++) {
    auto sj = F::PermType::elementary_transposition(j);
    BOOST_CHECK_EQUAL(sj * si, si * sj);
    }
  }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( operator_mult_test, F, PermFixtures, F )
{
  //for (auto x : Plist) {
  for (auto it = F::Plist.begin(); it != F::Plist.end(); ++it) {
    auto x = *it;
    BOOST_CHECK_EQUAL(F::id * x, x);
    BOOST_CHECK_EQUAL(x * F::id, x);
  }
  BOOST_CHECK_EQUAL(F::RandPerm * F::RandPerm,
		    typename F::PermType({14,1,3,8,9,4,13,6,0,12,5,7,10,15,2,11}));
}


BOOST_AUTO_TEST_SUITE_END()

