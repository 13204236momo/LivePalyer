package com.demo.livePlayer.util.live;

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
        videoChannel = new VideoChannel(this, activity, width, height, bitrate, fps, cameraId);
        audioChannel = new AudioChannel(this);
    }

    public void setPreviewDisplay(SurfaceHolder surfaceHolder) {
        videoChannel.setPreviewDisplay(surfaceHolder);
    }

    public void switchCamera() {
        videoChannel.switchCamera();
    }
}

