package com.demo.livePlayer;

import android.Manifest;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.SeekBar;
import android.widget.Toast;

import com.demo.livePlayer.util.LivePlayerUtil;
import com.demo.livePlayer.util.PermissionUtility;

import java.io.File;

import io.reactivex.functions.Consumer;

public class MainActivity extends AppCompatActivity {

    private SurfaceView surfaceView;
    private SeekBar seekBar;

    private LivePlayerUtil util;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        initView();
    }

    private void initView() {
        surfaceView = findViewById(R.id.surfaceView);
        seekBar = findViewById(R.id.seekBar);

        util = new LivePlayerUtil();
        util.setSurfaceView(surfaceView);
    }


    /**
     * 播放
     *
     * @param view 文件路径可以是本地路径或URI
     */
    public void open(View view) {
        getPermissions(view);
    }

    private void getPermissions(final View view) {
        PermissionUtility.getRxPermission(MainActivity.this)
                .request(Manifest.permission.READ_EXTERNAL_STORAGE) //申请所需权限
                .subscribe(new Consumer<Boolean>() {
                    @Override
                    public void accept(Boolean granted) throws Exception {
                        if (granted) {
                            switch (view.getId()) {
                                case R.id.btn_start:
                                    File file = new File(Environment.getExternalStorageDirectory(), "input.mp4");
                                    util.start(file.getAbsolutePath());
                                    break;
                                case R.id.btn_play:
                                    String input = new File(Environment.getExternalStorageDirectory(), "input.mp3").getAbsolutePath();
                                    String output = new File(Environment.getExternalStorageDirectory(), "output.pcm").getAbsolutePath();
                                    util.sound(input,output);
                                    break;
                            }

                        } else {
                            Toast.makeText(MainActivity.this, "请开启读写权限", Toast.LENGTH_LONG).show();
                        }
                    }
                });
    }

    public void play(View view) {
        getPermissions(view);
    }
}
