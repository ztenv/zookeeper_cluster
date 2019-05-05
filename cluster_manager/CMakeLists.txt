cmake_minimum_required(VERSION 3.0)
project(cluster)

if(MSVC)
  list(APPEND add_cppflags /W4)
else()
  list(APPEND add_cppflags -fvisibility=hidden)
  list(APPEND add_cppflags -Wall -Wextra)
  list(APPEND add_cppflags -std=c++11)
  LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND add_cppflags -O0 -g)
  else()
    list(APPEND add_cppflags -O2)
  endif()
  list(APPEND add_cppflags -Wno-unused-but-set-variable)
  list(APPEND add_cppflags -Wno-unused-parameter)
endif()

if(MSVC)
  if(NOT DEFINED ENV{3RDPARTY})
      message(FATAL_ERROR "not defined system environment variable:3RDPARTY")
  else()
  endif()

  list(APPEND add_defines WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0600)
else()
  list(APPEND add_includes /usr/include /usr/local/include ./src/zookeeper ./include/)
  list(APPEND add_libraries zookeeper_mt)
endif()

set(cluster_srcs
    ./src/cluster_manager.cpp
    )

add_library(cluster SHARED ${cluster_srcs})

target_compile_definitions(cluster PRIVATE ${add_defines} CLUSTER_LIB)
target_compile_options(cluster PRIVATE ${add_cppflags})
target_include_directories(cluster PRIVATE ${add_includes})
target_link_libraries(cluster ${add_libraries})


if(UNIX)
   install (TARGETS cluster LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/lib)
   install (TARGETS cluster LIBRARY DESTINATION /usr/lib/uvframe)
endif()
