set INPUT_FILEPATH= "C:\Users\Andrew\source\repos\Yandex Coursera C++ Specialization\dev\brown-belt\week_5\lichnyi_biudzhiet_professional_desktop_version\lichnyi-biudzhiet.cpp"
set OUTPUT_FILEPATH= "C:\Users\Andrew\source\repos\Yandex Coursera C++ Specialization\dev\common_unity_build\main.cpp"

quom -S "./" -S "./../src" -I "C:\Users\Andrew\source\c++libraries\math\public" -I "C:\Users\Andrew\source\c++libraries\misc\public" -I "C:\Users\Andrew\source\c++libraries\data_structures\public" -I "C:\Users\Andrew\source\c++libraries\my_testing_tools\public" %INPUT_FILEPATH% %OUTPUT_FILEPATH%