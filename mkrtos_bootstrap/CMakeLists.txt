cmake_minimum_required(VERSION 3.13)

# include(setting.cmake)
# project(mkrtos_bootstrap)
set(PROJECT_NAME mkrtos_bootstrap)

if (BOARD STREQUAL "stm32f103rc")
    set(CMAKE_C_FLAGS "${GLOBAL_C_FLAG} \
    -fmessage-length=0 \
    --specs=nosys.specs \
    -Xlinker --print-map \
    -Xlinker --gc-sections \
    -Wall -W -fno-stack-protector -g \
    -D__dietlibc__ -D__arm__ \
    " )
elseif (BOARD STREQUAL "stm32f205rft6")
    set(CMAKE_C_FLAGS "${GLOBAL_C_FLAG} \
    -fmessage-length=0 \
    --specs=nosys.specs \
    -Xlinker --print-map \
    -Xlinker --gc-sections \
    -Wall -W -fno-stack-protector -g \
    -D__dietlibc__ -D__arm__ \
    -D__MPU_PRESENT=1 -DUSE_STDPERIPH_DRIVER \
    " )
    
endif()

set(CMAKE_CXX_FLAGS CMAKE_C_FLAGS)
set(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})
 
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/cortex-m3)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/stm32f205rft6)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/stm32f205rft6/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/dietlibc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/dietlibc/include)

set(CMAKE_GEN_ELF ${PROJECT_NAME}.elf)

add_subdirectory(src)

file(GLOB deps *.c *.S)

add_executable(${CMAKE_GEN_ELF}
    ${deps}
)
if (BOARD STREQUAL "stm32f103rc")
    set_target_properties(${CMAKE_GEN_ELF} PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/gd32f103rc_link.lds")
elseif (BOARD STREQUAL "stm32f205rft6")
    set_target_properties(${CMAKE_GEN_ELF} PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/stm32f205rft6_link.lds")
endif()

TARGET_LINK_LIBRARIES(${CMAKE_GEN_ELF}
    boot_src
)