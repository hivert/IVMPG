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
        r"""
        sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
        sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
        sage: perm16mod.PermGroup16(G6)
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
        r"""
        sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
        sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
        sage: G6cpp = perm16mod.PermGroup16(G6)
        sage: del G6cpp
        """
        del self._g

    def __repr__(self):
        r"""
        sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
        sage: G = PermutationGroup([[(3,5),(4,6)]])
        sage: perm16mod.PermGroup16(G)
        C++ wrapper around Permutation Group with generators [(3,5)(4,6)]
        """
        return "C++ wrapper around "+repr(self._sage_g)

    cpdef _check_sgs(self):
        r"""
        sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
        sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
        sage: G6cpp = perm16mod.PermGroup16(G6)
        sage: G6cpp._check_sgs()
        """
        if not self._g.check_sgs():
            raise ValueError, "Incorrect strong genrating system"

    cpdef Vect16List element_of_depth(self, int depth):
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: G6cpp.element_of_depth(0)
            <perm16mod.Vect16List object at 0x...>
            sage: list(G6cpp.element_of_depth(0))
            [[0,0,0,0,0,0]]
            sage: list(G6cpp.element_of_depth(1))
            [[1,0,0,0,0,0]]
            sage: list(G6cpp.element_of_depth(2))
            [[2,0,0,0,0,0], [1,1,0,0,0,0], [1,0,1,0,0,0], [1,0,0,1,0,0]]
            sage: list(G6cpp.element_of_depth(3))
            [[3,0,0,0,0,0], [2,1,0,0,0,0], [2,0,1,0,0,0], [2,0,0,1,0,0],
             [1,1,1,0,0,0], [1,0,1,0,1,0], [1,0,0,1,0,1]]

        TESTS::

            sage: l6 = G6cpp.element_of_depth(10)
            sage: len(l6)
            280
            sage: all(len(x) == 6 for x in l6)
            True
        """
        cdef Vect16List res = Vect16List.__new__(Vect16List)
        res.dom =  self.dom
        sig_on()
        with nogil:
            res._l = self._g.elements_of_depth(depth)
        sig_off()
        return res

    cpdef bint is_canonical(self, Vect16 v):
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: G6cpp.is_canonical(perm16mod.Vect16([7,1,0,2,0,0]))
            True
            sage: G6cpp.is_canonical(perm16mod.Vect16([7,1,0,2,0,3]))
            False

        TESTS::

            sage: l6 = G6cpp.element_of_depth(10)
            sage: all(G6cpp.is_canonical(x) for x in l6)
            True
        """
        return self._g.is_canonical(v._p)

    cpdef Vect16 canonical(self, Vect16 v):
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: G6cpp.canonical(perm16mod.Vect16([7,1,0,2,0,0]))
            [7,1,0,2,0,0]
            sage: G6cpp.canonical(perm16mod.Vect16([7,1,0,2,0,3]))
            [7,1,0,3,0,2]
            sage: G6cpp.canonical( perm16mod.Vect16([1,2,3,4,5,6], G6cpp) )
            [6,5,3,4,2,1]


        TESTS::

            sage: l6 = G6cpp.element_of_depth(10)
            sage: all(G6cpp.canonical(x) == x for x in l6)
            True
            sage: all(G6cpp.is_canonical(G6cpp.canonical(perm16mod.Vect16(v, G6cpp)))
            ....:     for v in IntegerVectors(5, 6).list())
            True

        """
        cdef Vect16 res = Vect16.__new__(Vect16)
        res.dom =  self.dom
        res._p = self._g.canonical(v._p)
        return res

cdef list defaultdom = range(1,17)

cdef class Vect16(SageObject):
    def __init__(self, list l, PermGroup16 dom=None):
        cdef int i
        if dom is None:
            self.dom = defaultdom
        else:
            self.dom = dom.dom

        if len(l) > len(self.dom):
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

    # < 0    <= 1    == 2    != 3    > 4    >= 5
    def __richcmp__(Vect16 self, Vect16 other, int op):
        """
        Lexicographic order

        EXAMPLE::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: V = perm16mod.Vect16
            sage: V([1,2]) < V([2,1])
            True
            sage: V([1,2]) <= V([2,1])
            True
            sage: V([1,2]) == V([2,1])
            False
            sage: V([1,2]) > V([2,1])
            False
            sage: V([1,2]) >= V([2,1])
            False

        TESTS::

            sage: comps = [ lambda x,y:x<y, lambda x,y:x<=y, lambda x,y:x==y,
            ....:           lambda x,y:x>y, lambda x,y:x>=y, lambda x,y:x!=y ]
            sage: check = [[0,0,0],[0,1,0],[0,0,1],[1,2],[2,1],[1,1],[1,0,2],[1,0,3],[1,1,3]]
            sage: all(c(V(a), V(b)) == c(a,b) for c in comps
            ....:     for a in check for b in check)
            True
        """
        if   op == 0: return self._p < other._p
        elif op == 1: return self._p < other._p or self._p == other._p
        elif op == 2: return self._p == other._p
        elif op == 3: return not (self._p == other._p)
        elif op == 4: return other._p < self._p
        elif op == 5: return other._p < self._p or self._p == other._p


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


