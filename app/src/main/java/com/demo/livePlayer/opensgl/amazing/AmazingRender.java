package com.demo.livePlayer.opensgl.amazing;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Environment;

import com.demo.livePlayer.opencv.OpencvJni;
import com.demo.livePlayer.util.CameraHelper1;
import com.demo.livePlayer.util.FileUtil;

import java.io.File;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

class AmazingRender implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener, Camera.PreviewCallback {
    private CameraHelper1 mCameraHelper;
    private AmazingView mView;
    private SurfaceTexture mSurfaceTexture;
    private int[] mTextures;
    private float[] mtx = new float[16];
    CameraFilter mCameraFilter;
    ScreenFilter mScreenFilter;
    OpencvJni opencvJni;
    String path;
    public AmazingRender( AmazingView amazingView) {
        mView = amazingView;
        init();
    }

    private void init() {
        path = new File(Environment.getExternalStorageDirectory(),"lbpcascade_frontalface.xml").getAbsolutePath();
        FileUtil.copyAssets(mView.getContext(),"lbpcascade_frontalface.xml");

    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
//            打开摄像头
        //初始化的操作
        mCameraHelper = new CameraHelper1(Camera.CameraInfo.CAMERA_FACING_BACK);
//        纹理   ==数据的输入
        mTextures = new int[1];
        GLES20.glGenTextures(mTextures.length,mTextures, 0);
//     BindTexture(GLES20.GL_TEXTURE_2D,textureId)--》 mTextures==mSurfaceTexture-->摄像头采集数据
        mSurfaceTexture = new SurfaceTexture(mTextures[0]);
        mSurfaceTexture.setOnFrameAvailableListener(this);
//        矩阵--》   摄像头的数据 不会变形  顶点
        mSurfaceTexture.getTransformMatrix(mtx);
        mCameraFilter = new CameraFilter(mView.getContext());
        mScreenFilter = new ScreenFilter(mView.getContext());
        mCameraFilter.setMatrix(mtx);
    }
    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        mView.requestRender();
    }
    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mCameraHelper.startPreview(mSurfaceTexture);
        mCameraHelper.setPreviewCallback(this);
        mCameraFilter.onReady(width,height);
        mScreenFilter.onReady(width,height);
        opencvJni = new OpencvJni(path,mCameraHelper);
        opencvJni.startTrack();
    }

    @Override
    public void onDrawFrame(GL10 gl) {
//        摄像头获取一帧数据   会回调此方法
        GLES20.glClearColor(0, 0, 0, 0);
//        执行清空
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        mSurfaceTexture.updateTexImage();
        mSurfaceTexture.getTransformMatrix(mtx);
        mCameraFilter.setMatrix(mtx);
//         信息  int   类型---》GPU 纹理  Request    ---》返回 一个新的   Request
        int id=mCameraFilter.onDrawFrame(mTextures[0]);
//        id ==效果1.onDrawFrame(id);  帽子
        // id ==效果2.onDrawFrame(id);  眼镜
        // id ==效果2.onDrawFrame(id);  大耳朵

//        mScreenFilter 将最终的特效运用到SurfaceView中
        mScreenFilter.onDrawFrame(id);
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        opencvJni.detector(data);
    }
}
