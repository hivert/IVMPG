import SCons.SConf
from SCons.Script import *

def Fail(message):
    print(message)
    Exit(1)
