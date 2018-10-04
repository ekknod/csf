cmake_minimum_required(VERSION 2.6)
project(csf)
set(OUT csf)

add_executable ( ${OUT}
    src/aimassist.cpp src/u_config.cpp src/cs/cs_convar.cpp src/cs/cs_inputsystem.cpp src/cs/cs_netvars.cpp src/cs/cs_engine.cpp src/cs/cs_interfaces.cpp src/cs/cs_player.cpp )


set_target_properties(${OUT} PROPERTIES LINKER_LANGUAGE CXX)


set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -DNDEBUG -Wno-format-truncation -Wno-strict-aliasing -O2 -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-format-truncation -Wno-strict-aliasing")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
target_link_libraries(${OUT} rx "-Wall -DNDEBUG -pthread -s -O2")

