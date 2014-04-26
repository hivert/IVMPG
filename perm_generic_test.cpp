#define BOOST_TEST_DYN_LINK

#define BOOST_TEST_MODULE perm_generic
#include <boost/test/unit_test.hpp>

#include "perm_generic.hpp"

using PermType = PermGeneric<16>;
using VectType = PermType::vect;

struct Fixture {
  Fixture() : zero({0}), P01({0,1}), P10({1,0}), P11({1,1}),
	      P1( { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}),
	      PPa(completePerm({ 1, 2, 3, 4, 0, 5, 6, 7, 8, 9,10,11,12,13,14,15})),
	      PPb(completePerm({ 1, 2, 3, 6, 0, 5, 6, 7, 8, 9,10,11,12,13,14,15})),
	      czero(zero), cP01 (P01),
	      RandPerm(completePerm({3,1,0,14,15,13,5,10,2,11,6,12,7,4,8,9})),
	      Plist({zero, P01, P10, P11, P1, PPa, PPb, RandPerm})
  {
    //    completePerm(PPa); completePerm(PPb); completePerm(RandPerm);
    BOOST_TEST_MESSAGE( "setup fixture" );
  }
  ~Fixture()         { BOOST_TEST_MESSAGE( "teardown fixture" ); }

  VectType zero, P01, P10, P11, P1, PPa, PPb;
  const VectType czero, cP01, RandPerm;
  const std::vector<VectType> Plist;

  static VectType complete(VectType v, int k) {
    for (size_t i=16; i<VectType::N; i++) v[i] = k;
    return v;
  }
  static VectType completePerm(VectType v) {
    for (size_t i=16; i<VectType::N; i++) v[i] = i;
    return v;
  }
};

auto less     = [](const VectType a, const VectType b) {return a < b;};
auto not_less = [](const VectType a, const VectType b) {return not(a < b);};

auto is_perm     = [](const VectType a, int i=VectType::N) {return a.is_permutation(i);};
auto is_not_perm = [](const VectType a, int i=VectType::N) {return not a.is_permutation(i);};

//____________________________________________________________________________//

BOOST_FIXTURE_TEST_SUITE( VectType_test, Fixture )

BOOST_AUTO_TEST_CASE( equal_test )
{
  BOOST_CHECK_EQUAL( zero, zero );
  BOOST_CHECK_NE( zero, P01 );
  for (unsigned i = 0; i<Plist.size(); i++)
    for (unsigned j = 0; j<Plist.size(); j++)
      if (i == j)
	BOOST_CHECK_EQUAL( Plist[i], Plist[j] );
      else
	BOOST_CHECK_NE( Plist[i], Plist[j] );
}


BOOST_AUTO_TEST_CASE( operator_bracket_const_test )
{
  BOOST_CHECK_EQUAL( czero[0], 0 );
  BOOST_CHECK_EQUAL( czero[1], 0 );
  BOOST_CHECK_EQUAL( czero[15], 0 );
  BOOST_CHECK_EQUAL( cP01[0], 0 );
  BOOST_CHECK_EQUAL( cP01[1], 1 );
  BOOST_CHECK_EQUAL( cP01[2], 0 );
}


BOOST_AUTO_TEST_CASE( operator_bracket_test )
{
  BOOST_CHECK_EQUAL( zero[0], 0 );
  BOOST_CHECK_EQUAL( zero[1], 0 );
  BOOST_CHECK_EQUAL( zero[15], 0 );
  BOOST_CHECK_EQUAL( P01[0], 0 );
  BOOST_CHECK_EQUAL( P01[1], 1 );
  BOOST_CHECK_EQUAL( P01[2], 0 );
  BOOST_CHECK_EQUAL( PPa[4], 0 );
  BOOST_CHECK_EQUAL( PPa[5], 5 );
  zero[0] = 3;
  BOOST_CHECK_EQUAL( zero[0], 3 );
  BOOST_CHECK_EQUAL( zero[1], 0 );
  BOOST_CHECK_EQUAL( zero[15], 0 );
  PPa[2] = 0;
  BOOST_CHECK_EQUAL( PPa[1], 2 );
  BOOST_CHECK_EQUAL( PPa[2], 0 );
  BOOST_CHECK_EQUAL( PPa[3], 4 );
}


