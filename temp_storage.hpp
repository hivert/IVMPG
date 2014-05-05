#ifndef _TEMP_STORAGE_HPP
#define _TEMP_STORAGE_HPP

#ifdef USE_CILK

#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/holder.h>

template<class T>
class Storage_holder {
  cilk::holder< T > store;
public:
  T &get_store() { return store(); }
};

template<class T>
class Storage_thread_local {
  // We ensure that T are aligned on cache line boundary to prevent false sharing.
  struct alignas(256) PaddedT {
    T t;
  };
  PaddedT *store;
public:
  Storage_thread_local() : store(new PaddedT[__cilkrts_get_nworkers()]) { };
  ~Storage_thread_local() { delete [] store; }
  T &get_store() { return store[__cilkrts_get_worker_number()].t; }
};

#else

template<class T>
class Storage_dummy {
  T store;
public:
  T &get_store() { return store; }
};

#endif // USE_CILK

#endif // _TEMP_STORAGE_HPP

