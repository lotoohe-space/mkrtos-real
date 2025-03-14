
image文件打包分布：
bootstarp(bootloader) --> dtbo(设备树) --> knl image -> appfs image

appfs image
    --> init server
    --> block server
    --> appfs server

系统加载流程:
    bootstarp --> knl image --> init server --> block server --> appfs server --> other appliction.
