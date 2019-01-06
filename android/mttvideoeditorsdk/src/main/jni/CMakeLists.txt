cmake_minimum_required(VERSION 3.4.1)

# 当前文件存在的目录
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
# MyTiktokVideoEditor 的根目录
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)
# ffmpeg 的目录
set(FFMPEG_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../android_ffmpeg)
# protobuf 头文件与静态库的目录
set(PROTOBUF_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../android_protobuf)
# android 专用 c++ 代码的目录
set(EDITORSDK_JNI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/editorsdk)
# c++ 共享代码的目录
set(SHARED_CODE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../sharedcode)
# c++ 的版本
set(CMAKE_CXX_STANDARD 11)

# 找到 android ndk 的 log 库
find_library(log-lib log)

# 将 libffmpeg.so 添加到 libffmpeg 这个 动态 library中
add_library(libffmpeg SHARED IMPORTED)
set_target_properties(libffmpeg PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_LIB_DIR}/armeabi/libffmpeg.so)

# 将 libprotobuf.a 添加到 libprotobuf-lite 这个 静态 library 中
add_library(libprotobuf-lite STATIC IMPORTED)
set_target_properties(libprotobuf-lite PROPERTIES IMPORTED_LOCATION
        ${PROTOBUF_LIB_DIR}/armeabi/libprotobuf-lite.a)

aux_source_directory(${SOURCE_DIR} SOURCE_DIR_ROOT)
# 将所有自己写的 c++ 代码添加到 mttvideoeditorsdkjni 这个 动态 library 中
list(APPEND SOURCE_DIR_ROOT
        ${EDITORSDK_JNI_DIR}/native-lib.cc
        ${EDITORSDK_JNI_DIR}/ffmpeg_sample_six.cpp)
list(APPEND SOURCE_DIR_ROOT
        ${SHARED_CODE_DIR}/editorsdk/base/av_utils.cc
        ${SHARED_CODE_DIR}/editorsdk/generated_protobuf/editor_model.pb.cc)
add_library(mttvideoeditorsdkjni
            SHARED
            ${SOURCE_DIR_ROOT})
# 将所有头文件添加到一个列表中，在最后一起链接
list(APPEND SOURCE_DIR_INCLUDE
        ${SHARED_CODE_DIR}/editorsdk/base/av_utils.h
        ${SHARED_CODE_DIR}/editorsdk/base/blocking_queue.h
        ${SHARED_CODE_DIR}/editorsdk/generated_protobuf/editor_model.pb.h
        ${PROTOBUF_LIB_DIR}/include
        ${FFMPEG_LIB_DIR}/include) # 将 ffmpeg 的头文件放入一个列表中

target_include_directories(mttvideoeditorsdkjni PRIVATE ${SOURCE_DIR_INCLUDE}) # 连接列表中所有的头文件

list(APPEND LINK_LIBRARIES
        mttvideoeditorsdkjni
        -landroid
        libprotobuf-lite
        libffmpeg) # 将所有的库添加到一个列表中，最后一起链接
target_compile_options(mttvideoeditorsdkjni PUBLIC -D_LIBCPP_HAS_THREAD_SAFETY_ANNOTATIONS -Wthread-safety -Werror -Wall -Wno-documentation -Wno-shorten-64-to-32 -Wno-nullability-completeness)
target_link_libraries(${LINK_LIBRARIES} ${log-lib}) # 链接所有库
