#define BOOST_TEST_DYN_LINK

#define BOOST_TEST_MODULE perm16
#include <boost/test/unit_test.hpp>

#include "perm16.hpp"

struct Fixture {
  Fixture() : zero({0}), P01({0,1}), P10({1,0}), P11({1,1}),
	      P1( { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}),
	      PPa({ 1, 2, 3, 4, 0, 5, 6, 7, 8, 9,10,11,12,13,14,15}),
	      PPb({ 1, 2, 3, 6, 0, 5, 6, 7, 8, 9,10,11,12,13,14,15}),
	      czero(zero), cP01 (P01),
	      RandPerm({3,1,0,14,15,13,5,10,2,11,6,12,7,4,8,9}),
	      Plist({zero, P01, P10, P11, P1, PPa, PPb, RandPerm})
  { BOOST_TEST_MESSAGE( "setup fixture" ); }

  ~Fixture()         { BOOST_TEST_MESSAGE( "teardown fixture" ); }

  Vect16 zero, P01, P10, P11, P1, PPa, PPb;
  const Vect16 czero, cP01, RandPerm;
  const std::vector<Vect16> Plist;

};

auto less     = [](Vect16 a, Vect16 b) {return a < b;};
auto not_less = [](Vect16 a, Vect16 b) {return not(a < b);};

auto is_perm     = [](Vect16 a, int i=16) {return a.is_permutation(i);};
auto is_not_perm = [](Vect16 a, int i=16) {return not a.is_permutation(i);};

//____________________________________________________________________________//

BOOST_FIXTURE_TEST_SUITE( Vect16_test, Fixture )

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

BOOST_AUTO_TEST_CASE( search_first_zero_test )
{
  BOOST_CHECK_EQUAL( zero.search_index<FIRST_ZERO>(), 0);
  BOOST_CHECK_EQUAL( P01.search_index<FIRST_ZERO>(), 0);
  BOOST_CHECK_EQUAL( PPa.search_index<FIRST_ZERO>(), 4);
  BOOST_CHECK_EQUAL( P10.search_index<FIRST_ZERO>(), 1);
  BOOST_CHECK_EQUAL( P1.search_index<FIRST_ZERO>(), 16);
  BOOST_CHECK_EQUAL( P10.search_index<FIRST_ZERO>(1), 16);
  BOOST_CHECK_EQUAL( PPa.search_index<FIRST_ZERO>(5), 4);
  BOOST_CHECK_EQUAL( PPa.search_index<FIRST_ZERO>(3), 16);
}

BOOST_AUTO_TEST_CASE( search_last_zero_test )
{
  BOOST_CHECK_EQUAL( zero.search_index<LAST_ZERO>(), 15);
  BOOST_CHECK_EQUAL( P01.search_index<LAST_ZERO>(), 15);
  BOOST_CHECK_EQUAL( PPa.search_index<LAST_ZERO>(), 4);
  BOOST_CHECK_EQUAL( P1.search_index<LAST_ZERO>(), 16);
  BOOST_CHECK_EQUAL( P01.search_index<LAST_ZERO>(1), 0);
  BOOST_CHECK_EQUAL( P10.search_index<LAST_ZERO>(1), 16);
  BOOST_CHECK_EQUAL( PPa.search_index<LAST_ZERO>(5), 4);
  BOOST_CHECK_EQUAL( PPa.search_index<LAST_ZERO>(3), 16);
}

BOOST_AUTO_TEST_CASE( search_first_non_zero_test )
{
  BOOST_CHECK_EQUAL( zero.search_index<FIRST_NON_ZERO>(), 16);
  BOOST_CHECK_EQUAL( P01.search_index<FIRST_NON_ZERO>(), 1);
  BOOST_CHECK_EQUAL( PPa.search_index<FIRST_NON_ZERO>(), 0);
  BOOST_CHECK_EQUAL( P01.search_index<FIRST_NON_ZERO>(), 1);
  BOOST_CHECK_EQUAL( P01.search_index<FIRST_NON_ZERO>(1), 16);
  BOOST_CHECK_EQUAL( PPa.search_index<FIRST_NON_ZERO>(5), 0);
  BOOST_CHECK_EQUAL( PPa.search_index<FIRST_NON_ZERO>(3), 0);
}

