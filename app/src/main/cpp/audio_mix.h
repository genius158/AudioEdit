#ifndef APP_AUDIO_MIX_H
#define APP_AUDIO_MIX_H

#include <vector>
#include <list>
#include<stdio.h>

#define AUDIO_DATA_TYPE_MAX 32767    // 2^15（short）
#define AUDIO_DATA_TYPE_MIN -32768

using namespace std;

class CAudioMix {
public:
    // 切割时间片，单点混音
    static void TimeSliceByPoint(vector<vector<short>> allMixingSounds,
                                 int RawDataCnt,
                                 vector<short> *_pRawDataBuffer);

    // 切割时间片，分段混音
    static void TimeSliceBySection(vector<vector<short>> allMixingSounds,
                                   int RawDataCnt,
                                   vector<short> *_pRawDataBuffer);

    // CombinePointsToOneWay1
    static void CombinePointsToOneWay1(vector<vector<short>> allMixingSounds,
                                       int RawDataCnt,
                                       vector<short> *_pRawDataBuffer);

    // MixNewLC
    static void CombinePointsToOneNewLC(vector<vector<short>> allMixingSounds,
                                        int RawDataCnt,
                                        vector<short> *_pRawDataBuffer);

    // 直接简单地叠加
    static void MixSoundsBySimplyAdd(vector<vector<short>> allMixingSounds,
                                     int RawDataCnt,
                                     vector<short> *_pRawDataBuffer);

    // 叠加，然后取均值
    static void MixSoundsByMean(vector<vector<short>> allMixingSounds,
                                int RawDataCnt,
                                vector<short> *_pRawDataBuffer);


    static void AddAndNormalization(vector<vector<short>> allMixingSounds,
                                    int RawDataCnt,
                                    vector<short> *_pRawDataBuffer,
                                    double &decayFactor);
};


#endif //APP_AUDIO_MIX_H