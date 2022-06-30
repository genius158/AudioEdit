//
// Created by genius158 on 2022/6/20.
//

#include <jni.h>
#include <cstring>
#include <cstdlib>
#include "android_log.h"
#include "mix_controller.h"
#include <list>

jclass drummingEntityClazz;
jfieldID drummingEntityResPathFieldID;
jfieldID drummingEntityEnterTimeFieldID;
jfieldID drummingEntityStartTimeFieldID;
jfieldID drummingEntityEndTimeFieldID;

/// jstring to char*
char *jstringToString(JNIEnv *env, jobject jstr) {
    char *rtn = nullptr;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    auto barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_yan_audioedit_audiomix_AudioMixer_nInit(JNIEnv *env, jclass clazz, jstring file_dir_path) {

    char *path = jstringToString(env, file_dir_path);
    auto *mix_controller = new MixController(path);
    free(path);
    return reinterpret_cast<long >(mix_controller);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yan_audioedit_audiomix_AudioMixer_nMixAudio(JNIEnv *env, jclass clazz,
                                                     jlong p_audio_controller,
                                                     jobjectArray entities) {

    auto mix_controller = reinterpret_cast<MixController *>(p_audio_controller);
    int32_t length = env->GetArrayLength(entities);

    // 创建native对应的AMDrummingEntity
    std::list<SoundInfo> entities_list;
    std::list<FILE *> file_list;
    for (int i = 0; i < length; ++i) {
        jobject obj = env->GetObjectArrayElement(entities, i);
        jobject path = env->GetObjectField(obj, drummingEntityResPathFieldID);
        char *file_path = jstringToString(env, path);
        int enter_time = env->GetIntField(obj, drummingEntityEnterTimeFieldID);
        int start_time = env->GetIntField(obj, drummingEntityStartTimeFieldID);
        int end_time = env->GetIntField(obj, drummingEntityEndTimeFieldID);

        FILE *file = fopen(file_path, "rb+");
        file_list.push_back(file);
        WavHeader header = mix_controller->GetWavHeader(file);
        int short_count = header.wave.file_length + 8;
        int header_offset = short_count - header.data.data_length;
        SoundInfo entity{
                enter_time,
                start_time,
                end_time,
                file,
                header_offset,
                file_path,
                short_count

        };
        entities_list.push_back(entity);
        free(file_path);
    }
    mix_controller->Mix(entities_list);

    for (auto file: file_list) {
        fclose(file);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yan_audioedit_audiomix_AudioMixer_nTestWavHeader(JNIEnv *env, jclass clazz,
                                                          jlong p_audio_controller,
                                                          jstring sound_path) {
    auto mix_controller = reinterpret_cast<MixController *>(p_audio_controller);
    char *file_path = jstringToString(env, sound_path);
    mix_controller->TestWavHeader(file_path);
    free(file_path);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yan_audioedit_audiomix_AudioMixer_nRelease(JNIEnv *env, jclass clazz,
                                                    jlong p_audio_controller) {
    auto mix_controller = reinterpret_cast<MixController *>(p_audio_controller);
    delete mix_controller;
}


jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    drummingEntityClazz = env->FindClass(
            "com/yan/audioedit/audiomix/AudioEntity");

    drummingEntityResPathFieldID = env->GetFieldID(drummingEntityClazz, "resPath",
                                                   "Ljava/lang/String;");
    drummingEntityEnterTimeFieldID = env->GetFieldID(drummingEntityClazz, "entryTime", "I");
    drummingEntityStartTimeFieldID = env->GetFieldID(drummingEntityClazz, "startTime", "I");
    drummingEntityEndTimeFieldID = env->GetFieldID(drummingEntityClazz, "endTime", "I");
    return JNI_VERSION_1_4;
}
