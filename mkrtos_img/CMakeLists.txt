cmake_minimum_required(VERSION 3.13)

add_custom_target(
    mkrtos_img_dump ALL
    COMMAND
    cd ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    ls | cpio -H newc -o > ${CMAKE_SOURCE_DIR}/build/output/rootfs.cpio
    COMMAND
    srec_cat -output ${CMAKE_SOURCE_DIR}/build/output/kernel.img -binary
        ${CMAKE_SOURCE_DIR}/build/output/bootstrap -binary -offset 0x0
        ${CMAKE_SOURCE_DIR}/build/output/mkrtos -binary -offset $ENV{KEN_OFFSET}
        ${CMAKE_SOURCE_DIR}/build/output/init -binary -offset $ENV{INIT_OFFSET}
        ${CMAKE_SOURCE_DIR}/build/output/rootfs.cpio -binary -offset $ENV{BOOTFS_ADDR_OFFSET}
)
add_dependencies(mkrtos_img_dump
    bootstrap_dump
    mkrtos_dump
    init_dump
    shell_dump
    path_manager_dump
)