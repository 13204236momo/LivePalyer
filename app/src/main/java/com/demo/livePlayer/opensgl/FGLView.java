package com.demo.livePlayer.opensgl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public class FGLView extends GLSurfaceView {
    public FGLView(Context context) {
        super(context);
    }
    //三角形  ----等腰三角形   正方形    立方体
    public FGLView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setEGLContextClientVersion(2);
        setRenderer(new FGLRender(this));
//        效率高  自己调用渲染
//        requestRender();

        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

}
