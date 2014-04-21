import SCons.SConf
from utils import *

def SearchCilkCompiler(env):
    print('Searching for a Cilk compiler... ')
    cilk_dir = ARGUMENTS.get('cilk_dir', os.environ.get('CILK_ROOT', None))
    if cilk_dir is not None:
        for compiler in ['g++', 'clang++']:
            cilk_exec = os.path.join(cilk_dir, 'bin', compiler)
            if os.path.exists(cilk_exec):
                env['CXX'] = cilk_exec
                libpath = [os.path.join(cilk_dir, ld) for ld in ['lib', 'lib64']]
                env.Append(CPPDEFINES = ['USE_CILK'],
                           CPPPATH = [os.path.join(cilk_dir, 'include')],
                           CXXFLAGS = ['-fcilkplus'],
                           LIBS = ['cilkrts'],
                           LIBPATH = libpath,
                           RPATH   = libpath)
                return cilk_exec
    return None


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
    result = context.TryRun(test, '.cpp')
    context.Result(result[0])
    return result[0]

Tests = { key : val for key, val in globals().items() if key.startswith('Check') }
