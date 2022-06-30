//
// Created by genius158 on 2022/6/22.
//

#include "mix_controller.h"

#include <utility>
#include <list>
#include "android_log.h"
#include "audio_mix.h"
#include "entity.h"

using namespace std;

MixController::MixController(string mix_dir_path) :
        mix_dir_path(move(mix_dir_path)) {}

MixController::~MixController() {
    delete wav_file_reader;
}

void MixController::TestWavHeader(const string &test_sound_path) {
    FILE *test_sound_file = fopen(test_sound_path.c_str(), "rb");
    wav_file_reader->ReadWavFile(test_sound_file);
    wav_file_reader->CopyHeader(wav_header);
    fclose(test_sound_file);
}

/**
 * 根据音频信息直接混音
 * @param entity_list
 */
void MixController::Mix(const list<SoundInfo> &entity_list) {
    if (entity_list.empty())return;

    int sample_rate =
            wav_header.format.samples_per_sec == 0 ? 48000 : wav_header.format.samples_per_sec;
    int bit_pre_sample =
            wav_header.format.bits_per_sample == 0 ? 16 : wav_header.format.bits_per_sample;
    int channels = wav_header.format.channels == 0 ? 2 : wav_header.format.channels;

    // 音频信息
    list<SoundInfo> sound_list(entity_list);

    // 所有待混音的音频的数据
    vector<vector<short>> sound_beat_list;

    // AddAndNormalization 混音算法衰减比例
    double decay_factor = 1.0;

    // 当前混音的时间刻度
    int cur_time = 0;

    // 一次混音的数据量
    int buffer_time = 1000;

    // 每毫秒的short count 数
    int buffer_millisecond_count = sample_rate * bit_pre_sample / 8 * channels / 1000 / 2;

    // buffer_time毫秒的数据量 short count
    int buffer_count = buffer_millisecond_count * buffer_time;

    /*
     * 音频混合的文件名设置
     */
    string output = mix_dir_path;
    output.append("/");
    output.append(mix_sound_name);
    LOGE("output  %s", output.c_str());

    // 打开音频混音的文件
    FILE *output_file = fopen(output.c_str(), "wb+");
    // 设置wav header
    WavFileReader::WriteWavHeader(output_file, 44, channels,
                                  sample_rate,
                                  bit_pre_sample,
                                  0);

    vector<short> raw_data_buffer;

    short data;

    for (const auto &it: sound_list) {
        FILE *wav_file = it.wav_file;
        fseek(wav_file, it.header_offset, SEEK_SET);
        fseek(wav_file, it.start_time * buffer_millisecond_count * 2, SEEK_CUR);
    }

    while (!sound_list.empty()) {
        LOGE("sound_list size  %u", sound_list.size());

        for (auto it = sound_list.begin(); it != sound_list.end();) {
            // 音频开始时间
            int32_t enter_time = it->enter_time;

            // 音频文件
            const char *file_path = it->file_path.c_str();

            int headerOffset = it->header_offset;

            int start_time = it->start_time;
            int end_time = it->end_time;
            int source_duration = end_time - start_time;

            auto wav_file = it->wav_file;

            LOGE("enter_time file_path  %d  %s   %d", enter_time, file_path, source_duration);

            // 进度超过了音频开始的时间，说明音频已经使用过了，直接移除
            if (cur_time > enter_time + source_duration) {
                it = sound_list.erase(it);
                continue;
            }
            it++;

            // 音频还没出现
            if (cur_time + buffer_time <= enter_time)continue;

            vector<short> short_data(buffer_count, 0);

            int start_time_offset = enter_time - cur_time;
            int enter_source_time = min(source_duration + start_time_offset,
                                        buffer_time);

            // 0000
            //   00000000
            // 00000000000000
            if (start_time_offset >= 0) {
                int start_index = start_time_offset * buffer_millisecond_count;
                fseek(wav_file, headerOffset, SEEK_SET);
                fseek(wav_file, start_time * buffer_millisecond_count * 2, SEEK_CUR);
                for (int i = start_index;
                     i < enter_source_time * buffer_millisecond_count; i++) {
                    fread(&data, 2, 1, wav_file);
                    short_data[i] = data;
                }
            } else {
                int startOffset = abs(start_time_offset * buffer_millisecond_count);
                fseek(wav_file, headerOffset, SEEK_SET);
                fseek(wav_file, start_time * buffer_millisecond_count * 2+startOffset*2, SEEK_CUR);
                for (int i = 0; i < enter_source_time * buffer_millisecond_count; i++) {
                    fread(&data, 2, 1, wav_file);
                    short_data[i] = data;
                }
            }

            sound_beat_list.push_back(short_data);
        }

        // 当前混音音轨确认完毕，开始下一次混音进度
        cur_time += buffer_time;

        LOGE("cur_time cur_time  %d   sound_beat_list size %u ", cur_time, sound_beat_list.size());

        /*
         * 没有需要混音的内容，补齐空白音频
         */
        if (sound_beat_list.empty()) {
            vector<short> short_data(buffer_count, 0);
            sound_beat_list.push_back(short_data);
        }

        raw_data_buffer.clear();
        // 开始混音
        CAudioMix::AddAndNormalization(sound_beat_list, buffer_count, &raw_data_buffer,
                                       decay_factor);
        sound_beat_list.clear();

        for (short data: raw_data_buffer) {
            fwrite(&data, 2, 1, output_file);
        }
    }

    fseek(output_file, 0, SEEK_SET);

    int data_count = cur_time * buffer_millisecond_count * 2;
    WavFileReader::WriteWavHeader(output_file, data_count + 44, 2,
                                  sample_rate,
                                  bit_pre_sample,
                                  data_count);

    fclose(output_file);
}

WavHeader MixController::GetWavHeader(FILE *file) {
    wav_file_reader->ReadWavFile(file);
    return wav_file_reader->header;
}



