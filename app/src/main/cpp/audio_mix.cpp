

#include "audio_mix.h"
#include <vector>
#include <list>
#include "android_log.h"

using namespace std;

//---------------------------------------------------------------------------------------------
// TimeSliceByPoint()
// 切割时间片，单点混音，wav = 3、4个时，效果好一点
//---------------------------------------------------------------------------------------------
void CAudioMix::TimeSliceByPoint(vector<vector<short>> allMixingSounds,
                                 int RawDataCnt,
                                 vector<short> *_pRawDataBuffer) {
    for (int idxInEachSound = 0;
         idxInEachSound < RawDataCnt; idxInEachSound += allMixingSounds.size()) {
        for (int soundIdx = 0; soundIdx < allMixingSounds.size(); ++soundIdx) {
            if (idxInEachSound + soundIdx <
                RawDataCnt)                    // 即if(__pRawDataBuffer.size() < RawDataCnt)，但这样时间复杂度小
                _pRawDataBuffer->push_back(allMixingSounds[soundIdx][idxInEachSound + soundIdx]);
        }
    }
}

//---------------------------------------------------------------------------------------------
// TimeSliceBySection()
// 切割时间片，分段混音，wav=2时，效果好一些
//---------------------------------------------------------------------------------------------
void CAudioMix::TimeSliceBySection(vector<vector<short>> allMixingSounds,
                                   int RawDataCnt,
                                   vector<short> *_pRawDataBuffer) {
    const int SECTION_LEN = allMixingSounds.size();        // 2个wav时，SECTION_LEN=2是最好的，故推测SECTION_LEN = allMixingSounds.size()是最好的，晚上测试
    //	const int SECTION_LEN = 50;

    int idxInEachSound = 0;
    while (idxInEachSound < RawDataCnt) {
        for (int soundIdx = 0; soundIdx < allMixingSounds.size(); ++soundIdx) {
            for (int idxInEachSection = 0; idxInEachSection <
                                           SECTION_LEN; ++idxInEachSection)    // wav1先一次性填充满SECTION_LEN个点，wav2再重复上述过程
            {
                if (_pRawDataBuffer->size() <
                    RawDataCnt)                    // 即if(__pRawDataBuffer.size() < RawDataCnt)，但这样时间复杂度小 // idxInEachSound + soundIdx < RawDataCnt
                    _pRawDataBuffer->push_back(
                            allMixingSounds[soundIdx][idxInEachSound + idxInEachSection]);
            }
            idxInEachSound += SECTION_LEN;
        }

    }
}


//---------------------------------------------------------------------------------------------
// CombinePointsToOneWay1
// http://blog.sina.com.cn/s/blog_4d61a7570101arsr.html
// 通过“将多点组合为一个点”的方式混音
//---------------------------------------------------------------------------------------------
void CAudioMix::CombinePointsToOneWay1(vector<vector<short>> allMixingSounds,
                                       int RawDataCnt,
                                       vector<short> *_pRawDataBuffer) {
    // 初始化中间变量
    int tempMul = 1;
    int tempSum = 0;

    LOGE("mixedData mixedData RawDataCnt  %d ", RawDataCnt);


    for (int i = 0; i < RawDataCnt; ++i) {
        // 复位中间变量
        tempMul = 1;
        tempSum = 0;

        // 求中间变量
        for (int wavNum = 0; wavNum < allMixingSounds.size(); ++wavNum) {
            tempMul *= allMixingSounds[wavNum][i];
            tempSum += allMixingSounds[wavNum][i];
        }


        // 通过“将多点组合为一个点”的方式混音
        int mixedTempData = tempSum - (tempMul >> 0x10);
        short mixedData = static_cast<short>(mixedTempData);

        // 防止上下溢出
        if (mixedData > AUDIO_DATA_TYPE_MAX)
            mixedData = AUDIO_DATA_TYPE_MAX;
        else if (mixedData < AUDIO_DATA_TYPE_MIN)
            mixedData = AUDIO_DATA_TYPE_MIN;

        _pRawDataBuffer->push_back(mixedData);

    }
}

//---------------------------------------------------------------------------------------------
// MixNewLC
// http://blog.csdn.net/xuheazx/article/details/39523721 的方法3
//---------------------------------------------------------------------------------------------
void CAudioMix::CombinePointsToOneNewLC(vector<vector<short>> allMixingSounds,
                                        int RawDataCnt,
                                        vector<short> *_pRawDataBuffer) {
    // 初始化中间变量
    int tempMul = 1;
    int tempSum = 0;
    int mixedTempData;
    int howManyPointsArePos = 0;

    int sz = allMixingSounds.size();

    for (int i = 0; i < RawDataCnt; ++i) {
        // 复位中间变量
        tempMul = 1;
        tempSum = 0;

        // 求中间变量
        howManyPointsArePos = 0;                                // 统计每个点是不是都是正数
        for (int wavNum = 0; wavNum < sz; ++wavNum) {
            tempMul *= allMixingSounds[wavNum][i];
            tempSum += allMixingSounds[wavNum][i];

            if (allMixingSounds[wavNum][i] < 0)
                ++howManyPointsArePos;
        }

        // 混音
        if (howManyPointsArePos == sz)
            mixedTempData = tempSum - (tempMul / -(pow(2, 16 - 1) - 1));
        else
            mixedTempData = tempSum - (tempMul / (pow(2, 16 - 1) - 1));

        // 防止上下溢出
        if (mixedTempData > AUDIO_DATA_TYPE_MAX)
            mixedTempData = AUDIO_DATA_TYPE_MAX;
        else if (mixedTempData < AUDIO_DATA_TYPE_MIN)
            mixedTempData = AUDIO_DATA_TYPE_MIN;

        short mixedData = static_cast<short>(mixedTempData);
        _pRawDataBuffer->push_back(mixedData);

    }
}

