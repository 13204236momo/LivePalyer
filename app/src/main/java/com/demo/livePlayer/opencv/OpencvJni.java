package com.demo.livePlayer.opencv;

public class OpencvJni {

    static {
        System.loadLibrary("liveplayer");
    }

    public native void init(String path);

    public native void postData(byte[] data, int i, int i1, int cameraId);
}
