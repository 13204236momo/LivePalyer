package com.demo.livePlayer;

import android.Manifest;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import com.demo.livePlayer.util.PermissionUtility;

import io.reactivex.functions.Consumer;

public class StartActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_start);
    }

    public void videoPlay(View view) {
        startActivity(new Intent(this, MainActivity.class));
    }

    public void liveUp(View view) {
        startActivity(new Intent(this, LivePushActivity.class));
    }

    public void liveDown(View view) {
        startActivity(new Intent(this, LivePullActivity.class));
    }

    public void OPENSGL(View view) {
        startActivity(new Intent(this, OpenSGLActivity.class));
    }

    public void amazing(View view) {
        getPermission();
    }

    public void face(View view) {
        startActivity(new Intent(this,FollowFaceActivity.class));
    }

    private void getPermission() {
        PermissionUtility.getRxPermission(StartActivity.this)
                .request(Manifest.permission.CAMERA) //申请所需权限
                .subscribe(new Consumer<Boolean>() {
                    @Override
                    public void accept(Boolean granted) throws Exception {
                        if (granted) {
                            startActivity(new Intent(StartActivity.this, AmazingActivity.class));
                        } else {
                            Toast.makeText(StartActivity.this, "请开启摄像头权限", Toast.LENGTH_LONG).show();
                        }
                    }
                });
    }


}
