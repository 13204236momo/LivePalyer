package com.demo.livePlayer;

import android.hardware.Camera;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.demo.livePlayer.opencv.OpencvJni;
import com.demo.livePlayer.util.CameraHelper;
import com.demo.livePlayer.util.CameraHelper1;
import com.demo.livePlayer.util.FileUtil;

import java.io.File;

import butterknife.BindView;
import butterknife.ButterKnife;

public class FollowFaceActivity extends AppCompatActivity implements SurfaceHolder.Callback,Camera.PreviewCallback{

    @BindView(R.id.surfaceView)
    SurfaceView surfaceView;

    private OpencvJni opencvJni;
    private CameraHelper cameraHelper;
    private int cameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_follow_face);
        ButterKnife.bind(this);

        opencvJni = new OpencvJni();
        cameraHelper = new CameraHelper(cameraId,640,480);
        cameraHelper.setPreviewCallback(this);
        surfaceView.getHolder().addCallback(this);
        FileUtil.copyAssets(this,"lbpcascade_frontalface.xml");
    }

    @Override
    protected void onResume() {
        super.onResume();
        String path = new File(Environment.getExternalStorageDirectory(),"lbpcascade_frontalface.xml").getAbsolutePath();
        cameraHelper.startPreview();
        opencvJni.init(path);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
opencvJni.postData(data,640,480,cameraId);
    }
}
