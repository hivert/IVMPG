#include <utility>
#include <set>
#include <iostream>

#include "temp_storage.hpp"
#include "container/bounded_set.hpp"
#include "perm16.hpp"


// template<class T> using set = std::set< T >;
template<class T> using set = IVMPG::Container::bounded_set< T >;


// using Foo = Storage_dummy < std::pair< set<int>, set<int> > >;
using Foo = Storage_holder < std::pair< set<Vect16>, set<int> > >;

using namespace IVMPG;

int main() {
  Foo f;
  set<Vect16> &x = f.get_store().first;
  x.insert({3, 2});
  x.insert({6});
  std::cout << "X : " << x.size() << std::endl;
  for (auto t : x) std::cout << " == " << t << std::endl;

  set<Vect16> y = x;
  x.insert({8});
  std::cout << "X : " << x.size() << std::endl;
  for (auto t : x) std::cout << " == " << t << std::endl;
  std::cout << "Y : " << y.size() << std::endl;
  for (auto t : y) std::cout << " == " << t << std::endl;
}
