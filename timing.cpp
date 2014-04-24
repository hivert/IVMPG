#include <iostream>
#include <chrono>
#include <cassert>

#include "group16.hpp"
#include "group_examples.hpp"

using namespace std;
using namespace std::chrono;

PermutationGroup<> g_Borie  = GroupExamples<>::g_Borie;

long time_it(PermutationGroup16 gr, int level) {
  cout << gr.name << " ( " << level << " ) : ";

  high_resolution_clock::time_point tstart = high_resolution_clock::now(), tfin;
  auto lst = gr.elements_of_depth(level);
  tfin = high_resolution_clock::now();

  auto time = duration_cast<duration<double>>(tfin - tstart);
  cout << lst.size() << " (time = " << time.count() << "s)." << std::endl;
  return lst.size();
}

int main() {
  assert(time_it(g_Borie, 20) ==   57605 ); // Checked with Sage
  assert(time_it(g_Borie, 25) ==  375810 ); // Checked with Sage
  assert(time_it(g_Borie, 30) == 1983238 ); // Checked with Sage
}
