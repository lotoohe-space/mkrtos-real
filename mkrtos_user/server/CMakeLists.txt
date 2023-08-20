cmake_minimum_required(VERSION 3.13)


set(CMAKE_C_FLAGS "${GLOBAL_C_FLAG} -fPIC -n -pie -fpie -fpic \
-msingle-pic-base -mno-pic-data-is-text-relative --specs=nosys.specs \
-fmessage-length=0 -Wall -Xlinker --cref \
")

add_subdirectory(init)
