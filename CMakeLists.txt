cmake_minimum_required(VERSION 3.10)

project(aithena)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(PACKAGE_TESTS "Build the tests" ON)

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
find_package(Boost REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BOOST_CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDDEBUG -O0")

add_subdirectory(aithena)

add_executable(aithenarun aithena/main.cc aithena/benchmark.cc aithena/alphazero.cc)
target_link_libraries(aithenarun board_lib chess_lib generic_lib mcts_lib alphazero_lib)
target_link_libraries(aithenarun "${TORCH_LIBRARIES}")
target_link_libraries(aithenarun "${BOOST_LIBRARIES}")

if(PACKAGE_TESTS AND PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(test)
endif()