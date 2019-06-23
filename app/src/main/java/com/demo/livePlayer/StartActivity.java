package com.demo.livePlayer;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

public class StartActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_start);
    }

    public void videoPlay(View view) {
        startActivity(new Intent(this,MainActivity.class));
    }

    public void liveUp(View view) {
        startActivity(new Intent(this, LivePushActivity.class));
    }

    public void liveDown(View view) {
        startActivity(new Intent(this, LivePullActivity.class));
    }
}
