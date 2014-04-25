#ifndef _GROUP_EXAMPLE
#define _GROUP_EXAMPLE

#include "group.hpp"

template< class Group = PermutationGroup<> > struct GroupExamples {
  static Group S3, g100, g_Borie;
};


// Symmetric group of order 3
template< class Group >
Group GroupExamples< Group >::S3("S3", 3, {
    {{0,1,2}, {1,0,2}, {2,1,0}},
    {{0,1,2}, {0,2,1}},
    {{0,1,2}}
  });;

// transitive subgroup of S6 number 100 according to Sage
template< class Group >
Group GroupExamples< Group >::g100("Transitive Group (6, 100)", 6,
{{{0, 1, 2, 3, 4, 5},
  {1, 0, 3, 2, 5, 4},
  {3, 2, 4, 5, 1, 0},
  {2, 3, 5, 4, 0, 1},
  {5, 4, 1, 0, 2, 3},
  {4, 5, 0, 1, 3, 2}},
 {{0, 1, 2, 3, 4, 5}},
 {{0, 1, 2, 3, 4, 5},
  {0, 1, 4, 5, 2, 3}},
 {{0, 1, 2, 3, 4, 5}},
 {{0, 1, 2, 3, 4, 5}},
 {{0, 1, 2, 3, 4, 5}}});

// N. Borie's favorite faforite subgroup of S16
template< class Group >
Group GroupExamples< Group >::g_Borie("N. Borie's favorite", 16,
{{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  {7, 6, 5, 4, 3, 0, 2, 1, 13, 12, 15, 14, 11, 8, 10, 9},
  {12, 15, 14, 13, 11, 10, 8, 9, 5, 6, 7, 4, 0, 2, 1, 3},
  {13, 14, 15, 12, 10, 11, 9, 8, 7, 4, 5, 6, 1, 3, 0, 2},
  {6, 7, 4, 5, 2, 1, 3, 0, 12, 13, 14, 15, 8, 11, 9, 10},
  {8, 10, 9, 11, 15, 14, 12, 13, 1, 3, 0, 2, 6, 4, 7, 5},
  {9, 11, 8, 10, 13, 12, 14, 15, 2, 0, 3, 1, 5, 7, 4, 6},
  {11, 9, 10, 8, 14, 15, 13, 12, 0, 2, 1, 3, 7, 5, 6, 4},
  {1, 0, 3, 2, 6, 7, 4, 5, 9, 8, 11, 10, 13, 12, 15, 14},
  {3, 2, 1, 0, 5, 4, 7, 6, 10, 11, 8, 9, 14, 15, 12, 13},
  {2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12},
  {4, 5, 6, 7, 0, 3, 1, 2, 15, 14, 13, 12, 10, 9, 11, 8},
  {5, 4, 7, 6, 1, 2, 0, 3, 14, 15, 12, 13, 9, 10, 8, 11},
  {15, 12, 13, 14, 8, 9, 11, 10, 6, 5, 4, 7, 2, 0, 3, 1},
  {14, 13, 12, 15, 9, 8, 10, 11, 4, 7, 6, 5, 3, 1, 2, 0},
  {10, 8, 11, 9, 12, 13, 15, 14, 3, 1, 2, 0, 4, 6, 5, 7}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  {0, 2, 3, 1, 8, 9, 11, 10, 7, 4, 5, 6, 12, 14, 15, 13},
  {0, 3, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  {0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 14}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  {0, 1, 2, 3, 5, 4, 7, 6, 8, 9, 10, 11, 12, 13, 14, 15},
  {0, 1, 2, 3, 9, 8, 10, 11, 14, 12, 13, 15, 4, 6, 5, 7},
  {0, 1, 2, 3, 12, 14, 13, 15, 5, 4, 6, 7, 9, 10, 8, 11},
  {0, 1, 2, 3, 6, 7, 4, 5, 8, 9, 10, 11, 12, 13, 14, 15},
  {0, 1, 2, 3, 10, 11, 9, 8, 14, 12, 13, 15, 4, 6, 5, 7},
  {0, 1, 2, 3, 13, 15, 12, 14, 5, 4, 6, 7, 9, 10, 8, 11},
  {0, 1, 2, 3, 11, 10, 8, 9, 14, 12, 13, 15, 4, 6, 5, 7},
  {0, 1, 2, 3, 8, 9, 11, 10, 14, 12, 13, 15, 4, 6, 5, 7},
  {0, 1, 2, 3, 7, 6, 5, 4, 8, 9, 10, 11, 12, 13, 14, 15},
  {0, 1, 2, 3, 15, 13, 14, 12, 5, 4, 6, 7, 9, 10, 8, 11},
  {0, 1, 2, 3, 14, 12, 15, 13, 5, 4, 6, 7, 9, 10, 8, 11}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  {0, 1, 2, 3, 4, 6, 5, 7, 15, 12, 13, 14, 8, 9, 10, 11},
  {0, 1, 2, 3, 4, 7, 5, 6, 14, 12, 13, 15, 8, 9, 10, 11}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  {0, 1, 2, 3, 4, 5, 7, 6, 14, 12, 13, 15, 8, 9, 11, 10}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  {0, 1, 2, 3, 4, 5, 6, 7, 10, 11, 8, 9, 12, 13, 14, 15},
  {0, 1, 2, 3, 4, 5, 6, 7, 12, 14, 15, 13, 8, 9, 10, 11},
  {0, 1, 2, 3, 4, 5, 6, 7, 11, 10, 9, 8, 12, 13, 14, 15},
  {0, 1, 2, 3, 4, 5, 6, 7, 9, 8, 11, 10, 12, 13, 14, 15},
  {0, 1, 2, 3, 4, 5, 6, 7, 14, 12, 13, 15, 8, 9, 10, 11},
  {0, 1, 2, 3, 4, 5, 6, 7, 13, 15, 14, 12, 8, 9, 10, 11},
  {0, 1, 2, 3, 4, 5, 6, 7, 15, 13, 12, 14, 8, 9, 10, 11}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 9, 10, 12, 13, 14, 15},
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 9, 11, 12, 13, 15, 14}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 10, 12, 13, 15, 14}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 12, 15, 14},
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 15, 14, 13, 12},
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 14, 15, 12, 13}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 15, 13, 14},
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 13}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
 {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}}} );

#endif
