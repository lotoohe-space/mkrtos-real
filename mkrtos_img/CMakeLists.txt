cmake_minimum_required(VERSION 3.13)

add_custom_target(
    mkrtos_img_dump ALL
    COMMAND
    cd ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    find ./ -print | cpio -H newc -o > ${CMAKE_SOURCE_DIR}/build/output/rootfs.cpio
    COMMAND
    srec_cat -output ${CMAKE_SOURCE_DIR}/build/output/kernel.img -binary
        ${CMAKE_SOURCE_DIR}/build/output/bootstrap -binary -offset 0x0
        ${CMAKE_SOURCE_DIR}/build/output/mkrtos -binary -offset $ENV{KEN_OFFSET}
        ${CMAKE_SOURCE_DIR}/build/output/init -binary -offset $ENV{INIT_OFFSET}
        ${CMAKE_SOURCE_DIR}/build/output/rootfs.cpio -binary -offset $ENV{BOOTFS_ADDR_OFFSET}
    COMMAND
    ${CMAKE_OBJCOPY} -I binary -O elf32-littlearm -B arm ${CMAKE_SOURCE_DIR}/build/output/kernel.img 
    ${CMAKE_SOURCE_DIR}/build/output/kernel.img.out --rename-section .data=.text 
    COMMAND
    ${CMAKE_OBJCOPY}  --change-section-address .text=0x8000000 ${CMAKE_SOURCE_DIR}/build/output/kernel.img.out 
)
add_dependencies(mkrtos_img_dump
    bootstrap_dump
    mkrtos_dump
    init_dump
    # app_dump
    shell_dump
    fatfs_dump
    cpiofs_dump
    tcc_dump
)