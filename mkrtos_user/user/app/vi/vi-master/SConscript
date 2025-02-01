from building import *

cwd     = GetCurrentDir()
src     = Glob('*.c')
CPPPATH = [cwd]

group = DefineGroup('vi', src, depend = ['PKG_USING_VI', 'RT_USING_DFS'], CPPPATH = CPPPATH)

Return('group')
