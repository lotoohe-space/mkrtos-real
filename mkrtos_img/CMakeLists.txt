cmake_minimum_required(VERSION 3.13)

math(EXPR BOOTFS_OFFSET "${CONFIG_BOOTSTRAP_TEXT_SIZE} + ${CONFIG_KNL_TEXT_SIZE} + ${CONFIG_DTBO_TEXT_SIZE}")
math(EXPR KNL_OFFSET "${CONFIG_BOOTSTRAP_TEXT_SIZE} + ${CONFIG_DTBO_TEXT_SIZE}")

add_custom_target(
    mkrtos_img_dump ALL
    COMMAND
    dtc -I dts -O dtb -o ${CMAKE_SOURCE_DIR}/build/output/dtbo ${CMAKE_SOURCE_DIR}/mkrtos_configs/${BOARD}.dts
    COMMAND 
    cd ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    ls  | cpio -H newc -o > ${CMAKE_SOURCE_DIR}/build/output/rootfs.cpio
    COMMAND
    srec_cat -output ${CMAKE_SOURCE_DIR}/build/output/kernel.img -binary
        ${CMAKE_SOURCE_DIR}/build/output/bootstrap -binary -offset 0x0
        ${CMAKE_SOURCE_DIR}/build/output/dtbo -binary -offset ${CONFIG_BOOTSTRAP_TEXT_SIZE}
        ${CMAKE_SOURCE_DIR}/build/output/mkrtos -binary -offset ${KNL_OFFSET}
        ${CMAKE_SOURCE_DIR}/build/output/rootfs.cpio -binary -offset ${BOOTFS_OFFSET}
    COMMAND
    ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/build/output/kernel.img ${CMAKE_SOURCE_DIR}/build/output/kernel.bin
    COMMAND
    ${CMAKE_OBJCOPY} -I binary -O elf32-littlearm -B arm ${CMAKE_SOURCE_DIR}/build/output/kernel.img 
        ${CMAKE_SOURCE_DIR}/build/output/kernel.img.out --rename-section .data=.text 
    COMMAND
    ${CMAKE_OBJCOPY}  --change-section-address .text=${CONFIG_SYS_TEXT_ADDR} ${CMAKE_SOURCE_DIR}/build/output/kernel.img.out 
)
add_dependencies(mkrtos_img_dump
    bootstrap_dump
    mkrtos_dump
    init_dump
    # app_dump
    shell_dump
    # fatfs_dump
    cpiofs_dump
    # tcc_dump
)