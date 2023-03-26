find_package(Catch2 3 REQUIRED)

enable_testing ()

add_executable (DeterminantTests DeterminantTests.cpp ${Matrix_src})
target_link_libraries(DeterminantTests PRIVATE Catch2::Catch2WithMain)

add_test (NAME DeterminantTests COMMAND DeterminantTests)
