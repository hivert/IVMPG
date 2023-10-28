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

#include "group_examples.hpp"

#include "hpcombi/include/perm16.hpp"
#include "hpcombi/include/perm_generic.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

namespace {
  template <class PermType>
  struct Fixture {
    using GroupType          = IVMPG::PermutationGroup<PermType>;
    using VectType           = typename PermType::vect;
    const GroupType& S3      = IVMPG::GroupExamples<GroupType>::S3;
    const GroupType& g100    = IVMPG::GroupExamples<GroupType>::g100;
    const GroupType& g_Borie = IVMPG::GroupExamples<GroupType>::g_Borie;

    static bool is_canon(const GroupType& g, VectType v) {
      return g.is_canonical(v);
    }
    static bool is_not_canon(const GroupType& g, VectType v) {
      return not g.is_canonical(v);
    }
  };
}  // namespace
//
#define PERM_TYPES \
  HPCombi::Perm16, HPCombi::PermGeneric<16>, HPCombi::PermGeneric<32>

TEMPLATE_TEST_CASE_METHOD(Fixture, "check_sgs_test", "[000]", PERM_TYPES) {
  REQUIRE(Fixture<TestType>::S3.check_sgs());
  REQUIRE(Fixture<TestType>::g100.check_sgs());
  REQUIRE(Fixture<TestType>::g_Borie.check_sgs());
}

TEMPLATE_TEST_CASE_METHOD(Fixture, "is_canonical", "[001]", PERM_TYPES) {
  using V = typename Fixture<TestType>::VectType;
  REQUIRE(Fixture<TestType>::is_canon(Fixture<TestType>::S3, V({})));
  REQUIRE(Fixture<TestType>::is_canon(Fixture<TestType>::S3, V({1, 0})));
  REQUIRE(Fixture<TestType>::is_not_canon(Fixture<TestType>::S3, V({0, 1})));
  REQUIRE(Fixture<TestType>::is_not_canon(Fixture<TestType>::S3, V({4, 1, 3})));
  REQUIRE(Fixture<TestType>::is_canon(Fixture<TestType>::S3, V({4, 3, 3})));
}

TEMPLATE_TEST_CASE_METHOD(Fixture, "canonical", "[002]", PERM_TYPES) {
  using V = typename Fixture<TestType>::VectType;
  REQUIRE(Fixture<TestType>::S3.canonical(V({})) == V({}));
  REQUIRE(Fixture<TestType>::S3.canonical(V({1, 0})) == V({1, 0}));
  REQUIRE(Fixture<TestType>::S3.canonical(V({0, 1})) == V({1, 0}));
  REQUIRE(Fixture<TestType>::S3.canonical(V({4, 1, 3})) == V({4, 3, 1}));
  REQUIRE(Fixture<TestType>::S3.canonical(V({4, 3, 3})) == V({4, 3, 3}));
}

TEMPLATE_TEST_CASE_METHOD(Fixture, "elements_of_depth", "[003]", PERM_TYPES) {
  REQUIRE(Fixture<TestType>::S3.elements_of_depth(0).size() == 1u);
  REQUIRE(Fixture<TestType>::S3.elements_of_depth(5).size()
          == 5u);  // Checked with Sage
  REQUIRE(Fixture<TestType>::S3.elements_of_depth(10).size()
          == 14u);  // Checked with Sage
  REQUIRE(Fixture<TestType>::S3.elements_of_depth(20).size()
          == 44u);  // Checked with Sage

  REQUIRE(Fixture<TestType>::g100.elements_of_depth(0).size() == 1u);
  REQUIRE(Fixture<TestType>::g100.elements_of_depth(5).size()
          == 26u);  // Checked with Sage
  REQUIRE(Fixture<TestType>::g100.elements_of_depth(10).size()
          == 280u);  // Checked with Sage
  REQUIRE(Fixture<TestType>::g100.elements_of_depth(20).size()
          == 4576u);  // Checked with Sage

  REQUIRE(Fixture<TestType>::g_Borie.elements_of_depth(0).size() == 1u);
  REQUIRE(Fixture<TestType>::g_Borie.elements_of_depth(5).size()
          == 25u);  // Checked with Sage
  REQUIRE(Fixture<TestType>::g_Borie.elements_of_depth(10).size()
          == 545u);  // Checked with Sage
  REQUIRE(Fixture<TestType>::g_Borie.elements_of_depth(20).size()
          == 57605u);  // Checked with Sage
}

TEMPLATE_TEST_CASE_METHOD(Fixture,
                          "elements_of_depth_max_part",
                          "[004]",
                          PERM_TYPES) {
  std::vector<std::vector<size_t>> S3_sz{
      // Checked with Sage
      {1, 0},
      {1, 1, 1, 1, 0},
      {1, 1, 2, 2, 2, 1, 1, 0},
      {1, 1, 2, 3, 3, 3, 3, 2, 1, 1, 0},
      {1, 1, 2, 3, 4, 4, 5, 4, 4, 3, 2, 1, 1, 0}};
  for (size_t max = 0; max < S3_sz.size(); max++)
    for (size_t i = 0; i < S3_sz[max].size(); i++)
      REQUIRE(Fixture<TestType>::S3.elements_of_depth(i, max).size()
              == S3_sz[max][i]);

  std::vector<std::vector<size_t>> g100_sz{
      // Checked with Sage
      {1, 0},
      {1, 1, 3, 3, 3, 1, 1, 0},
      {1, 1, 4, 6, 12, 13, 18, 13, 12, 6, 4, 1, 1, 0},
      {1, 1, 4, 7, 15, 22, 37, 44, 56, 56, 56, 44, 37, 22, 15, 7, 4, 1, 1, 0}};
  for (size_t max = 0; max < g100_sz.size(); max++)
    for (size_t i = 0; i < g100_sz[max].size(); i++)
      REQUIRE(Fixture<TestType>::g100.elements_of_depth(i, max).size()
              == g100_sz[max][i]);

  std::vector<std::vector<size_t>> g_Borie_sz{
      // Checked with Sage
      {1, 0},
      {1, 1, 2, 3, 5, 5, 7, 7, 8, 7, 7, 5, 5, 3, 2, 1, 1, 0},
      {1,   1,   3,   5,   11,  16,  29,  41,  65,  85,  119, 145,
       185, 207, 239, 247, 262, 247, 239, 207, 185, 145, 119, 85,
       65,  41,  29,  16,  11,  5,   3,   1,   1,   0},
      {1,   1,   3,   6,   13,   22,   43,   70,   121,  189,
       297, 436, 642, 884, 1210, 1578, 2025, 2486, 3007, 3486}};
  for (size_t max = 0; max < g_Borie_sz.size(); max++)
    for (size_t i = 0; i < g_Borie_sz[max].size(); i++)
      REQUIRE(Fixture<TestType>::g_Borie.elements_of_depth(i, max).size()
              == g_Borie_sz[max][i]);
}
