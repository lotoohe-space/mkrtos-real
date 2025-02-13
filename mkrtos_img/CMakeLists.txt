cmake_minimum_required(VERSION 3.13)

math(EXPR BOOTFS_OFFSET "${CONFIG_BOOTSTRAP_TEXT_SIZE} + ${CONFIG_KNL_TEXT_SIZE} + ${CONFIG_DTBO_TEXT_SIZE}")
math(EXPR KNL_OFFSET "${CONFIG_BOOTSTRAP_TEXT_SIZE} + ${CONFIG_DTBO_TEXT_SIZE}")
math(EXPR APPFS_IMG_SIZE "${CONFIG_SYS_TEXT_SIZE} - ${BOOTFS_OFFSET}")
add_custom_target(
    mkrtos_img_dump ALL
    COMMAND
    dtc -I dts -O dtb -o ${CMAKE_SOURCE_DIR}/build/output/dtbo ${CMAKE_SOURCE_DIR}/mkrtos_configs/${BOARD}.dts
    COMMAND 
    cd ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    ls  | cpio -H newc -o > ${CMAKE_SOURCE_DIR}/build/output/rootfs.cpio
    COMMAND
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/fs/appfs/src/build/appfs -g ${CMAKE_SOURCE_DIR}/build/output/cpio -o ../appfs.img -s ${APPFS_IMG_SIZE} -b 4096 -j
    COMMAND
    srec_cat -output ${CMAKE_SOURCE_DIR}/build/output/kernel.img -binary
        ${CMAKE_SOURCE_DIR}/build/output/bootstrap -binary -offset 0x0
        ${CMAKE_SOURCE_DIR}/build/output/dtbo -binary -offset ${CONFIG_BOOTSTRAP_TEXT_SIZE}
        ${CMAKE_SOURCE_DIR}/build/output/mkrtos -binary -offset ${KNL_OFFSET}
        ${CMAKE_SOURCE_DIR}/build/output/appfs.img -binary -offset ${BOOTFS_OFFSET}
    COMMAND
    ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/build/output/kernel.img ${CMAKE_SOURCE_DIR}/build/output/kernel.bin
    COMMAND
    ${CMAKE_OBJCOPY} -I binary -O elf32-littlearm -B arm ${CMAKE_SOURCE_DIR}/build/output/kernel.img 
        ${CMAKE_SOURCE_DIR}/build/output/kernel.img.out --rename-section .data=.text 
    COMMAND
    ${CMAKE_OBJCOPY}  --change-section-address .text=${CONFIG_SYS_TEXT_ADDR} ${CMAKE_SOURCE_DIR}/build/output/kernel.img.out

    #以下命令用于生成binutils链接需要的库
    # COMMAND
    # cd ../../
    # COMMAND
    # rm -f -r usr_lib
    # COMMAND
    # mkdir -p usr_lib
    # COMMAND
    # find mkrtos_user -name "*.a" | xargs cp -t usr_lib
    # COMMAND
    # cd usr_lib
    # COMMAND
    # ${CMAKE_AR} x libmuslc.a 
    # COMMAND
    # ${CMAKE_AR} x liblibc_be.a 
    # COMMAND
    # ${CMAKE_AR} x libsys.a 
    # COMMAND
    # ${CMAKE_AR} x libsys_svr.a 
    # COMMAND
    # ${CMAKE_AR} x libsys_util.a 
    # COMMAND
    # ${CMAKE_AR} x libutil.a 
    # COMMAND
    # ${CMAKE_AR} cr libnewc.a *.obj
    # COMMAND
    # rm *.obj
)
add_dependencies(mkrtos_img_dump
    bootstrap_dump
    mkrtos_dump
    init_dump
    shell_dump
    appfs_dump
    vi_dump
    tst_dump
    # eth_dump
    # nes_dump
    # net_dump
)