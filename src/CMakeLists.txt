CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT (cbnf)

SET(LIBUSEFUL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../lib/libuseful/src")

ADD_SUBDIRECTORY(${LIBUSEFUL_DIR} "${CMAKE_CURRENT_BINARY_DIR}/libuseful")
ADD_SUBDIRECTORY(../lib/ngtemplate/src "${CMAKE_CURRENT_BINARY_DIR}/ngtemplate")

FIND_PROGRAM(DiffFound diff)
IF(DiffFound STREQUAL "DiffFound-NOTFOUND")
	MESSAGE(FATAL_ERROR "The diff tool is not found on your system.  This is required for testing!")
ENDIF(DiffFound STREQUAL "DiffFound-NOTFOUND")

INCLUDE_DIRECTORIES(${LIBUSEFUL_INCLUDES})
INCLUDE_DIRECTORIES(${NGTEMPLATE_INCLUDES})

SET (CBNF_SRCS 
	main.c 
	cbnf.c 
	scanner.c 
	parser.c 
	analyzer.c 
	ast.c
	codegen.c
)

SET (CMAKE_BUILD_TYPE Debug)

ADD_EXECUTABLE(cbnf ${CBNF_SRCS})
TARGET_LINK_LIBRARIES(cbnf useful ngtemplate)

ENABLE_TESTING()

ADD_EXECUTABLE(scanner_test scanner.c cbnf.c testing/scanner_test.c)
TARGET_LINK_LIBRARIES(scanner_test useful)
ADD_TEST(scanner_0 ${EXECUTABLE_OUTPUT_PATH}/scanner_test ../tests/scanner_0.tst ../tests/scanner_0.bmk)

ADD_EXECUTABLE(ast_test ast.c cbnf.c testing/ast_test.c)
TARGET_LINK_LIBRARIES(ast_test useful)
ADD_TEST(ast_0 ${EXECUTABLE_OUTPUT_PATH}/ast_test)

ADD_EXECUTABLE(parser_test parser.c scanner.c ast.c cbnf.c testing/parser_test.c)
#ADD_TEST(parser_0 ${EXECUTABLE_OUTPUT_PATH}/parser_test ../tests/parser_0.tst ../tests/parser_0.bmk)
ADD_TEST(parser_1 ${EXECUTABLE_OUTPUT_PATH}/parser_test ../tests/parser_1.tst ../tests/parser_1.bmk)
ADD_TEST(parser_2 ${EXECUTABLE_OUTPUT_PATH}/parser_test ../tests/parser_2.tst ../tests/parser_2.bmk)
ADD_TEST(parser_3 ${EXECUTABLE_OUTPUT_PATH}/parser_test ../tests/parser_3.tst ../tests/parser_3.bmk)

ADD_TEST(parser_C ${EXECUTABLE_OUTPUT_PATH}/parser_test ../tests/parser_C.tst ../tests/parser_C.bmk)
ADD_TEST(parser_JAVA ${EXECUTABLE_OUTPUT_PATH}/parser_test ../tests/parser_JAVA.tst ../tests/parser_JAVA.bmk)
