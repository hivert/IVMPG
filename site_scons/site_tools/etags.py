# -*- coding: utf-8 -*-
import SCons.Builder
import SCons.Action

def complain_etags(target, source, env):
    print 'INFORMATION: etags binary was not found (see above). Tags have not been built.'

def generate(env):
    env['ETAGS'] = find_etags(env)
    if env['ETAGS'] != None:
        env['ETAGSCOM'] = '$ETAGS $SOURCES'
        env['BUILDERS']['etags'] = SCons.Builder.Builder(action=env['ETAGSCOM'])
    else:
        env['BUILDERS']['etags'] = SCons.Builder.Builder(action=env.Action(complain_etags))

def find_etags(env):
    b=env.WhereIs('etags')
    if b == None:
        print 'Searching for etags: not found. Tags will not be built'
    else:
        print 'Searching for etags: ', b
    return b

def exists(env):
    print "Called exists !!!"
    if find_etags(env) == None:
        return 0
    return 1
