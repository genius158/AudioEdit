package com.yan.audioedit

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.yan.audioedit.audiomix.AudioEntity
import com.yan.audioedit.audiomix.AudioMixer
import com.yan.audioedit.databinding.ActivityMainBinding
import java.io.File

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    private val audioMixer by lazy { AudioMixer(this) }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Assets2Sdcard.ctx = application

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        setSupportActionBar(binding.toolbar)

        Assets2Sdcard(this).execute()

        audioMixer.testWavHeader( File(Assets2Sdcard.getMusicDir(), "wangbei.wav").absolutePath)
        audioMixer.mixAudio(
            arrayOf(
                AudioEntity(
                    File(Assets2Sdcard.getMusicDir(), "wangbei.wav").absolutePath,
                    0, 32 * 1000, 52 * 1000
                ),
                AudioEntity(
                    File(Assets2Sdcard.getMusicDir(), "wangbei.wav").absolutePath,
                    54, 32 * 1000, 52 * 1000
                ),
                AudioEntity(
                    File(Assets2Sdcard.getMusicDir(), "y932.wav").absolutePath,
                    3000, 10 * 1000, 20 * 1000
                ),
                AudioEntity(
                    File(Assets2Sdcard.getMusicDir(), "y932.wav").absolutePath,
                    5000, 15 * 1000, 20 * 1000
                )
            )
        )
    }

}