BOOST_AUTO_TEST_CASE( operator_less_test )
{
  for (unsigned i = 0; i<Plist.size(); i++)
    for (unsigned j = 0; j<Plist.size(); j++)
      if (i < j) BOOST_CHECK_PREDICATE(less, (Plist[i])(Plist[j]) );
      else       BOOST_CHECK_PREDICATE(not_less, (Plist[i])(Plist[j]) );
}


BOOST_AUTO_TEST_CASE( operator_less_partial_test )
{
  for (auto p : Plist)
    for (unsigned k = 0; k<16; k++)
      BOOST_CHECK_EQUAL( p.less_partial(p, k), 0 );
  for (auto p : Plist)
    for (auto q : Plist)
      BOOST_CHECK_EQUAL( p.less_partial(q, 0), 0 );

  BOOST_CHECK_EQUAL( zero.less_partial(P01, 1), 0 );
  BOOST_CHECK_EQUAL( P01.less_partial(zero, 1), 0 );
  BOOST_CHECK_LT( zero.less_partial(P01, 2), 0 );
  BOOST_CHECK_GT( P01.less_partial(zero, 2), 0 );

  BOOST_CHECK_LT( zero.less_partial(P10, 1), 0 );
  BOOST_CHECK_LT( zero.less_partial(P10, 2), 0 );
  BOOST_CHECK_GT( P10.less_partial(zero, 1), 0 );
  BOOST_CHECK_GT( P10.less_partial(zero, 2), 0 );

  BOOST_CHECK_EQUAL( PPa.less_partial(PPb, 1), 0 );
  BOOST_CHECK_EQUAL( PPa.less_partial(PPb, 2), 0 );
  BOOST_CHECK_EQUAL( PPa.less_partial(PPb, 3), 0 );
  BOOST_CHECK_LT( PPa.less_partial(PPb, 4), 0 );
  BOOST_CHECK_LT( PPa.less_partial(PPb, 5), 0 );
  BOOST_CHECK_GT( PPb.less_partial(PPa, 4), 0 );
  BOOST_CHECK_GT( PPb.less_partial(PPa, 5), 0 );
}

BOOST_AUTO_TEST_CASE( first_zero_test )
{
  BOOST_CHECK_EQUAL( zero.first_zero(), 0);
  
  BOOST_CHECK_EQUAL( P01.first_zero(), 0);
  BOOST_CHECK_EQUAL( PPa.first_zero(), 4);
  BOOST_CHECK_EQUAL( P10.first_zero(), 1);
  BOOST_CHECK_EQUAL( P1.first_zero(), 16);
  BOOST_CHECK_EQUAL( P10.first_zero(1), VectType::N);
  BOOST_CHECK_EQUAL( PPa.first_zero(5), 4);
  BOOST_CHECK_EQUAL( PPa.first_zero(3), VectType::N);
}

BOOST_AUTO_TEST_CASE( last_zero_test )
{
  BOOST_CHECK_EQUAL( zero.last_zero(), 15);
  BOOST_CHECK_EQUAL( P01.last_zero(), 15);
  BOOST_CHECK_EQUAL( PPa.last_zero(), 4);
  BOOST_CHECK_EQUAL( P1.last_zero(), VectType::N);
  BOOST_CHECK_EQUAL( P01.last_zero(1), 0);
  BOOST_CHECK_EQUAL( P10.last_zero(1), VectType::N);
  BOOST_CHECK_EQUAL( PPa.last_zero(5), 4);
  BOOST_CHECK_EQUAL( PPa.last_zero(3), VectType::N);
}

BOOST_AUTO_TEST_CASE( first_non_zero_test )
{
  BOOST_CHECK_EQUAL( zero.first_non_zero(), VectType::N);
  BOOST_CHECK_EQUAL( P01.first_non_zero(), 1);
  BOOST_CHECK_EQUAL( PPa.first_non_zero(), 0);
  BOOST_CHECK_EQUAL( P01.first_non_zero(), 1);
  BOOST_CHECK_EQUAL( P01.first_non_zero(1), VectType::N);
  BOOST_CHECK_EQUAL( PPa.first_non_zero(5), 0);
  BOOST_CHECK_EQUAL( PPa.first_non_zero(3), 0);
}

