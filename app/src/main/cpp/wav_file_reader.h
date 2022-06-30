#ifndef WAV_FILE_READER_H
#define WAV_FILE_READER_H

#include<string>

struct WaveRIFF {
    char id[4] = {'R', 'I', 'F', 'F'};
    uint32_t file_length;
    char wave_flag[4] = {'W', 'A', 'V', 'E'};
};

struct WaveFormat {
    char id[4] = {'f', 'm', 't', ' '};
    uint32_t block_size = 16;
    uint16_t format_tag;
    uint16_t channels;
    uint32_t samples_per_sec;
    uint32_t avg_bytes_per_sec;
    uint16_t block_align;
    uint16_t bits_per_sample;
};

typedef struct WaveData {
    char id[4] = {'d', 'a', 't', 'a'};
    uint32_t data_length;
} WaveData;

/// wav的头部
struct WavHeader {
    WaveRIFF wave;
    WaveFormat format;
    WaveData data;
};


class WavFileReader {
public:
    WavHeader header;

    /// <summary>
    /// 构造方法
    /// </summary>
    WavFileReader();

    /// <summary>
    /// 打开wav文件
    /// </summary>
    /// <param name="fileName">文件名</param>
    /// <returns>是否打开成功</returns>
    bool ReadWavFile(FILE *file);

    /// <summary>
    /// 获取文件长度
    /// </summary>
    /// <returns>文件长度</returns>
    int GetFileLength() const;



    /**
     * wav头部写入
     * @param output 写入的文件
     * @param file_length 文件总大小
     * @param channels 通道数
     * @param samples_per_sec 采样率
     * @param bits_per_sample 位深
     * @param data_length pcm数据大小
     */
    static void WriteWavHeader(FILE *output, uint32_t file_length, uint16_t channels,
                        uint32_t samples_per_sec,
                        uint16_t bits_per_sample,
                        uint32_t data_length);

    /// <summary>
    /// 获取音频数据长度
    /// </summary>
    /// <returns>音频数据长度</returns>
    int GetDataLength() const;


    int GetDataOffset() const;

    /// <summary>
    /// 获取声道数
    /// </summary>
    /// <returns>声道数</returns>
    int GetChannels() const;

    /// <summary>
    /// 获取采样率
    /// </summary>
    /// <returns>采样率，单位：hz</returns>
    int GetSampleRate() const;

    /// <summary>
    /// 获取位深
    /// </summary>
    /// <returns>位深，单位：bits</returns>
    int GetBitsPerSample() const;

    /// 获取比特率
    int GetBitsRate() const;

    void CopyHeader(WavHeader &wavHeader);

private:
    uint32_t _file_length = 0;
    uint32_t _data_length = 0;
    int _channels = 0;
    uint32_t _sample_rate = 0;
    uint16_t _bits_per_sample = 0;
    uint32_t _avg_bytes_per_sec = 0;
    int _data_offset = 0;
};

#endif //WAV_FILE_READER_H