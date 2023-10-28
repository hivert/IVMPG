#include <iostream>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "bounded_set.hpp"

using namespace std;

namespace IVMPG::Container {

  TEST_CASE("BMat8::col_permutation_matrix", "[BMat8][024]") {
    bounded_set<int> s, t;

    cout << s.size() << " : " << s << endl;
    s.insert(4);
    cout << s.size() << " : " << s << endl;
    s.insert(4);
    cout << s.size() << " : " << s << endl;
    s.insert(2348);
    cout << s.size() << " : " << s << endl;
    s.insert(22348);
    cout << s.size() << " : " << s << endl;
    s.insert(12322348);
    cout << s.size() << " : " << s << endl;
    s.clear();
    cout << s.size() << " : " << s << endl;
    s.insert(22348);
    cout << s.size() << " : " << s << endl;
    s.insert(12322348);
    cout << s.size() << " : " << s << endl;
    cout << "Before Swap " << endl;
    cout << "s=" << s.size() << " : " << s << endl;
    cout << "t=" << t.size() << " : " << t << endl;
    std::swap(s, t);
    cout << "After Swap " << endl;
    cout << "s=" << s.size() << " : " << s << endl;
    cout << "t=" << t.size() << " : " << t << endl;
  }
}  // namespace IVMPG::Container
