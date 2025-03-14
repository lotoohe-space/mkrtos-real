cmake_minimum_required(VERSION 3.13)
#-Wl,--no-dynamic-linker  -Wl,-T${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds -Wl,-pie 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w -D_XOPEN_SOURCE=700 -Wl,-e,_dlstart ")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -w -D_XOPEN_SOURCE=700 -Wa,-mimplicit-it=thumb -Wl,-e,_dlstart ")

file(GLOB_RECURSE deps
    ../mlibc/src/*.C
    ../mlibc/src/*.c
    ../mlibc/src/*.S
    ../mlibc/src/*.s 
    ../mlibc/ldso/*.c
    ../mlibc/crt/heap_stack.c
)
# list(ADD_ITEM deps ../mlibc/crt/heap_stack.c)
# list(REMOVE_ITEM deps ../mlibc/crt/crt1.c)
# list(REMOVE_ITEM deps ../mlibc/crt/crt1.S)
# list(REMOVE_ITEM deps ../mlibc/**/arm/*.c)
# list(REMOVE_ITEM deps ../mlibc/**/arm/*.S)
add_library(
    c 
    SHARED
    ${deps}
)

target_link_libraries(
    c
    PRIVATE
    libc_be
    sys
    sys_svr
    sys_util
    ${GCC_LIB_PATH}/libgcc.a 
)
#-z max-page-size=0x0 -z common-page-size=0x0
set_target_properties(c PROPERTIES LINK_FLAGS "-T${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds -pie -w")
target_include_directories(
    c
    BEFORE
    PUBLIC
    # ${CMAKE_SOURCE_DIR}/user/libc/backend
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc/${ARCH_NAME}

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include
)

add_custom_target(
    c_dump ALL
    COMMAND
    ${CMAKE_OBJCOPY}  -O binary -S libc.so libc.bin 
    # COMMAND
    # ${CMAKE_COMMAND} -E copy libc.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/libc
    # COMMAND
    # cp libc.so ${CMAKE_SOURCE_DIR}/build/output/libc.so
    # COMMAND
    # cp libc.so ${CMAKE_SOURCE_DIR}/build/output/cpio/libc.so
)

add_dependencies(c_dump c)
