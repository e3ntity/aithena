find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_library(board_lib board/board.cc board/board_plane.cc)
target_include_directories(board_lib
    PUBLIC .
)
target_link_libraries(board_lib "${TORCH_LIBRARIES}")


add_library(benchmark_lib benchmark/benchmark.cc benchmark/benchmark_set.cc)
target_include_directories(benchmark_lib
    PUBLIC .
)

add_library(generic_lib game/state.cc)
target_include_directories(generic_lib
    PUBLIC .
)
target_link_libraries(generic_lib board_lib)

add_library(chess_lib
    chess/game.cc
    chess/moves.cc
    chess/piece.cc
    chess/state.cc
    chess/util.cc
)

target_include_directories(chess_lib
    PUBLIC .
)

target_link_libraries(chess_lib generic_lib benchmark_lib "${TORCH_LIBRARIES}")

add_library(mcts_lib mcts/mcts.cc mcts/node.cc)
target_include_directories(mcts_lib
    PUBLIC .
)
target_link_libraries(mcts_lib generic_lib chess_lib)

add_library(alphazero_lib
    alphazero/alphazero.cc
    alphazero/move.cc
    alphazero/node.cc
)
target_include_directories(alphazero_lib
    PUBLIC .
)

target_link_libraries(alphazero_lib chess_lib mcts_lib "${TORCH_LIBRARIES}")