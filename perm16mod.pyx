#clang C++

import cython
from sage.rings.integer cimport Integer
from sage.structure.sage_object cimport SageObject

include 'sage/ext/interrupt.pxi'

cimport group16

from libcpp.vector cimport vector as stl_vector
from libcpp.string cimport string as stl_string
from libcpp.list cimport list as stl_list

from sage.groups.perm_gps.permgroup import PermutationGroup_generic as SagePG

cdef class PermGroup16(SageObject):
    def __init__(self, gr):
        """
        sage: import os; os.sys.path.insert(0,os.path.abspath('.'))
        sage: import perm16mod
        sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
        sage: G6cpp = perm16mod.PermGroup16(G6)
        sage: G6cpp._check_sgs()
        sage: G6cpp
        C++ wrapper around Permutation Group with generators [(3,5)(4,6), (1,2)(3,4)(5,6), (1,4,6)(2,3,5)]
        """
        cdef stl_vector[stl_vector[group16.Perm16]] sgs
        cdef stl_vector[group16.Perm16] lev
        cdef group16.Perm16 p16
        if not isinstance(gr, SagePG):
            raise ValueError, "%s is not a permutation group"%(SagePG)
        self.dom = list(gr.domain())
        N = len(self.dom)
        if N > 16:
            raise ValueError, "%s doesn't fit in S16 (len=%s)"%(SagePG, N)
        self._sage_g = gr
        SGS = gr.strong_generating_system()
        for level in SGS:
            lev.clear()
            for per in level:
                for i in range(N):
                    p16[i] = self.dom.index(per(self.dom[i]))
                lev.push_back(p16)
            sgs.push_back(lev)
        self._g = new group16.PermutationGroup16(repr(gr), N, sgs)

    def __del__(self):
        del self._g

    def __repr__(self):
        return "C++ wrapper around "+repr(self._sage_g)

    cpdef _check_sgs(self):
        if not self._g.check_sgs():
            raise ValueError, "Incorrect strong genrating system"

    cpdef Vect16List element_of_depth(self, int depth):
        cdef Vect16List res = Vect16List.__new__(Vect16List)
        res.dom =  self.dom
        sig_on()
        with nogil:
            res._l = self._g.elements_of_depth(depth)
        sig_off()
        return res

    cpdef bint is_canonical(self, Vect16 v):
        return self._g.is_canonical(v._p)

cdef list defaultdom = range(1,17)

cdef class Vect16(SageObject):
    def __init__(self, list l, PermGroup16 dom=None):
        cdef int i
        if dom is None:
            self.dom = defaultdom
        else:
            self.dom = dom.dom

        if len(l) >= len(self.dom):
            raise ValueError, "list too long"
        for i in range(len(l)):
            self._p[i] = l[i]
        for i in range(len(l), 16):
            self._p[i] = 0

    def __len__(self):
        return len(self.dom)

    def __getitem__(self, unsigned int i):
        if i < len(self):
            return self._p[i]
        else:
            raise IndexError

    def __repr__(self):
         res = ",".join(str(self._p[i]) for i in range(len(self.dom)))
         return "["+res+"]"

cdef class Vect16List(object):
    def __init__(self):
        raise RuntimeError, "You are not supposed to call init"

    def __len__(self):
        return self._l.size()

    def __iter__(self):
        return Vect16ListIterator(self)

cdef class Vect16ListIterator(object):
    def __cinit__(self, Vect16List l):
        self._vl = l  # keep a reference on _vl to prevent _vl._l from being deallocated
        self._it = self._vl._l.begin()
        self._end = self._vl._l.end()

    def __next__(self):
        cdef Vect16 res = Vect16.__new__(Vect16)
        if self._it != self._end:
            res.dom = self._vl.dom
            res._p = cython.operator.dereference(self._it)
            cython.operator.preincrement(self._it)
            return res
        else:
            raise StopIteration

    def __iter__(self):
        return self


