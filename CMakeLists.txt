cmake_minimum_required(VERSION 4.2.3)
project(serdec VERSION 0.0.1 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(serdec
  src/serdec.c
  src/serdec_buf.c
  src/serdec_arena.c
  src/serdec_json.c
  src/serdec_json_parser.c
  src/serdec_json_writer.c
)

target_include_directories(serdec
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(serdec PROPERTIES
  C_STANDARD 23
  C_STANDARD_REQUIRED YES
)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
