cmake_minimum_required(VERSION 3.13)

set(ENV{PROJECT_PATH} ${CMAKE_SOURCE_DIR})
set(ENV{CFLAGS} "${CMAKE_C_FLAGS} \
-I${CMAKE_SOURCE_DIR}/build/libc/output/include \
-L${GCC_LIB_PATH}/ \
-DHAVE_STPCPY -DHAVE_MEMPCPY -DHAVE_PSIGNAL \
-Wl,-no-dynamic-linker -T ${CMAKE_SOURCE_DIR}/mkrtos_user/user/app/binutils-2.33.1/armv7_8m/link.lds \
-L${CMAKE_SOURCE_DIR}/build/usr_lib \
-Wl,--start-group \
-static \
-lnewc \
-lstart \
-llibc_be \
-lmuslc \
-lgcc \
-lsys \
-lsys_svr \
-lsys_util \
-lutil \
-lnewc \
-Wl,--end-group \
")
message("binutils: CMAKE_C_FLAGS: $ENV{CFLAGS}")

set(ENV{LIBS} "-Wl,--start-group \
-static \
-lnewc \
-lstart \
-llibc_be \
-lmuslc \
-lgcc \
-lsys \
-lsys_svr \
-lsys_util \
-lutil \
-lnewc \
-Wl,--end-group \
"
)
message("binutils: CMAKE_C_LIBS: $ENV{LIBS}")

execute_process(
    COMMAND bash ${CMAKE_SOURCE_DIR}/mkrtos_user/user/app/binutils-2.33.1/build.sh
    COMMAND_ECHO STDOUT
)
