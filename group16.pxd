from libc.stdint cimport uint8_t, uint64_t, uint_fast64_t

from libcpp.vector cimport vector as stl_vector
from libcpp.string cimport string as stl_string
from libcpp.list cimport list as stl_list

cdef extern from "group16.hpp" nogil:
    cdef cppclass Vect16:
        uint8_t &operator[](int)
        bint operator==(const Vect16 &vp) const
        bint operator<(const Vect16 &vp) const
        bint is_permutation(uint_fast64_t N) const

    cdef cppclass Perm16(Vect16):
        Perm16 operator*(const Perm16 &p) const

    cdef Perm16 one "Perm16::one" ()
    cdef Perm16 elementary_transposition "Perm16::elementary_transposition" ()

    ctypedef stl_vector[ stl_vector[Perm16] ] StrongGeneratingSet

    # I have to redeclare the stl::list as I use a non standard allocator
    cdef cppclass PG16listIterator "PermutationGroup16::list::iterator":
        bint operator==(PG16listIterator) nogil
        bint operator!=(PG16listIterator) nogil
        void operator++() nogil
        Vect16 operator*() nogil
    cdef cppclass PG16list "PermutationGroup16::list":
        int size() const
        PG16listIterator begin()
        PG16listIterator end()


    cdef cppclass PermutationGroup16 nogil:

        PermutationGroup16(stl_string name, uint64_t N, StrongGeneratingSet sgs)

        bint is_canonical(const Vect16 &v) const
        bint check_sgs() const

        PG16list elements_of_depth(uint64_t depth) const

