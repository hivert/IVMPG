import os; os.sys.path.insert(0,os.path.abspath('.'))
import perm16mod

import sage.misc.stopgap
sage.misc.stopgap.set_state(False)

def applyPlambda(la, ex):
    return prod(ex.subs(x=x**i) for i in la)
def expandZ(Z, ex):
    return sum(co*applyPlambda(la, ex) for la,co in Z)

def serZ(Z, z):
    return sum(co*prod(1/(1-z**i) for i in la) for la,co in Z)



def CheckGroup(i, Gr, Deg=30):
    """
    sage: Sn=SymmetricGroup(5)
    sage: SGrs = Sn.subgroups()
    sage: len(SGrs)
    sage: all([CheckGroup(*x) for x in enumerate(SGrs)])
    0 [()] 0
    ...
    True
    """
    R = PowerSeriesRing(QQ, 'z', default_prec = Deg)
    z = R.gens()[0]

    Grcpp = perm16mod.PermGroup16(Gr)
    Z = Gr.cycle_index()
    # tay = taylor(expandZ(Z, 1/(1-x)), x, 0, Deg)
    tay = serZ(Z, z)
    mon = sum(Integer(len(Grcpp.element_of_depth(i))) * z**i for i in range(Deg))
    res = (tay-mon).polynomial()
    print i, Gr.gens(), res
    if res <> 0:
        print mon
        print tay
    return res == 0

# WARNING ! Thread / Fork problem seem to arise here
# @Parallel and Cilk interact badly.
@parallel
def CheckGroupParall(i, Gr, Deg=30):
    """
    sage: Sn=SymmetricGroup(6)
    sage: SGrs = Sn.subgroups()
    sage: len(SGrs)
    sage: all(x[1] for x in CheckGroupParall(list(enumerate(SGrs))))
    """
    R = PowerSeriesRing(QQ, 'z', default_prec = Deg)
    z = R.gens()[0]

    Grcpp = perm16mod.PermGroup16(Gr)
    Z = Gr.cycle_index()
    # tay = taylor(expandZ(Z, 1/(1-x)), x, 0, Deg)
    tay = serZ(Z, z)
    mon = sum(Integer(len(Grcpp.element_of_depth(i))) * z**i for i in range(Deg))
    res = (tay-mon).polynomial()
    print i, Gr.gens(), res
    if res <> 0:
        print mon
        print tay
    return res == 0

