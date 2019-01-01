cmake_minimum_required(VERSION 3.4.1)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)
set(FFMPEG_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../android_ffmpeg)
set(PROTOBUF_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../android_protobuf)
set(EDITORSDK_JNI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/editorsdk)
set(SHARED_CODE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../sharedcode)
set(CMAKE_CXX_STANDARD 11)

find_library(log-lib log)

add_library(libffmpeg SHARED IMPORTED)
set_target_properties(libffmpeg PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_LIB_DIR}/armeabi/libffmpeg.so)

add_library(libprotobuf-lite STATIC IMPORTED)
set_target_properties(libprotobuf-lite PROPERTIES IMPORTED_LOCATION
        ${PROTOBUF_LIB_DIR}/armeabi/libprotobuf-lite.a)

aux_source_directory(${SOURCE_DIR} SOURCE_DIR_ROOT)
list(APPEND SOURCE_DIR_ROOT
        ${EDITORSDK_JNI_DIR}/native-lib.cc
        ${EDITORSDK_JNI_DIR}/ffmpeg_sample_six.cpp)
list(APPEND SOURCE_DIR_ROOT
        ${SHARED_CODE_DIR}/editorsdk/base/av_utils.cc
        ${SHARED_CODE_DIR}/editorsdk/generated_protobuf/editor_model.pb.cc)
add_library(mttvideoeditorsdkjni
            SHARED
            ${SOURCE_DIR_ROOT})

list(APPEND SOURCE_DIR_INCLUDE
        ${SHARED_CODE_DIR}/editorsdk/base/av_utils.h
        ${SHARED_CODE_DIR}/editorsdk/base/blocking_queue.h
        ${SHARED_CODE_DIR}/editorsdk/generated_protobuf/editor_model.pb.h
        ${PROTOBUF_LIB_DIR}/include
        ${FFMPEG_LIB_DIR}/include)

target_include_directories(mttvideoeditorsdkjni PRIVATE ${SOURCE_DIR_INCLUDE})

list(APPEND LINK_LIBRARIES
        mttvideoeditorsdkjni
        -ljnigraphics
        -landroid
        libprotobuf-lite
        libffmpeg)
target_compile_options(mttvideoeditorsdkjni PUBLIC -D_LIBCPP_HAS_THREAD_SAFETY_ANNOTATIONS -Wthread-safety -Werror -Wall -Wno-documentation -Wno-shorten-64-to-32 -Wno-nullability-completeness)
target_link_libraries(${LINK_LIBRARIES} ${log-lib})
