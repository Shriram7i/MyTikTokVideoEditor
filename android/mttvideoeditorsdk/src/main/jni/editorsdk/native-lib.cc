#include <jni.h>
#include "ffmpeg_sample.h"
#include "../../../../../../sharedcode/editorsdk/generated_protobuf/editor_model.pb.h"
#include <google/protobuf/arena.h>
extern "C"
{
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
}
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"whensunset",FORMAT,##__VA_ARGS__);

#ifdef ANDROID

#include <android/log.h>

#ifndef LOG_TAG
#define  LOG_TAG    "FFMPEG"
#endif

#define  XLOGD(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  XLOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#else
#include <stdio.h>
#define XLOGE(format, ...)  fprintf(stdout, LOG_TAG ": " format "\n", ##__VA_ARGS__)
#define XLOGI(format, ...)  fprintf(stderr, LOG_TAG ": " format "\n", ##__VA_ARGS__)
#endif  //ANDROID

static void log_callback_null(void *ptr, int level, const char *fmt, va_list vl)
{
    static int print_prefix = 1;
    static char prev[1024];
    char line[1024];

    av_log_format_line(ptr, level, fmt, vl, line, sizeof(line), &print_prefix);

    strcpy(prev, line);

    if (level <= AV_LOG_WARNING)
    {
        XLOGE("%s", line);
    }
    else
    {
        XLOGD("%s", line);
    }
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_whensunset_mytiktokvideoeditor_GameActivity_play(JNIEnv *env, jclass type,
                                                          jobject surface) {

    play(env, surface);
    return 0;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_whensunset_mytiktokvideoeditor_GameActivity_initFfmpegLog(JNIEnv *env, jobject instance) {

    av_log_set_callback(log_callback_null);
    google::protobuf::Arena arena;
}