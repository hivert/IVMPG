import os
from utils import *
import cpuAVX, cilk

######################################################################################

Help("""
Type: 'scons [options] program' to build the production program,
      'scons [options] module' to build the sage module,
      'scons [options] check' to launch unit test suite,
      'scons [options] timing' to perfom timing measurement.
""")

######################################################################################

env = Environment(CXXFLAGS=['-std=c++11', '-O3', '-Wall', '-Wno-missing-braces'])

######################################################################################

if not env.GetOption('clean') and not env.GetOption('help'):

    if cilk.SearchCilkCompiler(env) is None:
        Fail('Unable to find a cilk compiler ! Not using cilk !')

    conf = Configure(env, config_h = "config.h" )
    conf.AddTests(cpuAVX.Tests)
    conf.AddTests(cilk.Tests)

    if not conf.CheckCXX():
        Fail('!! Your compiler and/or environment is not correctly configured.')

    if not conf.CheckCilkPlusCompiler():
        Fail('Your cilk compiler is not working !')

    for lib in Split('cstdint array iostream x86intrin.h'):
        if not conf.CheckCXXHeader(lib):
            Fail("You need '%s' to compile this program"%(lib))
    for typ, include in [('uint8_t', 'cstdint'),
                         ('uint64_t', 'cstdint'),
                         ('__m128i', 'x86intrin.h')]:
        if not conf.CheckType(typ, '#include <%s>\n'%include, 'c++'):
            Fail("Did not find '%s' in '%s'"%(typ, include))

    if not conf.CheckProcInstructionSets(["POPCNT", "SSE4_2", "AVX"]):
        Fail("Your processor doesn't seems to support avx instuction set !")
    else:
        env.Append(CXXFLAGS = ['-mavx', '-mtune=native']) # TODO check sse4.2 is ok
    if conf.CheckGCCVectorExtension():
        conf.Define('GCC_VECT_CMP', 1, 'Set to 1 if GCC has vector comparison')

    if conf.CheckCXXHeader('boost/container/flat_set.hpp'):
        conf.Define('USE_BOOST_FLAT_SET', 1,
                    "Set to 1 if using 'boost::container::flat_set' instead of 'std::set'")
    else:
        print("Unable to find 'boost::container::flat_set'!")
        print("Falling back to default 'std::set'.")

    tbb_dir = ARGUMENTS.get('tbb_dir', os.environ.get('TBB_ROOT', None))
    if tbb_dir is not None:
        env.Append(CPPPATH = [os.path.join(tbb_dir, 'include')],
                   LIBPATH = [os.path.join(tbb_dir, 'lib')],
                   RPATH   = [os.path.join(tbb_dir, 'lib')])

    if (conf.CheckLibWithHeader('tbb', 'tbb/scalable_allocator.h', 'c++') and
        conf.CheckLibWithHeader('tbbmalloc', 'tbb/scalable_allocator.h', 'c++')):
        conf.Define('USE_TBB', 1,
                    "Set to 1 if using 'tbb::scalable_allocator' insteaf of 'std::allocator'")
    else:
        print("Unable to find TBB ! "
              "Please add 'tbb_dir=<tbb_path> to the command line")
        print("Falling back to default allocator")
    env = conf.Finish()

######################################################################################

test_env = env.Clone()

if not env.GetOption('clean') and not env.GetOption('help'):
    test_conf = Configure(test_env)
    if not test_conf.CheckLibWithHeader('boost_unit_test_framework',
                                        'boost/test/unit_test.hpp', 'c++'):
        Fail("Did not find 'boost unit test', unable to perform check !")
    test_env = test_conf.Finish()
    test_env.Append(LIBPATH = ['.'], LIBS = ['libgroup16'], RPATH = ['.'])

######################################################################################

perm16_lib = env.SharedLibrary(target = 'perm16',
                               source = Split('perm16.cpp'))
group16_lib  = env.SharedLibrary(target = 'group16',
                                 source = Split('group16.cpp perm16.cpp group16_examples.cpp'))

perm16_test = test_env.Program(target = 'perm16_test', source = 'perm16_test.cpp')
group_test  = test_env.Program(
    target = 'group16_test', source = Split('group16_test.cpp group16_examples.cpp'))
group_time  = test_env.Program(
    target = 'timing', source = Split('timing.cpp group16_examples.cpp'))

######################################################################################

sage_env = env.Clone()
sage_env.Tool("cython")

import sage.env
SAGE_INC = os.path.join(sage.env.SAGE_LOCAL, 'include')
SAGE_LIB = os.path.join(sage.env.SAGE_LOCAL, 'lib')
SAGE_PYTHON  = os.path.join(SAGE_INC, 'python2.7')
SAGE_C   = os.path.join(sage.env.SAGE_SRC, 'c_lib', 'include')
SAGE_DEV = os.path.join(sage.env.SAGE_ROOT, 'src')

sage_env['CYTHONFLAGS'] = "--cplus -I"+SAGE_DEV
sage_env.Append(CPPPATH = [SAGE_PYTHON, SAGE_INC,
                           SAGE_C, SAGE_DEV],
                LIBPATH = ['.', SAGE_LIB],
                LIBS = ['libgroup16', 'csage'],
                RPATH   = ['.', SAGE_LIB]
)
perm16cython  = sage_env.Cython(target = 'perm16mod.cpp',
                                source = 'perm16mod.pyx')
Depends(perm16cython, Split('perm16mod.pxd group16.pxd'))
perm16mod_lib = sage_env.SharedLibrary(
    target = 'perm16mod', source = [perm16cython], SHLIBPREFIX='')


######################################################################################

test_env.AlwaysBuild(Alias('check'))
test_env.Alias('check', [perm16_test], perm16_test[0].abspath)
test_env.Alias('check', [group_test], group_test[0].abspath)

if not env.GetOption('clean') and not env.GetOption('help'):
    env.Tool("etags")
    Alias('tags', env.etags(source=Glob('*.[ch]pp'), target='etags'))

env.Clean("distclean", [ Split(".sconsign.dblite .sconf_temp config.h config.log TAGS"),
                         Glob(os.path.join("site_scons","*.pyc")),
                         Glob(os.path.join("site_scons", "site_tools", "*.pyc")),
                     ])

env.Default(perm16_lib, group16_lib, perm16mod_lib)

print "BUILD_TARGETS is", [str(node) for node in BUILD_TARGETS]
