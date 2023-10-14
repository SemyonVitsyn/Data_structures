add_executable(test_complex complex.cpp test_complex.cpp )
add_executable(test_abs test_abs.cpp complex.cpp)
add_test(NAME test_complex COMMAND test_complex)
add_test(NAME test_abs COMMAND test_abs)
