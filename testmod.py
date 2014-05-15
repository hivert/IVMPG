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

import os; os.sys.path.insert(0,os.path.abspath('.'))
import perm16mod

G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
G6cpp  = perm16mod.PermGroup16(G6)
"""
sage: G6cpp._check_sgs()
sage: l6 = G6cpp.element_of_depth(10)
sage: len(l6)
280
sage: all(len(x) == 6 for x in l6)
True
sage: l6sage = list(l6)
sage: len(l6sage)
280
sage: l6sage[0]
[10,0,0,0,0,0]
sage: l6sage[20]
[7,1,0,2,0,0]
sage: all(G6cpp.is_canonical(x) for x in l6sage)
True
sage: G6cpp.is_canonical(perm16mod.Vect16([7,1,0,2,0,0]))
True
sage: G6cpp.is_canonical(perm16mod.Vect16([7,1,0,2,0,3]))
False
sage: len(perm16mod.Vect16([1,2]))
16
sage: perm16mod.Vect16([1,2])
[1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
sage: len(perm16mod.Vect16([1,2], G6cpp))
6
sage: perm16mod.Vect16([1,2], G6cpp)
[1,2,0,0,0,0]
"""

# N. Borie's Favorite
G = PermutationGroup([[(1,8,14,12,3,7,13,9,2,5,16,11),(4,6,15,10)],
                      [(1,13,10),(2,14,12,3,15,9,4,16,11),(5,6),(7,8)]])
Gcpp  = perm16mod.PermGroup16(G)
"""
sage: Gcpp._check_sgs()
sage: ll = Gcpp.element_of_depth(15)
sage: len(ll)
6686
sage: lsage = list(ll)
sage: len(lsage)
6686
sage: lsage[0]
[15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
sage: lsage[20]
[12,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0]
sage: all(Gcpp.is_canonical(x) for x in lsage)
True
"""
