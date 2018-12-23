package com.whensunset.mttvideoeditorsdk;

/**
 * Created by whensunset on 2018/12/23.
 */

public class A {
  static {
    System.loadLibrary("ffmpeg");
    System.loadLibrary("mttvideoeditorsdkjni");
  }
}
