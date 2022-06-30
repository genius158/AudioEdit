package com.yan.audioedit.audiomix;

import android.content.Context;

import java.io.File;
/**
 * @author yanxianwei
 */
public class AudioMixer {

    static {
        System.loadLibrary("audio_mix");
    }

    private final long pAudioController;
    private final Context ctx;


    public File getMixFile() {
        File trickMixFile = new File(ctx.getExternalCacheDir(), "mix");
        trickMixFile.mkdirs();
        return trickMixFile;
    }

    public AudioMixer(Context ctx) {
        this.ctx = ctx;
        pAudioController = nInit( getMixFile().getAbsolutePath());
    }

    public void mixAudio(AudioEntity[] entities) {
        nMixAudio(pAudioController, entities);
    }

    public void testWavHeader(String drumPath) {
        nTestWavHeader(pAudioController, drumPath);
    }

    public void release() {
        nRelease(pAudioController);
    }

    private static native long nInit(String fileDirPath);

    private static native void nMixAudio(long pAudioController, AudioEntity[] entities);

    private static native void nTestWavHeader(long pAudioController, String drumPath);

    private static native void nRelease(long pAudioController);


}
