# -*- coding: utf-8 -*-
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

import SCons.Builder
import SCons.Action

def complain_etags(target, source, env):
    return 'etags binary was not found (see above). Tags have not been built.'

def generate(env):
    env['ETAGS'] = find_etags(env)
    if env['ETAGS'] != None:
        env['ETAGSCOM'] = '$ETAGS $SOURCES'
        env['BUILDERS']['etags'] = SCons.Builder.Builder(action=env['ETAGSCOM'])
    else:
        env['BUILDERS']['etags'] = SCons.Builder.Builder(action=env.Action(complain_etags))

def find_etags(env):
    return env.WhereIs('etags')

def exists(env):
    return find_etags(env) != None
