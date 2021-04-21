add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "extern/googletest")

mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)

macro(add_gtest TESTNAME GTESTFILE)
	add_executable(${TESTNAME} ${GTESTFILE})
	target_link_libraries(${TESTNAME}
	gtest
	gtest_main
	${ARGN}
	)
	add_test(${TESTNAME} ${PROJECT_BINARY_DIR}/test/${TESTNAME})
endmacro(add_gtest)

add_gtest(BOARD_TEST test_board.cc board_lib)
add_gtest(CHESS_TEST test_chess.cc board_lib chess_lib generic_lib)
add_gtest(DIRECTION_TEST test_direction.cc chess_lib)
add_gtest(PERFT_TEST test_perft.cc chess_lib)