BOOST_AUTO_TEST_CASE( last_non_zero_test )
{
  BOOST_CHECK_EQUAL( zero.last_non_zero(), VectType::N);
  BOOST_CHECK_EQUAL( P01.last_non_zero(), 1);
  BOOST_CHECK_EQUAL( PPa.last_non_zero(), 15);
  BOOST_CHECK_EQUAL( P01.last_non_zero(), 1);
  BOOST_CHECK_EQUAL( P01.last_non_zero(1), VectType::N);
  BOOST_CHECK_EQUAL( PPa.last_non_zero(5), 3);
  BOOST_CHECK_EQUAL( PPa.last_non_zero(3), 2);
}

BOOST_AUTO_TEST_CASE( permuted_test )
{
  BOOST_CHECK_EQUAL( zero.permuted(zero), zero );
  BOOST_CHECK_EQUAL( P01.permuted(P01), P01 );
  BOOST_CHECK_EQUAL( P10.permuted(P10),
		     complete(VectType({0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}), 1) );
  BOOST_CHECK_EQUAL( P10.permuted(P01),
		     complete(VectType({1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1}), 1) );
  BOOST_CHECK_EQUAL( P01.permuted(P10), P10 );
// TODO: check mult
}


BOOST_AUTO_TEST_CASE( operator_insert_test )
{
  std::ostringstream out, out2;
  out << zero;
  out2 << "[0"; for (size_t i=1; i<VectType::N; i++) out2 << ",0"; out2 << "]";
  BOOST_CHECK_EQUAL(out.str(), out2.str());

  out.str(""); out2.str("");
  out << P01;
  out2 << "[0,1"; for (size_t i=2; i<VectType::N; i++) out2 << ",0"; out2 << "]";
  BOOST_CHECK_EQUAL(out.str(), out2.str());
  out.str(""); out2.str("");
  out << PPa;
  out2 << "[1,2,3,4,0"; for (size_t i=5; i<VectType::N; i++) out2 << "," << i; out2 << "]";
  BOOST_CHECK_EQUAL(out.str(), out2.str());
}

BOOST_AUTO_TEST_CASE( is_permutation_test )
{
  BOOST_CHECK_PREDICATE(is_not_perm, (zero));
  BOOST_CHECK_PREDICATE(is_perm, (PPa)(16));
  BOOST_CHECK_PREDICATE(is_not_perm, (PPb));
  BOOST_CHECK_PREDICATE(is_perm, (RandPerm));
  BOOST_CHECK_PREDICATE(is_not_perm, (VectType({3,1,0,14,15,13,3,10,2,11,6,12,7,4,8,9})));
  BOOST_CHECK_PREDICATE(is_not_perm, (RandPerm)(4));
  BOOST_CHECK_PREDICATE(is_perm, (PPa)(5));
  BOOST_CHECK_PREDICATE(is_not_perm, (PPa)(4));
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

struct PermFixture {
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

BOOST_FIXTURE_TEST_SUITE( PermType_test, PermFixture )

BOOST_AUTO_TEST_CASE( constructor_is_permutation_test )
{
  for (auto x : Plist) {
    BOOST_CHECK_PREDICATE(is_perm, (x));
  }
  BOOST_CHECK_PREDICATE(is_perm, (PermType()));
  BOOST_CHECK_PREDICATE(is_perm, (PermType({1,0})));
  BOOST_CHECK_PREDICATE(is_perm, (PermType({1,2,0})));
  BOOST_CHECK_PREDICATE(is_not_perm, (PermType({1,2})));
}

BOOST_AUTO_TEST_CASE( operator_mult_coxeter_test )
{
  for (uint64_t i=0; i<15; i++) {
    PermType si = PermType::elementary_transposition(i);
    BOOST_CHECK_NE(si, id);
    BOOST_CHECK_EQUAL(si * si, id);
    if (i+1 < 15) {
      PermType si1 = PermType::elementary_transposition(i+1);
      BOOST_CHECK_EQUAL(si * si1 * si, si1 * si * si1);
    }
    for (uint64_t j=i+2; j<15; j++) {
    PermType sj = PermType::elementary_transposition(j);
    BOOST_CHECK_EQUAL(sj * si, si * sj);
    }
  }
}

BOOST_AUTO_TEST_CASE( operator_mult_test )
{
  for (auto x : Plist) {
    BOOST_CHECK_EQUAL(id * x, x);
    BOOST_CHECK_EQUAL(x * id, x);
  }
  BOOST_CHECK_EQUAL(RandPerm * RandPerm, PermType({14,1,3,8,9,4,13,6,0,12,5,7,10,15,2,11}));
}


BOOST_AUTO_TEST_SUITE_END()
