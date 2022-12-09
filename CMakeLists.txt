cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
include(setting.cmake)

enable_language(ASM C CXX)
project(mkrtos)


execute_process(
    COMMAND date +%Y-%m-%d
    OUTPUT_VARIABLE DATE
)
execute_process(
    COMMAND date +%H:%M:%S
    OUTPUT_VARIABLE TIME
)
execute_process(
    COMMAND git rev-parse --short HEAD
    OUTPUT_VARIABLE COMMIT
)
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    OUTPUT_VARIABLE BRANCH
)
string(STRIP ${DATE} DATE)
string(STRIP ${TIME} TIME)
# string(STRIP ${COMMIT} COMMIT)
# string(STRIP ${BRANCH} BRANCH)
set(code_version ${BRANCH}-${COMMIT} CACHE STRING " " FORCE)
set(compile_time "${DATE}\" \"${TIME}" CACHE STRING " " FORCE)

add_subdirectory(mkrtos_bootstrap)
add_subdirectory(mkrtos_real)
add_subdirectory(mkrtos_sdk)
add_subdirectory(mkrtos_ls)
add_subdirectory(mkrtos_ym)
add_subdirectory(mkrtos_shell)
add_subdirectory(mkrtos_test)
# add_custom_target(
#     gen_system_cpio
#     ALL
#     # COMMAND
#     # sudo rm ${CMAKE_SOURCE_DIR}/rootfs.cpio
#     COMMAND
#     cd ${CMAKE_SOURCE_DIR}/build/bin/ && ls mkrtos_shell.bin | cpio -o > ${CMAKE_SOURCE_DIR}/rootfs.cpio
#     # COMMAND
#     # sudo rm kernel.img
#     COMMAND
#     sudo srec_cat -output "kernel.img" -binary 
#         ${CMAKE_SOURCE_DIR}/build/bin/mkrtos_bootstrap.bin -binary
#         ${CMAKE_SOURCE_DIR}/build/bin/kernel.bin -binary -offset 0x00004000
#         ${CMAKE_SOURCE_DIR}/rootfs.cpio -binary -offset 0x00024000
#     # COMMAND
#     # sudo rm ${CMAKE_SOURCE_DIR}/rootfs.cpio
# )


