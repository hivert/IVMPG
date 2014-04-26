#define BOOST_TEST_DYN_LINK

#define BOOST_TEST_MODULE group
#include <boost/test/unit_test.hpp>

#include "group16.hpp"
#include "group_examples.hpp"
#include <iostream>

struct Fixture {

  PermutationGroup<> &S3 = GroupExamples<>::S3;
  PermutationGroup<> &g100  = GroupExamples<>::g100;
  PermutationGroup<> &g_Borie  = GroupExamples<>::g_Borie;

};

//____________________________________________________________________________//


BOOST_FIXTURE_TEST_SUITE( group16_test, Fixture )


BOOST_AUTO_TEST_CASE( elements_of_depth_test )
{
  BOOST_CHECK_EQUAL( S3.elements_of_depth( 0).size(),  1 );
  BOOST_CHECK_EQUAL( S3.elements_of_depth( 5).size(),  5 ); // Checked with Sage
  BOOST_CHECK_EQUAL( S3.elements_of_depth(10).size(), 14 ); // Checked with Sage
  BOOST_CHECK_EQUAL( S3.elements_of_depth(20).size(), 44 ); // Checked with Sage

  BOOST_CHECK_EQUAL( g100.elements_of_depth( 0).size(),    1 );
  BOOST_CHECK_EQUAL( g100.elements_of_depth( 5).size(),   26 ); // Checked with Sage
  BOOST_CHECK_EQUAL( g100.elements_of_depth(10).size(),  280 ); // Checked with Sage
  BOOST_CHECK_EQUAL( g100.elements_of_depth(20).size(), 4576 ); // Checked with Sage

  BOOST_CHECK_EQUAL( g_Borie.elements_of_depth( 0).size(),       1 );
  BOOST_CHECK_EQUAL( g_Borie.elements_of_depth( 5).size(),      25 ); // Checked with Sage
  BOOST_CHECK_EQUAL( g_Borie.elements_of_depth(10).size(),     545 ); // Checked with Sage
  BOOST_CHECK_EQUAL( g_Borie.elements_of_depth(20).size(),   57605 ); // Checked with Sage
}

#ifdef USE_CILK

BOOST_AUTO_TEST_CASE( elements_of_depth_huge_test )
{
  BOOST_CHECK_EQUAL( g_Borie.elements_of_depth(25).size(),  375810 ); // Checked with Sage
  BOOST_CHECK_EQUAL( g_Borie.elements_of_depth(30).size(), 1983238 ); // Checked with Sage
}

#endif

BOOST_AUTO_TEST_SUITE_END()
