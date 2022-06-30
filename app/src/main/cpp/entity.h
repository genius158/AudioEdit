#ifndef AUDIO_ENTITY_H
#define AUDIO_ENTITY_H

//
// Created by genius158 on 2022/6/20.
//
#include <cstdint>

typedef struct SoundInfo {
    int enter_time;
    int start_time;
    int end_time;
    FILE * wav_file;
    int header_offset;
    std::string file_path;
    int short_count;

} SoundInfo;


#endif //AUDIO_ENTITY_H