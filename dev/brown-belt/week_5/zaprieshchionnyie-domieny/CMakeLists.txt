add_executable(zaprieshchionnyie_domieny)
set_target_properties(zaprieshchionnyie_domieny PROPERTIES FOLDER "brown-belt/week_5/")

file(GLOB SOURCE_FILES *.cpp *.hpp)

target_sources(zaprieshchionnyie_domieny PUBLIC ${SOURCE_FILES})

target_link_libraries(zaprieshchionnyie_domieny PUBLIC misc)
