cmake_minimum_required(VERSION 3.13)

add_custom_target(
    tcc_test_dump ALL
    COMMAND
    cp ${CMAKE_CURRENT_SOURCE_DIR}/* ${CMAKE_SOURCE_DIR}/build/output/cpio/
)
