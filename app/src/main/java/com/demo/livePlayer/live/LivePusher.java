package com.demo.livePlayer.live;

import android.app.Activity;
import android.view.SurfaceHolder;

public class LivePusher {
    private AudioChannel audioChannel;
    private VideoChannel videoChannel;

    static {
        System.loadLibrary("liveplayer");
    }

    public LivePusher(Activity activity, int width, int height, int bitrate,
                      int fps, int cameraId) {
        native_init();
        videoChannel = new VideoChannel(this, activity, width, height, bitrate, fps, cameraId);
        audioChannel = new AudioChannel(this);
    }

    public void setPreviewDisplay(SurfaceHolder surfaceHolder) {
        videoChannel.setPreviewDisplay(surfaceHolder);
    }

    public void switchCamera() {
        videoChannel.switchCamera();
    }

    public void startLive(String url) {
        native_start(url);
        videoChannel.startLive();
        audioChannel.startLive();
    }


    public native void native_init();
    public native void native_setVideoEncInfo(int width,int height,int fps,int bitrate);
    public native void native_start(String url);
    public native void native_pushVideo(byte[] data);
    public native void native_pushAudio(byte[] data);
    public native void native_setAudioEncInfo(int i,int channels);
    public native int getInoutSamples();
}

