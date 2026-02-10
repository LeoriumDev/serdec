cmake_minimum_required(VERSION 4.2.3)
project(serdec VERSION 0.0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra)
# add_compile_options(-Wall -Wextra -Wpedantic -Werror)

option(SERDEC_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(SERDEC_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(SERDEC_ENABLE_SANITIZERS "Enable ASan + UBSan" OFF)

if(SERDEC_ENABLE_SANITIZERS OR SERDEC_ENABLE_ASAN)
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address)
endif()

if(SERDEC_ENABLE_SANITIZERS OR SERDEC_ENABLE_UBSAN)
  add_compile_options(-fsanitize=undefined)
  add_link_options(-fsanitize=undefined)
endif()      
          

add_library(serdec STATIC
  src/core/arena.c
  src/core/buffer.c
  src/core/error.c
)

target_include_directories(serdec PUBLIC include)
target_include_directories(serdec PRIVATE src)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
