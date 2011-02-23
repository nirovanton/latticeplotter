add_executable(simulate main.cpp simulate.cpp)
target_link_libraries(simulate newgraph)
target_link_libraries(simulate error)
target_link_libraries(simulate boost_program_options)

