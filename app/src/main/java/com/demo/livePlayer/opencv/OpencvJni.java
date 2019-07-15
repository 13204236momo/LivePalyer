package com.demo.livePlayer.opencv;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;

import com.demo.livePlayer.util.CameraHelper1;

public class OpencvJni {
    private static final int CHECK_FACE = 1;
    private long self;
    private Handler mHandler;
    private HandlerThread mHandlerThread;

    static {
        System.loadLibrary("liveplayer");
    }

    public OpencvJni(String path, final CameraHelper1 cameraHelper) {
        self = init(path, "");
        mHandlerThread = new HandlerThread("track");
        mHandlerThread.start();
        mHandler = new Handler(mHandlerThread.getLooper()) {
            @Override
            public void handleMessage(Message msg) {
                //子线程
                native_detector(self,(byte[])msg.obj,cameraHelper.getCameraId(),cameraHelper.WIDTH,cameraHelper.HEIGHT);
            }
        };
    }

    private native Face native_detector(long self, byte[] obj, int cameraId, int width, int height);


    public void startTrack() {
        native_startTrack(self);
    }

    public void detector(byte[] data) {
        //先把之前的消息移除掉,防止检测旧的数据
        mHandler.removeMessages(CHECK_FACE);
        //加入新的任务
        Message message = mHandler.obtainMessage(CHECK_FACE);
        message.obj = data;
        mHandler.sendEmptyMessage(CHECK_FACE);

    }

    private native void native_startTrack(long self);

    private native long init(String path, String seeta);


//    public native void init(String path);
//
//    public native void postData(byte[] data, int i, int i1, int cameraId);
}
