cmake_minimum_required(VERSION 3.22)
project(kryptografia_lab2)

set(CMAKE_CXX_STANDARD 14)

add_executable(kryptografia_lab2
    Zad-2/Zad-2.cpp)
find_package(GSL REQUIRED)    # See below (2)
target_link_libraries(kryptografia_lab2 GSL::gsl GSL::gslcblas)