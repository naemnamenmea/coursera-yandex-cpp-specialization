add_executable(transportnyi_spravochnik_chast_a)
set_target_properties(transportnyi_spravochnik_chast_a PROPERTIES FOLDER "brown-belt/week_6")

file(GLOB SOURCE_FILES *.cpp *.hpp)

target_include_directories(transportnyi_spravochnik_chast_a PRIVATE .)
target_sources(transportnyi_spravochnik_chast_a PUBLIC ${SOURCE_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(transportnyi_spravochnik_chast_a PUBLIC misc)
target_link_libraries(transportnyi_spravochnik_chast_a PUBLIC math)
target_link_libraries(transportnyi_spravochnik_chast_a PUBLIC data_structures)
target_link_libraries(transportnyi_spravochnik_chast_a PUBLIC my_testing_tools)