//---------------------------------------------------------------------------------------------
// MixSoundsBySimplyAdd()

// 直接简单地叠加
// 由于叠加可能会超出上限32767导致溢出，因此需要将"short"(有符号2字节）用更大的范围"int"（有符号4字节）来表示。
// 效果很好
//---------------------------------------------------------------------------------------------
void CAudioMix::MixSoundsBySimplyAdd(vector<vector<short>> allMixingSounds,
                                     int RawDataCnt,
                                     vector<short> *_pRawDataBuffer) {
    int Sum = 0;                                    // 用更大的范围来表示（用有符号的int，而不要用无符号的DWORD）

    for (int i = 0; i < RawDataCnt; ++i) {
        Sum = 0;                                                // 复位叠加的值
        for (int wavNum = 0; wavNum < allMixingSounds.size(); ++wavNum) {
            Sum += allMixingSounds[wavNum][i];
        }
        LOGE("RawDataCnt  %d  %d    Sum %d", i, RawDataCnt, Sum);

        // 叠加之后，会溢出
        if (Sum > AUDIO_DATA_TYPE_MAX)
            Sum = AUDIO_DATA_TYPE_MAX;
        else if (Sum < AUDIO_DATA_TYPE_MIN)
            Sum = AUDIO_DATA_TYPE_MIN;


        // FIXME
//        Sum = allMixingSounds[0][i];
        _pRawDataBuffer->push_back(short(Sum));        // 把int再强制转换回为short
    }
}

//---------------------------------------------------------------------------------------------
// MixSoundsByMean()

// 叠加，然后取均值
// 由于叠加可能会超出上限32767导致溢出，因此需要将"short"(有符号2字节）用更大的范围"int"（有符号4字节）来表示。
// 缺点：wav文件越多，混音后的值越小
//---------------------------------------------------------------------------------------------
void CAudioMix::MixSoundsByMean(vector<vector<short>> allMixingSounds,
                                int RawDataCnt,
                                vector<short> *_pRawDataBuffer) {
    int Sum = 0;                                    // 用更大的范围来表示（用有符号的int，而不要用无符号的DWORD）
    int Mean = 0;
    int sz = allMixingSounds.size();

    for (int i = 0; i < RawDataCnt; ++i) {
        // 复位叠加的值
        Sum = 0;
        Mean = 0;

        for (int wavNum = 0; wavNum < allMixingSounds.size(); ++wavNum) {
            Sum += allMixingSounds[wavNum][i];
        }
        Mean = Sum / sz;

        // 叠加之后，会溢出
        if (Mean > AUDIO_DATA_TYPE_MAX)
            Mean = AUDIO_DATA_TYPE_MAX;
        else if (Mean < AUDIO_DATA_TYPE_MIN)
            Mean = AUDIO_DATA_TYPE_MIN;

        _pRawDataBuffer->push_back(short(Mean));        // 把int再强制转换回为short
    }
}

//---------------------------------------------------------------------------------------------
// AddAndNormalization()
// 叠加，然后归一化混音
// 自适应混音加权(衰减因子法)（改进版归一化因子法）
// 由于叠加可能会超出上限32767导致溢出，因此需要将"short"(有符号2字节）用更大的范围"int"（有符号4字节）来表示。
// 效果很好
//---------------------------------------------------------------------------------------------
void CAudioMix::AddAndNormalization(vector<vector<short>> allMixingSounds,
                                      int RawDataCnt,
                                      vector<short> *_pRawDataBuffer,
                                      double &decayFactor) {
    // 用更大的范围来表示声音叠加的
    int Sum;

    for (int i = 0; i < RawDataCnt; ++i) {
        Sum = 0;
        for (auto &allMixingSound: allMixingSounds) {
            Sum += allMixingSound[i];
        }
        // 将衰减因子作用在叠加的音频上
        Sum *= decayFactor;


        // 计算衰减因子
        // 1. 叠加之后，会溢出，计算溢出的倍数（即衰减因子）
        if (Sum > AUDIO_DATA_TYPE_MAX) {
            // 算大了，就用小数0.8衰减
            decayFactor = static_cast<double>(AUDIO_DATA_TYPE_MAX) /
                          static_cast<double>(Sum);
            Sum = AUDIO_DATA_TYPE_MAX;
        } else if (Sum < AUDIO_DATA_TYPE_MIN) {
            // 算小了，就用大数1.2增加
            decayFactor = static_cast<double>(AUDIO_DATA_TYPE_MIN) /
                          static_cast<double>(Sum);
            Sum = AUDIO_DATA_TYPE_MIN;
        }

        // 2. 衰减因子的平滑（为了防止个别点偶然的溢出）
        if (decayFactor < 1) {
            decayFactor += static_cast<double>(1 - decayFactor) / static_cast<double>(32);
        }

        // 把int再强制转换回为short
        _pRawDataBuffer->push_back(short(Sum));
    }
}
