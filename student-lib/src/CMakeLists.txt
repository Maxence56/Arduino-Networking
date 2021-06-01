cmake_minimum_required(VERSION 3.5)
project(ard_net_student_lib)

set(ard_net_student_lib_sources
        ard_network_layer_two.cpp 
        ard_network_layer_two_functor.cpp
        ard_peer_context.cpp
        ard_network_layer_three_message.cpp
        ard_network_layer_three_routing.cpp
        ard_network_layer_three.cpp)


add_library(ard_net_student_lib STATIC ${ard_net_student_lib_sources})
target_include_directories(ard_net_student_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ard_net_student_lib ard_net_lib)

