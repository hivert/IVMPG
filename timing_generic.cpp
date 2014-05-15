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

#include <iostream>
#include <chrono>
#include <cassert>

#include "perm_generic.hpp"
#include "group16.hpp"
#include "group_examples.hpp"

using namespace std;
using namespace std::chrono;
using namespace IVMPG;

template <class MyGroup>
long time_it(MyGroup &gr, int level, unsigned long expected_size) {
  high_resolution_clock::time_point tstart, tfin;

  cout << gr.name << " ( " << level << " ) : " << flush;

  tstart = high_resolution_clock::now();
  auto lst = gr.elements_of_depth(level);
  tfin = high_resolution_clock::now();

  auto time = duration_cast<duration<double>>(tfin - tstart);
  cout << lst.size() << " (time = " << time.count() << "s)." << std::endl;
  assert(lst.size() == expected_size);
  return lst.size();
}

template <class MyGroup>
long count_it(MyGroup &gr, int level, unsigned long expected_size) {
  high_resolution_clock::time_point tstart, tfin;

  cout << gr.name << " ( " << level << " ) : " << flush;

  tstart = high_resolution_clock::now();
  auto res = gr.elements_of_depth_number(level);
  tfin = high_resolution_clock::now();

  auto time = duration_cast<duration<double>>(tfin - tstart);
  cout << res << " (time = " << time.count() << "s)." << std::endl;
  assert(res == expected_size);
  return res;
}

template <class MyGroup>
void do_timing(std::string name) {
  MyGroup g_Borie = GroupExamples< MyGroup >::g_Borie;

  cout << name << " counting: " << endl;
  count_it(g_Borie, 15,    6686 ); // Checked with Sage      10.2 s
  count_it(g_Borie, 20,   57605 ); // Checked with Sage 1min 27s
  count_it(g_Borie, 25,  375810 ); // Checked with Sage 9min 23s
  count_it(g_Borie, 30, 1983238 ); // Checked with Sage

  cout << name << " listing: " << endl;
  time_it(g_Borie, 15,    6686 ); // Checked with Sage      10.2 s
  time_it(g_Borie, 20,   57605 ); // Checked with Sage 1min 27s
  time_it(g_Borie, 25,  375810 ); // Checked with Sage 9min 23s
  time_it(g_Borie, 30, 1983238 ); // Checked with Sage
}

int main() {
  do_timing< PermutationGroup16 >("Fast");
  do_timing< PermutationGroup< PermGeneric<16> > >("Generic 16");
  do_timing< PermutationGroup< PermGeneric<32> > >("Generic 32");
}
