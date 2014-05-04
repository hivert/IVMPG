import os
import cpuAVX, cilk
from SCons.Errors import StopError
from SCons.Warnings import warn, Warning, enableWarningClass

class ConfigureWarning(Warning): pass
enableWarningClass(ConfigureWarning)

######################################################################################

Help("""
Type: 'scons [options] program' to build the production program,
      'scons [options] module' to build the sage module,
      'scons [options] check' to launch unit test suite,
      'scons [options] timing' to perfom timing measurement.
""")

######################################################################################


env = Environment(CXXFLAGS=['-std=c++11', '-O3', '-ggdb', '-funroll-loops', '-DNDEBUG',
                            '-Wall', '-Wno-missing-braces', '-Wno-unused-variable'])

vars = Variables()

env.Append(TBB_ROOT = os.environ.get('TBB_ROOT', 'yes'))
vars.Add(PackageVariable('tbb', 'thread building block', '${TBB_ROOT}'))
env.Append(CILK_ROOT = os.environ.get('CILK_ROOT', 'yes'))
vars.Add(PackageVariable('cilk', 'cilk compiler installation', '${CILK_ROOT}'))

vars.Update(env)
Help(vars.GenerateHelpText(env))

######################################################################################


if not env.GetOption('clean') and not env.GetOption('help'):

    conf = Configure(env, config_h = "config.h" )
    conf.AddTests(cpuAVX.Tests)
    conf.AddTests(cilk.Tests)

    if env['cilk'] and conf.CheckCilkPlusCompiler():
        conf.Define('USE_CILK', 1, 'Set to 1 if using Cilk compiler')
    elif not conf.CheckCXX():
        raise StopError('!! Your compiler and/or environment is not correctly configured.')

    for lib in Split('cstdint array iostream x86intrin.h'):
        if not conf.CheckCXXHeader(lib):
            raise StopError("You need '%s' to compile this program"%(lib))
    for typ, include in [('uint8_t', 'cstdint'),
                         ('uint64_t', 'cstdint'),
                         ('__m128i', 'x86intrin.h')]:
        if not conf.CheckType(typ, '#include <%s>\n'%include, 'c++'):
            raise StopError("Did not find '%s' in '%s'"%(typ, include))

    if not conf.CheckProcInstructionSets(["POPCNT", "SSE4_2", "AVX"]):
        raise StopError("Your processor doesn't seems to support avx instuction set !")
    else:
        env.Append(CXXFLAGS = ['-mavx', '-mtune=native']) # TODO check sse4.2 is ok
    if conf.CheckGCCVectorExtension():
        conf.Define('GCC_VECT_CMP', 1, 'Set to 1 if GCC has vector comparison')

    if conf.CheckCXXHeader('boost/container/flat_set.hpp'):
        conf.Define('USE_BOOST_FLAT_SET', 1,
                    "Set to 1 if using 'boost::container::flat_set' instead of 'std::set'")
    else:
        warn(ConfigureWarning,
             "Unable to find 'boost::container::flat_set'!"
             "\n                Falling back to default 'std::set'.\n")

    if isinstance(env['tbb'], str):
        env.Append(CPPPATH = [os.path.join(env['tbb'], 'include')],
                   LIBPATH = [os.path.join(env['tbb'], 'lib')],
                   RPATH   = [os.path.join(env['tbb'], 'lib')])

    if env['tbb']:
        if ( conf.CheckLibWithHeader('tbb', 'tbb/scalable_allocator.h', 'c++') and
             conf.CheckLibWithHeader('tbbmalloc', 'tbb/scalable_allocator.h', 'c++')):
           conf.Define('USE_TBB', 1,
                       "Set to 1 if using 'tbb::scalable_allocator'"
                       "instead of 'std::allocator'")
        else:
            warn(ConfigureWarning, "Unable to find TBB ! "
                 "Please add 'tbb=<tbb_path> to the command line"
                 "\n                Falling back to default allocator\n")
    env = conf.Finish()

######################################################################################

test_env = env.Clone(SAGE_TESTS_OPTS=[])

if not env.GetOption('clean') and not env.GetOption('help'):
    test_conf = Configure(test_env)
    if not test_conf.CheckLibWithHeader('boost_unit_test_framework',
                                        'boost/test/unit_test.hpp', 'c++'):
        raise StopError("Did not find 'boost unit test', unable to perform check !")
    test_env = test_conf.Finish()

######################################################################################

env.Tool("cython")

import sage.env

# from SAGE_ROOT/src/setup.py:

SAGE_INC = os.path.join(sage.env.SAGE_LOCAL,'include')
SAGE_INCLUDE_DIR = [SAGE_INC, os.path.join(SAGE_INC, 'csage'),
                    sage.env.SAGE_SRC,
                    os.path.join(sage.env.SAGE_SRC, 'sage', 'ext'),
                    os.path.join(SAGE_INC, 'python2.7')]
SAGE_LIB = os.path.join(sage.env.SAGE_LOCAL, 'lib')

perm16mod  = env.Cython('perm16mod.pyx',
                        CYTHONLANG = 'c++',
                        CYTHONFLAGS = ["-I"+sage.env.SAGE_SRC])
Depends(perm16mod, Split('perm16mod.pxd group16.pxd'))

sage_env = env.Clone()
sage_env.Append(CPPFLAGS = ['-fno-strict-aliasing', '-Wno-attributes'],
                CPPPATH = SAGE_INCLUDE_DIR,
                LIBPATH = [SAGE_LIB],
                LIBS    = ['csage'],
                RPATH   = [SAGE_LIB]
)
perm16mod = sage_env.SharedLibrary(source = perm16mod, SHLIBPREFIX='')

######################################################################################

perm16_o = env.Object('perm16.cpp')
perm_test = test_env.Program(['perm_test.cpp', perm16_o])
group_test  = test_env.Program(['group_test.cpp', perm16_o])
group16_test  = test_env.Program(['group16_test.cpp', perm16_o])

group_time  = test_env.Program('timing.cpp')
group_gen_time  = test_env.Program('timing_generic.cpp')

######################################################################################


def sage_test(env,target,source):
    import subprocess
    try:
        result = subprocess.call(
            [os.path.join(sage.env.SAGE_ROOT, 'sage'), '-t'] +
            env['SAGE_TESTS_OPTS'] +
            [source[0].abspath])
    except Exception,e:
        return "Unable to call Sage: %s"%(str(e))
    else:
        return result

test_env.AlwaysBuild(Alias('check'), Alias('checkperm16mod'), Alias('checktestmod'))
checkperm16mod = test_env.Alias('checkperm16mod', ['perm16mod.pyx', perm16mod],
                          sage_test, SAGE_TESTS_OPTS=['--force-lib'])

checktestmod = test_env.Alias('checktestmod', ['testmod.py', perm16mod], sage_test)
test_env.Alias('check', [checkperm16mod, checktestmod])
test_env.Alias('check', [perm_test], perm_test[0].abspath)
test_env.Alias('check', [group_test], group_test[0].abspath)
test_env.Alias('check', [group16_test], group16_test[0].abspath)

######################################################################################

if not env.GetOption('clean') and not env.GetOption('help'):
    env.Tool("etags")
    Alias('tags', env.etags(source=Glob('*.[ch]pp'), target='etags'))

######################################################################################

env.Clean("distclean", [ Split(".sconsign.dblite .sconf_temp config.h config.log TAGS"),
                         Glob(os.path.join("site_scons","*.pyc")),
                         Glob(os.path.join("site_scons", "site_tools", "*.pyc")),
                     ])

env.Default(perm16mod)
