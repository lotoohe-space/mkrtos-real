

# 目标
- 支持app直接再flash上运行，下载，删除等，直接以文件的方式进行操作。
- app文件的块必须连续存储。
# 结构
info    nodeinfo    dirinfo     block...block

info：
存储磁盘容量
nodeinfo入口
dirinfo入口
block入口
nodeinfo：
块信息，每一位代表块是否使用
dirinfo：
存储目录信息（该文件系统暂时只有一层目录）
block：
可以用来存储文件内容的块

