# TestRunner - High-performance C++ test runner for PyAE
cmake_minimum_required(VERSION 3.21)

project(TestRunner
    VERSION 1.0.0
    DESCRIPTION "C++ test runner for PyAE"
    LANGUAGES CXX
)

add_executable(TestRunner
    main.cpp
)

# C++17 required
target_compile_features(TestRunner PRIVATE cxx_std_17)

# Windows-specific settings
if(WIN32)
    # Link against Windows libraries
    target_link_libraries(TestRunner PRIVATE
        kernel32
        user32
    )
endif()
