#include <iostream>
#include <chrono>
#include <cassert>

#include "group16.hpp"
#include "group_examples.hpp"

using namespace std;
using namespace std::chrono;

using MyGroup = PermutationGroup16;
MyGroup g_Borie = GroupExamples< MyGroup >::g_Borie;

long time_it(MyGroup gr, int level) {
  high_resolution_clock::time_point tstart, tfin;

  cout << gr.name << " ( " << level << " ) : " << flush;

  tstart = high_resolution_clock::now();
  auto lst = gr.elements_of_depth(level);
  tfin = high_resolution_clock::now();

  auto time = duration_cast<duration<double>>(tfin - tstart);
  cout << lst.size() << " (time = " << time.count() << "s)." << std::endl;
  return lst.size();
}

long count_it(MyGroup gr, int level) {
  high_resolution_clock::time_point tstart, tfin;

  cout << gr.name << " ( " << level << " ) : " << flush;

  tstart = high_resolution_clock::now();
  auto res = gr.elements_of_depth_number(level);
  tfin = high_resolution_clock::now();

  auto time = duration_cast<duration<double>>(tfin - tstart);
  cout << res << " (time = " << time.count() << "s)." << std::endl;
  return res;
}

int main() {

  cout << "Counting: " << endl;
  assert(count_it(g_Borie, 15) ==    6686 ); // Checked with Sage      10.2 s
  assert(count_it(g_Borie, 20) ==   57605 ); // Checked with Sage 1min 27s
  assert(count_it(g_Borie, 25) ==  375810 ); // Checked with Sage 9min 23s
  assert(count_it(g_Borie, 30) == 1983238 ); // Checked with Sage

  cout << "Listing: " << endl;
  assert(time_it(g_Borie, 15) ==    6686 ); // Checked with Sage      10.2 s
  assert(time_it(g_Borie, 20) ==   57605 ); // Checked with Sage 1min 27s
  assert(time_it(g_Borie, 25) ==  375810 ); // Checked with Sage 9min 23s
  assert(time_it(g_Borie, 30) == 1983238 ); // Checked with Sage
}
