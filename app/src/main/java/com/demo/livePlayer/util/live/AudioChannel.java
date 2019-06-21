package com.demo.livePlayer.util.live;


import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class AudioChannel {
    private LivePusher mLivePusher;
    private AudioRecord audioRecord;
    private int inputSamples;
    private int channels = 2;
    private  int minBufferSize;
    int channelConfig;
    private boolean isLiving;
    private ExecutorService executor;
    public AudioChannel(LivePusher livePusher) {
        executor = Executors.newSingleThreadExecutor();
        mLivePusher = livePusher;
        if (channelConfig ==2){
            channelConfig = AudioFormat.CHANNEL_IN_STEREO;
        }else {
            channelConfig = AudioFormat.CHANNEL_IN_MONO;
        }
        mLivePusher.native_setAudioEncInfo(44100,channels);
        inputSamples = mLivePusher.getInoutSamples() * 2;
        minBufferSize = AudioRecord.getMinBufferSize(44100,channelConfig,AudioFormat.ENCODING_PCM_16BIT)*2;
        audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,44100,
                channelConfig,AudioFormat.ENCODING_PCM_16BIT,minBufferSize>inputSamples?inputSamples:minBufferSize);

        
    }
    
    public void startLive(){
        isLiving = true;
        executor.submit(new AudioTask());
    }

    class AudioTask implements Runnable{

        @Override
        public void run() {
            audioRecord.startRecording();
            //pcm 音频原始数据
            byte[] bytes = new byte[minBufferSize];
            while (isLiving){
                int len = audioRecord.read(bytes,0,bytes.length);
                mLivePusher.native_pushAudio(bytes);
            }
        }
    }

    public int getChannels() {
        return channels;
    }

    public void setChannels(int channels) {
        this.channels = channels;
    }
}