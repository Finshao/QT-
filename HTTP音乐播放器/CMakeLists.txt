cmake_minimum_required(VERSION 3.10)  #cmake版本最低3.10

if(WIN32)
    set(CMAKE_SYSTEM_VERSION "10.0.18362.0")
endif()

SET(CMAKE_TOOLCHAIN_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty_install/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(Develop-Train)  #添加当前项目名称

set(CMAKE_CXX_STANDARD 11) #设置C++标准

# 设置输出目录为顶层 bin 文件夹
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(GMockExample)
add_subdirectory(GtestExample)



add_subdirectory(QtMusic) 


set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT QtMusic)  



