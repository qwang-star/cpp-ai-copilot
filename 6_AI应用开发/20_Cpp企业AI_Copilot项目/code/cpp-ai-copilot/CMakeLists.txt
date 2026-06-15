cmake_minimum_required(VERSION 3.20)

project(cpp_ai_copilot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(copilot_core
    src/application.cpp
    src/config.cpp
    src/http.cpp
    src/logger.cpp
    src/router.cpp
)

target_include_directories(copilot_core PUBLIC include)

add_executable(cpp-ai-copilot
    src/main.cpp
    src/simple_server.cpp
)

target_link_libraries(cpp-ai-copilot PRIVATE copilot_core)

if(WIN32)
    target_link_libraries(cpp-ai-copilot PRIVATE ws2_32)
endif()

add_executable(test_core
    tests/test_core.cpp
)

target_link_libraries(test_core PRIVATE copilot_core)

enable_testing()
add_test(NAME core COMMAND test_core)
