cmake_minimum_required(VERSION 2.8)
project(sylar)

include(cmake/utils.cmake)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Wno-unused-function")

include_directories(.)
include_directories(/user/local/include)

link_directories(/usr/local/lib)

find_library(YAMLCPP yaml-cpp)
message("***",${YAMLCPP})
#link_directories(/usr/lib64)
#find_library(YAMLCPP libyaml-cpp.a)
set(LIB_SRC
    sylar/log.cc
    sylar/util.cc
    sylar/config.cc
    sylar/fiber.cc
    sylar/thread.cc
)

add_library(sylar SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(sylar)

set(LIB_LIB
    sylar
    pthread
    ${YAMLCPP}
)
add_executable(test tests/test.cc)
add_dependencies(test sylar)
force_redefine_file_macro_for_sources(test)
target_link_libraries(test sylar ${LIB_LIB})

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config ${LIB_LIB})
force_redefine_file_macro_for_sources(test_config)
target_link_libraries(test_config sylar ${LIB_LIB})

add_executable(test_thread tests/test_thread.cc)
add_dependencies(test_thread ${LIB_LIB})
force_redefine_file_macro_for_sources(test_thread)
target_link_libraries(test_thread sylar ${LIB_LIB})


add_executable(test_util tests/test_util.cc)
add_dependencies(test_util ${LIB_LIB})
force_redefine_file_macro_for_sources(test_util)
target_link_libraries(test_util sylar ${LIB_LIB})


add_executable(test_fiber tests/test_fiber.cc)
add_dependencies(test_fiber ${LIB_LIB})
force_redefine_file_macro_for_sources(test_fiber)
target_link_libraries(test_fiber sylar ${LIB_LIB})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
