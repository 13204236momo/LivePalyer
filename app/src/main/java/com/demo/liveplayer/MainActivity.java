package com.demo.liveplayer;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.SeekBar;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private SurfaceView surfaceView;
    private SeekBar seekBar;

    private LivePlayerUtil util;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        initView();
    }

    private void initView(){
        surfaceView = findViewById(R.id.surfaceView);
        seekBar = findViewById(R.id.seekBar);

        util = new LivePlayerUtil();
        util.setSurfaceView(surfaceView);
    }



    /**
     * 播放
     * @param view
     * 文件路径可以是本地路径或URI
     */
    public void open(View view) {
        File file = new File(Environment.getExternalStorageDirectory(),"intput.mp4");
        util.start(file.getAbsolutePath());
    }

}
