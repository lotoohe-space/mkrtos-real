cmake_minimum_required(VERSION 3.13)


if (${ARCH_NAME} STREQUAL "armv7_8m")
    # add_subdirectory(modbus)
    add_subdirectory(mlibc)
    # add_subdirectory(mlibc_shared)
    add_subdirectory(mr)
    # add_subdirectory(at_device)
elseif(${CONFIG_ARCH} STREQUAL "aarch64")
endif()
add_subdirectory(mkrtos-musl)
add_subdirectory(sys)
add_subdirectory(sys_util)
add_subdirectory(util)
add_subdirectory(sys_svr)
add_subdirectory(libc_backend)
add_subdirectory(cpio)
add_subdirectory(libfdt)
add_subdirectory(letter-shell/demo/mkrtos)
add_subdirectory(printf)
add_subdirectory(lwip)
add_subdirectory(cJSON)
add_subdirectory(cutest)
add_subdirectory(fd)


# add_custom_target(
#     mkrtos_static_libc ALL
#     #以下命令用于生成binutils链接需要的库
#     COMMAND
#     cd ${CMAKE_SOURCE_DIR}/build
#     COMMAND
#     rm -f -r usr_lib
#     COMMAND
#     mkdir -p usr_lib
#     COMMAND
#     find mkrtos_user -name "*.a" | xargs cp -t usr_lib
#     COMMAND
#     cd usr_lib
#     COMMAND
#     ${CMAKE_AR} x libmuslc.a 
#     COMMAND
#     ${CMAKE_AR} x liblibc_be.a 
#     COMMAND
#     ${CMAKE_AR} x libsys.a 
#     COMMAND
#     ${CMAKE_AR} x libsys_svr.a 
#     COMMAND
#     ${CMAKE_AR} x libsys_util.a 
#     COMMAND
#     ${CMAKE_AR} x libutil.a 
#     COMMAND
#     ${CMAKE_AR} cr libnewc.a *.obj
#     COMMAND
#     rm *.obj
# )
# add_dependencies(
#     mkrtos_static_libc
#     muslc
#     libc_be
#     sys
#     sys_svr
#     sys_util
#     util
# )
