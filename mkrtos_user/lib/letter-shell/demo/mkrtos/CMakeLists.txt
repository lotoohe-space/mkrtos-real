cmake_minimum_required(VERSION 3.13)
project(LetterShell VERSION 0.1.0)

add_definitions(-DSHELL_CFG_USER="shell_cfg_user.h")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w" )
add_library(LetterShell
               # main.c
               shell_port.c
               shell_fs_ext.c
               shell_test.c
            #    shell_cpp.cpp
               ../../src/shell.c
               ../../src/shell_companion.c
               ../../src/shell_ext.c
               ../../src/shell_cmd_list.c
               # ../../extensions/fs_support/shell_fs.c
               ../../extensions/log/log.c
            #    ../../extensions/telnet/telnetd.c
               ../../extensions/shell_enhance/shell_passthrough.c
               ../../extensions/shell_enhance/shell_cmd_group.c
               ../../extensions/shell_enhance/shell_secure_user.c
               ../../extensions/game/game.c
               ../../extensions/game/2048/2048.c
               ../../extensions/game/pushbox/pushbox.c
               )

target_include_directories(LetterShell PUBLIC 
                           "${PROJECT_BINARY_DIR}"
                           ./
                           ../../src
                           ../../extensions/fs_support
                           ../../extensions/cpp_support
                           ../../extensions/log
                           ../../extensions/shell_enhance
                        #    ../../extensions/telnet

                           ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
                           ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc/${ARCH_NAME}
                           ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
                           ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
                           ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/util/inc
                           ${CMAKE_SOURCE_DIR}/mkrtos_user/server/app/drv
                           
                           ) 

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
#     -T \"${CMAKE_CURRENT_SOURCE_DIR}/shell.lds\" \
#     -Wl,-Map=LetterShell.map")
# set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -pthread")
