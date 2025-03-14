cmake_minimum_required(VERSION 3.13)
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
#appfs lib
file(GLOB appfs_src  src/appfs.c src/appfs_open.c)
add_library(
    appfs_lib
    STATIC
    ${appfs_src}
)
target_include_directories(
    appfs_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc/${ARCH_NAME}
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_drv
)
add_dependencies(
    appfs_lib
    sys
    # mk_drv
)

# appfstiny lib
file(GLOB appfs_tiny_src  src/appfs_tiny.c src/appfs_tiny.h)
add_library(
    appfs_tiny
    STATIC
    ${appfs_tiny_src}
)
target_include_directories(
    appfs_tiny
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    # ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    # ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    # ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
)

file(
    GLOB deps  
    heap_stack.c
    main.c
    hw_block.c
    fs_rpc.c
)

add_executable(
    appfs.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    appfs.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    ${START_LIB}
    sys
    sys_util
    sys_svr
    appfs_lib
    mk_drv
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    appfs.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/util/inc

    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_dependencies(
    appfs.elf
    ${START_LIB}
    sys
    sys_util
    appfs_lib

)
set_target_properties(
    appfs.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    appfs_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S appfs.elf > ${CMAKE_SOURCE_DIR}/build/output/appfs.S
    COMMAND
    ${CMAKE_READELF} -a appfs.elf > ${CMAKE_SOURCE_DIR}/build/output/appfs.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S appfs.elf appfs.bin
    COMMAND
    ${CMAKE_SIZE} appfs.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp appfs.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/appfs
    COMMAND
    cp appfs.elf ${CMAKE_SOURCE_DIR}/build/output/
)

add_dependencies(appfs_dump appfs.elf)

