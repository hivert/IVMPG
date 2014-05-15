#*****************************************************************************
#       Copyright (C) 2014 Florent Hivert <Florent.Hivert@univ-rouen.fr>,
#
#  Distributed under the terms of the GNU General Public License (GPL)
#
#    This code is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    General Public License for more details.
#
#  The full text of the GPL is available at:
#
#                  http://www.gnu.org/licenses/
#*****************************************************************************

from libc.stdint cimport uint8_t, uint64_t, uint_fast64_t

from libcpp.vector cimport vector as stl_vector
from libcpp.string cimport string as stl_string
from libcpp.list cimport list as stl_list

cdef extern from "group16.hpp" namespace "IVMPG" nogil:
    cdef cppclass Vect16:
        uint8_t &operator[](int)
        bint operator==(const Vect16 &vp) const
        bint operator<(const Vect16 &vp) const
        bint is_permutation(uint_fast64_t N) const

    cdef cppclass Perm16(Vect16):
        Perm16 operator*(const Perm16 &p) const

    cdef Perm16 one "IVMPG::Perm16::one" ()
    cdef Perm16 elementary_transposition "IVMPG::Perm16::elementary_transposition" ()

    ctypedef stl_vector[ stl_vector[Perm16] ] StrongGeneratingSet

    # I have to redeclare the stl::list as I use a non standard allocator
    cdef cppclass PG16listIterator "IVMPG::PermutationGroup16::list::iterator":
        bint operator==(PG16listIterator) nogil
        bint operator!=(PG16listIterator) nogil
        void operator++() nogil
        Vect16 operator*() nogil
    cdef cppclass PG16list "IVMPG::PermutationGroup16::list":
        int size() const
        PG16listIterator begin()
        PG16listIterator end()


    cdef cppclass PermutationGroup16 nogil:

        PermutationGroup16(stl_string name, uint64_t N, StrongGeneratingSet sgs)

        Vect16 canonical(Vect16 v) const
        bint is_canonical(Vect16 v) const
        bint check_sgs() const

        PG16list elements_of_depth(uint64_t depth) const

