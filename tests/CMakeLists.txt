add_executable(jsonny_tests main.cpp)
target_link_libraries(jsonny_tests PRIVATE jsonny)
target_compile_features(jsonny_tests PRIVATE cxx_std_17)
