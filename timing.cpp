#include <iostream>
#include <chrono>
#include <cassert>

#include "group16.hpp"
#include "group_examples.hpp"

using namespace std;
using namespace std::chrono;
using namespace IVMPG;

using MyGroup = PermutationGroup16;
MyGroup g_Borie = GroupExamples< MyGroup >::g_Borie;

long time_it(MyGroup gr, int level, size_t expected_size) {
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

long count_it(MyGroup gr, int level, size_t expected_size) {
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


static void show_usage(string name)
{
  cerr << "Usage: " << name << " [-n <proc_number>] " << endl;
  exit(1);
}

int main(int argc, char **argv) {

  string nproc = "0";

  if (argc != 1 and argc != 3) show_usage(argv[0]);
  if (argc == 3) {
    if (string(argv[1]) != "-n") show_usage(argv[0]);
    nproc = argv[2];
  }
  if (nproc != "0")
    if (__cilkrts_set_param("nworkers", nproc.c_str() ) != __CILKRTS_SET_PARAM_SUCCESS)
      cerr << "Failed to set the number of Cilk workers" << endl;

  cout << "Counting: " << endl;
  count_it(g_Borie, 15,    6686 ); // Checked with Sage      10.2 s
  count_it(g_Borie, 20,   57605 ); // Checked with Sage 1min 27s
  count_it(g_Borie, 25,  375810 ); // Checked with Sage 9min 23s
  count_it(g_Borie, 30, 1983238 ); // Checked with Sage

  cout << "Listing: " << endl;
  time_it(g_Borie, 15,    6686 ); // Checked with Sage      10.2 s
  time_it(g_Borie, 20,   57605 ); // Checked with Sage 1min 27s
  time_it(g_Borie, 25,  375810 ); // Checked with Sage 9min 23s
  time_it(g_Borie, 30, 1983238 ); // Checked with Sage
  
}
