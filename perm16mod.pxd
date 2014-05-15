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

cimport group16
from sage.structure.sage_object cimport SageObject

from libcpp.list cimport list as stl_list

cdef class PermGroup16(SageObject):
    cdef group16.PermutationGroup16 *_g
    cdef _sage_g     # PermutationGroup_generic is a Python type
    cdef list dom

    cpdef _check_sgs(self)
    cpdef Vect16List element_of_depth(self, int depth)
    cpdef bint is_canonical(self, Vect16 v)
    cpdef Vect16 canonical(self, Vect16 v)

cdef class Vect16(SageObject):
    cdef group16.Vect16 _p
    cdef list dom

cdef class Vect16List(object):
    cdef group16.PG16list _l
    cdef dom

cdef class Vect16ListIterator(object):
    cdef Vect16List _vl
    cdef group16.PG16listIterator _it, _end
