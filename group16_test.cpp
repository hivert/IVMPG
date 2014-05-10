#define BOOST_TEST_MODULE group

#include "config.h"

#ifdef BOOST_TEST_USE_LIB
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#endif
#ifdef BOOST_TEST_USE_INCLUDE
#define BOOST_TEST_NO_LIB
#include <boost/test/included/unit_test.hpp>
#endif

//____________________________________________________________________________//

#include "group16.hpp"
#include "group_examples.hpp"
#include <iostream>

//____________________________________________________________________________//

struct Fixture {

  PermutationGroup<> &S3 = GroupExamples<>::S3;
  PermutationGroup<> &g100  = GroupExamples<>::g100;
  PermutationGroup<> &g_Borie  = GroupExamples<>::g_Borie;

};

//____________________________________________________________________________//


BOOST_FIXTURE_TEST_SUITE( group16_test, Fixture )


BOOST_AUTO_TEST_CASE( elements_of_depth_test )
{
  BOOST_CHECK_EQUAL( S3.elements_of_depth( 0).size(),  1u );
  BOOST_CHECK_EQUAL( S3.elements_of_depth( 5).size(),  5u ); // Checked with Sage
  BOOST_CHECK_EQUAL( S3.elements_of_depth(10).size(), 14u ); // Checked with Sage
  BOOST_CHECK_EQUAL( S3.elements_of_depth(20).size(), 44u ); // Checked with Sage

  BOOST_CHECK_EQUAL( g100.elements_of_depth( 0).size(),    1u );
  BOOST_CHECK_EQUAL( g100.elements_of_depth( 5).size(),   26u ); // Checked with Sage
  BOOST_CHECK_EQUAL( g100.elements_of_depth(10).size(),  280u ); // Checked with Sage
  BOOST_CHECK_EQUAL( g100.elements_of_depth(20).size(), 4576u ); // Checked with Sage

  BOOST_CHECK_EQUAL( g_Borie.elements_of_depth( 0).size(),       1u );
  BOOST_CHECK_EQUAL( g_Borie.elements_of_depth( 5).size(),      25u ); // Checked with Sage
  BOOST_CHECK_EQUAL( g_Borie.elements_of_depth(10).size(),     545u ); // Checked with Sage
  BOOST_CHECK_EQUAL( g_Borie.elements_of_depth(20).size(),   57605u ); // Checked with Sage
}

#ifdef USE_CILK

BOOST_AUTO_TEST_CASE( elements_of_depth_huge_test )
{
  BOOST_CHECK_EQUAL( g_Borie.elements_of_depth(25).size(),  375810u ); // Checked with Sage
  BOOST_CHECK_EQUAL( g_Borie.elements_of_depth(30).size(), 1983238u ); // Checked with Sage
}

#endif

BOOST_AUTO_TEST_SUITE_END()
