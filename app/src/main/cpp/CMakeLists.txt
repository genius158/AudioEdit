cmake_minimum_required(VERSION 3.4.1)

project("audio_mix")

add_library(audio_mix SHARED
        audio_mix.cpp
        audio_mix_point.cpp
        wav_file_reader.cpp
        mix_controller.cpp
        )

find_library(log-lib log)

target_link_libraries(audio_mix ${log-lib} )