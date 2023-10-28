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

#ifndef _TEMP_STORAGE_HPP
#define _TEMP_STORAGE_HPP

#ifdef USE_CILK

#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/holder.h>

namespace IVMPG {

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

} // namespace IVMPG

#else

namespace IVMPG {

template<class T>
class Storage_dummy {
  T store;
public:
  T &get_store() { return store; }
};

} // namespace IVMPG 

#endif // USE_CILK

#endif // _TEMP_STORAGE_HPP

