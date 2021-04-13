cmake_minimum_required(VERSION 3.5)
project(sar_sample)
# enable_testing()

# from https://stackoverflow.com/a/50545260 
# this way cmake should pass the compiler flags to use C++11
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(samples main.cpp)
target_sources(samples
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/MyClass.cpp"
        )
target_link_libraries(samples ard_net_lib
        ard_net_student_lib)