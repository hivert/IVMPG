#define BOOST_TEST_MODULE group

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

#include "perm16.hpp"
#include "perm_generic.hpp"
#include "group_examples.hpp"

//____________________________________________________________________________//

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

//____________________________________________________________________________//

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
  BOOST_CHECK_EQUAL( F::S3.elements_of_depth( 0).size(),  1u );
  BOOST_CHECK_EQUAL( F::S3.elements_of_depth( 5).size(),  5u ); // Checked with Sage
  BOOST_CHECK_EQUAL( F::S3.elements_of_depth(10).size(), 14u ); // Checked with Sage
  BOOST_CHECK_EQUAL( F::S3.elements_of_depth(20).size(), 44u ); // Checked with Sage

  BOOST_CHECK_EQUAL( F::g100.elements_of_depth( 0).size(),    1u );
  BOOST_CHECK_EQUAL( F::g100.elements_of_depth( 5).size(),   26u ); // Checked with Sage
  BOOST_CHECK_EQUAL( F::g100.elements_of_depth(10).size(),  280u ); // Checked with Sage
  BOOST_CHECK_EQUAL( F::g100.elements_of_depth(20).size(), 4576u ); // Checked with Sage

  BOOST_CHECK_EQUAL( F::g_Borie.elements_of_depth( 0).size(),       1u );
  BOOST_CHECK_EQUAL( F::g_Borie.elements_of_depth( 5).size(),      25u ); // Checked with Sage
  BOOST_CHECK_EQUAL( F::g_Borie.elements_of_depth(10).size(),     545u ); // Checked with Sage
  BOOST_CHECK_EQUAL( F::g_Borie.elements_of_depth(20).size(),   57605u ); // Checked with Sage
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE( elements_of_depth_max_part_test, F, Fixtures, F )
{
  std::vector< std::vector<int> > S3_sz { // Checked with Sage
    {1, 0},
    {1, 1, 1, 1, 0},
    {1, 1, 2, 2, 2, 1, 1, 0},
    {1, 1, 2, 3, 3, 3, 3, 2, 1, 1, 0},
    {1, 1, 2, 3, 4, 4, 5, 4, 4, 3, 2, 1, 1, 0} };
  for (unsigned int max = 0; max < S3_sz.size(); max++)
    for (unsigned int i=0; i<S3_sz[max].size(); i++)
      BOOST_CHECK_EQUAL( F::S3.elements_of_depth(i, max).size(), S3_sz[max][i] );

  std::vector< std::vector<int> > g100_sz { // Checked with Sage
    {1, 0},
    {1, 1, 3, 3, 3, 1, 1, 0},
    {1, 1, 4, 6, 12, 13, 18, 13, 12, 6, 4, 1, 1, 0},
    {1, 1, 4, 7, 15, 22, 37, 44, 56, 56, 56, 44, 37, 22, 15, 7, 4, 1, 1, 0} };
  for (unsigned int max = 0; max < g100_sz.size(); max++)
    for (unsigned int i=0; i<g100_sz[max].size(); i++)
      BOOST_CHECK_EQUAL( F::g100.elements_of_depth(i, max).size(), g100_sz[max][i] );

  std::vector< std::vector<int> > g_Borie_sz { // Checked with Sage
    {1, 0},
    {1, 1, 2, 3, 5, 5, 7, 7, 8, 7, 7, 5, 5, 3, 2, 1, 1, 0},
    {1, 1, 3, 5, 11, 16, 29, 41, 65, 85, 119, 145, 185, 207, 239, 247, 262,
	247, 239, 207, 185, 145, 119, 85, 65, 41, 29, 16, 11, 5, 3, 1, 1, 0},
    {1, 1, 3, 6, 13, 22, 43, 70, 121, 189, 297, 436, 642, 884, 1210, 1578,
	2025, 2486, 3007, 3486 } };
  for (unsigned int max = 0; max < g_Borie_sz.size(); max++)
    for (unsigned int i=0; i<g_Borie_sz[max].size(); i++)
      BOOST_CHECK_EQUAL( F::g_Borie.elements_of_depth(i, max).size(), g_Borie_sz[max][i] );
}

BOOST_AUTO_TEST_SUITE_END()
