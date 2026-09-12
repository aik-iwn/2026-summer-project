#Cmake最低版本限制
cmake_minimum_required(VERSION 3.15)
project(TestProject)

# 強制將版本設為C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Debug除錯符號(-g)
add_compile_options(-g)

#編譯執行檔
add_executable(my_app main.cpp Account.cpp BackTestEngine.cpp)