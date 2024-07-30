add_library(
    cjson
    STATIC
    cJSON_Utils.c
    cJSON.c
)
target_include_directories(
    cpio
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cJSON
)

