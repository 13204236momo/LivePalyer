package com.demo.livePlayer.live;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class Player implements SurfaceHolder.Callback {
    static {
        System.loadLibrary("liveplayer");
    }

    private SurfaceHolder surfaceHolder;

    public void setSurfaceView(SurfaceView surfaceView) {
        if (null != this.surfaceHolder) {
            this.surfaceHolder.removeCallback(this);
        }
        this.surfaceHolder = surfaceView.getHolder();
        this.surfaceHolder.addCallback(this);
    }

    public void prepare(String dataSource) {
        native_prepare(dataSource);
    }

    public void start() {
        native_start();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        native_set_surface(holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    public void onPrepared() {
        if (listener != null) {
            listener.onPrepared();
        }
    }

    public void onProgress(int progress) {
        if (listener != null) {
            listener.onProgress(progress);
        }
    }

    public void onError(int errorCode) {
        if (listener != null) {
            listener.onError(errorCode);
        }
    }

    public int getDuration(){
        return native_getDuration();
    }

    public OnNativePlayStateListener listener;

    public void setOnNativePlayStateListener(OnNativePlayStateListener listener) {
        this.listener = listener;
    }

    public void seek(final int progress) {
        new Thread(){
            @Override
            public void run() {
                native_seek(progress);
            }
        }.start();
    }

    public void suspend() {
        new Thread(){
            @Override
            public void run() {
                native_suspend();
            }
        }.start();
    }

    public void continuePlay() {
        new Thread(){
            @Override
            public void run() {
                native_continuePlay();
            }
        }.start();
    }

    public void stop(){
        native_stop();
    }

    public void release(){
        if (null !=this.surfaceHolder){
            this.surfaceHolder.removeCallback(this);
        }
        //native_stop();
        native_release();
    }


    public interface OnNativePlayStateListener {
        void onPrepared();

        void onProgress(int progress);

        void onError(int errorCode);
    }


   /*
    *  jni 方法
    */

    private native void native_prepare(String dataSource);

    private native void native_start();

    private native void native_set_surface(Surface surface);

    private native int native_getDuration();

    private native void native_seek(int progress);

    private native void native_suspend();

    private native void native_continuePlay();

    private native void native_stop();

    private native void native_release();
}
