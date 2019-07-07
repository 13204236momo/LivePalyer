package com.demo.livePlayer;

import android.Manifest;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.Toast;

import com.demo.livePlayer.opensgl.amazing.AmazingView;
import com.demo.livePlayer.util.PermissionUtility;

import butterknife.BindView;
import butterknife.ButterKnife;
import io.reactivex.functions.Consumer;

public class AmazingActivity extends AppCompatActivity {

    @BindView(R.id.amazingView)
    AmazingView amazingView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_amazing);
        ButterKnife.bind(this);

    }

}
