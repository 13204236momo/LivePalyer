package com.demo.livePlayer.opencv;

//native反射生成 jni反射技术
public class Face {
    public float[] faceRects;
    public int width;
    public int height;
    //检测的宽高
    public int imgWidth;
    public int imgHeight;

    public Face(float[] faceRects, int width, int height, int imgWidth, int imgHeight) {
        this.faceRects = faceRects;
        this.width = width;
        this.height = height;
        this.imgWidth = imgWidth;
        this.imgHeight = imgHeight;
    }
}
