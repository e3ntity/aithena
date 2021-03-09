cmake_minimum_required(VERSION 3.10)

project(aithena)

set(CMAKE_CXX_STANDARD 17)

option(PACKAGE_TESTS "Build the tests" OFF)

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
find_package(Boost REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BOOST_CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDDEBUG -O0")

add_subdirectory(src)

add_executable(aithena src/main.cc)
target_link_libraries(aithena board_lib chess_lib generic_lib mcts_lib alphazero_lib)
target_link_libraries(aithena "${TORCH_LIBRARIES}")
target_link_libraries(aithena "${BOOST_LIBRARIES}")

if(PACKAGE_TESTS AND PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(test)
endif()