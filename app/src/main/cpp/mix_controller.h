//
// Created by genius158 on 2022/6/22.
//

#ifndef APP_MIX_CONTROLLER_H
#define APP_MIX_CONTROLLER_H

#include "wav_file_reader.h"
#include "audio_mix.h"
#include "entity.h"

/**
 * 混音总控制器
 */
class MixController {

private:
    // wav header读取工具
    WavFileReader *wav_file_reader = new WavFileReader();

    std::string mix_dir_path;
    std::string mix_sound_name = "test.wav";

    WavHeader wav_header;

public:
    MixController(std::string mix_dir_path);

    ~MixController();

    /**
     * 从设置的音频数据中，获取音频信息参数
     * @param test_sound_path 测试的音频路径
     */
    void TestWavHeader(const string &test_sound_path);

    /**
     * 直接混音
     * @param p_entity 鼓声混音信息
     * @param size 大小
     */
    void Mix(const list<SoundInfo> &entity_list);

    WavHeader GetWavHeader(FILE * file);

};


#endif //APP_MIX_CONTROLLER_H
