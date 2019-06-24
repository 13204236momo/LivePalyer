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

import com.demo.livePlayer.util.PermissionUtility;
import com.demo.livePlayer.util.player.Player;

import java.io.File;

import butterknife.BindView;
import butterknife.ButterKnife;
import io.reactivex.functions.Consumer;

public class LivePullActivity extends AppCompatActivity implements SeekBar.OnSeekBarChangeListener {

    @BindView(R.id.seekBar)
    SeekBar seekBar;
    @BindView(R.id.surfaceView)
    SurfaceView surfaceView;

    private Player player;
    private int progress;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_live_pull);
        ButterKnife.bind(this);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        seekBar.setOnSeekBarChangeListener(this);
        player = new Player();
        player.setSurfaceView(surfaceView);
        player.setOnNativePlayStateListener(new Player.OnNativePlayStateListener() {
            @Override
            public void onPrepared() {
                player.start();
            }

            @Override
            public void onProgress(int progress) {

            }

            @Override
            public void onError(int errorCode) {

            }
        });
    }

    public void startWatch(View view) {
        getPermissions(view);
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {

    }

    private void getPermissions(final View view) {
        PermissionUtility.getRxPermission(LivePullActivity.this)
                .request(Manifest.permission.READ_EXTERNAL_STORAGE) //申请所需权限
                .subscribe(new Consumer<Boolean>() {
                    @Override
                    public void accept(Boolean granted) throws Exception {
                        if (granted) {
                            switch (view.getId()) {
                                case R.id.btn_start:
                                    File file = new File(Environment.getExternalStorageDirectory(), "input.mp4");
                                    player.prepare(file.getAbsolutePath());
                                    break;
                                case R.id.btn_play:
                                    String input = new File(Environment.getExternalStorageDirectory(), "input.mp3").getAbsolutePath();
                                    String output = new File(Environment.getExternalStorageDirectory(), "output.pcm").getAbsolutePath();

                                    break;
                            }

                        } else {
                            Toast.makeText(LivePullActivity.this, "请开启读写权限", Toast.LENGTH_LONG).show();
                        }
                    }
                });
    }

}
