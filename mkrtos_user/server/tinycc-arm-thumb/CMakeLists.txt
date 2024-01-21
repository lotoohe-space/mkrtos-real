cmake_minimum_required(VERSION 3.13)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w -DUSE_STDPERIPH_DRIVER=1 \
-DTCC_TARGET_ARM_THUMB -DTCC_ARM_VFP -DTCC_ARM_EABI \
-DCONFIG_TCC_STATIC -DNO_SEMAPHORE_H \
")
# -DTCC_PROFILE
#  -DTCC_ARM_HARDFLOAT

file(GLOB deps 
    heap_stack.c
    # arm-thumb-gen.c
    # arm-thumb-instructions.c
    arm-link.c
    arm-asm.c
    # arm-gen.c
    # il-gen.c
    tcc.c
    tccasm.c
    # tccgen.c
    tccpp.c
    # libtcc.c
    # tcccoff.c
    # tccelf.c
    # tccpe.c
    # tccrun.c
    tcctools.c
    # lib/alloca-arm.S
    # lib/armeabi.c
    # lib/armflush.c
    # lib/bcheck.c
    # lib/libtcc1.c
    # lib/va_list.c
)
add_executable(tcc.elf
    ${deps}
)
target_link_libraries(tcc.elf
    PUBLIC
    muslc
    --whole-archive    
    start
    sys
    sys_util
    sys_svr
    mr
    --no-whole-archive
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    tcc.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/tcc/src/test

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/tinycc-arm-thumb
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/tinycc-arm-thumb/lib
    # ${CMAKE_SOURCE_DIR}/mkrtos_user/server/tcc-0.9.27/include

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/tcc/bsp/core_inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/tcc/bsp/inc
)
add_dependencies(tcc.elf
muslc
)
set_target_properties(tcc.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/link.lds -pie --gc-section -no-dynamic-linker "
#--no-warn-rwx-segments
)
# 
add_custom_target(
    tcc_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S tcc.elf > ${CMAKE_SOURCE_DIR}/build/output/tcc.S
    COMMAND
    ${CMAKE_READELF} -a tcc.elf > ${CMAKE_SOURCE_DIR}/build/output/tcc.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S tcc.elf tcc.bin
    COMMAND
    ${CMAKE_SIZE} tcc.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    # COMMAND
    # cp tcc.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/tcc
    # COMMAND
    # cp tcc.elf ${CMAKE_SOURCE_DIR}/build/output/tcc.elf
)

add_dependencies(tcc_dump tcc.elf)
add_dependencies(tcc_dump sys)
add_dependencies(tcc_dump sys_util)
add_dependencies(tcc_dump mr)
add_dependencies(tcc_dump sys_svr)
add_dependencies(tcc_dump start)
add_dependencies(tcc_dump muslc)
 