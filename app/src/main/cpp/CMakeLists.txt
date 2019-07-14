cmake_minimum_required(VERSION 3.4.1)

#CMAKE_SOURCE_DIR代表的是CMakeLists.txt的路径
file(GLOB SOURCE ${CMAKE_SOURCE_DIR}/*.cpp)
add_subdirectory(${CMAKE_SOURCE_DIR}/librtmp)
add_library(
        # liveplayer这个是可以随意取名的
        liveplayer
        SHARED
        ${SOURCE})


find_library( # Sets the name of the path variable.
        log-lib
        log)

#引入头文件
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include_x264 ${CMAKE_SOURCE_DIR}/include_faac ${CMAKE_SOURCE_DIR}/include_opencv)
#include_directories(${CMAKE_SOURCE_DIR}/include_x264)
#CMAKE_CXX_FLAGS是一个系统变量
#ANDROID_ABI：内置系统变量，表示当前编译的平台即（"armeabi-v7a"）
#my_lib_path 是我们自己定义的变量(可以自己命名)
#千万需要注意的是这个路径是相对于CMakeLists.txt的路径，否则在编译时会提示找不到静态库文件
#注意：Ndk版本不同CMakeLists.txt的位置是不同的，低版本的CMakeLists.txt文件是放在src目录中的
set(my_lib_path ${CMAKE_SOURCE_DIR}/../../../libs/${ANDROID_ABI})

#设置静态库路径
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${my_lib_path}")

target_link_libraries(
        #和上面add_library中的库名保持一致
        liveplayer
        #这些是要引入的静态库，编译时会在头尾加上lib和.a,即lib+avfilter+.a
        avfilter avformat avcodec avutil swresample swscale
        ${log-lib}
        android
        z
        OpenSLES
        rtmp
        x264
        faac
        opencv_java4)