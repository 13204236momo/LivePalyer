package com.demo.livePlayer;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.demo.livePlayer.opensgl.amazing.AmazingView;

import butterknife.BindView;
import butterknife.ButterKnife;

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
