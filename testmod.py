import os; os.sys.path.insert(0,os.path.abspath('.'))
import perm16mod

G6 = PermutationGroup([[(3,5),(4,6)], [(1,2),(3,4),(5,6)], [(1,4,6),(2,3,5)]])
G6cpp  = perm16mod.PermGroup16(G6)
G6cpp._check_sgs()
l6 = G6cpp.element_of_depth(10)
assert(len(l6) == 280)
assert(all(len(x) == 6) for x in l6)

# N. Borie's Favorite
G = PermutationGroup([[(1,8,14,12,3,7,13,9,2,5,16,11),(4,6,15,10)],
                      [(1,13,10),(2,14,12,3,15,9,4,16,11),(5,6),(7,8)]])
Gcpp  = perm16mod.PermGroup16(G)
Gcpp._check_sgs()
ll = Gcpp.element_of_depth(15)
assert(len(ll) == 6686)
assert(len(list(ll)) == 6686)

"""
sage: len(ll) == 6686
True
"""
