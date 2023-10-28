/******************************************************************************/
/*       Copyright (C) 2014 Florent Hivert <Florent.Hivert@lri.fr>,           */
/*                                                                            */
/*  Distributed under the terms of the GNU General Public License (GPL)       */
/*                                                                            */
/*    This code is distributed in the hope that it will be useful,            */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       */
/*   General Public License for more details.                                 */
/*                                                                            */
/*  The full text of the GPL is available at:                                 */
/*                                                                            */
/*                  http://www.gnu.org/licenses/                              */
/******************************************************************************/

// #include "config.h"

#include <iostream>

#include <catch2/catch_test_macros.hpp>

#include "group16.hpp"
#include "group_examples.hpp"

struct Fixture {
  IVMPG::PermutationGroup<>& S3      = IVMPG::GroupExamples<>::S3;
  IVMPG::PermutationGroup<>& g100    = IVMPG::GroupExamples<>::g100;
  IVMPG::PermutationGroup<>& g_Borie = IVMPG::GroupExamples<>::g_Borie;
};

TEST_CASE_METHOD(Fixture, "elements_of_depth_test", "[000]") {
  REQUIRE(S3.elements_of_depth(0).size() == 1);
  REQUIRE(S3.elements_of_depth(5).size() == 5);    // Checked with Sage
  REQUIRE(S3.elements_of_depth(10).size() == 14);  // Checked with Sage
  REQUIRE(S3.elements_of_depth(20).size() == 44);  // Checked with Sage

  REQUIRE(g100.elements_of_depth(0).size() == 1);
  REQUIRE(g100.elements_of_depth(5).size() == 26);      // Checked with Sage
  REQUIRE(g100.elements_of_depth(10).size() == 280);    // Checked with Sage
  REQUIRE(g100.elements_of_depth(20).size() == 4'576);  // Checked with Sage

  REQUIRE(g_Borie.elements_of_depth(0).size() == 1);
  REQUIRE(g_Borie.elements_of_depth(5).size() == 25);       // Checked with Sage
  REQUIRE(g_Borie.elements_of_depth(10).size() == 545);     // Checked with Sage
  REQUIRE(g_Borie.elements_of_depth(20).size() == 57'605);  // Checked with Sage
}

TEST_CASE_METHOD(Fixture, "elements_of_depth_huge_test", "[001]") {
  REQUIRE(g_Borie.elements_of_depth(25).size()
          == 375'810);  // Checked with Sage
  REQUIRE(g_Borie.elements_of_depth(30).size()
          == 1'983'238);  // Checked with Sage
}
