import os

def CheckVectorExtension(context):
    test_vector_ext = """
    #include <cstdint>
    #include <x86intrin.h>
    using epi8 = uint8_t __attribute__ ((vector_size (16), aligned(16)));
    int main(int argc, char **argv) {
       __m128i x {1,2}, y = (x < x); epi8 v {1,2};
       y = _mm_shuffle_epi8(x, y);
       return 0;
    }
    """
    context.Message('Checking for Advanced Vector eXtension instruction set... ')
    result = context.TryCompile(test_vector_ext, '.cpp')
    context.Result(result)
    return result

def Fail(message):
    print(message)
    Exit(1)

######################################################################################

Help("""
Type: 'scons program' to build the production program,
      'scons check' to launch unit test suite.
""")

######################################################################################

env = Environment(CXXFLAGS=['-std=c++11', '-O3', '-Wall', '-Wno-missing-braces'])

######################################################################################

if os.environ.has_key('CXX'):
    env['CXX'] = os.environ['CXX']


######################################################################################

if not env.GetOption('clean') and not env.GetOption('help'):
    conf = Configure(env)
    conf.AddTests({'CheckVectorExtension' : CheckVectorExtension})

    if not conf.CheckCXX():
        Fail('!! Your compiler and/or environment is not correctly configured.')
    for lib in Split('cstdint array iostream x86intrin.h'):
        if not conf.CheckCXXHeader(lib):
            Fail("You need '%s' to compile this program"%(lib))
    for typ, include in {'uint8_t': 'cstdint',
                         '__m128i': 'x86intrin.h'}.items():
        if not conf.CheckType(typ, '#include <%s>\n'%include, 'c++'):
            Fail("Did not find '%s' in '%s'"%(typ, include))
    env.Append(CXXFLAGS = ['-mavx', '-mtune=corei7-avx'])
    if conf.CheckVectorExtension():
        env.Append(CPPDEFINES = ['GCC_VECT_CMP'])

    env = conf.Finish()

######################################################################################

test_env = env.Clone()

if not env.GetOption('clean') and not env.GetOption('help'):
    test_conf = Configure(test_env)
    if not test_conf.CheckLibWithHeader('boost_unit_test_framework',
                                        'boost/test/unit_test.hpp', 'c++'):
        Fail("Did not find 'boost unit test', unable to perform check !")
    test_env = test_conf.Finish()

######################################################################################

perm16_lib = env.SharedLibrary(target = 'perm16',
                               source = Split('perm16.cpp'))
group16_lib  = env.SharedLibrary(target = 'group16',
                                 source = Split('group16.cpp perm16.cpp'))

perm16_test = test_env.Program(
    target = 'perm16_test',
    source = Split('perm16_test.cpp perm16.cpp'))
group_test  = test_env.Program(
    target = 'group16_test',
    source = Split('group16_test.cpp group16_examples.cpp perm16.cpp'))

######################################################################################

test_env.AlwaysBuild(Alias('check'))
test_env.Alias('check', [perm16_test], perm16_test[0].abspath)
test_env.Alias('check', [group_test], group_test[0].abspath)

env.Tool("etags")
Alias('tags', env.etags(source=Glob('*.[ch]pp'), target='etags'))

env.Clean("distclean", [ ".sconsign.dblite", ".sconf_temp", "config.log", "TAGS", ])

env.Default(perm16_lib, group16_lib)

print "BUILD_TARGETS is", [str(node) for node in BUILD_TARGETS]
