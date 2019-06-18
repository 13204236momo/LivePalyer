package com.demo.livePlayer;


import android.Manifest;
import android.hardware.Camera;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Toast;

import com.demo.livePlayer.util.PermissionUtility;
import com.demo.livePlayer.util.live.LivePusher;

import java.io.File;

import butterknife.BindView;
import butterknife.ButterKnife;
import io.reactivex.functions.Consumer;

public class LiveActivity extends AppCompatActivity {
    @BindView(R.id.surfaceView)
    SurfaceView surfaceView;

    private LivePusher livePusher;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_live);
        ButterKnife.bind(this);

        getPermission();
    }

    private void init() {
        livePusher = new LivePusher(this, 800, 480, 800_00, 10, Camera.CameraInfo.CAMERA_FACING_BACK);
        //设置摄像头预览界面
        livePusher.setPreviewDisplay(surfaceView.getHolder());
    }

    public void switchCamera(View view) {
        toStringX264();
    }

    public void startLive(View view) {
        livePusher.startLive("");
    }

    public void stopLive(View view) {
    }

    private void getPermission() {
        PermissionUtility.getRxPermission(LiveActivity.this)
                .request(Manifest.permission.CAMERA) //申请所需权限
                .subscribe(new Consumer<Boolean>() {
                    @Override
                    public void accept(Boolean granted) throws Exception {
                        if (granted) {
                            init();
                        } else {
                            Toast.makeText(LiveActivity.this, "请开启摄像头权限", Toast.LENGTH_LONG).show();
                        }
                    }
                });
    }

    private native void toStringX264();
}
