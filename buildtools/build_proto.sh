#!/bin/bash
show_msg() {
  echo -e "\033[36m$1\033[0m"
}

show_err() {
  echo -e "\033[31m$1\033[0m"
}
# protobuf 的版本
v3_0_0="v3.0.0"
# 当前的目录
script_path=$(cd `dirname $0`; pwd)
# protoc 是 protobuf 编译之后生成的可执行文件，可以用来根据 proto 文件生成 java、c++等等代码
protoc_path=$script_path/tools/protoc
# protobuf 的源码地址
protoc_src=$script_path/protobuf
# 生成的 java 文件需要移动到的位置
java_target_path="$script_path/../android/mttvideoeditorsdk/src/main"
# 生成的 c++ 文件需要移动的位置
cpp_target_path="$script_path/../sharedcode/editorsdk/generated_protobuf"

# 本方法用于执行 protobuf 源码的脚本进行编译
build_protobuf() {
  mkdir -p $protoc_src/host
  mkdir -p $protoc_path/$1
  cd $protoc_src/host
  ../configure --prefix=$protoc_path/$1 && make -j8 && make install

  if test $? != 0; then
    show_err "Build protobuf failed"
    exit 1
  fi

  cd $script_path
  rm -rf $protoc_src/host
}

# 本方法用于 clone protobuf 的源码，然后 checkout 到3.0.0的版本，然后调用 build_protobuf 进行编译
build() {
  git clone https://github.com/google/protobuf.git

  show_msg "Building android protobuff source code"
  cd protobuf
  git checkout $v3_0_0
  git cherry-pick bba446b  # fix issue https://github.com/google/protobuf/issues/2063
  ./autogen.sh
  build_protobuf $v3_0_0

  show_msg "Build protobuf complete"
  cd $script_path
  rm -rf protobuf
}

# 如果 protoc 不存在，那么就去 clone protobuf 的源码，然后编译
if [ ! -x "$protoc_path/$v3_0_0/bin/protoc" ]; then
  build
fi

# 删除之前已经生成的 java c++ 文件
rm $java_target_path/java/com/whensunset/mttvideoeditorsdk/model/protobuf/*.java
rm $cpp_target_path/*.pb.cc $cpp_target_path/*.pb.h

cd $script_path/../sharedproto

mkdir -p java cpp

# 用 protoc 生成 java c++ 文件
$protoc_path/$v3_0_0/bin/protoc *.proto --java_out=java --cpp_out=cpp

# 将生成的 java c++ 文件移动到对应的文件夹下
cp -r java $java_target_path
mkdir -p $cpp_target_path
cp cpp/* $cpp_target_path
rm -rf java cpp


