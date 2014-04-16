import os

def CheckVectorExtension(context):
    test_bracket_init = """
    #include <cstdint>
    #include <x86intrin.h>
    using epi8 = uint8_t __attribute__ ((vector_size (16), aligned(16)));
    int main(int argc, char **argv) { __m128i x {1,2}, y = (x < x); epi8 v {1,2}; return 0; }
    """
    context.Message('Checking for vector bracket initialisation... ')
    result = context.TryCompile(test_bracket_init, '.cpp')
    context.Result(result)
    return result

######################################################################################

env = Environment(CXXFLAGS=['-std=c++11', '-O3'])

######################################################################################

if os.environ.has_key('CXX'):
    env['CXX'] = os.environ['CXX']

env.Append(CXXFLAGS = ['-mavx', '-mtune=corei7-avx'])
env.Append(LIBS = ['boost_unit_test_framework'])

######################################################################################

conf = Configure(env)
conf.AddTests({'CheckVectorExtension' : CheckVectorExtension})

if not env.GetOption('clean'):

    if not conf.CheckCXX():
        print('!! Your compiler and/or environment is not correctly configured.')
        Exit(1)

    for lib in Split('cstdint array iostream x86intrin.h'):
        if not conf.CheckCXXHeader(lib):
            print "You need '%s' to compile this program"%(lib)
            Exit(1)

    for typ, include in {'uint8_t': 'cstdint',
                         '__m128i': 'x86intrin.h'}.items():
        if not conf.CheckType(typ, '#include <%s>\n'%include, 'c++'):
            print "Did not find '%s' in '%s'"%(typ, include)
            Exit(1)

    if not conf.CheckLibWithHeader('boost_unit_test_framework',
                                   'boost/test/unit_test.hpp', 'c++'):
        print "Did not find 'boost unit test', exiting!"
        Exit(1)

    if conf.CheckVectorExtension():
        env.Append(CPPDEFINES = ['GCC_VECT_CMP'])

env = conf.Finish()

######################################################################################

shlib = env.SharedLibrary(target='perm16', source = ["perm16.cpp"])
test = env.Program(target = 'perm16_test', source = ["perm16_test.cpp", "perm16.cpp"])

######################################################################################

env.AlwaysBuild(Alias('check', [test], test[0].abspath))
env.Clean("distclean", [ ".sconsign.dblite", ".sconf_temp", "config.log", ])

Default(shlib)

print "BUILD_TARGETS is", [str(node) for node in BUILD_TARGETS]
