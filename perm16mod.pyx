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

    cpdef Vect16List elements_of_depth(self, int depth):
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: G6cpp.elements_of_depth(0)
            <perm16mod.Vect16List object at 0x...>
            sage: list(G6cpp.elements_of_depth(0))
            [[0,0,0,0,0,0]]
            sage: list(G6cpp.elements_of_depth(1))
            [[1,0,0,0,0,0]]
            sage: list(G6cpp.elements_of_depth(2))
            [[2,0,0,0,0,0], [1,1,0,0,0,0], [1,0,1,0,0,0], [1,0,0,1,0,0]]
            sage: list(G6cpp.elements_of_depth(3))
            [[3,0,0,0,0,0], [2,1,0,0,0,0], [2,0,1,0,0,0], [2,0,0,1,0,0],
             [1,1,1,0,0,0], [1,0,1,0,1,0], [1,0,0,1,0,1]]

        TESTS::

            sage: l6 = G6cpp.elements_of_depth(10)
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

    cpdef Vect16List elements_of_evaluation(self, Vect16 v):
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: V = perm16mod.Vect16
            sage: G6cpp.elements_of_evaluation(V([6,0,0,0,0,0]))
            <perm16mod.Vect16List object at 0x...>
            sage: list(G6cpp.elements_of_evaluation(V([6,0,0,0,0,0])))
            [[0,0,0,0,0,0]]
            sage: list(G6cpp.elements_of_evaluation(V([5,1,0,0,0,0])))
            [[1,0,0,0,0,0]]
            sage: list(G6cpp.elements_of_evaluation(V([4,2,0,0,0,0])))
            [[1,1,0,0,0,0], [1,0,1,0,0,0], [1,0,0,1,0,0]]
            sage: list(G6cpp.elements_of_evaluation(V([5,0,1,0,0,0])))
            [[2,0,0,0,0,0]]
            sage: list(G6cpp.elements_of_evaluation(V([0,1,3,2,0,0])))
            [[3,2,2,2,1,3], [3,2,2,3,1,2], [3,2,2,3,2,1], [3,2,3,1,2,2], [3,2,3,2,2,1], [3,3,2,2,2,1]]

        TESTS::

            sage: list(G6cpp.elements_of_evaluation(V([1,0,0,0,0,0])))
            Traceback (most recent call last):
            ...
            ValueError: Evaluation should be of sum 6
        """
        cdef Vect16List res = Vect16List.__new__(Vect16List)
        if sum(v) != len(self.dom):
            raise ValueError("Evaluation should be of sum %s"%len(self.dom))
        res.dom =  self.dom
        sig_on()
        with nogil:
            res._l = self._g.elements_of_evaluation(v._p)
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

            sage: l6 = G6cpp.elements_of_depth(10)
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

            sage: l6 = G6cpp.elements_of_depth(10)
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
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: perm16mod.Vect16([7,1,0,2,0,0])
            [7,1,0,2,0,0,0,0,0,0,0,0,0,0,0,0]
            sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: perm16mod.Vect16([7,1,0,2,0,0], G6cpp)
            [7,1,0,2,0,0]
        """
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
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: len(perm16mod.Vect16([7,1,0,2,0,0]))
            16
            sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: len(perm16mod.Vect16([7,1,0,2,0,0], G6cpp))
            6
        """
        return len(self.dom)

    def __getitem__(self, unsigned int i):
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: perm16mod.Vect16([7,1,0,2,0,0])[3]
            2
            sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: perm16mod.Vect16([7,1,0,2,0,0], G6cpp)[3]
            2

            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: perm16mod.Vect16([7,1,0,2,0,0], G6cpp)[6]
            Traceback (most recent call last):
            ...
            IndexError
        """
        if i < len(self):
            return self._p[i]
        else:
            raise IndexError

    def __repr__(self):
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: perm16mod.Vect16([7,1,0,2,0,0])
            [7,1,0,2,0,0,0,0,0,0,0,0,0,0,0,0]
            sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: perm16mod.Vect16([7,1,0,2,0,0], G6cpp)
            [7,1,0,2,0,0]
        """
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
    r"""
    Internal class wrapping a C++ linked list

    EXAMPLES::

        sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
        sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
        sage: G6cpp = perm16mod.PermGroup16(G6)
        sage: ll = G6cpp.elements_of_depth(0)
        sage: isinstance(ll, perm16mod.Vect16List)
        True
    """
    def __init__(self):
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: V = perm16mod.Vect16List
            sage: V()
            Traceback (most recent call last):
            ...
            RuntimeError: C++ interface class ! You cannot build a Vect16List from Python
        """
        raise RuntimeError, "C++ interface class ! You cannot build a Vect16List from Python"

    def __len__(self):
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: ll = G6cpp.elements_of_depth(0)
            sage: len(ll)
            1
            sage: ll = G6cpp.elements_of_depth(5)
            sage: len(ll)
            26
        """
        return self._l.size()

    def __iter__(self):
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: ll = G6cpp.elements_of_depth(0)
            sage: list(ll)
            [[0,0,0,0,0,0]]
            sage: ll = G6cpp.elements_of_depth(5)
            sage: l = list(ll)
            sage: len(l)
            26
            sage: it = iter(ll)
            sage: it.next()
            [5,0,0,0,0,0]
        """
        return Vect16ListIterator(self)


cdef class Vect16ListIterator(object):
    r"""
    EXAMPLES::

        sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
        sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
        sage: G6cpp = perm16mod.PermGroup16(G6)
        sage: l = G6cpp.elements_of_depth(0);
        sage: it = perm16mod.Vect16ListIterator(l)
        sage: isinstance(it, perm16mod.Vect16ListIterator)
        True
        """
    def __cinit__(self, Vect16List l not None):
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: l = G6cpp.elements_of_depth(0);
            sage: it = perm16mod.Vect16ListIterator(l)
            sage: isinstance(it, perm16mod.Vect16ListIterator)
            True

        TESTS::

            sage: perm16mod.Vect16ListIterator()
            Traceback (most recent call last):
            ...
            TypeError: __cinit__() takes exactly 1 positional argument (0 given)
            """
        self._vl = l  # keep a reference on _vl to prevent _vl._l from being deallocated
        self._it = self._vl._l.begin()
        self._end = self._vl._l.end()

    def __next__(self):
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: l = G6cpp.elements_of_depth(5);
            sage: it = perm16mod.Vect16ListIterator(l)
            sage: it.next()
            [5,0,0,0,0,0]
            sage: it.next()
            [4,1,0,0,0,0]
            sage: it.next()
            [4,0,1,0,0,0]
        """
        cdef Vect16 res = Vect16.__new__(Vect16)
        if self._it != self._end:
            res.dom = self._vl.dom
            res._p = cython.operator.dereference(self._it)
            cython.operator.preincrement(self._it)
            return res
        else:
            raise StopIteration

    def __iter__(self):
        r"""
        EXAMPLES::

            sage: import os; os.sys.path.insert(0,os.path.abspath('.')); import perm16mod
            sage: G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
            sage: G6cpp = perm16mod.PermGroup16(G6)
            sage: l = G6cpp.elements_of_depth(5);
            sage: it = perm16mod.Vect16ListIterator(l)
            sage: iter(it)
            <perm16mod.Vect16ListIterator object at 0x...>
        """
        return self


