cmake_minimum_required(VERSION 3.13)

include(setting.cmake)
project(mkrtos_real)


if (BOARD STREQUAL "stm32f103rc")
    set(CMAKE_C_FLAGS "${GLOBAL_C_FLAG} -fmessage-length=0 \
    --specs=nosys.specs \
    -Xlinker --print-map \
    -Xlinker --gc-sections\
    -Wl,--gc-sections -Wall -W -fno-stack-protector -g -nostartfiles -nodefaultlibs\
    -nostdlib -nostdinc \
    -D__dietlibc__ -D__arm__ \
    -DGD32F10X_HD \
    -D__MPU_PRESENT=1 -DUSE_STDPERIPH_DRIVER \
    -DDEBUG
    " )
elseif (BOARD STREQUAL "stm32f205rft6")
    set(CMAKE_C_FLAGS "${GLOBAL_C_FLAG} -fmessage-length=0 \
    --specs=nosys.specs \
    -Xlinker --print-map \
    -Xlinker --gc-sections \
    -Wall -W -fno-stack-protector -g \
    -D__dietlibc__ -D__arm__ \
    -D__MPU_PRESENT=1 -DUSE_STDPERIPH_DRIVER \
    -DDEBUG
    " )
endif()

set(CMAKE_CXX_FLAGS CMAKE_C_FLAGS)
set(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})

message("mkrtos-real CMAKE_CFLAGS:${CMAKE_C_FLAGS}")
message("mkrtos-real CMAKE_CURRENT_SOURCE_DIR:${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/dietlibc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/dietlibc/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/arch)

if (BOARD STREQUAL "stm32f103rc")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/arch/bsp/cortex-m3)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/arch/bsp/cpu/gd32f103rc)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/arch/bsp/cpu/gd32f103rc/lib)
elseif (BOARD STREQUAL "stm32f205rft6")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/arch/bsp/cortex-m3)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/arch/bsp/cpu/stm32f205rft6)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/arch/bsp/cpu/stm32f205rft6/lib)
endif()


# add_subdirectory(app)
add_subdirectory(arch)
add_subdirectory(device)
add_subdirectory(exec)
add_subdirectory(fs)
add_subdirectory(ipc)
add_subdirectory(knl)
add_subdirectory(lib)
add_subdirectory(start)

set(CMAKE_GEN_ELF ${CMAKE_PROJECT_NAME}_real.elf)
file(GLOB deps *.c *.S)
add_executable(${CMAKE_GEN_ELF}
   ${deps}
)
target_link_libraries(
    ${CMAKE_GEN_ELF}
    # $<TARGET_OBJECTS:kernel_arch>
    # kernel_app
    kernel_arch
    # kernel_device
    $<TARGET_OBJECTS:kernel_device>
    kernel_exec
    kernel_fs
    kernel_ipc
    kernel_knl
    kernel_lib
    $<TARGET_OBJECTS:kernel_start>
)

# message("LINK_FLAGS:${LINK_FLAGS}")
if (BOARD STREQUAL "stm32f103rc")
    set_target_properties(${CMAKE_GEN_ELF} PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/gd32f103rc_link.lds")
elseif (BOARD STREQUAL "stm32f205rft6")
    set_target_properties(${CMAKE_GEN_ELF} PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/stm32f205rft6_link.lds")
endif()


# message(" ${CMAKE_SIZE} build/mkrtos/mkrtos_real/${CMAKE_GEN_ELF}")
# execute_process(
#     COMMAND
#     ${CMAKE_SIZE} ${CMAKE_SOURCE_DIR}/build/mkrtos/mkrtos_real/${CMAKE_GEN_ELF}
# )
# execute_process(
#     COMMAND
#     ${CMAKE_OBJCOPY} -O binary ${CMAKE_SOURCE_DIR}/build/mkrtos/mkrtos_real/${CMAKE_GEN_ELF} ${CMAKE_SOURCE_DIR}/build/mkrtos/mkrtos_real/kernel.bin
# )

# execute_process(
#     COMMAND
#     sudo cp ${CMAKE_SOURCE_DIR}/build/mkrtos/mkrtos_real/kernel.bin ${CMAKE_SOURCE_DIR}/build/bin
# )