BOOST_AUTO_TEST_CASE( search_last_non_zero_test )
{
  BOOST_CHECK_EQUAL( zero.search_index<LAST_NON_ZERO>(), 16);
  BOOST_CHECK_EQUAL( P01.search_index<LAST_NON_ZERO>(), 1);
  BOOST_CHECK_EQUAL( PPa.search_index<LAST_NON_ZERO>(), 15);
  BOOST_CHECK_EQUAL( P01.search_index<LAST_NON_ZERO>(), 1);
  BOOST_CHECK_EQUAL( P01.search_index<LAST_NON_ZERO>(1), 16);
  BOOST_CHECK_EQUAL( PPa.search_index<LAST_NON_ZERO>(5), 3);
  BOOST_CHECK_EQUAL( PPa.search_index<LAST_NON_ZERO>(3), 2);
}

BOOST_AUTO_TEST_CASE( permuted_test )
{
  BOOST_CHECK_EQUAL( zero.permuted(zero), zero );
  BOOST_CHECK_EQUAL( P01.permuted(P01), P01 );
  BOOST_CHECK_EQUAL( P10.permuted(P10), Vect16({0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}) );
  BOOST_CHECK_EQUAL( P10.permuted(P01), Vect16({1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1}) );
  BOOST_CHECK_EQUAL( P01.permuted(P10), P10 );
// TODO: check mult
}


BOOST_AUTO_TEST_CASE( operator_insert_test )
{
  std::ostringstream out;
  out << zero;
  BOOST_CHECK_EQUAL(out.str(), "[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]");
  out.str("");
  out << P01;
  BOOST_CHECK_EQUAL(out.str(), "[0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0]");
  out.str("");
  out << PPa;
  BOOST_CHECK_EQUAL(out.str(), "[1,2,3,4,0,5,6,7,8,9,10,11,12,13,14,15]");
}

BOOST_AUTO_TEST_CASE( is_permutation_test )
{
  BOOST_CHECK_PREDICATE(is_not_perm, (zero));
  BOOST_CHECK_PREDICATE(is_perm, (PPa));
  BOOST_CHECK_PREDICATE(is_not_perm, (PPb));
  BOOST_CHECK_PREDICATE(is_perm, (RandPerm));
  BOOST_CHECK_PREDICATE(is_not_perm, (Vect16({3,1,0,14,15,13,3,10,2,11,6,12,7,4,8,9})));
  BOOST_CHECK_PREDICATE(is_not_perm, (RandPerm)(4));
  BOOST_CHECK_PREDICATE(is_perm, (PPa)(5));
  BOOST_CHECK_PREDICATE(is_not_perm, (PPa)(4));
}

BOOST_AUTO_TEST_SUITE_END()


//____________________________________________________________________________//


struct PermFixture {
  PermFixture() : id(Perm16::one()),
		  RandPerm({3,1,0,14,15,13,5,10,2,11,6,12,7,4,8,9}),
		  Plist({id, RandPerm})
  {
    for (uint64_t i=0; i<15; i++)
      Plist.insert(Plist.begin()+1+i, Perm16::elementary_transposition(i));
    BOOST_TEST_MESSAGE( "setup fixture" );
  }

  ~PermFixture()         { BOOST_TEST_MESSAGE( "teardown fixture" ); }

  Perm16 id, s1, s2, s3;
  const Perm16 RandPerm;
  std::vector<Perm16> Plist;
};


//____________________________________________________________________________//

BOOST_FIXTURE_TEST_SUITE( Perm16_test, PermFixture )

BOOST_AUTO_TEST_CASE( constructor_is_permutation_test )
{
  for (auto x : Plist) {
    BOOST_CHECK_PREDICATE(is_perm, (x));
  }
  BOOST_CHECK_PREDICATE(is_perm, (Perm16()));
  BOOST_CHECK_PREDICATE(is_perm, (Perm16({1,0})));
  BOOST_CHECK_PREDICATE(is_perm, (Perm16({1,2,0})));
  BOOST_CHECK_PREDICATE(is_not_perm, (Perm16({1,2})));
}

BOOST_AUTO_TEST_CASE( operator_mult_coxeter_test )
{
  for (uint64_t i=0; i<15; i++) {
    Perm16 si = Perm16::elementary_transposition(i);
    BOOST_CHECK_NE(si, id);
    BOOST_CHECK_EQUAL(si * si, id);
    if (i+1 < 15) {
      Perm16 si1 = Perm16::elementary_transposition(i+1);
      BOOST_CHECK_EQUAL(si * si1 * si, si1 * si * si1);
    }
    for (uint64_t j=i+2; j<15; j++) {
    Perm16 sj = Perm16::elementary_transposition(j);
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
  BOOST_CHECK_EQUAL(RandPerm * RandPerm, Perm16({14,1,3,8,9,4,13,6,0,12,5,7,10,15,2,11}));
}


BOOST_AUTO_TEST_SUITE_END()
