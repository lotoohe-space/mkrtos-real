cmake_minimum_required(VERSION 3.13)

add_custom_target(
    mkrtos_img_dump ALL
    COMMAND
    srec_cat -output ${CMAKE_SOURCE_DIR}/build/output/kernel.img -binary
        ${CMAKE_SOURCE_DIR}/build/output/bootstrap -binary -offset 0x0
        ${CMAKE_SOURCE_DIR}/build/output/mkrtos -binary -offset $ENV{KEN_OFFSET}
        ${CMAKE_SOURCE_DIR}/build/output/init -binary -offset $ENV{INIT_OFFSET}
)
add_dependencies(mkrtos_img_dump
    bootstrap_dump
    mkrtos_dump
    init_dump
)