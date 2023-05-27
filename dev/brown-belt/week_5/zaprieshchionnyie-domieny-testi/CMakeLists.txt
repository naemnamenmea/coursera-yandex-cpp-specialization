add_executable(zaprieshchionnyie-domieny-testi)
set_target_properties(zaprieshchionnyie-domieny-testi PROPERTIES FOLDER "brown-belt/week_5/")

file(GLOB SOURCE_FILES *.cpp *.hpp)

target_sources(zaprieshchionnyie-domieny-testi PUBLIC ${SOURCE_FILES})

target_link_libraries(zaprieshchionnyie-domieny-testi PUBLIC misc)
