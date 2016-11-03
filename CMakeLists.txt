cmake_minimum_required(VERSION 3.3)
project(AAQL)

SET(CMAKE_C_COMPILER "/usr/local/gcc-4.9.3/bin/gcc")
SET(CMAKE_CXX_COMPILER "/usr/local/gcc-4.9.3/bin/g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -L lib -lNLPIR")

set(SOURCE_FILES src/storage.cpp src/query_plan.cpp src/nodes.cpp src/parser.cpp src/lexer.cpp src/extractor.cpp src/util.cpp src/main.cpp src/operator.cpp src/op_output.cpp src/op_scan.cpp src/op_create.cpp src/op_create.h src/op_union.cpp src/op_union.h src/op_join.cpp src/op_join.h src/op_project.cpp src/op_project.h src/op_apply_func.cpp src/op_apply_func.h src/op_regex.cpp src/op_regex.h src/op_block.cpp src/op_block.h src/op_dictionary.cpp src/op_dictionary.h src/op_create_dict.cpp src/op_create_dict.h src/tokenizer.cpp src/tokenizer.h src/aql_func.cpp src/aql_func.h src/op_filter.cpp src/op_filter.h src/plan_nodes.cpp src/plan_nodes.h)
add_executable(AAQL ${SOURCE_FILES})