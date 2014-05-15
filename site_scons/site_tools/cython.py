#*****************************************************************************
#       Copyright (C) 2014 Florent Hivert <Florent.Hivert@univ-rouen.fr>,
#
#  Distributed under the terms of the GNU General Public License (GPL)
#
#    This code is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    General Public License for more details.
#
#  The full text of the GPL is available at:
#
#                  http://www.gnu.org/licenses/
#*****************************************************************************

import SCons
from SCons.Builder import Builder
from SCons.Action import Action

def cython_generate(target, source, env, for_signature):
    if env["CYTHONLANG"] == 'c++':
        return "$CYTHON --cplus $CYTHONFLAGS -o $TARGET $SOURCE"
    else:
        return "$CYTHON $CYTHONFLAGS -o $TARGET $SOURCE"

def create_builder(env):
    try:
        cython = env['BUILDERS']['Cython']
    except KeyError:
        cython = SCons.Builder.Builder(
                  generator = cython_generate,
                  emitter = {},
                  suffix = cython_suffix_emitter,
                  single_source = 1)
        env['BUILDERS']['Cython'] = cython

    return cython

def cython_suffix_emitter(env, source):
    if env["CYTHONLANG"] == 'c++':
        return ".cpp"
    else:
        return ".c"

def generate(env):
    env["CYTHON"] = "cython"
    env["CYTHONLANG"] = 'c'

    # See Example at SCons/Tool/yacc.py
    #c_file, cxx_file = SCons.Tool.createCFileBuilders(env)

    #c_file.suffix['.pyx'] = cython_suffix_emitter
    #c_file.add_action('.pyx', cythonAction)

    #c_file.suffix['.py'] = cython_suffix_emitter
    #c_file.add_action('.py', cythonAction)

    create_builder(env)

def exists(env):
    try:
        import Cython
    except ImportError:
        return False
    else:
        return True
