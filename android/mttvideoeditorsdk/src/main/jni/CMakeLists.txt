cmake_minimum_required(VERSION 3.4.1)

set(source_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(root_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)
set(ffmpeg_lib_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../android_ffmpeg)
set(EDITORSDK_JNI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/editorsdk)
set(CMAKE_CXX_STANDARD 14)

find_library(log-lib log)

add_library(libffmpeg SHARED IMPORTED)
set_target_properties(libffmpeg PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_DIR}/armeabi/libffmpeg.so)

aux_source_directory(${source_DIR} SOURCE_DIR_ROOT)
list(APPEND SOURCE_DIR_ROOT
        ${EDITORSDK_JNI_DIR}/native-lib.cc
        ${EDITORSDK_JNI_DIR}/ffmpeg_sample_six.cpp)
add_library(mttvideoeditorsdkjni
            SHARED
            ${SOURCE_DIR_ROOT})

list(APPEND SOURCE_DIR_INCLUDE
        ${ffmpeg_lib_DIR}/include)
target_include_directories(mttvideoeditorsdkjni PRIVATE ${SOURCE_DIR_INCLUDE})

list(APPEND LINK_LIBRARIES
        mttvideoeditorsdkjni
        -ljnigraphics
        -landroid
        libffmpeg)
target_compile_options(mttvideoeditorsdkjni PUBLIC -D_LIBCPP_HAS_THREAD_SAFETY_ANNOTATIONS -Wthread-safety -Werror -Wall -Wno-documentation -Wno-shorten-64-to-32 -Wno-nullability-completeness)
target_link_libraries(${LINK_LIBRARIES} ${log-lib})
