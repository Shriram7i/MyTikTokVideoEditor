#!/usr/bin/env bash
# exit 不注释的时候，表示 android 项目编译的时候不需要编译 ffmepg，注释的时候，表示 android 项目编译的时候要编译 ffmpeg。
 exit

# 执行 FFmpeg 源码项目中的编译脚本
sh /Users/whensunset/AndroidStudioProjects/KSVideoProject/ffmpeg/build_android.sh

# 当前项目的 so 文件的存放目录，需要改成自己的
so_path="/Users/whensunset/AndroidStudioProjects/KSVideoProject/MyTikTokVideoEditor/android/android_ffmpeg/armeabi/"

# 所有 so 文件编译生成后的默认命名
libffmpeg_name="libffmpeg.so"

# 删除当前项目中的老的 so 文件删除
rm ${so_path}${libffmpeg_name}

# FFmpeg 源码项目中，编译好的 so 文件的路径，需要改成自己的
build_so_path="/Users/whensunset/AndroidStudioProjects/KSVideoProject/ffmpeg/android/arm/"

# 将新编译的 so 文件拷贝到当前项目的 so 目录下
cd /Users/whensunset/AndroidStudioProjects/KSVideoProject/FFmpeglearning/app
cp ${build_so_path}${libffmpeg_name} ${so_path}${libffmpeg_name}
