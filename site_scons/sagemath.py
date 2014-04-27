import subprocess, os

def call_sage(env, cmd):
    if isinstance(cmd, str):
        cmd = [cmd]
    cmd = [os.path.join(env['SAGE_ROOT'], 'sage')] + cmd
    try:
        result = subprocess.check_output(cmd)
    except (subprocess.CalledProcessError, OSError) as e:
        return 0, "Unable to call Sage: %s"%(str(e))
    else:
        return 1, result

def sage_set_variables(env, vars):
    for var in vars:
        print var, hasattr(env, var)
        res, val = call_sage(env, ['-c', 'import sage.env; print sage.env.'+var])
        val = val.strip()
        env[var] = val

def CheckSage(context):
    context.Message('Checking SageMath ... ')
    env = context.sconf.env
    if isinstance(env['sage'], str):
        sage_root = env['sage']
    else:
        sage_root = ''
    env['SAGE_ROOT'] = sage_root
    res, sage_root = call_sage(env, ["-sh", "-c", "echo $SAGE_ROOT"])
    sage_root = sage_root.strip()
    if res:
        env['SAGE_ROOT'] = sage_root
        context.Result(sage_root)
        sage_set_variables(env, ['SAGE_LOCAL', 'SAGE_SRC', 'SAGE_LIB'])
    else:
        env['SAGE_ROOT'] = None
    return res

Tests = { key : val for key, val in globals().items() if key.startswith('Check') }
