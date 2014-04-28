import subprocess, os
from SCons.Action import Action

def call_sage(env, cmd):
    if isinstance(cmd, str):
        cmd = [cmd]
    cmd = [env['SAGECMD']] + cmd
    try:
        result = subprocess.check_output(cmd)
    except (subprocess.CalledProcessError, OSError) as e:
        return 0, "Unable to call Sage: %s"%(str(e))
    else:
        return 1, result

def sage_set_variables(context, env, vars):
    res, val = call_sage(env, ['-c',
"""
import sage.env;
for v in %s:
    print getattr(sage.env, v)
"""%str(vars)])
    vals = val.strip().split("\n")
    for var, val in zip(vars, vals):
        env[var] = val
        context.Log("Found ${%s} = '%s'\n"%(var, val))

def CheckSage(context):
    context.Message('Checking SageMath ... ')
    env = context.sconf.env
    if not env['sage']: return None
    if env['SAGE_ROOT'] is not None:
        context.sconf.cached = 1
        context.Result(env['SAGE_ROOT'])
        return True
    context.sconf.cached = 0
    if isinstance(env['sage'], str):
        sage = env.WhereIs('sage', env['sage'])
    else:
        sage = env.WhereIs('sage', os.environ['PATH'])
    if not sage: return None
    env['SAGE_ROOT'] = os.path.dirname(sage)
    env['SAGECMD'] = sage
    res, sage_root = call_sage(env, ["-sh", "-c", "echo $SAGE_ROOT"])
    sage_root = sage_root.strip()
    if res:
        env['SAGE_ROOT'] = sage_root
        env['SAGECMD'] = os.path.join(sage_root, 'sage')
        context.Result(sage_root)
        sage_set_variables(context, env, ['SAGE_LOCAL', 'SAGE_SRC', 'SAGE_LIB'])
    else:
        env['SAGE_ROOT'] = None
    return res

Tests = { key : val for key, val in globals().items() if key.startswith('Check') }
