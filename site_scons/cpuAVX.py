import SCons.SConf
from SCons.Errors import StopError

cpuid_header = """
    #include <cpuid.h>
    #include <stdio.h>
    #include <stdlib.h>

    #define FAIL( b, message ) if (!(b)) { printf(message); return EXIT_FAILURE; }

    int main() {
      unsigned int ax, bx, cx, dx;
      FAIL( __get_cpuid(0x00000001, &ax, &bx, &cx, &dx),
            "Unable to determine the processor type !" );
"""
footer = """
      return EXIT_SUCCESS;
    }
"""

cpuid_checked = False

def EnsureCPUID(context):
    global cpuid_checked
    if cpuid_checked:
        return
    if not SCons.SConf.CheckCXXHeader(context, "cpuid.h"):
        raise StopError("You need 'cpuid.h' to check CPU instruction sets")
    cpuid_checked = True

def CheckProcInstructionSets(context, names):
    for name in names:
        res = CheckProcInstructionSet(context, name)
        if not res:
            return res
    return res

def CheckProcInstructionSet(context, name):
    EnsureCPUID(context)
    known = ('SSE3 PCLMUL LZCNT SSSE3 FMA CMPXCHG16B SSE4_1 SSE4_2 MOVBE '+
             'POPCNT AES XSAVE OSXSAVE AVX F16C RDRND').split()
    if name not in known:
        raise ValueError, "unknown instruction set name: %s"%name
    test = cpuid_header + """
      FAIL( cx & bit_%s, "This programm require %s instructions set !");
    """%(name, name) + footer
    context.Message('Checking CPU for %s instruction set... '%name)
    result = context.TryRun(test, '.c')
    context.Result(result[0])
    return result[0]

def CheckGCCVectorExtension(context):
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
    context.Message('Checking compiler for vector extension... ')
    result = context.TryCompile(test_vector_ext, '.cpp')
    context.Result(result)
    return result

Tests = { key : val for key, val in globals().items() if key.startswith('Check') }
