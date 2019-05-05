cmake_minimum_required(VERSION 3.0)
project(cluster_demo)

if(MSVC)
  list(APPEND add_cppflags /W4)
else()
  list(APPEND add_cppflags -fvisibility=hidden)
  list(APPEND add_cppflags -Wall -Wextra)
  list(APPEND add_cppflags -std=c++11)
  LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../lib)
  LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../build)
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
    list(APPEND add_includes /usr/include /usr/local/include ${PROJECT_SOURCE_DIR}/../include/)
  list(APPEND add_libraries cluster)
endif()

set(cluster_demo_srcs
    ./main.cpp
    )

add_executable(cluster_demo ${cluster_demo_srcs})

target_compile_definitions(cluster_demo PRIVATE ${add_defines})
target_compile_options(cluster_demo PRIVATE ${add_cppflags})
target_include_directories(cluster_demo PRIVATE ${add_includes})
target_link_libraries(cluster_demo ${add_libraries})


if(UNIX)
    install (TARGETS cluster_demo RUNTIME DESTINATION ${PROJECT_SOURCE_DIR})
endif()
