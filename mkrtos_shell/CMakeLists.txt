cmake_minimum_required(VERSION 3.13)

include(setting.cmake)
project(mkrtos_shell)


if (BOARD STREQUAL "stm32f103rc")
    set(CMAKE_C_FLAGS "${GLOBAL_C_FLAG} -fPIC -n -pie -fpie -fPIC -fpic\
    -msingle-pic-base -mno-pic-data-is-text-relative -fmessage-length=0 \
    --specs=nosys.specs \
    -Xlinker --print-map \
    -Xlinker --gc-sections\
    -Wl,--gc-sections -Wall -W -fno-stack-protector -g -nostartfiles -nodefaultlibs\
    -nostdlib -nostdinc \
    -D__dietlibc__ -D__arm__ 
    " )
elseif (BOARD STREQUAL "stm32f205rft6")
    set(CMAKE_C_FLAGS "${GLOBAL_C_FLAG} -fPIC -n -pie -fpie -fPIC -fpic\
    -msingle-pic-base -mno-pic-data-is-text-relative \
    -fmessage-length=0 -Wall -Xlinker --cref \
    -D__dietlibc__ -D__arm__ \
    " )
endif()

set(CMAKE_CXX_FLAGS CMAKE_C_FLAGS)
set(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})

message("mkrtos-shell CMAKE_CFLAGS:${CMAKE_C_FLAGS}")
# message("mkrtos-shell CMAKE_CURRENT_SOURCE_DIR:${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
include_directories(${CMAKE_SOURCE_DIR}/mkrtos_sdk/src/dietlibc-0.34/include)

# link_libraries("${CMAKE_SOURCE_DIR}/build/mkrtos/mkrtos_sdk/libmkrtos_sdk.a")
# link_libraries("${CMAKE_CURRENT_SOURCE_DIR}/libmkrtos_sdk.a")
set(CMAKE_GEN_ELF ${CMAKE_PROJECT_NAME}_shell.elf)
# message("mkrtos-shell:${CMAKE_SOURCE_DIR}/mkrtos_sdk/src/dietlibc-0.34/include")
# message("mkrtos-shell:${CMAKE_SOURCE_DIR}/build/mkrtos/mkrtos_sdk/libmkrtos_sdk.a")
file(GLOB_RECURSE deps **/*.c **/*.S *.c *.S)
add_executable(${CMAKE_GEN_ELF}
    ${deps}
)
target_link_libraries(${CMAKE_GEN_ELF} ${CMAKE_SOURCE_DIR}/build/mkrtos/mkrtos_sdk/libmkrtos_sdk.a)
set_target_properties(${CMAKE_GEN_ELF} PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/link.lds -pie")

message("link: ${CMAKE_C_LINK_EXECUTABLE}")
 