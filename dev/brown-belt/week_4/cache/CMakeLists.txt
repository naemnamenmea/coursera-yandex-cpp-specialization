add_executable(cache228)
set_target_properties(cache228 PROPERTIES FOLDER "brown-belt/week_4")

file(GLOB SOURCE_FILES *.cpp *.hpp *.h)

target_include_directories(cache228 PRIVATE .)
target_sources(cache228 PUBLIC ${SOURCE_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(cache228 PUBLIC misc)
