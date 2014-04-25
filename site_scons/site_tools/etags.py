# -*- coding: utf-8 -*-
import SCons.Builder
import SCons.Action

def complain_etags(target, source, env):
    return 'etags binary was not found (see above). Tags have not been built.'

def generate(env):
    env['ETAGS'] = find_etags(env)
    if env['ETAGS'] != None:
        env['ETAGSCOM'] = '$ETAGS $SOURCES'
        env['BUILDERS']['etags'] = SCons.Builder.Builder(action=env['ETAGSCOM'])
    else:
        env['BUILDERS']['etags'] = SCons.Builder.Builder(action=env.Action(complain_etags))

def find_etags(env):
    return env.WhereIs('etags')

def exists(env):
    return find_etags(env) != None
