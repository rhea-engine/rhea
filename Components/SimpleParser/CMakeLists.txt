add_library(SimpleParser "SimpleParser.cpp" "IniFileParser.cpp" "Variant.cpp" "Type.cpp" "Method.h" "CssParser.h" "CssParser.cpp")
add_library(Meworla::SimpleParser ALIAS SimpleParser)
target_compile_features(SimpleParser PUBLIC cxx_std_17)

# Tests
if(TARGET GTest::gtest)

    #add_executable(SimpleParser_Compile_test "tests/SimpleParser.compile.cpp" "Variant.cpp" "Type.cpp" "Method.cpp")

    add_executable(SimpleParser_test "tests/SimpleParser.test.cpp")
    target_link_libraries(SimpleParser_test 
        PRIVATE 
        SimpleParser
        GTest::gtest_main
        GTest::gmock_main
    )
    add_test(SimpleParser_test ${CMAKE_CURRENT_BINARY_DIR}/SimpleParser_test)
endif()
