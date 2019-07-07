package com.demo.livePlayer.opensgl.amazing;


import android.content.Context;

import com.demo.livePlayer.R;

//显示滤镜  显示cameraFilter已经渲染好的特效
public class ScreenFilter extends AbstractFilter {

    public ScreenFilter(Context context) {
        super(context, R.raw.base_vertex, R.raw.base_frag);
    }

    @Override
    protected void initCoordinate() {

    }

//    public void onReady(int width, int height) {
//        super.onReady(width, height);
//    }
}
