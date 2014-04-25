import os
import SCons.SConf

def CheckCilkPlusCompiler(context):
    test = """
    #include <cilk/cilk.h>
    #include <assert.h>
    int fib(int n) {
      if (n < 2) return n;
      int a = cilk_spawn fib(n-1);
      int b = fib(n-2);
      cilk_sync;
      return a + b;
    }
    int main() {
      int result = fib(30);
      assert(result == 832040);
      return 0;
    }
    """
    context.Message('Checking Cilk++ compiler ... ')
    env = context.sconf.env
    if isinstance(env['cilk'], str):
        for compiler in ['g++', 'clang++']:
            cilk_exec = os.path.join(env['cilk'], 'bin', compiler)
            if os.path.exists(cilk_exec):
                break
        else:
            context.Result('not found')
            return None
        env['CXX'] = cilk_exec
        libpath = [os.path.join(env['cilk'], ld) for ld in ['lib', 'lib64']]
        env.Prepend(CPPPATH = [os.path.join(env['cilk'], 'include')],
                    LIBPATH = libpath,
                    RPATH   = libpath)
    env.Prepend(CXXFLAGS = ['-fcilkplus'], LIBS = ['cilkrts'])
    result = context.TryRun(test, '.cpp')
    context.Result(result[0])
    if not result[0]: # cleanup environment
        env['CXXFLAGS'] = env['CXXFLAGS'][1:]
        env['LIBS'] = env['LIBS'][1:]
    return result[0]

Tests = { key : val for key, val in globals().items() if key.startswith('Check') }
