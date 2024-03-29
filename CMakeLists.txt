cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(rbtree VERSION 1.0.0)

foreach(LANGUAGE C CXX)
  set(${LANGUAGE}_COMPILER "${CMAKE_${LANGUAGE}_COMPILER_ID}")

  if(NOT "${CMAKE_${LANGUAGE}_SIMULATE_ID}" STREQUAL "")
    set(${LANGUAGE}_COMPILER ${CMAKE_${LANGUAGE}_SIMULATE_ID})
  endif()

  if(NOT "${CMAKE_${LANGUAGE}_COMPILER_FRONTEND_VARIANT}" STREQUAL "")
    set(${LANGUAGE}_COMPILER ${CMAKE_${LANGUAGE}_COMPILER_FRONTEND_VARIANT})
  endif()

  if(${LANGUAGE}_COMPILER MATCHES "^(GNU|(ARM|Apple)?Clang|Intel(LLVM)?)$")
    set(${LANGUAGE}_FLAGS "-pedantic-errors -Wall -Wextra")
  elseif(${LANGUAGE}_COMPILER STREQUAL MSVC)
    set(${LANGUAGE}_FLAGS "/W4")
  else()
    set(${LANGUAGE}_FLAGS "")
  endif()

  if(NOT ${LANGUAGE}_FLAGS STREQUAL "")
    if("${CMAKE_${LANGUAGE}_FLAGS}" STREQUAL "")
      set(CMAKE_${LANGUAGE}_FLAGS "${${LANGUAGE}_FLAGS}")
    else()
      string(APPEND CMAKE_${LANGUAGE}_FLAGS " ${${LANGUAGE}_FLAGS}")
    endif()
  endif()
endforeach()

add_executable(test allocator.c comparator.c layout.c map.c test.cpp)
target_compile_features(test PRIVATE cxx_std_17)
target_compile_features(test PRIVATE c_std_11)
