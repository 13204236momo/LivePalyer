package com.demo.livePlayer.util;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class LivePlayerUtil implements SurfaceHolder.Callback {
    static {
        System.loadLibrary("liveplayer");
    }

    private SurfaceHolder surfaceHolder;


    /**
     * 绘制 ndk path surfaceview
     *
     * @param surfaceView
     */
    public void setSurfaceView(SurfaceView surfaceView) {
        if (null != null) {
            this.surfaceHolder.removeCallback(this);
        }
        this.surfaceHolder = surfaceView.getHolder();
        this.surfaceHolder.addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        this.surfaceHolder = holder;
        holder.getSurface();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    public void start(String path) {
        native_start(path,surfaceHolder.getSurface());
    }

    public void sound(String input,String output){
        native_sound(input,output);
    }

    private native void native_start(String path, Surface surface);

    private native void native_sound(String input,String output);
}
