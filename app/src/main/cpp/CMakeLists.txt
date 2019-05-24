cmake_minimum_required(VERSION 3.4.1)

file(GLOB SOURCE ${CMAKE_SOURCE_DIR}/*.cpp)
add_library( # Sets the name of the library.
        liveplayer
        SHARED
        ${SOURCE})


find_library( # Sets the name of the path variable.
        log-lib
        log)

include_directories(${CMAKE_SOURCE_DIR}/include)
set(my_lib_path ${CMAKE_SOURCE_DIR}/../../../libs/${ANDROID_ABI})
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -L${my_lib_path}")

target_link_libraries( # Specifies the target library.
        liveplayer
        avfilter avformat avcodec avutil swresample swscale
        ${log-lib})