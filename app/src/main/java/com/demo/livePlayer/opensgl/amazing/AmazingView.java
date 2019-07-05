package com.demo.livePlayer.opensgl.amazing;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public class AmazingView extends GLSurfaceView {
    public AmazingView(Context context) {
        super(context);
    }

    public AmazingView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setEGLContextClientVersion(2);
        setRenderer(new AmazingRender(this));
    }
}
