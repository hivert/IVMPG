#define BOOST_TEST_DYN_LINK

#define BOOST_TEST_MODULE group

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

#include "perm16.hpp"
#include "perm_generic.hpp"
#include "group_examples.hpp"
#include <iostream>

template <class PermType>
struct Fixture {

  using GroupType = PermutationGroup<PermType>;
  using VectType = typename PermType::vect;
  const GroupType &S3 = GroupExamples<GroupType>::S3;
  const GroupType &g100  = GroupExamples<GroupType>::g100;
  const GroupType &g_Borie  = GroupExamples<GroupType>::g_Borie;

  static bool is_canon(const GroupType &g, VectType v) {return g.is_canonical(v);};
  static bool is_not_canon(const GroupType &g, VectType v) {return not g.is_canonical(v);};

};

typedef boost::mpl::list<
  Fixture< Perm16 >,
  Fixture< PermGeneric<16> >,
  Fixture< PermGeneric<32> >
> Fixtures;

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE( group16_test )
//____________________________________________________________________________//

BOOST_FIXTURE_TEST_CASE_TEMPLATE( check_sgs_test, F, Fixtures, F )
{
  BOOST_CHECK(F::S3.check_sgs());
  BOOST_CHECK(F::g100.check_sgs());
  BOOST_CHECK(F::g_Borie.check_sgs());
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( is_canonical_test, F, Fixtures, F )
{
  using V = typename F::VectType;
  BOOST_CHECK_PREDICATE( F::is_canon, (F::S3)(V({})) );
  BOOST_CHECK_PREDICATE( F::is_canon, (F::S3)(V({1,0})) );
  BOOST_CHECK_PREDICATE( F::is_not_canon, (F::S3)(V({0,1})) );
  BOOST_CHECK_PREDICATE( F::is_not_canon, (F::S3)(V({4,1,3})) );
  BOOST_CHECK_PREDICATE( F::is_canon, (F::S3)(V({4,3,3})) );
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( canonical_test, F, Fixtures, F )
{
  using V = typename F::VectType;
  BOOST_CHECK_EQUAL( F::S3.canonical(V({})),      V({}) );
  BOOST_CHECK_EQUAL( F::S3.canonical(V({1,0})),   V({1,0}) );
  BOOST_CHECK_EQUAL( F::S3.canonical(V({0,1})),   V({1,0}) );
  BOOST_CHECK_EQUAL( F::S3.canonical(V({4,1,3})), V({4,3,1}) );
  BOOST_CHECK_EQUAL( F::S3.canonical(V({4,3,3})), V({4,3,3}) );
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( elements_of_depth_test, F, Fixtures, F )
{
  BOOST_CHECK_EQUAL( F::S3.elements_of_depth( 0).size(),  1 );
  BOOST_CHECK_EQUAL( F::S3.elements_of_depth( 5).size(),  5 ); // Checked with Sage
  BOOST_CHECK_EQUAL( F::S3.elements_of_depth(10).size(), 14 ); // Checked with Sage
  BOOST_CHECK_EQUAL( F::S3.elements_of_depth(20).size(), 44 ); // Checked with Sage

  BOOST_CHECK_EQUAL( F::g100.elements_of_depth( 0).size(),    1 );
  BOOST_CHECK_EQUAL( F::g100.elements_of_depth( 5).size(),   26 ); // Checked with Sage
  BOOST_CHECK_EQUAL( F::g100.elements_of_depth(10).size(),  280 ); // Checked with Sage
  BOOST_CHECK_EQUAL( F::g100.elements_of_depth(20).size(), 4576 ); // Checked with Sage

  BOOST_CHECK_EQUAL( F::g_Borie.elements_of_depth( 0).size(),       1 );
  BOOST_CHECK_EQUAL( F::g_Borie.elements_of_depth( 5).size(),      25 ); // Checked with Sage
  BOOST_CHECK_EQUAL( F::g_Borie.elements_of_depth(10).size(),     545 ); // Checked with Sage
  BOOST_CHECK_EQUAL( F::g_Borie.elements_of_depth(20).size(),   57605 ); // Checked with Sage
}

BOOST_AUTO_TEST_SUITE_END()
