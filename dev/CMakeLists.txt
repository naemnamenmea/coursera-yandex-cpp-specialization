cmake_minimum_required(VERSION 3.19)

project(YandexCourseraC++Specialization)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_MODULE_PATH
    "C:/Users/Andrew/source/repos/cmake-tutorial/dev/cmake")
include(common)

add_external_lib(my_testing_tools)
add_external_lib(data_structures)
add_external_lib(math)
add_external_lib(misc)

add_compile_definitions(
    LOCAL_LAUNCH
    INPUT_FILENAME="input.txt"
    OUTPUT_FILENAME="output.txt"
    EXPECTED_FILENAME="expected.txt"
)

# add_subdirectory(white-belt)
# add_subdirectory(yellow-belt)
# add_subdirectory(red-belt)
add_subdirectory(brown-belt)
add_subdirectory(common_unity_build)
# add_subdirectory(black-belt)

set(CMAKE_SUPPRESS_REGENERATION OFF)
