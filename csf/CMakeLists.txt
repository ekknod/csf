cmake_minimum_required(VERSION 2.6)
project(csf)

set(OUT csf)

add_executable ( ${OUT}
    src/main.cpp src/cs/cs_convar.cpp src/cs/cs_inputsystem.cpp src/cs/cs_netvars.cpp src/cs/cs_engine.cpp src/cs/cs_interfaces.cpp src/cs/cs_player.cpp )


target_link_libraries( ${OUT} ${CMAKE_SOURCE_DIR}/build/rx/librx.a -lpthread )

set_target_properties(${OUT} PROPERTIES LINKER_LANGUAGE CXX)
set(CMAKE_C_FLAGS "-Wno-format-truncation")

