add_executable(Textures)
set_target_properties(Textures PROPERTIES FOLDER "brown-belt/week_4")

file(GLOB SOURCE_FILES *.cpp *.hpp *.h)

target_include_directories(Textures PRIVATE .)
target_sources(Textures PUBLIC ${SOURCE_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Textures PUBLIC misc)
