cmake_minimum_required(VERSION 3.30)

project(
  Simply
  VERSION 1.0.0
  LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

option(SIMPLY_INSTALL_PRE_COMMIT_HOOKS
       "Install pre-commit hooks during configuration" ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(SIMPLY_INSTALL_PRE_COMMIT_HOOKS)
  include(pre-commit)
endif()

include(simply)

add_library(simply INTERFACE)
add_library(simply::simply ALIAS simply)
target_compile_features(simply INTERFACE cxx_std_26)
# target_compile_options(simply INTERFACE -Wall -Wextra -Werror -Wpedantic)

target_include_directories(simply INTERFACE include)

enable_testing()

add_subdirectory(tests)
