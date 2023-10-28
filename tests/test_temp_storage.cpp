#include <utility>

#include "bounded_set.hpp"
#include "temp_storage.hpp"

#include "hpcombi/include/perm16.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

namespace IVMPG {
#ifdef USE_CILK
  template <class T>
  using set    = IVMPG::Container::bounded_set<T>;
  using Vect16 = HPCombi::Vect16;

  // FIXME Storage_holder is only defined if using CILK
  using Foo = Storage_holder<std::pair<set<Vect16>, set<int>>>;

  TEST_CASE("temporary storage", "[000]") {
    Foo          f;
    set<Vect16>& x = f.get_store().first;
    x.insert({3, 2});
    x.insert({6});
    std::cout << "X : " << x.size() << std::endl;
    for (auto t : x)
      std::cout << " == " << t << std::endl;

    set<Vect16> y = x;
    x.insert({8});
    std::cout << "X : " << x.size() << std::endl;
    for (auto t : x)
      std::cout << " == " << t << std::endl;
    std::cout << "Y : " << y.size() << std::endl;
    for (auto t : y)
      std::cout << " == " << t << std::endl;
  }
#endif
}  // namespace IVMPG
