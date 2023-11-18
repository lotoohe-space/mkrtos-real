
set(PYTHON_EXECUTABLE $ENV{PYTHON_EXECUTABLE})

message(
  ${PYTHON_EXECUTABLE} "\n"
  ${CMAKE_SOURCE_DIR}/mkrtos_tool/kconfig/kconfig.py "\n"
  ${CMAKE_SOURCE_DIR}/Kconfig "\n"
  ${CMAKE_SOURCE_DIR}/build/auto.conf "\n"
  ${CMAKE_SOURCE_DIR}/build/autoconf.h "\n"
  ${CMAKE_SOURCE_DIR}/build/log.txt "\n"
  ${CMAKE_SOURCE_DIR}/build/.config "\n"
  )
execute_process(
  COMMAND
  ${PYTHON_EXECUTABLE} 
  ${CMAKE_SOURCE_DIR}/mkrtos_tool/kconfig/kconfig.py
  ${CMAKE_SOURCE_DIR}/Kconfig 
  ${CMAKE_SOURCE_DIR}/build/auto.conf 
  ${CMAKE_SOURCE_DIR}/build/autoconf.h 
  ${CMAKE_SOURCE_DIR}/build/log.txt 
  ${CMAKE_SOURCE_DIR}/build/.config 
  
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  # The working directory is set to the app dir such that the user
  # can use relative paths in CONF_FILE, e.g. CONF_FILE=nrf5.conf
  RESULT_VARIABLE ret
  )
if(NOT "${ret}" STREQUAL "0")
  message(FATAL_ERROR "command failed with return code: ${ret}")
endif()
