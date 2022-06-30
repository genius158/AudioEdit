#include"wav_file_reader.h"
#include "android_log.h"

WavFileReader::WavFileReader() = default;


bool WavFileReader::ReadWavFile(FILE *file) {
    uint32_t userDataSize;
    if (!file) {
        LOGE("打开文件失败!\n");
        return false;
    }
    fseek(file, 0, SEEK_SET);


    //读取头部信息
    if (fread(&header.wave, 1, sizeof(header.wave), static_cast<FILE *>(file)) !=
        sizeof(header.wave)) {
        LOGE("文件读取错误，读取riff失败!\n");
        goto error;
    }
    if (std::string(header.wave.id, 4) != "RIFF" ||
        std::string(header.wave.wave_flag, 4) != "WAVE") {
        LOGE("头部信息不正确，不是wav文件!\n");
        goto error;
    }
    if (fread(&header.format, 1, sizeof(header.format), static_cast<FILE *>(file)) !=
        sizeof(header.format)) {
        LOGE("文件读取错误，读取format失败!\n");
        goto error;
    }
    if (std::string(header.format.id, 4) != "fmt ") {
        LOGE("头部信息不正确，缺少fmt!\n");
        goto error;
    }
    if (header.format.format_tag != 1) {
        LOGE("程序不支持，数据格式非pcm，只支持pcm格式的数据!\n");
        goto error;
    }
    userDataSize = header.format.block_size - sizeof(header.format) + 8;
    if (userDataSize < 0) {
        LOGE("头部信息不正确，blockSize大小异常!\n");
        goto error;
    } else if (userDataSize > 0) {
        if (fseek(static_cast<FILE *>(file), userDataSize, SEEK_CUR) != 0) {
            LOGE("文件读取错误!\n");
            goto error;
        }
    }
    while (true) {
        if (fread(&header.data, 1, sizeof(header.data), static_cast<FILE *>(file)) !=
            sizeof(header.data)) {
            LOGE("文件读取错误!\n");
            goto error;
        };
        if (std::string(header.data.id, 4) != "data") {
            if (fseek(static_cast<FILE *>(file), header.data.data_length, SEEK_CUR) != 0) {
                LOGE("文件读取错误!\n");
                goto error;
            }
            continue;
        }
        break;
    }

    _data_offset = ftell(static_cast<FILE *>(file));
    _file_length = header.wave.file_length + 8;
    _data_length = header.data.data_length;
    _channels = header.format.channels;
    _avg_bytes_per_sec = header.format.avg_bytes_per_sec;
    _sample_rate = header.format.samples_per_sec;
    _bits_per_sample = header.format.bits_per_sample;

    LOGE("_data_offset %d ", _data_offset);
    LOGE("_file_length %d ", _file_length);
    LOGE("_data_length %d ", _data_length);
    LOGE("_channels %d ", _channels);
    LOGE("_sample_rate %d ", _sample_rate);
    LOGE("_bits_per_sample %d ", _bits_per_sample);
    LOGE("avg_bytes_per_sec %d ", _avg_bytes_per_sec);
    LOGE("GetBitsRate() %d ", GetBitsRate());

    fseek(static_cast<FILE *>(file), 0, SEEK_SET);
    return true;
    error:
    LOGE("error error error!\n");
    return false;
}

void WavFileReader::WriteWavHeader(FILE *output, uint32_t file_length, uint16_t channels,
                                   uint32_t samples_per_sec,
                                   uint16_t bits_per_sample,
                                   uint32_t data_length) {
    WavHeader wavHeader;

    wavHeader.wave.file_length = file_length - 8;

    wavHeader.format.block_size = 16;
    wavHeader.format.format_tag = 1;
    wavHeader.format.channels = channels;
    wavHeader.format.samples_per_sec = samples_per_sec;
    wavHeader.format.avg_bytes_per_sec = samples_per_sec * channels * bits_per_sample / 8;
    wavHeader.format.block_align = channels * (16 / 8);
    wavHeader.format.bits_per_sample = bits_per_sample;

    wavHeader.data.data_length = data_length - 44;

    fseek(output, 0, SEEK_SET);
    fwrite(&wavHeader, 1, 44, output);

}

int WavFileReader::GetFileLength() const {
    return _file_length;
}

int WavFileReader::GetDataLength() const {
    return _data_length;
}

int WavFileReader::GetDataOffset() const {
    return _data_offset;
}

int WavFileReader::GetChannels() const {
    return _channels;
}

int WavFileReader::GetSampleRate() const {
    return _sample_rate;
}

int WavFileReader::GetBitsPerSample() const {
    return _bits_per_sample;
}

int WavFileReader::GetBitsRate() const {
    return _channels * _bits_per_sample * _sample_rate / 8;
}

void WavFileReader::CopyHeader(WavHeader &wavHeader) {
//    wavHeader.wave = WaveRIFF();
    wavHeader.wave.file_length = GetFileLength() - 8;

//    wavHeader.format = WaveFormat();
    wavHeader.format.block_size = 16;
    wavHeader.format.format_tag = 1;
    wavHeader.format.channels = GetChannels();
    wavHeader.format.samples_per_sec = GetSampleRate();
    wavHeader.format.avg_bytes_per_sec = GetSampleRate() * GetChannels() * GetBitsPerSample() / 8;
    wavHeader.format.block_align = GetChannels() * (16 / 8);
    wavHeader.format.bits_per_sample = GetBitsPerSample();

//    wavHeader.data = WaveData();
    wavHeader.data.data_length = GetFileLength() - 44;

}

