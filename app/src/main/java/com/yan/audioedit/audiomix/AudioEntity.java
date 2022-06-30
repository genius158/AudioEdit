package com.yan.audioedit.audiomix;

import androidx.annotation.Keep;

import java.lang.annotation.Native;

/**
 * @author yanxianwei
 */
@Keep
public class AudioEntity {

    /**
     * 资源路径，
     * native有使用，不能混淆
     */
    @Keep
    @Native
    public final String resPath;

    /**
     * 资源合入时间点，
     * native有使用，不能混淆
     */
    @Keep
    @Native
    public final int entryTime;

    public final int startTime;
    public final int endTime;

    public AudioEntity(String resPath, int entryTime, int startTime, int endTime) {
        this.resPath = resPath;
        this.entryTime = entryTime;
        this.startTime = startTime;
        this.endTime = endTime;
    }
}
