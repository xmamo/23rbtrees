cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_LANGUAGES C CXX)
project(rbtree VERSION 1.0.0 LANGUAGES ${PROJECT_LANGUAGES})

foreach(LANGUAGE ${PROJECT_LANGUAGES})
  set(${LANGUAGE}_COMPILER "${CMAKE_${LANGUAGE}_COMPILER_ID}")

  if(NOT "${CMAKE_${LANGUAGE}_SIMULATE_ID}" STREQUAL "")
    set(${LANGUAGE}_COMPILER ${CMAKE_${LANGUAGE}_SIMULATE_ID})
  endif()

  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.14)
    if(NOT "${CMAKE_${LANGUAGE}_COMPILER_FRONTEND_VARIANT}" STREQUAL "")
      set(${LANGUAGE}_COMPILER ${CMAKE_${LANGUAGE}_COMPILER_FRONTEND_VARIANT})
    endif()
  endif()

  if(${LANGUAGE}_COMPILER MATCHES "^(GNU|(ARM|Apple)?Clang|Intel(LLVM)?)$")
    set(${LANGUAGE}_FLAGS "-pedantic-errors -Wall -Wextra")
  elseif(${LANGUAGE}_COMPILER STREQUAL MSVC)
    set(${LANGUAGE}_FLAGS /W4)
  else()
    set(${LANGUAGE}_FLAGS "")
  endif()

  if(NOT ${LANGUAGE}_FLAGS STREQUAL "")
    if("${CMAKE_${LANGUAGE}_FLAGS}" STREQUAL "")
      set(CMAKE_${LANGUAGE}_FLAGS ${${LANGUAGE}_FLAGS})
    else()
      string(APPEND CMAKE_${LANGUAGE}_FLAGS " " ${${LANGUAGE}_FLAGS})
    endif()
  endif()
endforeach()

set(CMAKE_CXX_CLANG_TIDY clang-tidy)
add_executable(test test.cpp map.hpp)
target_compile_features(test PRIVATE cxx_std_20)
